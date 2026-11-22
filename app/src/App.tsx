import { Navigate, Route, Routes } from 'react-router-dom'
import { useAuth } from './auth'
import { Header } from './components/Header'
import { LiveProvider } from './live'
import DeviceList from './pages/DeviceList'
import DevicePage from './pages/DevicePage'
import Login from './pages/Login'

export default function App() {
  const { caregiver, loading } = useAuth()
  if (loading) return <p className="muted center">Loading…</p>
  if (!caregiver) return <Login />
  return (
    <LiveProvider>
      <Header />
      <main className="container">
        <Routes>
          <Route path="/" element={<DeviceList />} />
          <Route path="/devices/:deviceId" element={<DevicePage />} />
          <Route path="*" element={<Navigate to="/" replace />} />
        </Routes>
      </main>
    </LiveProvider>
  )
}
