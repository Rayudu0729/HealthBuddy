import type { DeviceStatus } from '../types'

export const ONLINE_MS = 2 * 60_000
export const STALE_MS = 10 * 60_000

/** Status computed client-side so it degrades without new messages. */
export function computeStatus(lastSeen: string | null, now: number): DeviceStatus {
  if (!lastSeen) return 'offline'
  const t = Date.parse(lastSeen)
  if (Number.isNaN(t)) return 'offline'
  const age = now - t
  if (age < ONLINE_MS) return 'online'
  if (age < STALE_MS) return 'stale'
  return 'offline'
}

export function timeAgo(iso: string | null, now: number): string {
  if (!iso) return 'No data yet'
  const t = Date.parse(iso)
  if (Number.isNaN(t)) return 'unknown'
  const s = Math.max(0, Math.floor((now - t) / 1000))
  if (s < 60) return `${s} s ago`
  const m = Math.floor(s / 60)
  if (m < 60) return `${m} min ago`
  const h = Math.floor(m / 60)
  if (h < 48) return `${h} h ago`
  return `${Math.floor(h / 24)} d ago`
}

export function fmtTime(iso: string | null): string {
  if (!iso) return '—'
  const d = new Date(iso)
  return Number.isNaN(d.getTime()) ? '—' : d.toLocaleString()
}
