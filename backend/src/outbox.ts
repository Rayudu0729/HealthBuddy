import type { Bus } from './bus';
import { tx, type Db } from './db';
import { getAlert } from './repo';
import type { SmsAdapter } from './sms';

export const MAX_ATTEMPTS = 5;
const LEASE_MS = 60_000;

export interface OutboxOptions {
  db: Db;
  sms: SmsAdapter;
  bus: Bus;
  /** base backoff; delay = base * 2^(attempts-1). Tests set 0. */
  backoffMs?: number;
  /** FOR UPDATE SKIP LOCKED on the claim query (real Postgres). pg-mem can't parse it. */
  skipLocked?: boolean;
  batch?: number;
}

/** Claim due rows (leasing them by pushing next_attempt_at), send, record results. Returns rows processed. */
export async function processOutboxOnce(o: OutboxOptions): Promise<number> {
  const { db, sms, bus, backoffMs = 5000, skipLocked = true, batch = 10 } = o;
  const claimed = await tx(db, async (q) => {
    const lock = skipLocked ? 'FOR UPDATE SKIP LOCKED' : '';
    const { rows } = await q(
      `SELECT id FROM sms_outbox WHERE status = 'pending' AND next_attempt_at <= $1 ORDER BY id LIMIT ${Number(batch)} ${lock}`,
      [new Date()],
    );
    const lease = new Date(Date.now() + LEASE_MS);
    const out = [];
    for (const r of rows) {
      const { rows: full } = await q('UPDATE sms_outbox SET next_attempt_at = $2 WHERE id = $1 RETURNING *', [r.id, lease]);
      out.push(full[0]);
    }
    return out;
  });

  for (const row of claimed) {
    const attempts = row.attempts + 1;
    try {
      await sms.send({ to: row.to_number, content: row.content, requestId: `bindu-alert-${row.alert_id}` });
      await db.query(
        `UPDATE sms_outbox SET status = 'sent', attempts = $2, sent_at = $3, last_error = NULL WHERE id = $1`,
        [row.id, attempts, new Date()],
      );
    } catch (err) {
      const failed = attempts >= MAX_ATTEMPTS;
      const next = new Date(Date.now() + backoffMs * 2 ** (attempts - 1));
      await db.query(
        `UPDATE sms_outbox SET status = $2, attempts = $3, last_error = $4, next_attempt_at = $5 WHERE id = $1`,
        [row.id, failed ? 'failed' : 'pending', attempts, String((err as Error).message ?? err).slice(0, 500), next],
      );
    }
    const alert = await getAlert(db.query.bind(db) as Db['query'], row.alert_id);
    if (alert) bus.publish({ kind: 'alert', deviceId: alert.device_id, data: alert });
  }
  return claimed.length;
}

export function startOutboxWorker(o: OutboxOptions, intervalMs = 3000) {
  let running = false;
  const timer = setInterval(async () => {
    if (running) return;
    running = true;
    try { await processOutboxOnce(o); } catch (e) { console.error('[outbox]', e); } finally { running = false; }
  }, intervalMs);
  return { stop: () => clearInterval(timer) };
}
