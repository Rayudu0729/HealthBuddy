import crypto from 'crypto';
import fs from 'fs';
import path from 'path';
import bcrypt from 'bcryptjs';
import { Pool } from 'pg';
import { hashToken } from './app';
import { config } from './config';
import { tx } from './db';

async function main() {
  const email = (process.env.SEED_CAREGIVER_EMAIL || 'demo@bindu.local').toLowerCase();
  const password = process.env.SEED_CAREGIVER_PASSWORD || 'bindu-demo-123';
  const pool = new Pool({ connectionString: config.databaseUrl });
  const token = crypto.randomBytes(32).toString('hex');
  try {
    const hash = await bcrypt.hash(password, 10);
    await tx(pool, async (q) => {
      const cg = await q(
        `INSERT INTO caregivers (email, name, password_hash) VALUES ($1, 'Demo Caregiver', $2)
         ON CONFLICT (email) DO UPDATE SET password_hash = EXCLUDED.password_hash RETURNING id`,
        [email, hash],
      );
      await q(`INSERT INTO devices (id, name) VALUES ('bindu-001', 'Bindu demo watch') ON CONFLICT (id) DO NOTHING`);
      await q(`INSERT INTO caregiver_devices (caregiver_id, device_id) VALUES ($1, 'bindu-001') ON CONFLICT DO NOTHING`, [cg.rows[0].id]);
      await q(`UPDATE device_tokens SET revoked_at = $1 WHERE device_id = 'bindu-001' AND revoked_at IS NULL`, [new Date()]);
      await q(`INSERT INTO device_tokens (device_id, token_hash) VALUES ('bindu-001', $1)`, [hashToken(token)]);
    });
    fs.writeFileSync(path.join(__dirname, '..', '.device-token'), token + '\n');
    console.log(`Caregiver: ${email} / ${password}`);
    console.log(`Device bindu-001 token (shown once, also saved to backend/.device-token):\n${token}`);
  } finally {
    await pool.end();
  }
}
main().catch((e) => { console.error(e); process.exitCode = 1; });
