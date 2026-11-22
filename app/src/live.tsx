import { createContext, useCallback, useContext, useEffect, useMemo, useRef, useState, type ReactNode } from 'react'
import { api } from './lib/api'
import type { Alert, Device, DeviceEvent } from './types'

export type Connection = 'connected' | 'reconnecting' | 'offline'

interface LiveCtx {
  devices: Device[]
  events: DeviceEvent[]
  alerts: Alert[]
  loaded: boolean
  error: string | null
  connection: Connection
  refresh: () => Promise<void>
  upsertAlert: (a: Alert) => void
}

const Ctx = createContext<LiveCtx | null>(null)

export function upsertById<T extends { id: number | string }>(list: T[], item: T): T[] {
  const i = list.findIndex((x) => x.id === item.id)
  if (i === -1) return [item, ...list]
  const next = list.slice()
  next[i] = item
  return next
}

function parse<T>(m: Event): T | null {
  try {
    return JSON.parse((m as MessageEvent).data as string) as T
  } catch {
    return null
  }
}

export function LiveProvider({ children }: { children: ReactNode }) {
  const [devices, setDevices] = useState<Device[]>([])
  const [events, setEvents] = useState<DeviceEvent[]>([])
  const [alerts, setAlerts] = useState<Alert[]>([])
  const [loaded, setLoaded] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [connection, setConnection] = useState<Connection>('reconnecting')
  const seq = useRef(0)

  const refresh = useCallback(async () => {
    const my = ++seq.current
    try {
      const { devices: ds } = await api.devices()
      const per = await Promise.all(
        ds.map(async (d) => {
          const [e, a] = await Promise.all([api.events(d.id), api.alerts(d.id)])
          return { e: e.events, a: a.alerts }
        }),
      )
      if (my !== seq.current) return
      setDevices(ds)
      setEvents(per.flatMap((p) => p.e))
      setAlerts(per.flatMap((p) => p.a))
      setError(null)
      setLoaded(true)
    } catch (e) {
      if (my !== seq.current) return
      setError(e instanceof Error ? e.message : 'Failed to load data')
      setLoaded(true)
    }
  }, [])

  useEffect(() => {
    let es: EventSource
    let retry: ReturnType<typeof setTimeout> | undefined
    let stopped = false
    const connect = () => {
      es = new EventSource('/api/v1/stream', { withCredentials: true })
      es.onopen = () => {
        setConnection('connected')
        void refresh()
      }
      // The browser retries on its own while CONNECTING; a non-200 reply (e.g. proxy 502
      // while the API restarts) closes the stream for good, so reconnect manually.
      es.onerror = () => {
        if (es.readyState !== EventSource.CLOSED) return setConnection('reconnecting')
        setConnection('offline')
        if (!stopped) retry = setTimeout(connect, 5000)
      }
      listen(es)
    }
    const listen = (es: EventSource) => {
    es.addEventListener('device', (m) => {
      const d = parse<Device>(m)
      if (d) setDevices((l) => upsertById(l, d))
    })
    es.addEventListener('event', (m) => {
      const e = parse<DeviceEvent>(m)
      if (e) setEvents((l) => upsertById(l, e))
    })
    es.addEventListener('alert', (m) => {
      const a = parse<Alert>(m)
      if (a) setAlerts((l) => upsertById(l, a))
    })
    }
    connect()
    return () => {
      stopped = true
      clearTimeout(retry)
      es.close()
    }
  }, [refresh])

  const upsertAlert = useCallback((a: Alert) => setAlerts((l) => upsertById(l, a)), [])

  const value = useMemo(
    () => ({ devices, events, alerts, loaded, error, connection, refresh, upsertAlert }),
    [devices, events, alerts, loaded, error, connection, refresh, upsertAlert],
  )
  return <Ctx.Provider value={value}>{children}</Ctx.Provider>
}

export function useLive(): LiveCtx {
  const c = useContext(Ctx)
  if (!c) throw new Error('useLive outside LiveProvider')
  return c
}
