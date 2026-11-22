import connectPgSimple from 'connect-pg-simple';
import session from 'express-session';
import { Pool } from 'pg';
import { createApp } from './app';
import { Bus } from './bus';
import { config } from './config';
import { startOutboxWorker } from './outbox';
import { FakeSmsAdapter, HttpSmsAdapter } from './sms';

const db = new Pool({ connectionString: config.databaseUrl });
const bus = new Bus();
const useHttp = !!config.httpsmsApiKey;
const sms = useHttp ? new HttpSmsAdapter(config.httpsmsApiKey, config.httpsmsFrom) : new FakeSmsAdapter();
const PgStore = connectPgSimple(session);

const app = createApp({ db, bus, config, sessionStore: new PgStore({ pool: db, tableName: 'session' }) });
const worker = startOutboxWorker({ db, sms, bus });
const server = app.listen(config.port, () => {
  console.log(`Bindu API on :${config.port} (SMS: ${useHttp ? 'httpSMS' : 'fake'})`);
  if (config.sessionSecret === 'dev-only-insecure-secret') console.warn('SESSION_SECRET not set: using insecure dev secret');
});

function shutdown() {
  worker.stop();
  server.close(() => db.end().finally(() => process.exit(0)));
  setTimeout(() => process.exit(0), 3000).unref();
}
process.on('SIGINT', shutdown);
process.on('SIGTERM', shutdown);
