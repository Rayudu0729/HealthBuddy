# Bindu caregiver dashboard (Member 1)

Reviewed 13 November 2026. [Current project status](../docs/current-status.md). The caregiver dashboard prototype is implemented here (login, device list, device page with live alerts).

## Selected stack

- React and TypeScript, built with Vite.
- Responsive web interface for caregiver phones and laptops; static frontend container deployed to the existing Kubernetes VM.
- Backend-managed caregiver sign-in and authenticated API access; Server-Sent Events for live device state and alert updates.
- In-dashboard alert banners and acknowledgement history while the dashboard is open and connected. Background push delivery is not included in the first version.

The planned backend must enforce caregiver/device ownership. Caregivers can read assigned devices and acknowledge alerts but cannot forge device telemetry. SMS gateway credentials remain on the server. Device provisioning and caregiver assignments are maintained through a trusted administration workflow. Display last-update time, unknown/stale readings and device connectivity explicitly. Do not present mock or missing readings as live measurements.

Use open-source frontend dependencies and the existing infrastructure; no Firebase subscription is required. SMS alerts will be sent by the backend through the selected HTTP gateway, subject to its connectivity and credit limits. Local watch warnings are intended to work independently; integration is pending. Report stale/disconnected dashboard state and support reconnecting the live event stream.

## Run

1. Start the backend first (see [../backend/README.md](../backend/README.md)); it listens on port 3000.
2. In `app/`: `npm install && npm run dev`, then open http://localhost:5173 (Vite proxies `/api` to `localhost:3000`).
3. Demo login: `demo@bindu.local` / `bindu-demo-123`.
4. Trigger a test alert: `npm run simulate -- SOS` in `backend/`; the banner appears live. Simulated data is badged SIMULATED.

Scripts: `npm run build`, `npm run lint`, `npm test`.

## Live vs stubbed

- Live: login/session, device list with online/stale/offline status (recomputed every second), device page with alert banner and acknowledge, alert history with SMS status, recent events, SSE connection indicator (full refetch on reconnect).
- Not built: vitals, map, medicine log, fall-risk trends, contacts/settings screens. No push notifications; alerts show only while the dashboard is open.

## Planned screens
- Live vitals and map (last known location)
- Alerts and history
- Experimental fall-risk trends; do not present correlations as proven medical causes
- Medicine log
- Caregiver contacts and alert settings
- Watch worn/removed/unknown state, charging exceptions and worn-again history
- Alert acknowledgement and connection status

## References

- [React with Vite and TypeScript](https://react.dev/learn/build-a-react-app-from-scratch)
- [Express API backend](https://expressjs.com/)
