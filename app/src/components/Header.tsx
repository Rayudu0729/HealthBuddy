import { Link } from 'react-router-dom'
import { useAuth } from '../auth'
import { ConnectionIndicator } from './ConnectionIndicator'

export function Header() {
  const { caregiver, logout } = useAuth()
  return (
    <header className="header">
      <Link to="/" className="brand">
        Bindu
      </Link>
      <div className="header-right">
        <ConnectionIndicator />
        <span className="who">{caregiver?.name ?? caregiver?.email}</span>
        <button className="secondary" onClick={() => void logout()}>
          Log out
        </button>
      </div>
    </header>
  )
}
