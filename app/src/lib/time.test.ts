import { computeStatus, timeAgo } from './time'

const now = Date.parse('2026-01-01T12:00:00Z')
const ago = (s: number) => new Date(now - s * 1000).toISOString()

describe('computeStatus', () => {
  it('maps age to status', () => {
    expect(computeStatus(ago(30), now)).toBe('online')
    expect(computeStatus(ago(300), now)).toBe('stale')
    expect(computeStatus(ago(700), now)).toBe('offline')
    expect(computeStatus(null, now)).toBe('offline')
  })
})

describe('timeAgo', () => {
  it('formats', () => {
    expect(timeAgo(null, now)).toBe('No data yet')
    expect(timeAgo(ago(5), now)).toBe('5 s ago')
    expect(timeAgo(ago(180), now)).toBe('3 min ago')
  })
})
