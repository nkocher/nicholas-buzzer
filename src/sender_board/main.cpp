#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <AceButton.h>
#include "config.h"

using namespace ace_button;

// ---------- state ----------
enum State { IDLE, BUZZING, DISMISSED };
State state = IDLE;
unsigned long stateEnteredAt = 0;
unsigned long lastWifiCheck = 0;

// ---------- peripherals ----------
WebSocketsClient ws;
AceButton buzzBtn(PIN_BUZZ_BTN);

// ---------- helpers ----------
void enterState(State s) {
    state = s;
    stateEnteredAt = millis();

    switch (s) {
    case IDLE:
        digitalWrite(PIN_RGB_RED, LOW);
        digitalWrite(PIN_RGB_GREEN, LOW);
        break;
    case BUZZING:
        digitalWrite(PIN_RGB_RED, LOW);
        digitalWrite(PIN_RGB_GREEN, HIGH);
        break;
    case DISMISSED:
        digitalWrite(PIN_RGB_GREEN, LOW);
        digitalWrite(PIN_RGB_RED, HIGH);
        break;
    }
}

// ---------- callbacks ----------
void handleButtonEvent(AceButton*, uint8_t eventType, uint8_t) {
    if (eventType == AceButton::kEventPressed && state == IDLE) {
        ws.sendTXT("buzz");
        enterState(BUZZING);
        Serial.println("Buzz sent");
    }
}

void onWsEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_CONNECTED:
        Serial.println("[WS] Connected to server");
        break;
    case WStype_DISCONNECTED:
        Serial.println("[WS] Disconnected");
        break;
    case WStype_TEXT:
        if (length == 7 && memcmp(payload, "dismiss", 7) == 0) {
            Serial.println("[WS] Received dismiss");
            enterState(DISMISSED);
        }
        break;
    default:
        break;
    }
}

// ---------- setup ----------
void setup() {
    Serial.begin(115200);

    // pins
    pinMode(PIN_RGB_RED, OUTPUT);
    pinMode(PIN_RGB_GREEN, OUTPUT);
    pinMode(PIN_BUZZ_BTN, INPUT);  // HW-483 has onboard pull-down
    digitalWrite(PIN_RGB_RED, LOW);
    digitalWrite(PIN_RGB_GREEN, LOW);

    // button — HW-483 is LOW when released, HIGH when pressed
    ButtonConfig* cfg = buzzBtn.getButtonConfig();
    cfg->setEventHandler(handleButtonEvent);
    cfg->setFeature(ButtonConfig::kFeatureClick);

    // WiFi (DHCP)
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi connected — IP: %s\n", WiFi.localIP().toString().c_str());

    // WebSocket client
    IPAddress serverIP(BUZZER_IP);
    ws.begin(serverIP.toString(), SERVER_PORT, "/ws");
    ws.onEvent(onWsEvent);
    ws.setReconnectInterval(WS_RECONNECT_MS);
    Serial.println("WebSocket client started");
}

// ---------- loop ----------
void loop() {
    ws.loop();
    buzzBtn.check();

    // dismissed → idle after timeout
    if (state == DISMISSED && millis() - stateEnteredAt >= DISMISSED_DURATION) {
        enterState(IDLE);
    }

    // WiFi reconnect check
    if (millis() - lastWifiCheck >= WIFI_CHECK_INTERVAL) {
        lastWifiCheck = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi lost, reconnecting...");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        }
    }
}
