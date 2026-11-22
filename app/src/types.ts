export type DeviceStatus = 'online' | 'stale' | 'offline'

export interface Device {
  id: string
  name: string
  last_seen: string | null
  status: DeviceStatus
  open_alerts: number
}

export interface DeviceEvent {
  id: number
  device_id: string
  event_id: string
  type: string
  value: number | null
  mode: string | null
  source: 'device' | 'simulated'
  device_ts: string | null
  received_at: string
}

export interface Alert {
  id: number
  device_id: string
  event_id: string
  type: 'SOS' | 'FALL'
  value: number | null
  source: 'device' | 'simulated'
  state: 'open' | 'acknowledged' | 'cancelled'
  created_at: string
  acknowledged_at: string | null
  acknowledged_by: string | null
  sms: {
    status: 'pending' | 'sent' | 'failed' | 'none'
    attempts: number
    last_error: string | null
    sent_at: string | null
  }
}

export interface Caregiver {
  id: number | string
  email: string
  name: string
}
