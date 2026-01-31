# Nicholas Buzzer

A two-board ESP32 buzzer system with a mobile-friendly web interface. One board hosts a WebSocket server that drives a physical buzzer and LEDs; the other board acts as a wireless trigger with its own button and status LEDs. Any device on the local network can also trigger the buzzer through the PWA served by the first board.

## Architecture

```
┌─────────────────────┐         WebSocket          ┌─────────────────────┐
│   Sender Board      │ ◄──────────────────────►   │   Buzzer Board      │
│   (ESP32 #2)        │        ws://x.x.x.x/ws     │   (ESP32 #1)        │
│                     │                             │                     │
│  [Buzz Button]      │                             │  [Buzzer Speaker]   │
│  [Red/Green LEDs]   │                             │  [Red LED]          │
│                     │                             │  [Green LED]        │
└─────────────────────┘                             │  [Dismiss Button]   │
                                                    │                     │
                                                    │  HTTP :80 (PWA)     │
┌─────────────────────┐         WebSocket           │  WebSocket :80      │
│   Phone / Browser   │ ◄──────────────────────►    │                     │
│   (PWA)             │        ws://x.x.x.x/ws     └─────────────────────┘
└─────────────────────┘
```

## Wiring

### Buzzer Board (ESP32 #1)

| Component        | GPIO |
|------------------|------|
| Buzzer           | 25   |
| Dismiss Button   | 26   |
| Red LED          | 32   |
| Green LED        | 33   |

- Dismiss button wired with internal pull-up (active LOW)
- LEDs wired with appropriate current-limiting resistors

### Sender Board (ESP32 #2)

| Component        | GPIO |
|------------------|------|
| Buzz Button      | 18   |
| RGB Red          | 16   |
| RGB Green        | 17   |

- Button is an HW-483 module with onboard pull-down (active HIGH)

## WebSocket Protocol

All messages are plain text over `ws://<buzzer-ip>/ws`.

| Message     | Direction              | Meaning                     |
|-------------|------------------------|-----------------------------|
| `buzz`      | Client → Buzzer Board  | Trigger the buzzer          |
| `dismiss`   | Buzzer Board → Clients | Buzzer was dismissed        |

## State Behavior

### Buzzer Board

| State       | Buzzer | Red LED          | Green LED | Transition                              |
|-------------|--------|------------------|-----------|-----------------------------------------|
| `IDLE`      | OFF    | OFF              | OFF       | → `BUZZING` on `buzz` message           |
| `BUZZING`   | ON     | Flashing (500ms) | OFF       | → `DISMISSED` on button press or 10s timeout |
| `DISMISSED` | OFF    | OFF              | ON        | → `IDLE` after 5s                       |

### Sender Board

| State       | Red LED | Green LED | Transition                                  |
|-------------|---------|-----------|---------------------------------------------|
| `IDLE`      | OFF     | OFF       | → `BUZZING` on button press                 |
| `BUZZING`   | OFF     | ON        | → `DISMISSED` on `dismiss` message          |
| `DISMISSED` | ON      | OFF       | → `IDLE` after 5s                           |

### Web App

| State       | Indicator | Buzz Button | Transition                                  |
|-------------|-----------|-------------|---------------------------------------------|
| `IDLE`      | None      | Enabled     | → `BUZZING` on button tap                   |
| `BUZZING`   | Green     | Disabled    | → `DISMISSED` on `dismiss` message          |
| `DISMISSED` | Red       | Disabled    | → `IDLE` after 5s                           |

## Build Instructions

### Prerequisites

- [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html)
- Two ESP32 dev boards

### Setup

1. Copy the secrets template and fill in your WiFi credentials:

   ```sh
   cp include/secrets.h.example include/secrets.h
   # edit include/secrets.h with your SSID and password
   ```

2. Review `include/config.h` and adjust the static IP, gateway, subnet, or pin assignments if needed.

### Flash

```sh
# Buzzer board (ESP32 #1)
pio run -e buzzer_board -t upload

# Sender board (ESP32 #2)
pio run -e sender_board -t upload
```

### Monitor

```sh
pio device monitor -e buzzer_board
pio device monitor -e sender_board
```

## Photos

<!-- Add photos of your assembled boards here -->
