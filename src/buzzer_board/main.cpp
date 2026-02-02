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

// ---------- note frequencies ----------
#define REST 0
#define C3  131
#define D3  147
#define E3  165
#define F3  175
#define G3  196
#define A3  220
#define B3  247
#define C4  262
#define Cs4 277
#define D4  294
#define Ds4 311
#define Eb4 311
#define E4  330
#define F4  349
#define Fs4 370
#define G4  392
#define Gs4 415
#define Ab4 415
#define A4  440
#define Bb4 466
#define B4  494
#define C5  523
#define Cs5 554
#define Db5 554
#define D5  587
#define Ds5 622
#define Eb5 622
#define E5  659
#define F5  698
#define Fs5 740
#define G5  784
#define Gs5 831
#define Ab5 831
#define A5  880
#define Bb5 932
#define B5  988
#define C6  1047
#define D6  1175
#define E6  1319
#define F6  1397
#define G6  1568

// ---------- melodies ----------
// Each melody: array of {frequency, duration_ms} pairs, terminated by {0, 0}

static const uint16_t melodyLaCucaracha[][2] = {
    {C5, 188}, {C5, 188}, {C5, 188}, {F5, 375}, {A5, 375},
    {REST, 375},
    {C5, 188}, {C5, 188}, {C5, 188}, {F5, 375}, {A5, 375},
    {REST, 375},
    {F5, 375}, {F5, 375}, {E5, 375}, {E5, 375},
    {D5, 375}, {D5, 375}, {C5, 750},
    {0, 0}
};

static const uint16_t melodyAxelF[][2] = {
    {D4, 250}, {REST, 250}, {F4, 333}, {D4, 125}, {REST, 125},
    {D4, 125}, {G4, 250}, {D4, 250}, {C4, 250},
    {D4, 250}, {REST, 250}, {A4, 333}, {D4, 125}, {REST, 125},
    {D4, 125}, {Bb4, 250}, {A4, 250}, {F4, 250},
    {D4, 250}, {A4, 250}, {D5, 250}, {D4, 125}, {C4, 125}, {REST, 125},
    {C4, 125}, {A3, 250}, {E4, 250}, {D4, 1000},
    {REST, 1000},
    {0, 0}
};

static const uint16_t melodyGodfather[][2] = {
    {E4, 375}, {A4, 375}, {C5, 375},
    {B4, 375}, {A4, 375}, {C5, 375}, {A4, 375}, {B4, 375}, {A4, 375}, {F4, 375}, {G4, 375},
    {E4, 1500},
    {0, 0}
};

static const uint16_t melodyDieForelle[][2] = {
    {D5, 300}, {D5, 300}, {E5, 300}, {Fs5, 300},
    {Fs5, 300}, {Fs5, 300}, {Fs5, 300}, {G5, 150}, {Fs5, 150},
    {E5, 600}, {D5, 600},
    {D5, 300}, {D5, 300}, {E5, 300}, {Fs5, 300},
    {A5, 300}, {A5, 300}, {G5, 300}, {Fs5, 300},
    {E5, 600}, {REST, 600},
    {0, 0}
};

static const uint16_t melodyTetris[][2] = {
    {E5, 417}, {B4, 208}, {C5, 208}, {D5, 417}, {C5, 208}, {B4, 208},
    {A4, 417}, {A4, 208}, {C5, 208}, {E5, 417}, {D5, 208}, {C5, 208},
    {B4, 625}, {C5, 208}, {D5, 417}, {E5, 417},
    {C5, 417}, {A4, 417}, {A4, 417}, {REST, 417},
    {0, 0}
};

static const uint16_t melodyNokia[][2] = {
    {E5, 167}, {D5, 167}, {Fs4, 333}, {Gs4, 333},
    {Cs5, 167}, {B4, 167}, {D4, 333}, {E4, 333},
    {B4, 167}, {A4, 167}, {Cs4, 333}, {E4, 333},
    {A4, 667},
    {0, 0}
};

static const uint16_t melodyFurElise[][2] = {
    {E5, 188}, {Ds5, 188}, {E5, 188}, {Ds5, 188}, {E5, 188}, {B4, 188}, {D5, 188}, {C5, 188},
    {A4, 563}, {C4, 188}, {E4, 188}, {A4, 188},
    {B4, 563}, {E4, 188}, {Gs4, 188}, {B4, 188},
    {C5, 375}, {REST, 188}, {E4, 188}, {E5, 188}, {Ds5, 188},
    {E5, 188}, {Ds5, 188}, {E5, 188}, {B4, 188}, {D5, 188}, {C5, 188},
    {A4, 563}, {C4, 188}, {E4, 188}, {A4, 188},
    {B4, 563}, {E4, 188}, {C5, 188}, {B4, 188},
    {A4, 750}, {REST, 375},
    {0, 0}
};

// Pink Panther — robsoncouto, tempo=120, wholenote=2000ms
// Skipping 3 intro rests; Ds4 through E4 half note
static const uint16_t melodyPinkPanther[][2] = {
    {Ds4, 500}, {E4, 250}, {REST, 250}, {Fs4, 500}, {G4, 250}, {REST, 250},
    {Ds4, 500}, {E4, 250}, {REST, 250}, {Fs4, 500}, {G4, 250}, {REST, 250},
    {C5, 500}, {B4, 500}, {E4, 500}, {G4, 500},
    {B4, 500}, {Ab4, 1000}, {REST, 250},
    {E4, 500}, {G4, 500}, {B4, 500},
    {0, 0}
};

// Super Mario Bros — robsoncouto, tempo=200, wholenote=1200ms
// First 30 notes of opening section
static const uint16_t melodyMario[][2] = {
    {E5, 150}, {E5, 150}, {REST, 150}, {E5, 150},
    {REST, 150}, {C5, 150}, {E5, 150}, {REST, 150},
    {G5, 150}, {REST, 450}, {G4, 150}, {REST, 450},
    {C5, 300}, {REST, 150}, {G4, 300}, {REST, 150},
    {E4, 300}, {REST, 150}, {A4, 150}, {REST, 150},
    {B4, 150}, {REST, 150}, {Bb4, 150}, {A4, 150}, {REST, 150},
    {G4, 200}, {E5, 200}, {G5, 200},
    {A5, 150}, {REST, 150}, {F5, 150},
    {0, 0}
};

// Take On Me — robsoncouto, tempo=140, wholenote=1714ms
// 2× of the 16-note synth riff, all 8th notes = 214ms
static const uint16_t melodyTakeOnMe[][2] = {
    {Fs5, 214}, {Fs5, 214}, {D5, 214}, {B4, 214},
    {REST, 214}, {B4, 214}, {REST, 214}, {E5, 214},
    {REST, 214}, {E5, 214}, {REST, 214}, {E5, 214},
    {Gs5, 214}, {Gs5, 214}, {A5, 214}, {B5, 214},
    {Fs5, 214}, {Fs5, 214}, {D5, 214}, {B4, 214},
    {REST, 214}, {B4, 214}, {REST, 214}, {E5, 214},
    {REST, 214}, {E5, 214}, {REST, 214}, {E5, 214},
    {Gs5, 214}, {Gs5, 214}, {A5, 214}, {B5, 214},
    {0, 0}
};

// Pac-Man — robsoncouto, tempo=105, wholenote=2286ms
// Full intro, fast 16th/32nd notes
static const uint16_t melodyPacMan[][2] = {
    {B4, 143}, {B5, 143}, {Fs5, 143}, {Ds5, 143},
    {B5, 72}, {Fs5, 286}, {Ds5, 286},
    {C5, 143}, {C6, 143}, {G5, 143}, {E5, 143},
    {C6, 72}, {G5, 286}, {E5, 286},
    {B4, 143}, {B5, 143}, {Fs5, 143}, {Ds5, 143},
    {B5, 72}, {Fs5, 286}, {Ds5, 286},
    {Ds5, 143}, {E5, 143}, {F5, 143},
    {REST, 143}, {F5, 143}, {Fs5, 143}, {G5, 143},
    {REST, 143}, {G5, 143}, {Ab5, 143}, {A5, 143},
    {B5, 571},
    {0, 0}
};

// Cantina Band (Star Wars) — robsoncouto, tempo=140, wholenote=1714ms
// First phrase through dotted half
static const uint16_t melodyCantina[][2] = {
    {A4, 214}, {D5, 214}, {A4, 214}, {D5, 214},
    {A4, 214}, {D5, 428}, {A4, 214},
    {REST, 214}, {A4, 214}, {D5, 214}, {A4, 214},
    {REST, 214}, {G4, 214}, {REST, 214},
    {G4, 214}, {Fs4, 214}, {G4, 214}, {Fs4, 214},
    {A4, 428}, {D4, 1286},
    {0, 0}
};

// Ode to Joy (Beethoven) — robsoncouto, tempo=114, wholenote=2105ms
// First phrase, all quarter notes = 526ms
static const uint16_t melodyOdeToJoy[][2] = {
    {E4, 526}, {E4, 526}, {F4, 526}, {G4, 526},
    {G4, 526}, {F4, 526}, {E4, 526}, {D4, 526},
    {C4, 526}, {C4, 526}, {D4, 526}, {E4, 526},
    {E4, 789}, {D4, 263}, {D4, 1053},
    {0, 0}
};

// Coffin Dance / Astronomia — LaplaceTW, fixed timing
// dur 5→200ms, dur 3→333ms. Intro riff + first melodic phrase
static const uint16_t melodyCoffinDance[][2] = {
    {REST, 200}, {A4, 200}, {REST, 200}, {A4, 200},
    {A4, 200}, {Ab4, 200}, {A4, 200}, {REST, 200},
    {A4, 200}, {Ab4, 200}, {A4, 200}, {G4, 200},
    {REST, 200}, {G4, 200}, {A4, 200}, {REST, 200},
    {D4, 200}, {REST, 200}, {D4, 333},
    {A4, 333}, {G4, 333}, {Fs4, 333}, {E4, 333}, {Fs4, 333},
    {A4, 200}, {G4, 200}, {A4, 200}, {B4, 200},
    {A4, 200}, {G4, 200}, {A4, 200}, {REST, 200},
    {A4, 333}, {G4, 333}, {Fs4, 333}, {E4, 333}, {Fs4, 333},
    {A4, 200}, {G4, 200}, {A4, 200}, {B4, 200},
    {A4, 200}, {G4, 200}, {A4, 200}, {REST, 200},
    {D5, 333}, {D5, 333}, {D5, 333},
    {0, 0}
};

// Keyboard Cat — robsoncouto, tempo=160, wholenote=1500ms
// Skip 2 whole-note rests. Three 8-note descending arpeggios
static const uint16_t melodyKeyboardCat[][2] = {
    {C5, 375}, {E5, 375}, {G5, 375}, {E5, 375},
    {C5, 375}, {E5, 375}, {G5, 375}, {E5, 375},
    {B4, 375}, {D5, 375}, {G5, 375}, {D5, 375},
    {B4, 375}, {D5, 375}, {G5, 375}, {D5, 375},
    {A4, 375}, {C5, 375}, {E5, 375}, {C5, 375},
    {A4, 375}, {C5, 375}, {E5, 375}, {C5, 375},
    {0, 0}
};

// Still D.R.E. — HiBit, dur=1000/val
// A5×11 + G5×5 + A5×11 + G5×5 + sustained ending
static const uint16_t melodyStillDRE[][2] = {
    {A5, 200}, {A5, 200}, {A5, 200}, {A5, 200},
    {A5, 200}, {A5, 200}, {A5, 200}, {A5, 200},
    {A5, 200}, {A5, 200}, {A5, 200},
    {G5, 200}, {G5, 200}, {G5, 200}, {G5, 200}, {G5, 200},
    {A5, 200}, {A5, 200}, {A5, 200}, {A5, 200},
    {A5, 200}, {A5, 200}, {A5, 200}, {A5, 200},
    {A5, 200}, {A5, 200}, {A5, 200},
    {G5, 200}, {G5, 200}, {G5, 200}, {G5, 200}, {G5, 200},
    {A5, 1000}, {G5, 500}, {A5, 1000}, {G5, 500},
    {0, 0}
};

// Funkytown — 8th=300ms
// 2× of the hook: C5 C5 Bb4 C5 r G4 r G4 C5 F5 E5 C5 r(600)
static const uint16_t melodyFunkytown[][2] = {
    {C5, 300}, {C5, 300}, {Bb4, 300}, {C5, 300},
    {REST, 300}, {G4, 300}, {REST, 300}, {G4, 300},
    {C5, 300}, {F5, 300}, {E5, 300}, {C5, 300},
    {REST, 600},
    {C5, 300}, {C5, 300}, {Bb4, 300}, {C5, 300},
    {REST, 300}, {G4, 300}, {REST, 300}, {G4, 300},
    {C5, 300}, {F5, 300}, {E5, 300}, {C5, 300},
    {REST, 600},
    {0, 0}
};

// Mask Off (Future) — D minor flute melody
// Transcribed from the iconic flute riff
static const uint16_t melodyMaskOff[][2] = {
    {A5, 250}, {G5, 250}, {F5, 250}, {A5, 250},
    {G5, 250}, {F5, 250}, {E5, 250}, {D5, 250},
    {REST, 250},
    {D5, 250}, {E5, 250}, {F5, 250}, {E5, 250}, {D5, 500},
    {REST, 250},
    {A5, 250}, {G5, 250}, {F5, 250}, {A5, 250},
    {G5, 250}, {F5, 250}, {E5, 250}, {D5, 250},
    {REST, 250},
    {D5, 250}, {E5, 250}, {F5, 250}, {E5, 250}, {D5, 500},
    {0, 0}
};

// Hotline Bling (Drake) — D minor bell melody
// D-D-D-F-E-D-C pattern, transcribed from the bell riff
static const uint16_t melodyHotlineBling[][2] = {
    {D5, 300}, {D5, 300}, {D5, 300}, {F5, 450},
    {E5, 300}, {D5, 300}, {C5, 600},
    {REST, 300},
    {D5, 300}, {D5, 300}, {D5, 300}, {F5, 450},
    {E5, 300}, {D5, 300}, {C5, 600},
    {REST, 300},
    {A4, 300}, {A4, 300}, {C5, 300}, {D5, 600},
    {REST, 300},
    {0, 0}
};

struct MelodyEntry {
    const uint16_t (*notes)[2];
    uint16_t length;
};

#define MELODY_LEN(arr) (sizeof(arr) / sizeof(arr[0]) - 1)  // exclude terminator

static const MelodyEntry melodies[] = {
    { melodyLaCucaracha,   MELODY_LEN(melodyLaCucaracha)   },
    { melodyAxelF,         MELODY_LEN(melodyAxelF)         },
    { melodyGodfather,     MELODY_LEN(melodyGodfather)     },
    { melodyDieForelle,    MELODY_LEN(melodyDieForelle)    },
    { melodyTetris,        MELODY_LEN(melodyTetris)        },
    { melodyNokia,         MELODY_LEN(melodyNokia)         },
    { melodyFurElise,      MELODY_LEN(melodyFurElise)      },
    { melodyPinkPanther,   MELODY_LEN(melodyPinkPanther)   },
    { melodyMario,         MELODY_LEN(melodyMario)         },
    { melodyTakeOnMe,      MELODY_LEN(melodyTakeOnMe)      },
    { melodyPacMan,        MELODY_LEN(melodyPacMan)        },
    { melodyCantina,       MELODY_LEN(melodyCantina)       },
    { melodyOdeToJoy,      MELODY_LEN(melodyOdeToJoy)      },
    { melodyCoffinDance,   MELODY_LEN(melodyCoffinDance)   },
    { melodyKeyboardCat,   MELODY_LEN(melodyKeyboardCat)   },
    { melodyStillDRE,      MELODY_LEN(melodyStillDRE)      },
    { melodyFunkytown,     MELODY_LEN(melodyFunkytown)     },
    { melodyMaskOff,       MELODY_LEN(melodyMaskOff)       },
    { melodyHotlineBling,  MELODY_LEN(melodyHotlineBling)  },
};
static const uint8_t MELODY_COUNT = sizeof(melodies) / sizeof(melodies[0]);

// ---------- melody player ----------
struct MelodyPlayer {
    const uint16_t (*melody)[2];
    uint16_t length;
    uint16_t noteIndex;
    unsigned long noteStartedAt;
    uint16_t gapDuration;
    bool playing;
    bool inGap;
    bool inLoopPause;
    uint8_t loopCount;
};

MelodyPlayer player = { nullptr, 0, 0, 0, 0, false, false, false, 0 };
uint8_t currentMelodyIndex = 0;

void playCurrentNote() {
    uint16_t freq = player.melody[player.noteIndex][0];
    uint16_t duration = player.melody[player.noteIndex][1];
    if (freq > 0) {
        ledcAttachPin(PIN_BUZZER, BUZZER_LEDC_CHANNEL);
        ledcWriteTone(BUZZER_LEDC_CHANNEL, freq);
        player.gapDuration = duration / 10;
        if (player.gapDuration < 20) player.gapDuration = 20;
        if (player.gapDuration >= duration) player.gapDuration = 0;
    } else {
        ledcDetachPin(PIN_BUZZER);
        player.gapDuration = 0;
    }
    player.noteStartedAt = millis();
    player.inGap = false;
    player.inLoopPause = false;
}

void startMelody(uint8_t index) {
    player.melody = melodies[index].notes;
    player.length = melodies[index].length;
    player.noteIndex = 0;
    player.playing = true;
    player.inGap = false;
    player.inLoopPause = false;
    player.loopCount = 0;
    playCurrentNote();
}

void stopMelody() {
    ledcDetachPin(PIN_BUZZER);
    player.playing = false;
}

void updateMelody() {
    if (!player.playing) return;
    unsigned long now = millis();

    if (player.inLoopPause) {
        if (now - player.noteStartedAt >= MELODY_LOOP_PAUSE_MS) {
            player.loopCount++;
            if (player.loopCount >= MELODY_LOOP_COUNT) {
                player.playing = false;
                ledcDetachPin(PIN_BUZZER);
                return;
            }
            player.noteIndex = 0;
            playCurrentNote();
        }
        return;
    }

    if (player.inGap) {
        if (now - player.noteStartedAt >= player.gapDuration) {
            player.noteIndex++;
            if (player.noteIndex >= player.length) {
                player.inLoopPause = true;
                player.noteStartedAt = now;
                return;
            }
            playCurrentNote();
        }
    } else {
        uint16_t duration = player.melody[player.noteIndex][1];
        uint16_t toneDuration = (player.gapDuration > 0)
            ? (duration - player.gapDuration) : duration;

        if (now - player.noteStartedAt >= toneDuration) {
            if (player.gapDuration > 0) {
                ledcDetachPin(PIN_BUZZER);
                player.inGap = true;
                player.noteStartedAt = now;
            } else {
                player.noteIndex++;
                if (player.noteIndex >= player.length) {
                    player.inLoopPause = true;
                    player.noteStartedAt = now;
                    return;
                }
                playCurrentNote();
            }
        }
    }
}

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
        stopMelody();
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_GREEN, LOW);
        flashOn = false;
        break;
    case BUZZING:
        startMelody(currentMelodyIndex);
        currentMelodyIndex = (currentMelodyIndex + 1) % MELODY_COUNT;
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, HIGH);
        flashOn = true;
        lastFlashToggle = millis();
        break;
    case DISMISSED:
        stopMelody();
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
    ledcSetup(BUZZER_LEDC_CHANNEL, 1000, BUZZER_LEDC_RESOLUTION);
    ledcAttachPin(PIN_BUZZER, BUZZER_LEDC_CHANNEL);
    ledcDetachPin(PIN_BUZZER);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_DISMISS_BTN, INPUT_PULLUP);
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
    updateMelody();

    // buzzing: flash red LED + auto-dismiss on melody completion or timeout
    if (state == BUZZING) {
        if (!player.playing) {
            Serial.println("Melody finished, auto-dismissing");
            enterState(DISMISSED);
            return;
        }
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
