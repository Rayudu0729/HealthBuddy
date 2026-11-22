import type { Db } from './db';

type Q = Db['query'];
const iso = (v: unknown): string | null => (v == null ? null : new Date(v as string | Date).toISOString());

export function deviceStatus(lastSeen: Date | string | null, now = Date.now()): 'online' | 'stale' | 'offline' {
  if (!lastSeen) return 'offline';
  const age = now - new Date(lastSeen).getTime();
  return age < 2 * 60_000 ? 'online' : age < 10 * 60_000 ? 'stale' : 'offline';
}

const DEVICE_SQL = 'SELECT d.id, d.name, d.last_seen FROM devices d';

async function withOpenAlerts(q: Q, rows: any[]) {
  if (!rows.length) return [];
  const { rows: counts } = await q(
    "SELECT device_id, COUNT(*) AS n FROM alerts WHERE state = 'open' GROUP BY device_id",
  );
  const byId = new Map(counts.map((c) => [c.device_id, Number(c.n)]));
  return rows.map((r) => ({
    id: r.id, name: r.name, last_seen: iso(r.last_seen), status: deviceStatus(r.last_seen), open_alerts: byId.get(r.id) ?? 0,
  }));
}

export async function getDevice(q: Q, id: string) {
  const { rows } = await q(`${DEVICE_SQL} WHERE d.id = $1`, [id]);
  return (await withOpenAlerts(q, rows))[0] ?? null;
}

export async function listDevices(q: Q, caregiverId: number) {
  const { rows } = await q(
    `${DEVICE_SQL} JOIN caregiver_devices cd ON cd.device_id = d.id WHERE cd.caregiver_id = $1 ORDER BY d.id`,
    [caregiverId],
  );
  return withOpenAlerts(q, rows);
}

export async function caregiverOwnsDevice(q: Q, caregiverId: number, deviceId: string) {
  const { rows } = await q('SELECT 1 FROM caregiver_devices WHERE caregiver_id = $1 AND device_id = $2', [caregiverId, deviceId]);
  return rows.length > 0;
}

export async function caregiverDeviceIds(q: Q, caregiverId: number): Promise<string[]> {
  const { rows } = await q('SELECT device_id FROM caregiver_devices WHERE caregiver_id = $1', [caregiverId]);
  return rows.map((r) => r.device_id);
}

export function mapEvent(r: any) {
  return {
    id: r.id, device_id: r.device_id, event_id: r.event_id, type: r.type,
    value: r.value == null ? null : Number(r.value), mode: r.mode ?? null, source: r.source,
    device_ts: iso(r.device_ts), received_at: iso(r.received_at),
  };
}

export async function listEvents(q: Q, deviceId: string, limit: number) {
  const { rows } = await q('SELECT * FROM events WHERE device_id = $1 ORDER BY id DESC LIMIT $2', [deviceId, limit]);
  return rows.map(mapEvent);
}

const ALERT_SQL = `SELECT a.*, c.email AS acked_by_email,
  o.status AS sms_status, o.attempts AS sms_attempts, o.last_error AS sms_last_error, o.sent_at AS sms_sent_at
  FROM alerts a
  LEFT JOIN caregivers c ON c.id = a.acknowledged_by
  LEFT JOIN sms_outbox o ON o.alert_id = a.id`;

export function mapAlert(r: any) {
  return {
    id: r.id, device_id: r.device_id, event_id: r.event_id, type: r.type,
    value: r.value == null ? null : Number(r.value), source: r.source, state: r.state,
    created_at: iso(r.created_at), acknowledged_at: iso(r.acknowledged_at), acknowledged_by: r.acked_by_email ?? null,
    sms: {
      status: r.sms_status ?? 'none', attempts: Number(r.sms_attempts ?? 0),
      last_error: r.sms_last_error ?? null, sent_at: iso(r.sms_sent_at),
    },
  };
}

export async function getAlert(q: Q, id: number) {
  const { rows } = await q(`${ALERT_SQL} WHERE a.id = $1`, [id]);
  return rows[0] ? mapAlert(rows[0]) : null;
}

export async function getAlertByEvent(q: Q, deviceId: string, eventId: string) {
  const { rows } = await q(`${ALERT_SQL} WHERE a.device_id = $1 AND a.event_id = $2`, [deviceId, eventId]);
  return rows[0] ? mapAlert(rows[0]) : null;
}

export async function listAlerts(q: Q, deviceId: string, limit: number) {
  const { rows } = await q(`${ALERT_SQL} WHERE a.device_id = $1 ORDER BY a.id DESC LIMIT $2`, [deviceId, limit]);
  return rows.map(mapAlert);
}
