#pragma once

#include "secrets.h"  // WIFI_SSID, WIFI_PASS (gitignored)

// ESP32 #1 static IP
#define BUZZER_IP 192, 168, 0, 200
#define GATEWAY   192, 168, 0, 1
#define SUBNET    255, 255, 255, 0

// Port (HTTP + WebSocket on same port for iOS standalone compatibility)
#define SERVER_PORT 80

// ESP32 #1 pins (buzzer board)
#define PIN_BUZZER       25
#define PIN_DISMISS_BTN  26
#define PIN_LED_RED      32
#define PIN_LED_GREEN    33

// ESP32 #2 pins (sender board)
#define PIN_BUZZ_BTN     18
#define PIN_RGB_RED      16
#define PIN_RGB_GREEN    17

// Timing
#define FLASH_INTERVAL_MS    500
#define DISMISSED_DURATION   5000
#define WS_RECONNECT_MS      5000
#define BUZZER_TIMEOUT       10000
#define WIFI_CHECK_INTERVAL  10000
