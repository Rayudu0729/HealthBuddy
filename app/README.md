# Bindu caregiver dashboard (Member 1)

## Selected stack — implementation pending

- React and TypeScript, built with Vite.
- Responsive web interface for caregiver phones and laptops; static frontend container deployed to the existing Kubernetes VM.
- Backend-managed caregiver sign-in and authenticated API access; Server-Sent Events for live device state and alert updates.
- In-dashboard alert banners and acknowledgement history while the dashboard is open and connected. Background push delivery is not included in the first version.

The backend enforces caregiver/device ownership. Caregivers can read assigned devices and acknowledge alerts but cannot forge device telemetry. SMS gateway credentials remain on the server. Device provisioning and caregiver assignments are maintained through a trusted administration workflow. Display last-update time, unknown/stale readings and device connectivity explicitly. Do not present mock or missing readings as live measurements.

Use open-source frontend dependencies and the existing infrastructure; no Firebase subscription is required. SMS alerts are sent by the backend through the selected HTTP gateway, subject to its connectivity and credit limits. Local watch warnings work independently. Report stale/disconnected dashboard state and support reconnecting the live event stream.

Start with a web dashboard against mock data; switch to live data once the wearable syncs.

## Screens
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
