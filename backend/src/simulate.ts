import crypto from 'crypto';
import fs from 'fs';
import path from 'path';

const TYPES = ['SOS', 'FALL', 'FALL_CANCELLED', 'HEARTBEAT'];

async function main() {
  const args = process.argv.slice(2);
  const type = args.find((a) => !a.startsWith('--'))?.toUpperCase();
  const ci = args.indexOf('--count');
  const count = ci >= 0 ? Number(args[ci + 1]) : 1;
  if (!type || !TYPES.includes(type) || !Number.isInteger(count) || count < 1) {
    console.error('Usage: npm run simulate -- <SOS|FALL|FALL_CANCELLED|HEARTBEAT> [--count n]');
    process.exit(1);
  }
  const file = path.join(__dirname, '..', '.device-token');
  const token = process.env.DEVICE_TOKEN || (fs.existsSync(file) ? fs.readFileSync(file, 'utf8').trim() : '');
  if (!token) { console.error('No token: set DEVICE_TOKEN or run npm run seed'); process.exit(1); }
  const url = (process.env.API_URL || 'http://localhost:3000').replace(/\/$/, '') + '/api/v1/events';
  for (let i = 0; i < count; i++) {
    const body = { event_id: crypto.randomUUID(), type, ts: new Date().toISOString(), source: 'simulated' };
    const res = await fetch(url, { method: 'POST', headers: { Authorization: `Bearer ${token}`, 'Content-Type': 'application/json' }, body: JSON.stringify(body) });
    console.log(`${type} -> ${res.status} ${await res.text()}`);
  }
}
main().catch((e) => { console.error(e); process.exit(1); });
