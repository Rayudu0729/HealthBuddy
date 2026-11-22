import request from 'supertest';
import { beforeEach, describe, expect, it } from 'vitest';
import { MAX_ATTEMPTS, processOutboxOnce } from '../src/outbox';
import type { SmsAdapter } from '../src/sms';
import { ev, setup } from './helpers';

type Ctx = Awaited<ReturnType<typeof setup>>;
let c: Ctx;
let tokenA: string;

const post = (token: string | null, body: unknown) => {
  const r = request(c.app).post('/api/v1/events');
  if (token) r.set('Authorization', 'Bearer ' + token);
  return r.send(body as object);
};
async function login(email: string, password = 'pw12345') {
  const agent = request.agent(c.app);
  const res = await agent.post('/api/v1/auth/login').send({ email, password });
  expect(res.status).toBe(200);
  return agent;
}

beforeEach(async () => {
  c = await setup();
  tokenA = await c.addDevice('dev-a');
  await c.addCaregiver('a@x.io', ['dev-a']);
});

describe('device ingestion', () => {
  it('rejects missing, bad and revoked tokens with 401', async () => {
    expect((await post(null, ev('HEARTBEAT'))).status).toBe(401);
    expect((await post('nope', ev('HEARTBEAT'))).status).toBe(401);
    await c.q('UPDATE device_tokens SET revoked_at = $1', [new Date()]);
    expect((await post(tokenA, ev('HEARTBEAT'))).status).toBe(401);
  });

  it('validates the body with 400', async () => {
    expect((await post(tokenA, { type: 'SOS' })).status).toBe(400);
    expect((await post(tokenA, ev('NOPE'))).status).toBe(400);
    expect((await post(tokenA, ev('SOS', { ts: 'yesterday' }))).status).toBe(400);
  });

  it('creates 201 then dedupes with 200 and a single row', async () => {
    const body = ev('SOS');
    const r1 = await post(tokenA, body);
    expect(r1.status).toBe(201);
    expect(r1.body.event.source).toBe('device');
    expect(r1.body.alert.state).toBe('open');
    const r2 = await post(tokenA, body);
    expect(r2.status).toBe(200);
    expect(r2.body.event.id).toBe(r1.body.event.id);
    expect(r2.body.alert.id).toBe(r1.body.alert.id);
    expect((await c.q('SELECT * FROM events')).rows).toHaveLength(1);
    expect((await c.q('SELECT * FROM alerts')).rows).toHaveLength(1);
  });

  it('updates last_seen on heartbeat and creates no alert', async () => {
    const r = await post(tokenA, ev('HEARTBEAT', { source: 'simulated' }));
    expect(r.status).toBe(201);
    expect(r.body.alert).toBeNull();
    expect(r.body.event.source).toBe('simulated');
    const agent = await login('a@x.io');
    const d = (await agent.get('/api/v1/devices/dev-a')).body.device;
    expect(d.status).toBe('online');
  });

  it('FALL_CANCELLED cancels the open FALL without sending SMS again', async () => {
    await post(tokenA, ev('FALL'));
    await post(tokenA, ev('FALL_CANCELLED'));
    const agent = await login('a@x.io');
    const alerts = (await agent.get('/api/v1/devices/dev-a/alerts')).body.alerts;
    expect(alerts[0].state).toBe('cancelled');
    expect((await c.q('SELECT * FROM sms_outbox')).rows).toHaveLength(1);
  });
});

describe('caregiver access', () => {
  it('requires login and enforces ownership', async () => {
    expect((await request(c.app).get('/api/v1/devices')).status).toBe(401);
    await c.addDevice('dev-b');
    await c.addCaregiver('b@x.io', ['dev-b']);
    const alertId = (await post(tokenA, ev('SOS'))).body.alert.id;

    const b = await login('b@x.io');
    expect((await b.get('/api/v1/devices')).body.devices.map((d: any) => d.id)).toEqual(['dev-b']);
    expect((await b.get('/api/v1/devices/dev-a')).status).toBe(404);
    expect((await b.get('/api/v1/devices/dev-a/events')).status).toBe(404);
    expect((await b.get('/api/v1/devices/dev-a/alerts')).status).toBe(404);
    expect((await b.post('/api/v1/alerts/' + alertId + '/acknowledge')).status).toBe(404);

    const a = await login('a@x.io');
    expect((await a.get('/api/v1/devices')).body.devices[0].open_alerts).toBe(1);
  });

  it('rejects wrong password and supports logout', async () => {
    const res = await request(c.app).post('/api/v1/auth/login').send({ email: 'a@x.io', password: 'bad' });
    expect(res.status).toBe(401);
    const a = await login('a@x.io');
    expect((await a.get('/api/v1/auth/me')).body.caregiver.email).toBe('a@x.io');
    expect((await a.post('/api/v1/auth/logout')).status).toBe(204);
    expect((await a.get('/api/v1/auth/me')).status).toBe(401);
  });

  it('acknowledges an alert (idempotent) recording who and when', async () => {
    const alertId = (await post(tokenA, ev('FALL'))).body.alert.id;
    const a = await login('a@x.io');
    const r = await a.post('/api/v1/alerts/' + alertId + '/acknowledge');
    expect(r.status).toBe(200);
    expect(r.body.alert.state).toBe('acknowledged');
    expect(r.body.alert.acknowledged_by).toBe('a@x.io');
    expect(r.body.alert.acknowledged_at).toBeTruthy();
    const again = await a.post('/api/v1/alerts/' + alertId + '/acknowledge');
    expect(again.status).toBe(200);
    expect(again.body.alert.acknowledged_at).toBe(r.body.alert.acknowledged_at);
  });
});

describe('sms outbox', () => {
  it('enqueues with the alert and the worker marks it sent', async () => {
    const r = await post(tokenA, ev('SOS'));
    expect(r.body.alert.sms.status).toBe('pending');
    const rows = (await c.q('SELECT * FROM sms_outbox')).rows;
    expect(rows).toHaveLength(1);
    expect(rows[0].content).toContain('Bindu SOS from dev-a');

    expect(await processOutboxOnce(c.outboxOpts)).toBe(1);
    expect(c.sms.sent).toHaveLength(1);
    expect(c.sms.sent[0].to).toBe('+15550001111');
    const a = await login('a@x.io');
    const alert = (await a.get('/api/v1/devices/dev-a/alerts')).body.alerts[0];
    expect(alert.sms.status).toBe('sent');
    expect(alert.sms.attempts).toBe(1);
    expect(alert.sms.sent_at).toBeTruthy();
    expect(await processOutboxOnce(c.outboxOpts)).toBe(0);
  });

  it('retries then fails after max attempts', async () => {
    await post(tokenA, ev('SOS'));
    const failing: SmsAdapter = { send: async () => { throw new Error('gateway down'); } };
    for (let i = 1; i < MAX_ATTEMPTS; i++) {
      await processOutboxOnce({ ...c.outboxOpts, sms: failing });
      const row = (await c.q('SELECT * FROM sms_outbox')).rows[0];
      expect(row.status).toBe('pending');
      expect(row.attempts).toBe(i);
    }
    await processOutboxOnce({ ...c.outboxOpts, sms: failing });
    const row = (await c.q('SELECT * FROM sms_outbox')).rows[0];
    expect(row.status).toBe('failed');
    expect(row.attempts).toBe(MAX_ATTEMPTS);
    expect(row.last_error).toContain('gateway down');
    expect(await processOutboxOnce({ ...c.outboxOpts, sms: failing })).toBe(0);
  });

  it('queues nothing and reports status none when no caregiver number is set', async () => {
    c = await setup({ smsTo: '' });
    const t = await c.addDevice('dev-z');
    const r = await post(t, ev('SOS'));
    expect(r.body.alert.sms.status).toBe('none');
    expect((await c.q('SELECT * FROM sms_outbox')).rows).toHaveLength(0);
  });
});
