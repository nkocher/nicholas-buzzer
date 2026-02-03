# Nicholas Buzzer

Two-board ESP32 buzzer system with a PWA web interface. PlatformIO project, Arduino framework.

## Architecture

- **Buzzer board** (`src/buzzer_board/main.cpp`) — ESP32 #1: async HTTP server, WebSocket server, buzzer, LEDs, dismiss button, DS18B20 temp sensor. Static IP (192.168.0.200).
- **Sender board** (`src/sender_board/main.cpp`) — ESP32 #2: physical buzz button, RGB LED, WebSocket client. DHCP.
- **Shared config** (`include/config.h`) — pin definitions, timing constants, network config.
- **Secrets** (`include/secrets.h`) — WiFi credentials, gitignored. See `secrets.h.example`.

## Build & Flash

- `pio run -e buzzer_board` — compile buzzer board
- `pio run -e sender_board` — compile sender board
- `pio run -e buzzer_board -t upload` — flash buzzer board
- `pio run -e sender_board -t upload` — flash sender board

## Code Patterns

- Embedded HTML/CSS/JS in PROGMEM raw string literals — the webapp lives inside main.cpp
- Non-blocking loop() — never use delay() for sensor reads or periodic tasks, use millis()-based timers
- WebSocket text protocol: short messages like `"buzz"`, `"dismiss"`, `"temp:72.3"`
- Sender board ignores unknown WS messages by checking `length == 7` for "dismiss" — new message types are safe to add without sender changes
- State machine pattern: `enum State { IDLE, BUZZING, DISMISSED }` with `enterState()` helper
- Dismiss button uses sustained-LOW debounce (30ms) — `digitalRead()` must return LOW for 30 consecutive ms before firing. Any pin change resets the timer. No arm guard — the 30ms sustained check is sufficient; arm guards cause double-press UX issues. AceButton was removed because its debounce silently dropped presses on marginal contacts
- Song strings in `include/songs.h` are PROGMEM (flash); parsed at boot by `parseSongDefs()` with a heap buffer (`STR_BUF_SZ`, currently 6144). Songs longer than this are silently skipped. Check with `curl http://192.168.0.200/songs.json | python3 -c "import json,sys;print(len(json.load(sys.stdin)))"` after flashing.
- Song indices are `uint16_t` throughout (arrays sized 384, NVS uses `putUShort`/`getUShort`). Never use `uint8_t` for melody indices — it silently truncates above 255, corrupting shuffle state.

## Testing

- **Chrome DevTools MCP** — navigate to `http://192.168.0.200` to verify the PWA. The status dot is `#status`; class `ok` (green, `rgb(56, 161, 105)`) means WebSocket connected, class `err` means disconnected.
- **Brave browser** — Brave supports CDP. Quit Brave fully, then relaunch with `--remote-debugging-port=9222` to use it with Chrome DevTools MCP instead of Chrome.
- **WS message logger** — inject via `evaluate_script`: `window._wsLog=[]; origOnMsg=sock.onmessage; sock.onmessage=function(e){window._wsLog.push({time:Date.now(),data:e.data});origOnMsg.call(this,e);}` then poll `_wsLog` to diagnose timing issues.

## Gotchas

- **ESP32 heap pressure** — each parsed song mallocs a note array (~100-256 notes × 4 bytes) that persists forever. At ~253 songs (237 songDefs + 16 hardcoded) the board is at its heap limit. Songs with longer MML strings produce more notes and consume more heap. Adding songs beyond this causes `parseSongDefs()` malloc failures — the board boots and serves HTTP but `MELODY_COUNT` stays 0. Always verify with `curl http://192.168.0.200/songs.json | python3 -c "import json,sys;print(len(json.load(sys.stdin)))"` after flashing. Allow 25-30s boot time before checking — parsing 237 songs takes significant time.
- **Song parse buffer** — `parseSongDefs()` in main.cpp silently skips songs whose PROGMEM string exceeds `STR_BUF_SZ`. If songs go missing after adding new ones, check string lengths vs buffer size before assuming a bug elsewhere.
- **BUZZER_TIMEOUT** — hard stop for melody playback (currently 120s). If songs cut out before finishing, check this value in `config.h` before looking at melody logic. `MELODY_LOOP_PAUSE_MS` only controls the gap between loops.
- **iOS Safari WebSocket reconnection** — `onerror` → `sock.close()` does not reliably fire `onclose` for sockets that never reached OPEN state. Always schedule reconnection directly from both `onerror` and `onclose`. The fix: call `scheduleReconnect()` from both `onerror` and `onclose`, guard with a `reconnectTimer` to prevent duplicates, and never call `sock.close()` from inside `onerror`.
- **iOS standalone PWA backgrounding** — iOS kills WebSocket connections when a standalone PWA is backgrounded. Use `visibilitychange` to detect and reconnect.
- **iOS standalone PWA cold starts** — Standalone PWAs always cold-start on iOS; first-load reliability is critical since there's no "reload" option.
- **No serial monitor from Claude** — `pio device monitor` requires an interactive TTY (termios); reading `/dev/cu.usbserial-*` via `cat`/`stty` crashes the non-interactive shell. Never attempt raw serial reads.
- **IDE diagnostics are false positives** — PlatformIO headers won't resolve in VS Code's default C++ intellisense. Build with `pio run` to verify correctness.
- **secrets.h is required** — build will fail without it. Copy `secrets.h.example` to `secrets.h` and fill in WiFi credentials.
- **ESP32 dual-core race in `enterState()`** — ESPAsyncWebServer runs WS callbacks on core 0; Arduino `loop()` runs on core 1. `enterState()` sets `state` before the switch body executes, so `loop()` can observe intermediate state (e.g. `state == BUZZING` before `startMelody()` sets `player.playing = true`). Guard with `STATE_SETTLE_MS` (200ms) in `loop()` before checking flags set inside the switch body. Do not pre-set `player.playing` before `state = s` -- `updateMelody()` would dereference `player.melody` (still null/stale) and crash.
- **Button GPIO noise during buzzer playback** — single-read edge detection on button GPIOs causes false triggers during buzzer playback. Buzzer vibration, ground bounce, and capacitive coupling from the PWM pin can produce transient LOW readings. Always require sustained readings (30ms+) for debounce, not single-sample edge detection.
- **Arduino String heap corruption** — building large JSON responses with Arduino `String` concatenation causes heap corruption at ~250+ entries (garbled first ~48 bytes). Use `AsyncResponseStream` (chunked response) instead: `request->beginResponseStream("application/json")` with `response->print()` calls. Never build large responses as a single `String`.
- **Boot timing after flash** — the board needs 25-30s after flash to connect to WiFi and parse all songs. Checking `songs.json` before parsing completes returns `content-length: 0` (looks like a crash). Always `sleep 25` before curl verification.
- **PlatformIO LDF** — `[env]` lib_deps are shared across all boards but LDF `chain` mode only links libraries that are `#include`d. Safe to remove an `#include` from one board without touching platformio.ini.
