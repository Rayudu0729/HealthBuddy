import type { DeviceStatus } from '../types'

export function StatusBadge({ status }: { status: DeviceStatus }) {
  return <span className={`badge status-${status}`}>{status}</span>
}

export function SimulatedBadge({ source }: { source: string }) {
  return source === 'simulated' ? <span className="badge simulated">SIMULATED</span> : null
}
