CREATE TABLE devices (
  id text PRIMARY KEY,
  name text NOT NULL,
  last_seen timestamptz,
  created_at timestamptz NOT NULL DEFAULT now()
);

CREATE TABLE device_tokens (
  id serial PRIMARY KEY,
  device_id text NOT NULL REFERENCES devices(id),
  token_hash text NOT NULL UNIQUE,
  created_at timestamptz NOT NULL DEFAULT now(),
  revoked_at timestamptz
);

CREATE TABLE caregivers (
  id serial PRIMARY KEY,
  email text NOT NULL UNIQUE,
  name text NOT NULL,
  password_hash text NOT NULL,
  created_at timestamptz NOT NULL DEFAULT now()
);

CREATE TABLE caregiver_devices (
  caregiver_id integer NOT NULL REFERENCES caregivers(id),
  device_id text NOT NULL REFERENCES devices(id),
  PRIMARY KEY (caregiver_id, device_id)
);

CREATE TABLE events (
  id serial PRIMARY KEY,
  device_id text NOT NULL REFERENCES devices(id),
  event_id text NOT NULL,
  type text NOT NULL,
  value double precision,
  mode text,
  source text NOT NULL DEFAULT 'device',
  device_ts timestamptz NOT NULL,
  received_at timestamptz NOT NULL DEFAULT now(),
  UNIQUE (device_id, event_id)
);
CREATE INDEX events_device_idx ON events (device_id, id);

CREATE TABLE alerts (
  id serial PRIMARY KEY,
  device_id text NOT NULL REFERENCES devices(id),
  event_id text NOT NULL,
  type text NOT NULL,
  value double precision,
  source text NOT NULL DEFAULT 'device',
  state text NOT NULL DEFAULT 'open',
  created_at timestamptz NOT NULL DEFAULT now(),
  acknowledged_at timestamptz,
  acknowledged_by integer REFERENCES caregivers(id),
  UNIQUE (device_id, event_id)
);
CREATE INDEX alerts_device_idx ON alerts (device_id, id);

CREATE TABLE sms_outbox (
  id serial PRIMARY KEY,
  alert_id integer NOT NULL REFERENCES alerts(id),
  to_number text NOT NULL,
  content text NOT NULL,
  status text NOT NULL DEFAULT 'pending',
  attempts integer NOT NULL DEFAULT 0,
  next_attempt_at timestamptz NOT NULL DEFAULT now(),
  last_error text,
  sent_at timestamptz,
  created_at timestamptz NOT NULL DEFAULT now()
);
CREATE INDEX sms_outbox_due_idx ON sms_outbox (status, next_attempt_at);
