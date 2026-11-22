import crypto from 'crypto';
import bcrypt from 'bcryptjs';
import { newDb } from 'pg-mem';
import { createApp, hashToken } from '../src/app';
import { Bus } from '../src/bus';
import { runMigrations } from '../src/migrate';
import { FakeSmsAdapter } from '../src/sms';
import type { Db } from '../src/db';

export async function setup(opts: { smsTo?: string } = {}) {
  const mem = newDb();
  const { Pool } = mem.adapters.createPg();
  const db = new Pool() as unknown as Db;
  await runMigrations(db);
  const bus = new Bus();
  const sms = new FakeSmsAdapter();
  const smsTo = opts.smsTo ?? '+15550001111';
  const app = createApp({ db, bus, config: { sessionSecret: 't', dashboardOrigin: 'http://localhost:5173', caregiverSmsTo: smsTo } });
  const q = db.query.bind(db) as Db['query'];

  async function addDevice(id: string) {
    await q('INSERT INTO devices (id, name) VALUES ($1, $2)', [id, 'Watch ' + id]);
    const token = crypto.randomBytes(16).toString('hex');
    await q('INSERT INTO device_tokens (device_id, token_hash) VALUES ($1, $2)', [id, hashToken(token)]);
    return token;
  }
  async function addCaregiver(email: string, devices: string[]) {
    const r = await q('INSERT INTO caregivers (email, name, password_hash) VALUES ($1, $2, $3) RETURNING id', [email, email, await bcrypt.hash('pw12345', 4)]);
    for (const d of devices) await q('INSERT INTO caregiver_devices (caregiver_id, device_id) VALUES ($1, $2)', [r.rows[0].id, d]);
    return { id: r.rows[0].id as number, email, password: 'pw12345' };
  }
  return { app, db, q, bus, sms, addDevice, addCaregiver, outboxOpts: { db, sms, bus, backoffMs: 0, skipLocked: false } };
}

export const ev = (type: string, extra: object = {}) => ({ event_id: crypto.randomUUID(), type, ts: new Date().toISOString(), ...extra });
