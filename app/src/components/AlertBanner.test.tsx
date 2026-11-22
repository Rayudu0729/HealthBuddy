import { render, screen } from '@testing-library/react'
import userEvent from '@testing-library/user-event'
import { AlertBanner } from './AlertBanner'
import type { Alert } from '../types'

const alert: Alert = {
  id: 7, device_id: 'bindu-001', event_id: 'e1', type: 'SOS', value: null, source: 'simulated',
  state: 'open', created_at: '2026-01-01T12:00:00Z', acknowledged_at: null, acknowledged_by: null,
  sms: { status: 'pending', attempts: 0, last_error: null, sent_at: null },
}

it('renders SOS and acknowledges', async () => {
  const acked = { ...alert, state: 'acknowledged' as const }
  const fetchMock = vi.fn().mockResolvedValue(new Response(JSON.stringify({ alert: acked }), { status: 200 }))
  vi.stubGlobal('fetch', fetchMock)
  const onAck = vi.fn()
  render(<AlertBanner alert={alert} onAcknowledged={onAck} />)
  expect(screen.getByText('SOS alert')).toBeInTheDocument()
  expect(screen.getByText('SIMULATED')).toBeInTheDocument()
  await userEvent.click(screen.getByRole('button', { name: 'Acknowledge' }))
  expect(fetchMock.mock.calls[0][0]).toBe('/api/v1/alerts/7/acknowledge')
  expect(onAck).toHaveBeenCalledWith(acked)
})
