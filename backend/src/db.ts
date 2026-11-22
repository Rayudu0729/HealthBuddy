import { Pool } from 'pg';

/** Minimal pool shape so tests can pass a pg-mem pool. */
export type Db = Pick<Pool, 'query' | 'connect'>;

export async function tx<T>(db: Db, fn: (q: Db['query']) => Promise<T>): Promise<T> {
  const client = await db.connect();
  try {
    await client.query('BEGIN');
    const out = await fn(client.query.bind(client) as Db['query']);
    await client.query('COMMIT');
    return out;
  } catch (err) {
    await client.query('ROLLBACK').catch(() => {});
    throw err;
  } finally {
    client.release();
  }
}
