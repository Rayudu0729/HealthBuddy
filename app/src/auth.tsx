import { createContext, useCallback, useContext, useEffect, useState, type ReactNode } from 'react'
import { api } from './lib/api'
import type { Caregiver } from './types'

interface AuthCtx {
  caregiver: Caregiver | null
  loading: boolean
  login: (email: string, password: string) => Promise<void>
  logout: () => Promise<void>
}

const Ctx = createContext<AuthCtx | null>(null)

export function AuthProvider({ children }: { children: ReactNode }) {
  const [caregiver, setCaregiver] = useState<Caregiver | null>(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    let live = true
    api
      .me()
      .then((r) => live && setCaregiver(r.caregiver))
      .catch(() => live && setCaregiver(null))
      .finally(() => live && setLoading(false))
    return () => {
      live = false
    }
  }, [])

  const login = useCallback(async (email: string, password: string) => {
    const r = await api.login(email, password)
    setCaregiver(r.caregiver)
  }, [])

  const logout = useCallback(async () => {
    try {
      await api.logout()
    } finally {
      setCaregiver(null)
    }
  }, [])

  return <Ctx.Provider value={{ caregiver, loading, login, logout }}>{children}</Ctx.Provider>
}

export function useAuth(): AuthCtx {
  const c = useContext(Ctx)
  if (!c) throw new Error('useAuth outside AuthProvider')
  return c
}
