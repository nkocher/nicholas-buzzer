#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AceButton.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

using namespace ace_button;

// ---------- state ----------
enum State { IDLE, BUZZING, DISMISSED };
State state = IDLE;
unsigned long stateEnteredAt = 0;
unsigned long lastFlashToggle = 0;
bool flashOn = false;
unsigned long lastWifiCheck = 0;

// ---------- temperature sensor ----------
OneWire oneWire(PIN_TEMP_DATA);
DallasTemperature tempSensor(&oneWire);
DeviceAddress tempAddr;
float lastTempF;
bool hasTempReading = false;
bool tempSensorFound = false;
bool tempRequested = false;
unsigned long tempRequestedAt = 0;
unsigned long lastTempRead = 0;

// ---------- peripherals ----------
AsyncWebServer server(SERVER_PORT);
AsyncWebSocket ws("/ws");
AceButton dismissBtn(PIN_DISMISS_BTN);

// ---------- web app ----------
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<meta name="theme-color" content="#111111">
<link rel="manifest" href="/manifest.json">
<link rel="apple-touch-icon" href="/icon.svg">
<title>Nicholas Buzzer</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;display:flex;flex-direction:column;
     align-items:center;justify-content:center;height:100dvh;background:#111;color:#eee;
     -webkit-user-select:none;user-select:none}
#buzz{font-size:2rem;padding:1.2em 2.4em;border:none;border-radius:16px;
      background:#e53e3e;color:#fff;cursor:pointer;touch-action:manipulation;
      -webkit-tap-highlight-color:transparent}
#buzz:active{background:#c53030}
#buzz.off{opacity:.4;cursor:default;pointer-events:none}
#indicator{width:80px;height:80px;border-radius:50%;margin-top:2rem}
.green{background:#38a169}
.red{background:#e53e3e}
#status{position:fixed;top:12px;right:12px;width:10px;height:10px;border-radius:50%;
        background:#e53e3e;transition:background .3s}
#status.ok{background:#38a169}
#temp{position:fixed;top:12px;left:0;right:0;text-align:center;color:#888;font-size:0.85rem}
</style>
</head>
<body>
<div id="temp"></div>
<div id="status"></div>
<button id="buzz">BUZZ</button>
<div id="indicator"></div>
<script>
var state='IDLE',sock=null,timer=null,connected=false,rTimer=null;
var btn=document.getElementById('buzz');
var ind=document.getElementById('indicator');
var dot=document.getElementById('status');
var tmp=document.getElementById('temp');
var SERVER=window.location.hostname;

function ui(){
  btn.className=(state!=='IDLE'||!connected)?'off':'';
  ind.className='';
  if(state==='BUZZING') ind.classList.add('green');
  else if(state==='DISMISSED') ind.classList.add('red');
  dot.className=connected?'ok':'';
}

function doBuzz(){
  if(state!=='IDLE'||!sock||sock.readyState!==1) return;
  sock.send('buzz');
  state='BUZZING';ui();
}

function reconnect(){if(!rTimer)rTimer=setTimeout(function(){rTimer=null;connect();},3000);}
function connect(){
  if(sock){sock.onopen=sock.onclose=sock.onerror=sock.onmessage=null;try{sock.close();}catch(e){}}
  try{sock=new WebSocket('ws://'+SERVER+'/ws');}catch(e){reconnect();return;}
  sock.onopen=function(){connected=true;ui();};
  sock.onclose=function(){connected=false;ui();reconnect();};
  sock.onerror=function(){connected=false;ui();reconnect();};
  sock.onmessage=function(e){
    if(e.data==='dismiss'){
      state='DISMISSED';ui();
      clearTimeout(timer);
      timer=setTimeout(function(){state='IDLE';ui();},5000);
    } else if(e.data.startsWith('temp:')){
      var f=parseFloat(e.data.substring(5));
      if(!isNaN(f)) tmp.textContent='it is currently '+f.toFixed(1)+'\u00B0F in there';
    }
  };
}

btn.addEventListener('click',doBuzz);
btn.addEventListener('touchend',function(e){e.preventDefault();doBuzz();});

document.addEventListener('visibilitychange',function(){
  if(!document.hidden&&(!sock||sock.readyState!==1)){connected=false;ui();reconnect();}
});
connect();ui();
</script>
</body>
</html>
)rawliteral";

static const char MANIFEST_JSON[] PROGMEM = R"rawliteral(
{"name":"Nicholas Buzzer","short_name":"Buzzer","start_url":"/",
"display":"standalone","background_color":"#111111","theme_color":"#e53e3e",
"icons":[{"src":"/icon.svg","sizes":"any","type":"image/svg+xml"}]}
)rawliteral";

static const char ICON_SVG[] PROGMEM = R"rawliteral(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 180 180">
<rect width="180" height="180" rx="36" fill="#e53e3e"/>
<text x="90" y="128" font-size="120" font-family="sans-serif" font-weight="bold"
 text-anchor="middle" fill="#fff">B</text>
</svg>
)rawliteral";

// ---------- helpers ----------
void enterState(State s) {
    state = s;
    stateEnteredAt = millis();

    switch (s) {
    case IDLE:
        digitalWrite(PIN_BUZZER, LOW);
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_GREEN, LOW);
        flashOn = false;
        break;
    case BUZZING:
        digitalWrite(PIN_BUZZER, HIGH);
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, HIGH);
        flashOn = true;
        lastFlashToggle = millis();
        break;
    case DISMISSED:
        digitalWrite(PIN_BUZZER, LOW);
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_GREEN, HIGH);
        ws.textAll("dismiss");
        break;
    }
}

// ---------- callbacks ----------
void handleButtonEvent(AceButton*, uint8_t eventType, uint8_t) {
    if (eventType == AceButton::kEventPressed && state == BUZZING) {
        enterState(DISMISSED);
    }
}

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("[WS] Client #%u connected\n", client->id());
        if (hasTempReading) {
            char buf[16];
            snprintf(buf, sizeof(buf), "temp:%.1f", lastTempF);
            client->text(buf);
        }
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("[WS] Client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA: {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            if (len == 4 && memcmp(data, "buzz", 4) == 0 && state == IDLE) {
                Serial.println("[WS] Received buzz");
                enterState(BUZZING);
            }
        }
        break;
    }
    default:
        break;
    }
}

// ---------- setup ----------
void setup() {
    Serial.begin(115200);

    // pins
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_DISMISS_BTN, INPUT_PULLUP);
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);

    // button
    ButtonConfig* cfg = dismissBtn.getButtonConfig();
    cfg->setEventHandler(handleButtonEvent);

    // WiFi with static IP
    IPAddress ip(BUZZER_IP);
    IPAddress gw(GATEWAY);
    IPAddress sn(SUBNET);
    WiFi.config(ip, gw, sn);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi connected — IP: %s\n", WiFi.localIP().toString().c_str());

    // temperature sensor
    tempSensor.begin();
    tempSensor.setWaitForConversion(false);
    if (tempSensor.getDeviceCount() > 0 && tempSensor.getAddress(tempAddr, 0)) {
        tempSensorFound = true;
        tempSensor.setResolution(tempAddr, 12);
        Serial.printf("DS18B20 found, address: ");
        for (uint8_t i = 0; i < 8; i++) Serial.printf("%02X", tempAddr[i]);
        Serial.println();
        tempSensor.requestTemperatures();
        tempRequested = true;
        tempRequestedAt = millis();
    } else {
        Serial.println("DS18B20 not found — temperature disabled");
    }

    // WebSocket handler
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // HTTP routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", INDEX_HTML);
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        request->send(response);
    });
    server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "application/json", MANIFEST_JSON);
    });
    server.on("/icon.svg", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "image/svg+xml", ICON_SVG);
    });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/icon.svg");
    });
    server.onNotFound([](AsyncWebServerRequest* request) {
        Serial.printf("[HTTP] 404: %s\n", request->url().c_str());
        request->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("HTTP + WebSocket server started on port 80");
}

// ---------- loop ----------
void loop() {
    dismissBtn.check();
    ws.cleanupClients();

    // buzzing: flash red LED + auto-dismiss on timeout
    if (state == BUZZING) {
        unsigned long now = millis();
        if (now - stateEnteredAt >= BUZZER_TIMEOUT) {
            Serial.println("Buzzer timeout, auto-dismissing");
            enterState(DISMISSED);
        } else if (now - lastFlashToggle >= FLASH_INTERVAL_MS) {
            lastFlashToggle = now;
            flashOn = !flashOn;
            digitalWrite(PIN_LED_RED, flashOn ? HIGH : LOW);
        }
    }

    // dismissed → idle after timeout
    if (state == DISMISSED && millis() - stateEnteredAt >= DISMISSED_DURATION) {
        enterState(IDLE);
    }

    // temperature sensor — non-blocking async read
    if (tempSensorFound) {
        unsigned long now = millis();
        if (!tempRequested && now - lastTempRead >= TEMP_READ_INTERVAL) {
            tempSensor.requestTemperatures();
            tempRequested = true;
            tempRequestedAt = now;
        }
        if (tempRequested && now - tempRequestedAt >= TEMP_CONVERT_WAIT) {
            tempRequested = false;
            lastTempRead = now;
            float reading = tempSensor.getTempF(tempAddr);
            if (reading == DEVICE_DISCONNECTED_F) {
                Serial.println("[TEMP] Read failed (disconnected)");
            } else {
                lastTempF = reading;
                hasTempReading = true;
                char buf[16];
                snprintf(buf, sizeof(buf), "temp:%.1f", lastTempF);
                ws.textAll(buf);
                Serial.printf("[TEMP] %.1f°F\n", lastTempF);
            }
        }
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
