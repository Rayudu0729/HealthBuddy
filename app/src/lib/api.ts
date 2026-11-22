import type { Alert, Caregiver, Device, DeviceEvent } from '../types'

export class ApiError extends Error {
  status: number
  constructor(status: number, message: string) {
    super(message)
    this.status = status
  }
}

async function req<T>(path: string, init?: RequestInit): Promise<T> {
  let res: Response
  try {
    res = await fetch(`/api/v1${path}`, {
      credentials: 'include',
      headers: init?.body ? { 'Content-Type': 'application/json' } : undefined,
      ...init,
    })
  } catch {
    throw new ApiError(0, 'Cannot reach the server')
  }
  if (res.status === 204) return undefined as T
  let body: unknown = null
  try {
    body = await res.json()
  } catch {
    /* no body */
  }
  if (!res.ok) {
    const msg = (body as { error?: string } | null)?.error ?? `Request failed (${res.status})`
    throw new ApiError(res.status, msg)
  }
  return body as T
}

export const api = {
  login: (email: string, password: string) =>
    req<{ caregiver: Caregiver }>('/auth/login', { method: 'POST', body: JSON.stringify({ email, password }) }),
  logout: () => req<void>('/auth/logout', { method: 'POST' }),
  me: () => req<{ caregiver: Caregiver }>('/auth/me'),
  devices: () => req<{ devices: Device[] }>('/devices'),
  device: (id: string) => req<{ device: Device }>(`/devices/${encodeURIComponent(id)}`),
  events: (id: string, limit = 50) =>
    req<{ events: DeviceEvent[] }>(`/devices/${encodeURIComponent(id)}/events?limit=${limit}`),
  alerts: (id: string, limit = 50) =>
    req<{ alerts: Alert[] }>(`/devices/${encodeURIComponent(id)}/alerts?limit=${limit}`),
  acknowledge: (alertId: number) =>
    req<{ alert: Alert }>(`/alerts/${alertId}/acknowledge`, { method: 'POST' }),
}
