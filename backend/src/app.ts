import crypto from 'crypto';
import bcrypt from 'bcryptjs';
import cors from 'cors';
import express, { type NextFunction, type Request, type Response } from 'express';
import session from 'express-session';
import { z } from 'zod';
import type { Bus } from './bus';
import type { Config } from './config';
import { tx, type Db } from './db';
import * as repo from './repo';

declare module 'express-session' {
  interface SessionData { caregiverId?: number }
}
declare global {
  namespace Express {
    interface Request { deviceId?: string }
  }
}

export interface AppDeps {
  db: Db;
  bus: Bus;
  config: Pick<Config, 'sessionSecret' | 'dashboardOrigin' | 'caregiverSmsTo'>;
  /** express-session store; defaults to MemoryStore (tests). Server passes connect-pg-simple. */
  sessionStore?: session.Store;
}

export const hashToken = (t: string) => crypto.createHash('sha256').update(t).digest('hex');

class HttpError extends Error {
  constructor(public status: number, message: string) { super(message); }
}

const eventBody = z.object({
  event_id: z.uuid(),
  type: z.enum(['SOS', 'FALL', 'FALL_CANCELLED', 'HEARTBEAT']),
  value: z.number().finite().optional(),
  ts: z.iso.datetime({ offset: true }),
  mode: z.enum(['DAY', 'NIGHT', 'ALERT', 'SAFETY']).optional(),
  source: z.enum(['simulated']).optional(),
});
const limitQuery = z.coerce.number().int().min(1).max(200).default(50);
const loginBody = z.object({ email: z.string().min(1), password: z.string().min(1) });

// Valid bcrypt hash used to keep login timing similar for unknown emails.
const DUMMY_HASH = bcrypt.hashSync('not-a-real-password', 8);

export function createApp(deps: AppDeps) {
  const { db, bus, config } = deps;
  const q = db.query.bind(db) as Db['query'];
  const app = express();
  app.disable('x-powered-by');
  app.use(cors({ origin: config.dashboardOrigin, credentials: true }));
  app.use(express.json({ limit: '32kb' }));
  app.use(
    session({
      store: deps.sessionStore,
      secret: config.sessionSecret,
      name: 'bindu.sid',
      resave: false,
      saveUninitialized: false,
      cookie: { httpOnly: true, sameSite: 'lax', maxAge: 7 * 24 * 3600 * 1000 },
    }),
  );

  const api = express.Router();

  // ---- device ingestion ----
  const deviceAuth = async (req: Request, _res: Response, next: NextFunction) => {
    const m = /^Bearer (.+)$/.exec(req.header('authorization') ?? '');
    if (!m) throw new HttpError(401, 'Missing bearer token');
    const { rows } = await q('SELECT device_id FROM device_tokens WHERE token_hash = $1 AND revoked_at IS NULL', [hashToken(m[1].trim())]);
    if (!rows[0]) throw new HttpError(401, 'Invalid or revoked token');
    req.deviceId = rows[0].device_id;
    next();
  };

  api.post('/events', deviceAuth, async (req, res) => {
    const parsed = eventBody.safeParse(req.body);
    if (!parsed.success) throw new HttpError(400, 'Invalid body: ' + parsed.error.issues.map((i) => `${i.path.join('.')} ${i.message}`).join('; '));
    const b = parsed.data;
    const deviceId = req.deviceId!;
    const source = b.source ?? 'device';

    const result = await tx(db, async (tq) => {
      // Check first (pg-mem mishandles ON CONFLICT DO NOTHING); the unique index still guards races.
      const existing = await tq('SELECT id FROM events WHERE device_id = $1 AND event_id = $2', [deviceId, b.event_id]);
      if (existing.rows[0]) return { duplicate: true as const };
      const ins = await tq(
        `INSERT INTO events (device_id, event_id, type, value, mode, source, device_ts, received_at)
         VALUES ($1,$2,$3,$4,$5,$6,$7,$8) RETURNING *`,
        [deviceId, b.event_id, b.type, b.value ?? null, b.mode ?? null, source, new Date(b.ts), new Date()],
      );
      const event = repo.mapEvent(ins.rows[0]);
      await tq('UPDATE devices SET last_seen = $2 WHERE id = $1', [deviceId, new Date()]);

      const changed: number[] = [];
      if (b.type === 'SOS' || b.type === 'FALL') {
        const a = await tq(
          `INSERT INTO alerts (device_id, event_id, type, value, source, state, created_at)
           VALUES ($1,$2,$3,$4,$5,'open',$6) RETURNING id`,
          [deviceId, b.event_id, b.type, b.value ?? null, source, new Date()],
        );
        changed.push(a.rows[0].id);
        if (config.caregiverSmsTo) {
          await tq(
            'INSERT INTO sms_outbox (alert_id, to_number, content, next_attempt_at) VALUES ($1,$2,$3,$4)',
            [a.rows[0].id, config.caregiverSmsTo, `Bindu ${b.type} from ${deviceId} at ${b.ts}`, new Date()],
          );
        }
      } else if (b.type === 'FALL_CANCELLED') {
        const u = await tq(
          `UPDATE alerts SET state = 'cancelled' WHERE device_id = $1 AND type = 'FALL' AND state = 'open' RETURNING id`,
          [deviceId],
        );
        changed.push(...u.rows.map((r: any) => r.id));
      }
      return { duplicate: false as const, event, changed };
    }).catch((e) => { if (e?.code === '23505') return { duplicate: true as const }; throw e; });

    if (result.duplicate) {
      const { rows } = await q('SELECT * FROM events WHERE device_id = $1 AND event_id = $2', [deviceId, b.event_id]);
      const alert = await repo.getAlertByEvent(q, deviceId, b.event_id);
      return res.status(200).json({ event: repo.mapEvent(rows[0]), alert });
    }

    const device = await repo.getDevice(q, deviceId);
    bus.publish({ kind: 'event', deviceId, data: result.event });
    bus.publish({ kind: 'device', deviceId, data: device });
    for (const id of result.changed) {
      const alert = await repo.getAlert(q, id);
      if (alert) bus.publish({ kind: 'alert', deviceId, data: alert });
    }
    const own = await repo.getAlertByEvent(q, deviceId, b.event_id);
    res.status(201).json({ event: result.event, alert: own });
  });

  // ---- caregiver auth ----
  api.post('/auth/login', async (req, res) => {
    const p = loginBody.safeParse(req.body);
    if (!p.success) throw new HttpError(400, 'email and password required');
    const { rows } = await q('SELECT id, email, name, password_hash FROM caregivers WHERE email = $1', [p.data.email.trim().toLowerCase()]);
    const c = rows[0];
    const ok = await bcrypt.compare(p.data.password, c ? c.password_hash : DUMMY_HASH);
    if (!c || !ok) throw new HttpError(401, 'Invalid email or password');
    await new Promise<void>((resolve, reject) => req.session.regenerate((e) => (e ? reject(e) : resolve())));
    req.session.caregiverId = c.id;
    res.json({ caregiver: { id: c.id, email: c.email, name: c.name } });
  });

  api.post('/auth/logout', (req, res, next) => {
    req.session.destroy((e) => {
      if (e) return next(e);
      res.clearCookie('bindu.sid');
      res.status(204).end();
    });
  });

  const caregiverAuth = async (req: Request, res: Response, next: NextFunction) => {
    const id = req.session.caregiverId;
    if (id) {
      const { rows } = await q('SELECT id, email, name FROM caregivers WHERE id = $1', [id]);
      if (rows[0]) { res.locals.caregiver = rows[0]; return next(); }
    }
    throw new HttpError(401, 'Not signed in');
  };
  const cg = (res: Response) => res.locals.caregiver as { id: number; email: string; name: string };

  api.get('/auth/me', caregiverAuth, (_req, res) => {
    const c = cg(res);
    res.json({ caregiver: { id: c.id, email: c.email, name: c.name } });
  });

  // ---- caregiver data (ownership enforced; unassigned = 404) ----
  const ownedDevice = async (req: Request, res: Response) => {
    const id = String(req.params.deviceId);
    const device = (await repo.caregiverOwnsDevice(q, cg(res).id, id)) ? await repo.getDevice(q, id) : null;
    if (!device) throw new HttpError(404, 'Device not found');
    return device;
  };

  api.get('/devices', caregiverAuth, async (_req, res) => {
    res.json({ devices: await repo.listDevices(q, cg(res).id) });
  });
  api.get('/devices/:deviceId', caregiverAuth, async (req, res) => {
    res.json({ device: await ownedDevice(req, res) });
  });
  api.get('/devices/:deviceId/events', caregiverAuth, async (req, res) => {
    const device = await ownedDevice(req, res);
    const limit = limitQuery.safeParse(req.query.limit);
    if (!limit.success) throw new HttpError(400, 'Invalid limit');
    res.json({ events: await repo.listEvents(q, device.id, limit.data) });
  });
  api.get('/devices/:deviceId/alerts', caregiverAuth, async (req, res) => {
    const device = await ownedDevice(req, res);
    const limit = limitQuery.safeParse(req.query.limit);
    if (!limit.success) throw new HttpError(400, 'Invalid limit');
    res.json({ alerts: await repo.listAlerts(q, device.id, limit.data) });
  });

  api.post('/alerts/:alertId/acknowledge', caregiverAuth, async (req, res) => {
    const id = Number(req.params.alertId);
    const alert = Number.isInteger(id) ? await repo.getAlert(q, id) : null;
    if (!alert || !(await repo.caregiverOwnsDevice(q, cg(res).id, alert.device_id))) throw new HttpError(404, 'Alert not found');
    if (alert.state !== 'open') return res.json({ alert });
    const upd = await q(
      `UPDATE alerts SET state = 'acknowledged', acknowledged_at = $2, acknowledged_by = $3 WHERE id = $1 AND state = 'open' RETURNING id`,
      [id, new Date(), cg(res).id],
    );
    const fresh = (await repo.getAlert(q, id))!;
    if (upd.rows[0]) {
      bus.publish({ kind: 'alert', deviceId: fresh.device_id, data: fresh });
      bus.publish({ kind: 'device', deviceId: fresh.device_id, data: await repo.getDevice(q, fresh.device_id) });
    }
    res.json({ alert: fresh });
  });

  // ---- SSE ----
  api.get('/stream', caregiverAuth, async (req, res) => {
    const ids = new Set(await repo.caregiverDeviceIds(q, cg(res).id));
    res.set({ 'Content-Type': 'text/event-stream', 'Cache-Control': 'no-cache, no-transform', Connection: 'keep-alive', 'X-Accel-Buffering': 'no' });
    res.flushHeaders();
    let seq = 0;
    res.write('retry: 3000\n\n');
    const unsub = bus.subscribe((m) => {
      if (!ids.has(m.deviceId)) return;
      res.write(`id: ${++seq}\nevent: ${m.kind}\ndata: ${JSON.stringify(m.data)}\n\n`);
    });
    const ping = setInterval(() => res.write(': ping\n\n'), 15_000);
    req.on('close', () => { clearInterval(ping); unsub(); });
  });

  app.use('/api/v1', api);
  app.get('/health', (_req, res) => { res.json({ ok: true }); });
  app.use('/api/v1', (_req, _res, next) => next(new HttpError(404, 'Not found')));

  app.use((err: any, _req: Request, res: Response, _next: NextFunction) => {
    if (res.headersSent) return res.end();
    if (err instanceof HttpError) return res.status(err.status).json({ error: err.message });
    if (err?.type === 'entity.parse.failed' || err?.status === 400) return res.status(400).json({ error: 'Invalid JSON' });
    console.error(err);
    res.status(500).json({ error: 'Internal server error' });
  });
  return app;
}
