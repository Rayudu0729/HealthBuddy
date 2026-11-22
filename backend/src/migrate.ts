import fs from 'fs';
import path from 'path';
import { Pool } from 'pg';
import { config } from './config';
import type { Db } from './db';

export async function runMigrations(db: Db, dir = path.join(__dirname, '..', 'migrations')): Promise<string[]> {
  await db.query('CREATE TABLE IF NOT EXISTS schema_migrations (name text PRIMARY KEY, applied_at timestamptz NOT NULL DEFAULT now())');
  const done = new Set((await db.query('SELECT name FROM schema_migrations')).rows.map((r) => r.name));
  const applied: string[] = [];
  for (const f of fs.readdirSync(dir).filter((x) => x.endsWith('.sql')).sort()) {
    if (done.has(f)) continue;
    await db.query(fs.readFileSync(path.join(dir, f), 'utf8'));
    await db.query('INSERT INTO schema_migrations (name) VALUES ($1)', [f]);
    applied.push(f);
  }
  return applied;
}

if (require.main === module) {
  const pool = new Pool({ connectionString: config.databaseUrl });
  runMigrations(pool)
    .then((a) => console.log(a.length ? `Applied: ${a.join(', ')}` : 'Up to date'))
    .catch((e) => { console.error(e); process.exitCode = 1; })
    .finally(() => pool.end());
}
