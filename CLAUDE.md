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

## Testing

- **Chrome DevTools MCP** — navigate to `http://192.168.0.200` to verify the PWA. The status dot is `#status`; class `ok` (green, `rgb(56, 161, 105)`) means WebSocket connected, class `err` means disconnected.
- **Brave browser** — Brave supports CDP. Quit Brave fully, then relaunch with `--remote-debugging-port=9222` to use it with Chrome DevTools MCP instead of Chrome.

## Gotchas

- **iOS Safari WebSocket reconnection** — `onerror` → `sock.close()` does not reliably fire `onclose` for sockets that never reached OPEN state. Always schedule reconnection directly from both `onerror` and `onclose`. The fix: call `scheduleReconnect()` from both `onerror` and `onclose`, guard with a `reconnectTimer` to prevent duplicates, and never call `sock.close()` from inside `onerror`.
- **iOS standalone PWA backgrounding** — iOS kills WebSocket connections when a standalone PWA is backgrounded. Use `visibilitychange` to detect and reconnect.
- **iOS standalone PWA cold starts** — Standalone PWAs always cold-start on iOS; first-load reliability is critical since there's no "reload" option.
- **No serial monitor from Claude** — `pio device monitor` requires an interactive TTY (termios); reading `/dev/cu.usbserial-*` via `cat`/`stty` crashes the non-interactive shell. Never attempt raw serial reads.
- **IDE diagnostics are false positives** — PlatformIO headers won't resolve in VS Code's default C++ intellisense. Build with `pio run` to verify correctness.
- **secrets.h is required** — build will fail without it. Copy `secrets.h.example` to `secrets.h` and fill in WiFi credentials.
