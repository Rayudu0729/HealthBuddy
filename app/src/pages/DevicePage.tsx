import { Link, useParams } from 'react-router-dom'
import { AlertBanner } from '../components/AlertBanner'
import { SimulatedBadge, StatusBadge } from '../components/StatusBadge'
import { useLive } from '../live'
import { computeStatus, fmtTime, timeAgo } from '../lib/time'
import { useNow } from '../lib/useNow'
import type { Alert } from '../types'

function alertState(a: Alert): string {
  if (a.state === 'open') return 'open'
  if (a.state === 'cancelled') return 'cancelled'
  return `acknowledged by ${a.acknowledged_by ?? 'unknown'} at ${fmtTime(a.acknowledged_at)}`
}

function smsText(a: Alert): string {
  const s = a.sms
  if (!s || s.status === 'none') return 'none'
  const n = `${s.attempts} attempt${s.attempts === 1 ? '' : 's'}`
  return `${s.status} (${n})${s.status === 'failed' && s.last_error ? ` – ${s.last_error}` : ''}`
}

const byTimeDesc = (a: string | null, b: string | null) => (Date.parse(b ?? '') || 0) - (Date.parse(a ?? '') || 0)

export default function DevicePage() {
  const { deviceId = '' } = useParams()
  const { devices, events, alerts, loaded, error, refresh, upsertAlert } = useLive()
  const now = useNow()

  const device = devices.find((d) => d.id === deviceId)
  const devAlerts = alerts.filter((a) => a.device_id === deviceId).sort((a, b) => byTimeDesc(a.created_at, b.created_at))
  const devEvents = events
    .filter((e) => e.device_id === deviceId)
    .sort((a, b) => byTimeDesc(a.received_at, b.received_at))
    .slice(0, 50)
  const open = devAlerts.filter((a) => a.state === 'open')

  return (
    <section>
      <p>
        <Link to="/">← Devices</Link>
      </p>
      {error && (
        <p className="error">
          {error} <button className="secondary" onClick={() => void refresh()}>Retry</button>
        </p>
      )}
      {!loaded && <p className="muted">Loading…</p>}
      {loaded && !device && !error && <p className="muted">Device not found.</p>}
      {device && (
        <>
          <h2>
            {device.name || device.id} <StatusBadge status={computeStatus(device.last_seen, now)} />
          </h2>
          <p className="muted">Last update: {timeAgo(device.last_seen, now)}</p>

          {open.map((a) => (
            <AlertBanner key={a.id} alert={a} onAcknowledged={upsertAlert} />
          ))}

          <h3>Alert history</h3>
          {devAlerts.length === 0 ? (
            <p className="muted">No alerts recorded.</p>
          ) : (
            <div className="table-wrap">
              <table>
                <thead>
                  <tr>
                    <th>Type</th>
                    <th>Time</th>
                    <th>State</th>
                    <th>SMS</th>
                  </tr>
                </thead>
                <tbody>
                  {devAlerts.map((a) => (
                    <tr key={a.id}>
                      <td>
                        <span className={`badge type-${a.type}`}>{a.type}</span> <SimulatedBadge source={a.source} />
                      </td>
                      <td>{fmtTime(a.created_at)}</td>
                      <td>{alertState(a)}</td>
                      <td>{smsText(a)}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}

          <h3>Recent events</h3>
          {devEvents.length === 0 ? (
            <p className="muted">No events yet.</p>
          ) : (
            <div className="table-wrap">
              <table>
                <thead>
                  <tr>
                    <th>Type</th>
                    <th>Value</th>
                    <th>Mode</th>
                    <th>Device time</th>
                  </tr>
                </thead>
                <tbody>
                  {devEvents.map((e) => (
                    <tr key={e.id}>
                      <td>
                        {e.type} <SimulatedBadge source={e.source} />
                      </td>
                      <td>{e.value ?? '—'}</td>
                      <td>{e.mode ?? 'unknown'}</td>
                      <td>{fmtTime(e.device_ts)}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </>
      )}
    </section>
  )
}
