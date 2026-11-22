import { Link } from 'react-router-dom'
import { StatusBadge } from '../components/StatusBadge'
import { useLive } from '../live'
import { computeStatus, timeAgo } from '../lib/time'
import { useNow } from '../lib/useNow'

export default function DeviceList() {
  const { devices, alerts, loaded, error, refresh } = useLive()
  const now = useNow()

  return (
    <section>
      <h2>Devices</h2>
      {error && (
        <p className="error">
          {error} <button className="secondary" onClick={() => void refresh()}>Retry</button>
        </p>
      )}
      {!loaded && <p className="muted">Loading…</p>}
      {loaded && !error && devices.length === 0 && <p className="muted">No devices are assigned to you yet.</p>}
      <ul className="list">
        {devices.map((d) => {
          const mine = alerts.filter((a) => a.device_id === d.id)
          const open = mine.length > 0 ? mine.filter((a) => a.state === 'open').length : d.open_alerts
          return (
            <li key={d.id}>
              <Link to={`/devices/${encodeURIComponent(d.id)}`} className="card device-row">
                <div>
                  <strong>{d.name || d.id}</strong>
                  <div className="muted">{d.id}</div>
                  <div className="muted">Last update: {timeAgo(d.last_seen, now)}</div>
                </div>
                <div className="right">
                  <StatusBadge status={computeStatus(d.last_seen, now)} />
                  {open > 0 && <span className="badge alerts">{open} open alert{open > 1 ? 's' : ''}</span>}
                </div>
              </Link>
            </li>
          )
        })}
      </ul>
    </section>
  )
}
