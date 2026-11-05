# SIGLA Log Viewer (web app)

A phone web app that downloads SIGLA's saved sessions over Bluetooth and shows
session history, progress charts, and a CSV export for the therapist.
It works in **Chrome on Android**. iPhones do not support Web Bluetooth.

## Files
- `index.html`: the whole app (screens, sample data, Bluetooth, export)
- `manifest.webmanifest`, `sw.js`, `icon-192.png`, `icon-512.png`: let the app be added to the home screen and open without internet after the first visit
- `firmware/sigla_ble_logs.ino`: XIAO ESP32-S3 code that sends saved sessions to the app

## Try it now (no sensor needed)
1. Put this folder online (see below), open it in Chrome on Android.
2. Tap **Load sample data**. All four screens fill with 7 example sessions.
3. Tap **Clear all saved sessions** to remove them before using real data.

## Put it online with GitHub Pages (free)
1. Create a GitHub repository, for example `sigla-app`.
2. Upload every file in this folder (keep `firmware/` too, or leave it out).
3. Repository **Settings > Pages**: Source = `Deploy from a branch`, Branch = `main`, folder `/ (root)`, Save.
4. After a minute, open `https://<your-username>.github.io/sigla-app/` in Chrome on Android.
5. Chrome menu > **Add to Home screen** to install it like an app.

Web Bluetooth needs HTTPS, which GitHub Pages provides.

## When the sensor is ready
1. Upload `firmware/sigla_ble_logs.ino` to the XIAO ESP32-S3 (merge it into your main firmware).
2. Call `saveSession("date,time,minutes,extensor,cocontraction,flexor,rest,faults")` at the end of each training session.
3. In the app, tap **Connect to SIGLA**, choose **SIGLA**, and the sessions download.
4. Tap **Download sessions** any time to fetch new ones. Duplicates are skipped.

## Data format (one session per line)
`2026-11-05,09:10,40,22,7,3,2,1`
= date, start time, minutes, extensor-only reps, co-contraction count,
flexor-dominant count, rest count, signal faults. The device sends `END` after the last line.

## Bluetooth settings (must match the firmware)
- Device name starts with `SIGLA`
- Service `6e400001-b5a3-f393-e0a9-e50e24dcca9e`
- App to device (write `SEND`): `6e400002-b5a3-f393-e0a9-e50e24dcca9e`
- Device to app (notify lines): `6e400003-b5a3-f393-e0a9-e50e24dcca9e`

## Troubleshooting
- **"Bluetooth is not available"**: open the link in Chrome on Android, not in a messaging app's built-in browser.
- **SIGLA does not appear**: turn on Bluetooth and location, and keep the device nearby.
- **Data stays on the phone** (browser storage). Clearing Chrome's site data deletes it, so export a CSV first.
