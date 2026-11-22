import { useLive } from '../live'

export function ConnectionIndicator() {
  const { connection } = useLive()
  return (
    <span className={`conn conn-${connection}`} role="status">
      <span className="dot" /> {connection}
    </span>
  )
}
