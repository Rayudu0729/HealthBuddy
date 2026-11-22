import dotenv from 'dotenv';
dotenv.config();

const e = process.env;
export const config = {
  port: Number(e.PORT || 3000),
  databaseUrl: e.DATABASE_URL || 'postgres://bindu:bindu@localhost:5432/bindu',
  sessionSecret: e.SESSION_SECRET || 'dev-only-insecure-secret',
  dashboardOrigin: e.DASHBOARD_ORIGIN || 'http://localhost:5173',
  httpsmsApiKey: e.HTTPSMS_API_KEY || '',
  httpsmsFrom: e.HTTPSMS_FROM || '',
  caregiverSmsTo: e.CAREGIVER_SMS_TO || '',
};
if (e.NODE_ENV === 'production' && !e.SESSION_SECRET) {
  throw new Error('SESSION_SECRET must be set in production');
}
export type Config = typeof config;
