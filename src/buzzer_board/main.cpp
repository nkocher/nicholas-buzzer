#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AceButton.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>
#include "config.h"
#include "songs.h"

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
// Skipping 3 intro rests; re-converted with correct dotted note durations
static const uint16_t melodyPinkPanther[][2] = {
    {Ds4, 250}, {E4, 750}, {REST, 250}, {Fs4, 250}, {G4, 750}, {REST, 250},
    {Ds4, 250}, {E4, 375}, {Fs4, 250}, {G4, 375}, {C5, 250}, {B4, 375}, {E4, 250}, {G4, 375}, {B4, 250},
    {Bb4, 1000}, {A4, 188}, {G4, 188}, {E4, 188}, {D4, 188},
    {E4, 1000},
    {0, 0}
};

// Super Mario Bros — robsoncouto, tempo=200, wholenote=1200ms
// First 31 notes re-converted with correct quarter/dotted durations
static const uint16_t melodyMario[][2] = {
    {E5, 150}, {E5, 150}, {REST, 150}, {E5, 150}, {REST, 150}, {C5, 150}, {E5, 150},
    {G5, 300}, {REST, 300}, {G4, 150}, {REST, 300},
    {C5, 450}, {G4, 150}, {REST, 300}, {E4, 450},
    {A4, 300}, {B4, 300}, {Bb4, 150}, {A4, 300},
    {G4, 225}, {E5, 225}, {G5, 225}, {A5, 300}, {F5, 150}, {G5, 150},
    {REST, 150}, {E5, 300}, {C5, 150}, {D5, 150}, {B4, 450},
    {0, 0}
};

// Take On Me — robsoncouto, tempo=140, wholenote=1714ms
// Section 1 + Section 2 of synth riff, all 8th notes = 214ms
static const uint16_t melodyTakeOnMe[][2] = {
    // Section 1
    {Fs5, 214}, {Fs5, 214}, {D5, 214}, {B4, 214}, {REST, 214}, {B4, 214}, {REST, 214}, {E5, 214},
    {REST, 214}, {E5, 214}, {REST, 214}, {E5, 214}, {Gs5, 214}, {Gs5, 214}, {A5, 214}, {B5, 214},
    // Section 2
    {A5, 214}, {A5, 214}, {A5, 214}, {E5, 214}, {REST, 214}, {D5, 214}, {REST, 214}, {Fs5, 214},
    {REST, 214}, {Fs5, 214}, {REST, 214}, {Fs5, 214}, {E5, 214}, {E5, 214}, {Fs5, 214}, {E5, 214},
    {0, 0}
};

// Pac-Man — robsoncouto, tempo=105, wholenote=2286ms
// Full intro, corrected dotted 16th (214ms) and 32nd (71ms) durations
static const uint16_t melodyPacMan[][2] = {
    {B4, 143}, {B5, 143}, {Fs5, 143}, {Ds5, 143}, {B5, 71}, {Fs5, 214}, {Ds5, 286},
    {C5, 143}, {C6, 143}, {G5, 143}, {E5, 143}, {C6, 71}, {G5, 214}, {E5, 286},
    {B4, 143}, {B5, 143}, {Fs5, 143}, {Ds5, 143}, {B5, 71}, {Fs5, 214}, {Ds5, 286},
    {Ds5, 71}, {E5, 71}, {F5, 71}, {F5, 71}, {Fs5, 71}, {G5, 71}, {G5, 71}, {Gs5, 71}, {A5, 143}, {B5, 286},
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

// Coffin Dance / Astronomia — Arduino Project Hub source, 128 BPM, 8th=234ms
// Main melody: C5-Bb4-A4-F4 pattern in Bb4/A4 range
static const uint16_t melodyCoffinDance[][2] = {
    {C5, 234}, {Bb4, 234}, {A4, 234}, {F4, 234},
    {G4, 234}, {REST, 234}, {G4, 234}, {D5, 234},
    {C5, 234}, {REST, 234}, {Bb4, 234}, {REST, 234},
    {A4, 234}, {REST, 234}, {A4, 234}, {A4, 234},
    {C5, 234}, {REST, 234}, {Bb4, 234}, {A4, 234},
    {G4, 234}, {REST, 234}, {G4, 234}, {Bb4, 234},
    {A4, 234}, {Bb4, 234}, {A4, 234}, {Bb4, 234},
    {G4, 234}, {REST, 234}, {G4, 234}, {Bb4, 234},
    {A4, 234}, {Bb4, 234}, {A4, 234}, {Bb4, 234},
    {0, 0}
};

// Keyboard Cat — robsoncouto arrangement, tempo=160, wholenote=1500ms
// Octave 3/4 per robsoncouto source, mixed quarter/8th/dotted rhythms
static const uint16_t melodyKeyboardCat[][2] = {
    {C4, 375}, {E4, 375}, {G4, 375}, {E4, 375},
    {C4, 375}, {E4, 188}, {G4, 563}, {E4, 375},
    {A3, 375}, {C4, 375}, {E4, 375}, {C4, 375},
    {A3, 375}, {C4, 188}, {E4, 563}, {C4, 375},
    {G3, 375}, {B3, 375}, {D4, 375}, {B3, 375},
    {G3, 375}, {B3, 188}, {D4, 563}, {B3, 375},
    {0, 0}
};

// Still D.R.E. — HiBit, 275ms per fast note (~109 BPM)
// A5×11 + G5×5 + A5×11 + G5×5 + sustained ending. ~12.9s/loop, 3 loops ≈ 39.6s
static const uint16_t melodyStillDRE[][2] = {
    {A5, 275}, {A5, 275}, {A5, 275}, {A5, 275},
    {A5, 275}, {A5, 275}, {A5, 275}, {A5, 275},
    {A5, 275}, {A5, 275}, {A5, 275},
    {G5, 275}, {G5, 275}, {G5, 275}, {G5, 275}, {G5, 275},
    {A5, 275}, {A5, 275}, {A5, 275}, {A5, 275},
    {A5, 275}, {A5, 275}, {A5, 275}, {A5, 275},
    {A5, 275}, {A5, 275}, {A5, 275},
    {G5, 275}, {G5, 275}, {G5, 275}, {G5, 275}, {G5, 275},
    {A5, 1375}, {G5, 688}, {A5, 1375}, {G5, 688},
    {0, 0}
};

// Funkytown — ~128 BPM, 8th=234ms
// 2× of the hook: C5 C5 Bb4 C5 r G4 r G4 C5 F5 E5 C5 r(469)
static const uint16_t melodyFunkytown[][2] = {
    {C5, 234}, {C5, 234}, {Bb4, 234}, {C5, 234},
    {REST, 234}, {G4, 234}, {REST, 234}, {G4, 234},
    {C5, 234}, {F5, 234}, {E5, 234}, {C5, 234},
    {REST, 469},
    {C5, 234}, {C5, 234}, {Bb4, 234}, {C5, 234},
    {REST, 234}, {G4, 234}, {REST, 234}, {G4, 234},
    {C5, 234}, {F5, 234}, {E5, 234}, {C5, 234},
    {REST, 469},
    {0, 0}
};


// Like a Prayer (Madonna) — D minor, 112 BPM, chorus vocal
static const uint16_t melodyLikeAPrayer[][2] = {
    // "Life is a mystery" / "When you call my name"
    {G4, 268}, {G4, 268}, {G4, 536}, {G4, 268}, {F4, 536},
    {REST, 268},
    // "it's like a little prayer"
    {G4, 268}, {G4, 268}, {A4, 268}, {F4, 536}, {F4, 268},
    {REST, 268},
    // "I'm down on my knees"
    {G4, 268}, {G4, 268}, {G4, 268}, {F4, 536},
    {REST, 268},
    // "I wanna take you there"
    {A4, 268}, {F4, 268}, {F4, 268}, {A4, 268}, {G4, 268}, {F4, 536},
    {REST, 268},
    {0, 0}
};


struct MelodyEntry {
    const uint16_t (*notes)[2];
    uint16_t length;
};

#define MELODY_LEN(arr) (sizeof(arr) / sizeof(arr[0]) - 1)  // exclude terminator

// ---------- note frequency helper ----------
static const uint16_t NOTE_FREQS[12] = { 262,277,294,311,330,349,370,392,415,440,466,494 }; // C4..B4

uint16_t noteFreq(uint8_t semitone, uint8_t octave) {
    uint16_t f = NOTE_FREQS[semitone % 12];
    if (octave > 4) f <<= (octave - 4);
    else if (octave < 4) f >>= (4 - octave);
    return f;
}

// Map note letter to semitone index: c=0, d=2, e=4, f=5, g=7, a=9, b=11
static uint8_t letterToSemitone(char c) {
    switch (c) {
        case 'c': return 0;  case 'd': return 2;  case 'e': return 4;
        case 'f': return 5;  case 'g': return 7;  case 'a': return 9;
        case 'b': return 11; default:  return 0;
    }
}

// ---------- RTTTL parser ----------
// Format: name:d=4,o=5,b=120:8c,d,e.,4f#,p,g6
uint16_t parseRTTTL(const char* rtttl, uint16_t out[][2], uint16_t maxNotes) {
    const char* p = rtttl;
    // Skip name
    while (*p && *p != ':') p++;
    if (!*p) return 0;
    p++; // skip first ':'

    // Parse defaults
    uint8_t defDur = 4, defOct = 6;
    uint16_t bpm = 63;
    while (*p && *p != ':') {
        while (*p == ' ' || *p == ',') p++;
        if (*p == 'd' && *(p+1) == '=') { p += 2; defDur = atoi(p); while (*p >= '0' && *p <= '9') p++; }
        else if (*p == 'o' && *(p+1) == '=') { p += 2; defOct = atoi(p); while (*p >= '0' && *p <= '9') p++; }
        else if (*p == 'b' && *(p+1) == '=') { p += 2; bpm = atoi(p); while (*p >= '0' && *p <= '9') p++; }
        else p++;
    }
    if (!*p) return 0;
    p++; // skip second ':'

    if (bpm == 0) bpm = 63;
    uint32_t wholeNote = (60000UL * 4) / bpm;
    uint16_t count = 0;

    while (*p && count < maxNotes) {
        while (*p == ' ' || *p == ',') p++;
        if (!*p) break;

        // Optional duration prefix
        uint8_t dur = 0;
        while (*p >= '0' && *p <= '9') { dur = dur * 10 + (*p - '0'); p++; }
        if (dur == 0) dur = defDur;

        // Note letter or 'p' for pause
        uint16_t freq = 0;
        if (*p == 'p' || *p == 'P') {
            p++;
        } else if ((*p >= 'a' && *p <= 'g') || (*p >= 'A' && *p <= 'G')) {
            char note = *p | 0x20; // lowercase
            p++;
            uint8_t semi = letterToSemitone(note);
            // Sharp?
            if (*p == '#') { semi++; p++; }
            else if (*p == '_') { semi++; p++; } // alternate sharp notation
            // Octave?
            uint8_t oct = defOct;
            if (*p >= '0' && *p <= '9') { oct = *p - '0'; p++; }
            freq = noteFreq(semi, oct);
        } else {
            p++; continue; // skip unknown
        }

        // Duration in ms
        uint16_t ms = wholeNote / dur;
        // Dotted?
        if (*p == '.') { ms = ms + ms / 2; p++; }

        out[count][0] = freq;
        out[count][1] = ms;
        count++;
    }
    return count;
}

// ---------- MML parser ----------
// Format: MML@t140l8o5cde4f+g>ab<r4c&c,harmony1,harmony2;
uint16_t parseMML(const char* mml, uint16_t out[][2], uint16_t maxNotes, uint8_t track = 0) {
    const char* p = mml;

    // Strip MML@ prefix
    if (p[0]=='M'&&p[1]=='M'&&p[2]=='L'&&p[3]=='@') p += 4;

    // Find the end (strip trailing ;)
    const char* end = p;
    while (*end && *end != ';') end++;

    // Select track by comma separation
    uint8_t currentTrack = 0;
    while (currentTrack < track && p < end) {
        if (*p == ',') { currentTrack++; if (currentTrack == track) { p++; break; } }
        p++;
    }
    if (currentTrack != track) return 0;

    // Find end of this track
    const char* trackEnd = p;
    while (trackEnd < end && *trackEnd != ',') trackEnd++;

    // State
    uint8_t octave = 4;
    uint8_t defaultLength = 4;
    uint16_t tempo = 120;
    uint16_t count = 0;

    while (p < trackEnd && count < maxNotes) {
        char c = *p;

        // Tempo command
        if (c == 't' || c == 'T') {
            p++;
            uint16_t val = 0;
            while (p < trackEnd && *p >= '0' && *p <= '9') { val = val*10 + (*p-'0'); p++; }
            if (val > 0) tempo = val;
            continue;
        }
        // Default length
        if (c == 'l' || c == 'L') {
            p++;
            uint8_t val = 0;
            while (p < trackEnd && *p >= '0' && *p <= '9') { val = val*10 + (*p-'0'); p++; }
            if (val > 0) defaultLength = val;
            continue;
        }
        // Octave set
        if (c == 'o' || c == 'O') {
            p++;
            uint8_t val = 0;
            while (p < trackEnd && *p >= '0' && *p <= '9') { val = val*10 + (*p-'0'); p++; }
            octave = val;
            continue;
        }
        // Octave shift
        if (c == '>') { octave++; p++; continue; }
        if (c == '<') { octave--; p++; continue; }
        // Volume (ignore)
        if (c == 'v' || c == 'V') {
            p++;
            while (p < trackEnd && *p >= '0' && *p <= '9') p++;
            continue;
        }

        // Note or rest
        bool isNote = (c >= 'a' && c <= 'g') || (c >= 'A' && c <= 'G');
        bool isRest = (c == 'r' || c == 'R');

        if (!isNote && !isRest) { p++; continue; }

        uint16_t freq = 0;
        if (isNote) {
            char noteLower = c | 0x20;
            p++;
            uint8_t semi = letterToSemitone(noteLower);
            // Sharp / flat
            if (p < trackEnd && (*p == '+' || *p == '#')) { semi++; p++; }
            else if (p < trackEnd && *p == '-') { semi--; p++; }
            freq = noteFreq(semi, octave);
        } else {
            p++; // skip 'r'
        }

        // Optional length
        uint8_t noteLen = 0;
        while (p < trackEnd && *p >= '0' && *p <= '9') { noteLen = noteLen*10 + (*p-'0'); p++; }
        if (noteLen == 0) noteLen = defaultLength;

        uint32_t wholeNote = (60000UL * 4) / tempo;
        uint32_t ms = wholeNote / noteLen;

        // Dotted
        if (p < trackEnd && *p == '.') { ms = ms + ms/2; p++; }

        // Tie (&) — merge with next note of same pitch
        while (p < trackEnd && *p == '&') {
            p++;
            // Skip the tied note letter + modifiers
            if (p < trackEnd && ((*p >= 'a' && *p <= 'g') || (*p >= 'A' && *p <= 'G'))) {
                p++;
                if (p < trackEnd && (*p == '+' || *p == '#' || *p == '-')) p++;
            } else if (p < trackEnd && (*p == 'r' || *p == 'R')) {
                p++;
            }
            uint8_t tieLen = 0;
            while (p < trackEnd && *p >= '0' && *p <= '9') { tieLen = tieLen*10 + (*p-'0'); p++; }
            if (tieLen == 0) tieLen = defaultLength;
            ms += wholeNote / tieLen;
            if (p < trackEnd && *p == '.') { ms += (wholeNote / tieLen) / 2; p++; }
        }

        if (ms > 65535) ms = 65535;
        out[count][0] = freq;
        out[count][1] = (uint16_t)ms;
        count++;
    }
    return count;
}

// Hardcoded melody registry (used by buildMelodyRegistry at boot)
static const MelodyEntry hardcodedMelodies[] = {
    { melodyLikeAPrayer,   MELODY_LEN(melodyLikeAPrayer)   },
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
    { melodyOdeToJoy,      MELODY_LEN(melodyOdeToJoy)      },
    { melodyCoffinDance,   MELODY_LEN(melodyCoffinDance)   },
    { melodyKeyboardCat,   MELODY_LEN(melodyKeyboardCat)   },
    { melodyStillDRE,      MELODY_LEN(melodyStillDRE)      },
    { melodyFunkytown,     MELODY_LEN(melodyFunkytown)     },
};
static const char* hardcodedNames[] = {
    "Like a Prayer", "La Cucaracha", "Axel F", "The Godfather",
    "Die Forelle", "Tetris", "Nokia", "Fur Elise",
    "Pink Panther", "Super Mario", "Take On Me", "Pac-Man",
    "Ode to Joy", "Coffin Dance", "Keyboard Cat",
    "Still D.R.E.", "Funkytown",
};
static const uint8_t HARDCODED_COUNT = sizeof(hardcodedMelodies) / sizeof(hardcodedMelodies[0]);

// Mutable melody registry — populated at boot by buildMelodyRegistry()
static MelodyEntry melodies[256];
static const char* melodyNames[256];
static uint8_t MELODY_COUNT = 0;

// Dynamic parsed songs
#define MAX_NOTES_PER_SONG 256
static uint16_t (*parsedNotes[128])[2];
static uint16_t parsedLengths[128];
static char* parsedNames[128];
static uint8_t parsedCount = 0;

void parseSongs() {
    uint16_t tempBuf[MAX_NOTES_PER_SONG][2];
    char* strBuf = (char*)malloc(4096);
    if (!strBuf) { Serial.println("[SONGS] malloc failed for strBuf"); return; }

    for (uint8_t i = 0; i < SONG_DEF_COUNT; i++) {
        SongDef def;
        memcpy_P(&def, &songDefs[i], sizeof(SongDef));

        // Read PROGMEM string
        size_t len = strlen_P(def.str);
        if (len >= 4096) { Serial.printf("[SONGS] #%d too long (%d), skipping\n", i, len); continue; }
        strncpy_P(strBuf, def.str, 4095);
        strBuf[4095] = '\0';

        // Read name
        char nameBuf[64];
        strncpy_P(nameBuf, def.name, sizeof(nameBuf));
        nameBuf[sizeof(nameBuf)-1] = '\0';

        // Parse
        uint16_t count = 0;
        if (def.fmt == FMT_RTTTL)
            count = parseRTTTL(strBuf, tempBuf, MAX_NOTES_PER_SONG);
        else
            count = parseMML(strBuf, tempBuf, MAX_NOTES_PER_SONG);

        if (count == 0) {
            Serial.printf("[SONGS] #%d %s: parse failed\n", i, nameBuf);
            continue;
        }

        // Allocate exact size for notes
        parsedNotes[parsedCount] = (uint16_t(*)[2])malloc(count * sizeof(uint16_t[2]));
        if (!parsedNotes[parsedCount]) { Serial.printf("[SONGS] #%d malloc failed\n", i); continue; }
        memcpy(parsedNotes[parsedCount], tempBuf, count * sizeof(uint16_t[2]));
        parsedLengths[parsedCount] = count;

        // Copy name
        parsedNames[parsedCount] = strdup(nameBuf);

        Serial.printf("[SONGS] #%d %s: %d notes\n", i, nameBuf, count);
        parsedCount++;
    }
    free(strBuf);
    Serial.printf("[SONGS] Parsed %d/%d songs\n", parsedCount, SONG_DEF_COUNT);
}

void buildMelodyRegistry() {
    MELODY_COUNT = 0;

    // Parsed songs FIRST (for testing)
    for (uint8_t i = 0; i < parsedCount; i++) {
        melodies[MELODY_COUNT] = { parsedNotes[i], parsedLengths[i] };
        melodyNames[MELODY_COUNT] = parsedNames[i];
        MELODY_COUNT++;
    }

    // Hardcoded melodies
    for (uint8_t i = 0; i < HARDCODED_COUNT; i++) {
        melodies[MELODY_COUNT] = hardcodedMelodies[i];
        melodyNames[MELODY_COUNT] = hardcodedNames[i];
        MELODY_COUNT++;
    }

    Serial.printf("[SONGS] Registry: %d total melodies\n", MELODY_COUNT);
}

// ---------- shuffle ----------
static uint8_t shuffleOrder[256];
static uint8_t shufflePos = 0;
static uint32_t shuffleSeed = 0;
static bool playSpecific = false;
Preferences prefs;

static uint32_t xorshift32(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void shuffleMelodies(uint32_t seed) {
    for (uint8_t i = 0; i < MELODY_COUNT; i++) shuffleOrder[i] = i;
    uint32_t rng = seed;
    for (uint8_t i = MELODY_COUNT - 1; i > 0; i--) {
        uint8_t j = xorshift32(&rng) % (i + 1);
        uint8_t tmp = shuffleOrder[i];
        shuffleOrder[i] = shuffleOrder[j];
        shuffleOrder[j] = tmp;
    }
}

void saveShuffleState(bool saveSeed) {
    prefs.begin("buzz", false);
    if (saveSeed) prefs.putULong("seed", shuffleSeed);
    prefs.putUChar("pos", shufflePos);
    prefs.end();
}

void loadShuffleState() {
    prefs.begin("buzz", true);
    shuffleSeed = prefs.getULong("seed", 0);
    shufflePos = prefs.getUChar("pos", 0);
    prefs.end();
    if (shuffleSeed == 0 || shufflePos >= MELODY_COUNT) {
        shuffleSeed = esp_random();
        shufflePos = 0;
        saveShuffleState(true);
    }
    shuffleMelodies(shuffleSeed);
    Serial.printf("[SHUFFLE] seed=%lu pos=%d/%d\n", shuffleSeed, shufflePos, MELODY_COUNT);
}

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

static const char SONGS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<meta name="theme-color" content="#111111">
<title>Song Picker</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#111;color:#eee;padding:12px}
h1{text-align:center;margin-bottom:12px;font-size:1.3rem}
#status{display:inline-block;width:8px;height:8px;border-radius:50%;background:#e53e3e;margin-left:6px;vertical-align:middle}
#status.ok{background:#38a169}
#stop{display:block;margin:0 auto 12px;padding:8px 24px;border:none;border-radius:8px;
      background:#e53e3e;color:#fff;font-size:1rem;cursor:pointer;touch-action:manipulation}
#stop:active{background:#c53030}
#now{text-align:center;color:#888;margin-bottom:8px;font-size:0.85rem;min-height:1.2em}
#list{list-style:none;max-width:600px;margin:0 auto}
#list li{padding:10px 12px;border-bottom:1px solid #222;cursor:pointer;display:flex;
         align-items:center;gap:8px;-webkit-tap-highlight-color:transparent}
#list li:active{background:#222}
#list .idx{color:#555;font-size:0.75rem;min-width:24px;text-align:right}
#list .name{flex:1}
</style>
</head>
<body>
<h1>Songs <span id="status"></span></h1>
<button id="stop">Stop</button>
<div id="now"></div>
<ul id="list"></ul>
<script>
var sock=null,connected=false,rTimer=null,songs=[];
var dot=document.getElementById('status');
var list=document.getElementById('list');
var now=document.getElementById('now');
var SERVER=window.location.hostname;
function ui(){dot.className=connected?'ok':'';}
function reconnect(){if(!rTimer)rTimer=setTimeout(function(){rTimer=null;connect();},3000);}
function connect(){
  if(sock){sock.onopen=sock.onclose=sock.onerror=sock.onmessage=null;try{sock.close();}catch(e){}}
  try{sock=new WebSocket('ws://'+SERVER+'/ws');}catch(e){reconnect();return;}
  sock.onopen=function(){connected=true;ui();};
  sock.onclose=function(){connected=false;ui();reconnect();};
  sock.onerror=function(){connected=false;ui();reconnect();};
  sock.onmessage=function(e){
    if(e.data==='dismiss'){now.textContent='';}
  };
}
function play(i){
  if(!sock||sock.readyState!==1)return;
  sock.send('play:'+i);
  now.textContent='Playing: '+songs[i].n;
}
document.getElementById('stop').addEventListener('click',function(){
  if(sock&&sock.readyState===1)sock.send('dismiss');
  now.textContent='';
});
document.addEventListener('visibilitychange',function(){
  if(!document.hidden&&(!sock||sock.readyState!==1)){connected=false;ui();reconnect();}
});
fetch('/songs.json').then(function(r){return r.json();}).then(function(data){
  songs=data;
  data.forEach(function(s){
    var li=document.createElement('li');
    var idx=document.createElement('span');
    idx.className='idx';
    idx.textContent=s.i;
    var nm=document.createElement('span');
    nm.className='name';
    nm.textContent=s.n;
    li.appendChild(idx);
    li.appendChild(nm);
    li.addEventListener('click',function(){play(s.i);});
    list.appendChild(li);
  });
});
connect();ui();
</script>
</body>
</html>
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
        if (playSpecific) {
            startMelody(currentMelodyIndex);
            playSpecific = false;
        } else {
            currentMelodyIndex = shuffleOrder[shufflePos];
            startMelody(currentMelodyIndex);
            shufflePos++;
            if (shufflePos >= MELODY_COUNT) {
                shuffleSeed = esp_random();
                shufflePos = 0;
                shuffleMelodies(shuffleSeed);
                saveShuffleState(true);
            } else {
                saveShuffleState(false);
            }
        }
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
            } else if (len == 7 && memcmp(data, "dismiss", 7) == 0 && state == BUZZING) {
                Serial.println("[WS] Received dismiss");
                enterState(DISMISSED);
            } else if (len >= 5 && memcmp(data, "play:", 5) == 0) {
                // play:N — play specific song by index
                char numBuf[8];
                size_t numLen = len - 5;
                if (numLen > 0 && numLen < sizeof(numBuf)) {
                    memcpy(numBuf, data + 5, numLen);
                    numBuf[numLen] = '\0';
                    int idx = atoi(numBuf);
                    if (idx >= 0 && idx < MELODY_COUNT) {
                        Serial.printf("[WS] Playing song #%d: %s\n", idx, melodyNames[idx]);
                        currentMelodyIndex = idx;
                        playSpecific = true;
                        enterState(BUZZING);
                    }
                }
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
    server.on("/songs", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", SONGS_HTML);
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        request->send(response);
    });
    server.on("/songs.json", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "[";
        for (uint8_t i = 0; i < MELODY_COUNT; i++) {
            if (i > 0) json += ',';
            json += "{\"i\":";
            json += i;
            json += ",\"n\":\"";
            // Escape any quotes in song name
            const char* n = melodyNames[i];
            while (*n) {
                if (*n == '"') json += '\\';
                json += *n;
                n++;
            }
            json += "\"}";
        }
        json += "]";
        request->send(200, "application/json", json);
    });
    server.onNotFound([](AsyncWebServerRequest* request) {
        Serial.printf("[HTTP] 404: %s\n", request->url().c_str());
        request->send(404, "text/plain", "Not found");
    });

    // Parse songs and build melody registry
    parseSongs();
    buildMelodyRegistry();
    loadShuffleState();

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
