import { useState } from 'react'
import { api } from '../lib/api'
import { fmtTime } from '../lib/time'
import type { Alert } from '../types'
import { SimulatedBadge } from './StatusBadge'

interface Props {
  alert: Alert
  onAcknowledged?: (a: Alert) => void
}

export function AlertBanner({ alert, onAcknowledged }: Props) {
  const [busy, setBusy] = useState(false)
  const [err, setErr] = useState<string | null>(null)

  async function ack() {
    setBusy(true)
    setErr(null)
    try {
      const r = await api.acknowledge(alert.id)
      onAcknowledged?.(r.alert)
    } catch (e) {
      setErr(e instanceof Error ? e.message : 'Failed to acknowledge')
    } finally {
      setBusy(false)
    }
  }

  return (
    <div className={`banner banner-${alert.type}`} role="alert">
      <div>
        <strong>{alert.type === 'SOS' ? 'SOS alert' : 'Fall detected'}</strong> <SimulatedBadge source={alert.source} />
        <div className="muted-on-dark">{fmtTime(alert.created_at)}</div>
        {err && <div className="err-on-dark">{err}</div>}
      </div>
      <button onClick={ack} disabled={busy}>
        {busy ? 'Acknowledging…' : 'Acknowledge'}
      </button>
    </div>
  )
}
