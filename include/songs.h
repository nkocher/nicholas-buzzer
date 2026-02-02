#pragma once

#include <Arduino.h>

// ---------- song format ----------
enum SongFmt : uint8_t { FMT_RTTTL, FMT_MML };
struct SongDef {
    const char* str;   // PROGMEM pointer
    const char* name;  // PROGMEM pointer
    SongFmt fmt;
};

// ========== RTTTL Songs ==========

static const char rtttl_fiddler[] PROGMEM =
  "we-rock:d=4,o=6,b=40:32g#5,32f#5,32g#5,32f#5,16f5,8c#5,32f5,32f#5,32g#5,32f#5,32g#5,32f#5,32f5,32f#5,32g#5,32a#5,32b5,32a#5,32b5,32a#5,8g#5,16a5,16g#5,16g5,16f#5,32e5,32d#5,32c#5,32d#5,8e5,32e5,32d#5,32c#5,32d#5,16e5,16c#5,16g#5,";

static const char rtttl_larry[] PROGMEM =
  "Larry:d=4,o=5,b=180:c.,8b.4,c.,8d.,d#.,8d.,d#.,8f.,g.,8f#.,g.,8g#.,g,8d#.,p.,f.,8e.,f.,8g.,f,8d.,p.,d#.,8d.,d#.,8f.,d#,8g.,p.,c.,8b.4,c.,8d.,d#.,8d.,d#.,8f.,g.,8f#.,g.,8g#.,g,8d#.,p.,16g,16p,8g.,16p,16g.,16p,g#,a,2a#,2a#.4,16p.,16g,8a#,16p,16c.6,8g,16p,a#,c6,16g.,8a#,16p,16c.6,8a#,16p,d#.6,16p.,16b,8c6,16p,16d#.6,8b,16p,c6,d#6,16b.,8c6,16p,16d#.6,8f6,16p,f#6,16p.,16d#6,8p.,8g6,8p,8g6,8p,8g6,8p,8g6,8p,8g6,16p";

static const char rtttl_macarena[] PROGMEM =
  "Macarena:d=4,o=5,b=180:f,8f,8f,f,8f,8f,8f,8f,8f,8f,8f,8a,8c,8c,f,8f,8f,f,8f,8f,8f,8f,8f,8f,8d,8c,p,f,8f,8f,f,8f,8f,8f,8f,8f,8f,8f,8a,p,2c.6,a,8c6,8a,8f,p,2p";

static const char rtttl_who_let_dogs[] PROGMEM =
  "WhoLetDogs:d=4,o=5,b=140:8c,8g,8c,8g,8c,p,8p,8d,8p,8c,16e,16p,8e,8d,8c,8c,8g,8c,8g,8c,p,8p,8d,8p,8c,16e,16p,8e,8d,8c,8c,8g,8c,8g,8c,p,8p,8d,8p,8c,16e,16p,8e,8d,8c,";

static const char rtttl_staying_alive[] PROGMEM =
  "StayAlive:d=4,o=5,b=100:16c6,16c6,16c6,16c6,16c6,16a#,16a#,8a#,16g#,16g#,16g#,16g#,16g,16g,8g,16g#,16g#,g#,16g,16g#,16g#,g#,c6,a#,g#,g,16g,16g#,16g#,g#,16g,16g#,16g#,g#,c6,a#,g#,g,16g#,16g#,16g#,2c6,2a#,16c6,16a#,1g#,2g,c6,8a#,c6,";

static const char rtttl_small_things[] PROGMEM =
  "SmallThings:d=4,o=5,b=160:8e6,8e6,e6,e6,p,8e6,8f6,e6,d6,p,8d6,8e6,d6,c6,p,8e6,8e6,e6,c.6,e.6,8e6,e6,8e6,e6,8c6,c6,c6,c6,8d6,d6,8d6,d6,8d6,f6,8c6,c6,c6,c6,";

static const char rtttl_dammit[] PROGMEM =
  "Dammit:d=4,o=5,b=125:c7,8c7,d7,8d7,e7,g6,8g6,d7,8d7,e7,a6,8a6,d7,8d7,e7,f6,8f6,e7,8e7,d7,c7,8c7,d7,8d7,e7,g6,8g6,d7,8d7,e7,a6,8a6,d7,8d7,e7,f6,8f6,e7,8e7,d7,";

static const char rtttl_oops[] PROGMEM =
  "Oops:d=4,o=5,b=100:g#6,c#6,8c6,8c#6,8d#6,c#.6,8p,8b,8b,8d#6,8f#6,e.6,8p,8b,8f#6,8g#6,8f#6,e.6,8p,8g#6,8f#6,8e6,8c#6,8c6,g#6,c#6,8c6,8c#6,8d#6,16e6,16d#6,c#6,8p,16b,16b,8b,8d#6,8f#6,a6,g#6,f#6,8e6,8p,8e6,g#6,g#6,8f#6,8e6,c#6,";

static const char rtttl_cantina_rtttl[] PROGMEM =
  "Cantina:d=4,o=5,b=250:8a,8p,8d6,8p,8a,8p,8d6,8p,8a,8d6,8p,8a,8p,8g#,a,8a,8g#,8a,g,8f#,8g,8f#,f.,8d.,16p,p.,8a,8p,8d6,8p,8a,8p,8d6,8p,8a,8d6,8p,8a,8p,8g#,8a,8p,8g,8p,g.,8f#,8g,8p,8c6,a#,a,g";

static const char rtttl_genie[] PROGMEM =
  "Genie:d=4,o=5,b=90:16f6,16c6,16g#,8d#.6,8d#6,16p,8c#6,16c#6,16c6,16a#,16g#,16g,16f,16g#,16c6,16g6,16p,8f6,16d#6,16p,16g,16a#,16d#6,16p,8f6,16c#6,16p,16f,16g#,16d#6,16p,8c#.6,";

static const char rtttl_bulls[] PROGMEM =
  "Bulls:d=4,o=5,b=112:16f#,16a,16b,16f#,16c#6,16f#,1d.6,d.6,8c#6,16b,1b.,16f#,8b,8c#6,8d6,8e6,f#.6,16e6,16d6,1f#6,8f#6,8f#6,16f#6,16e6,16d6,1b6,";

static const char rtttl_fill_me_in[] PROGMEM =
  "FillMeIn:d=4,o=5,b=125:8c,8d#,g,8f,8d#,8d#,8c,d#,8d#.,16c,d#,c,8c,8d#,g,8f,8d#,8d#,8c,d#,8d#.,16c,d#,c,8c,8d#,g,8f,8d#,8d#,8c,d#,8d#.,16c,d#,c,8c,8d#,f,g,f,g,8f.,16d#,2c.,";

static const char rtttl_higher[] PROGMEM =
  "Higher:d=4,o=5,b=80:8d6,8d6,c#6,e6,d6,b,2p,8p,8d6,8d6,c#6,e6,d6,b,a,p,8p,8d6,8d6,c#6,e6,d6,b,2p,8p,8d6,8d6,c#6,e6,d6,b,a,";

static const char rtttl_ruff_ryders[] PROGMEM =
  "RuffRyders:d=4,o=5,b=90:16a#,8a#,16a#,8d#6,8d#6,16a#,8a#,16a#,8c#6,8c#6,8a#.,16a#,8c#6,8c#6,8a#,8c#6,8a#,8c#6,8a#.,16a#,8c#6,8c#6,8g#,8g#,8c#6,8c#6,8a#.,16a#,8a#.,16a#,8a#.,16a#,a#,";

static const char rtttl_sandstorm_rtttl[] PROGMEM =
  "Sandstorm:d=4,o=5,b=225:8e,8e,8e,8e,8e.,8e,8e,8e,8e,8e,8e,8e.,8a,8a,8a,8a,8a,8a,8a.,8g,8g,8g,8g,8g,8g,8g.,8d,8d,8d,8d,8d,8d,8d.,8e,8e,8e,8e,8e,8e,8e.,8a,8a,8e,8e,8e,8e,8e,8e,8e.,";

static const char rtttl_say_my_name[] PROGMEM =
  "SayMyName:d=4,o=5,b=70:16g6,16g6,16g6,16f6,16f6,16d#6,8d#.6,16a#,16a#,16c6,16d#6,16c6,8f6,8c6,16a#,16c6,16d#6,16c6,8f6,8c6,16a#,16c6,16d#6,16d#6,8d#6,16g6,16g6,16g6,16f6,16f6,16d#6,8d#.6,16g6,16g6,16f6,16f6,16d#6,8f6,8c6,16a#,16c6,16d#6,16c6,8f6,8c6,";

static const char rtttl_still_dre_rtttl[] PROGMEM =
  "StillDRE:d=4,o=5,b=100:8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8g6,8g6,8g6,8g6,8g6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8g6,8g6,8g6,8g6,8g6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8a6,8g6,8g6,8g6,8g6,8g6,";

static const char rtttl_forgot_dre[] PROGMEM =
  "ForgotDre:d=4,o=5,b=140:2g,p,8c6,8a6,a#.6,8p,a.6,8p,2d#.6,2p,d.6,8p,g#.,2g,p,8c6,8a6,a#.6,8p,a.6,8p,2d#.6,2p,d.6,8p,g#.,2g,p,8c6,8a6,a#.6,8p,a.6,8p,2d#.6,2p,d.6,8p,g#.,";

static const char rtttl_next_episode[] PROGMEM =
  "NextEpisode:d=4,o=5,b=90:8e,8p,8b,8p,16b,16p,16a,16p,8b,8p,16a,16p,16g,16p,8a,8p,16a,16p,16g,16p,16e,16p,16g,16p,8e,8p,8b,8p,16b,16p,16a,16p,8b,8p,16a,16p,16g,16p,8a,8p,16a,16p,16g,16p,16e,16p,16g,";

static const char rtttl_blue[] PROGMEM =
  "Blue:d=4,o=5,b=125:a,8a#,8d,8g,8a#,8c6,8f,8a,a#,8g,8a#,8d6,8d#6,8g,8d6,8c6,8a#,8d,8g,8a#,8c6,8f,8a,a#,8g,8a#,8d6,8d#6,8g,8d6,8c6,8a#,8d,8g,8a#,8a,8c,8f,g.,";

static const char rtttl_my_name_is[] PROGMEM =
  "MyNameIs:d=4,o=5,b=63:16f,16f6,p,16g,16g6,p,16a#,16a#6,p,16g#,16p,16f#6,16p,16f6,16d#6,16p,";

static const char rtttl_slim_shady[] PROGMEM =
  "SlimShady:d=4,o=5,b=100:8c6,8d#6,8g,8g#,8c6,p,8g#,8g,p,8g#,16g#,16g,8g,8f#,8g,8c6,8d#6,8g,8g#,8c6,p,8g#,8g,p,8g#,16g#,16g,8g,8f#,8g,8c6,8d#6,8g,8g#,8c6,p,8g#,8g,p,8g#,16g#,16g,8g,8f#,8g";

static const char rtttl_careless[] PROGMEM =
  "Careless:d=4,o=5,b=90:8f6,8e7,16d7,8a6,8f6,8e7,16d7,8a6,8f6,8c7,";

static const char rtttl_georgia[] PROGMEM =
  "Georgia:d=4,o=5,b=63:8e,2g.,8p,8e,2d.,8p,p,e,a,e,2d.,8c,8d,e,g,b,a,f,f,8e,e,1c";

static const char rtttl_tears[] PROGMEM =
  "Tears:d=4,o=5,b=112:p,8b,8g,d6,8d6,8b,16a,g.,2p,p,8c6,8c6,8b,8a,8g,b,2a";

static const char rtttl_banjos[] PROGMEM =
  "Banjos:d=4,o=5,b=200:8c#,8d,e,c#,d,b4,c#,d#4,b4,p,16c#6,16p,16d6,16p,8e6,8p,8c#6,8p,8d6,8p,8b,8p,8c#6,8p,8a,8p,b,p,a4,a4,b4,c#,d#4,c#,b4,p,8a,8p,8a,8p,8b,8p,8c#6,8p,8a,8p,8c#6,8p,8b";

static const char rtttl_california[] PROGMEM =
  "CalifLove:d=4,o=5,b=90:8g.6,16f6,8c#6,8d6,8f6,d.6,2p,8g,8a#,8d6,8d6,p,8p,8g,8g,16g,8f.,8g,2p,8g,16a#,16g,8d6,2d6,16g,8g.,g,2p,8g,8a#,8d6,2d.6,16g,16g,8g,8f,8g,p,8g,8c6,8c6,8a#,8a,g,p,8g,c6,8a#,8a,2g,";

static const char rtttl_all_eyez[] PROGMEM =
  "AllEyez:d=4,o=5,b=90:16f6,16e6,8d6,16a#,16a,2f,p,8a,8a,16g,8a,p,8p,16f6,16e6,8d6,16a#,16a,2f,p,8a,8a,16g,8a,";

static const char rtttl_hit_em_up[] PROGMEM =
  "HitEmUp:d=4,o=5,b=200:a,8p,a,8g,a,p,a,c6,a,d6,8p,d6,8c6,d6,2p,8p,8d6,8d#6,e6,8p,e6,8d6,e6,p,c6,8b,8g,e,a.,a,8g,a,";

static const char rtttl_mad_at_cha[] PROGMEM =
  "MadAtCha:d=4,o=5,b=80:16d6,16e6,16f6,16e6,8d6,8a,8p,f#6,2a6,8g6,16a6,16a#6,16a6,8g6,8e6,8c6,16c6,16d6,16e6,16d6,8c6,8g,8p,g6,a#6,a.6,8a6,g.6,";

static const char rtttl_mamma_mia[] PROGMEM =
  "MammaMia:d=4,o=5,b=40:32f6,32d#6,32f6,8d#6,32d#6,32d#6,32f6,32g6,32f6,16d#.6,32p,16f6,8d#6,16g#6,32g#6,32g#6,32g#6,16g6,16d#.6,32p,8a#6,32a#6,32a#6,16a#6,16f6,16g6,8g#6,16g6,16g6,32g6,16g6,16d6,16d#6,8f6,16f6,8d#6,16g#6,32g#6,32g#6,32g#6,32g6,32d#6,32f6,16d#6,";

static const char rtttl_better_off[] PROGMEM =
  "BetterOff:d=4,o=5,b=125:a,8a,f#,a,a,g#,8e,8e.6,8e.6,8c#6,a,8a,f#,a,a,g#,8e,8d.6,8d.6,8c#6,a,8a,f#,a,a,g#,8e,8e.6,8e.6,8c#6,a,8a,f#,a,a,g#,8d,8d.6,8d.6,8c#6,";

static const char rtttl_money[] PROGMEM =
  "Money:d=4,o=5,b=112:8e7,8e7,8e7,8e7,8e7,8e7,16e6,16a6,16c7,16e7,8d#7,8d#7,8d#7,8d#7,8d#7,8d#7,16f6,16a6,16c7,16d#7,d7,8c7,8a6,8c7,c7,2a6,32a6,32c7,32e7,8a7,";

static const char rtttl_larger_life[] PROGMEM =
  "LargerLife:d=4,o=5,b=112:d#6,d#6,8d#6,8d#6,8d#6,8c6,d6,8d6,8c6,2c6,8d#6,8d#6,8d#6,8d#6,8d#6,8c6,8c6,8c6,d6,8d6,c.6,p,8d#6,8d#6,8d#6,8c6,d#6,p,8d6,8d6,8d6,8c6,c6,g,d.6,c.6,d#6,d.6,c.6,a#,c6,";

static const char rtttl_jurassic[] PROGMEM =
  "Jurassic:d=32,o=6,b=28:p,b5,a#5,8b5,16p,b5,a#5,8b5,16p,b5,a#5,16b.5,c#,16c#.,e,8e,16p,d#,b5,16c#.,a#5,16f#5,d#,b5,8c#,16p,f#,b5,16e.,d#,16d#.,c#,8c#.";

static const char rtttl_mash[] PROGMEM =
  "Mash:d=8,o=5,b=140:4a,4g,f#,g,p,f#,p,g,p,f#,p,2e.,p,f#,e,4f#,e,f#,p,e,p,4d.,p,f#,4e,d,e,p,d,p,e,p,d,p,2c#.,p,d,c#,4d,c#,d,p,e,p,4f#,p,a,p,4b,a,b,p,a,p,b,p,2a.,4p,a,b,a,4b,a,b,p,2a.,a,4f#,a,b,p,d6,p,4e.6,d6,b,p,a,p,2b";

static const char rtttl_muppets[] PROGMEM =
  "Muppets:d=16,o=5,b=225:4c7,4c7,4a6,4b6,8a6,4b6,4g6,4p,4c7,4c7,4a6,8b6,8a6,8p,4g6,4p,4e6,4e6,4g6,4f6,8e6,4f6,8c7,8c6,8d6,4e6,8e6,8e6,8p,8e6,4g6";

static const char rtttl_phantom_rtttl[] PROGMEM =
  "Phantom:d=16,o=5,b=100:4e,4a,4e,4g,8f,2f,4d,4g,8d,1e,4e,4a,4c6,4e6,8d6,2d6,4d6,4g6,8d6,1e6,4e6,1a6,8g6,8f6,8e6,8d6,8c6,8b,8a,1g#,4f,4f,8e,1e";

static const char rtttl_seinfeld[] PROGMEM =
  "Seinfeld:d=16,o=5,b=56:32p,c#.,32c#,f,g#,8b,f.,f#.,8f#,e,8d#,d.,c#,32c#,c#,d#,d#,8e,f.,8f#";

static const char rtttl_sex_city[] PROGMEM =
  "SexCity:d=4,o=5,b=160:8e,e,a,c6,e6,8e6,8f#6,8g#6,a.6,8e6,g#.6,8e6,g.6,8e6,f#.6,8e,a,8a,8d#6,16d6,16c6,8d6,16a";

static const char rtttl_simpsons[] PROGMEM =
  "Simpsons:d=16,o=5,b=160:8c6,8a,4p,8c6,8a6,4p,8c6,8a,8c6,8a,8c6,8a6,4p,8p,8c6,8d6,8e6,8p,8e6,8f6,8g6,4p,8d6,8c6,4d6,8f6,4a#6,4a6,2c7";

static const char rtttl_golden_girls[] PROGMEM =
  "GoldenGi:d=8,o=6,b=125:d,c,a#5,c,a#5,g5,2a#5,1p,d,c,c,a#5,a#5,g5,d,c,c,4a#.5,4p,g5,c,4c,4d.,c,a#5,4d,c,a#5,4c,c,4a#.5";

static const char rtttl_friends[] PROGMEM =
  "Friends:d=16,o=5,b=180:4c#6,8c#6,8b,8a,4g,4g,8a,4b,4a,4p,4c#6,8c#6,8b,8a,4g,4g,8a,4b,4a,4c#6,4c#6,8c#6,8b,8a,4g,4g,8a,4b,4a,4p,4c#6,8c#6,8b,8a,4g,4g,8a,4b,4a";

static const char rtttl_jeopardy[] PROGMEM =
  "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#";

static const char rtttl_a_team[] PROGMEM =
  "A-Team:d=4,o=5,b=125:4d#6,8a#,2d#6,16p,8g#,4a#,4d#.,8p,16g,16a#,8d#6,8a#,8f6,2d#6,16p,8c#.6,16c6,16a#,8g#.,2a#";

static const char rtttl_addams[] PROGMEM =
  "AddamsFa:d=4,o=6,b=50:32p,32c#,16f#,32a#,16f#,32c#,16c,8g#,32f#,16f,32g#,16f,32c#,16a#5,8f#,32c#,16f#,32a#,16f#,32c#,16c,8g#,32f#,16f,32c#,16d#,32f,f#,32c#,32d#,32f,f#,32c#,32d#,32g,g#,32d#,32f,32g,16g#.,32d#,32f,32g,16g#.,32c#,32d#,32f,32f#";

static const char rtttl_back_future[] PROGMEM =
  "BackToFuture:d=4,o=5,b=160:32p,p,8c.,16p,g,16p,16c.6,16p,a#.,16p,16a,16p,16g,16p,8a,16p,8g,16p,8f,16p,1g.,1p,g.,16p,c.,16p,2f#.,16p,32g.,32p,32a.,32p,8g,32p,8e,32p,8c,32p,f#,16p,32g.,32p,32a.,32p,8g.,32p,8d.,32p,8g.,32p,8d.6,32p,d6,16p,c#6,16p,32b.,32p,32c#.6,32p,2d6";

static const char rtttl_zorba[] PROGMEM =
  "Zorba2:d=4,o=5,b=125:16c#6,2d6,2p,16c#6,2d6,2p,32e6,32d6,32c#6,2d6,2p,16c#6,2d6,2p,16b,2c6,2p,32d6,32c6,32b,2c6,2p,16a#,2b,p,8p,32c6,32b,32a,32g,32b,2a,2p,32a,32g,32f#,32a,1g,1p,8c#6,8d6,8d6,8d6,8d6,8d6,8d6,8d6,8c#6,8d6,8d6,8d6,8d6,8d6,16e6,16d6,16c#6,16e6,8c#6,8d6,8d6,8d6,8d6,8d6,8d6,8d6,8c#6,8d6,8d6,8d6,8d6,8d6";

static const char rtttl_phil_collins[] PROGMEM =
  "PhilCollins:d=4,o=6,b=40:32g#.,16g#.,16c#.7,16c#.7,16g#.,f#,32g#.,16g#.,16d#.7,16d#.7,16f#.,f,32g#.,32g#.,16c#.7,16c#.7,16g#.,16f#.,16d#.7,16d#.7,16f.,8d#.";

static const char rtttl_police[] PROGMEM =
  "Police:d=4,o=6,b=45:32c#6,32f#5,32f#5,16f#5,32c#5,8f#5,32f#5,32f#5,32c#6,32f#5,32f#5,16f#5,32f#6,8c#6,32g#5,32f#5,16g#5,16c#5,16d#5,8c#.5,16c#.6,16f#5,32g#5,16f#.5,16f#.6,32f6,32f6,32f6,16d#6,32d#6,8c#6,32c#6,32c#6,16d#6,16c#6,16c#6,8c#6,32g#5,32f#5,16g#5,16c#5,16d#5,8c#.5,16c#.6,16f#5,32g#5,16f#.5,16f#.6,32f6,32f6,32f6,16d#6,32d#6,8c#6,32c#6,32c#6,16d#6,16c#6,16f#6,8c#6,32g#5,32f#5,16g#5,16c#5,16c#6,16c#.6,";

static const char rtttl_poison[] PROGMEM =
  "Poison:d=4,o=5,b=112:8d,8d,8a,8d,8e6,8d,8d6,8d,8f#,8g,8c6,8f#,8g,8c6,8e,8d,8d,8d,8a,8d,8e6,8d,8d6,8d,8f#,8g,8c6,8f#,8g,8c6,8e,8d,8c,8d,8a,8d,8e6,8d,8d6,8d,8f#,8g,8c6,8f#,8g,8c6,8e,8d,8c,8d,8a,8d,8e6,8d,8d6,8d,8a,8d,8e6,8d,8d6,8d,2a,8d";

static const char rtttl_titanic[] PROGMEM =
  "Titanic:d=4,o=6,b=40:32c#.6,32d#.6,4f6,32d#.6,32c#.6,32d#.6,32g#.6,8g#.6,32f#.6,32f.6,16c#.6,8a#.5,32g#.5,32f#.5,32d#5,32f5,8d#.5,32c#.6,32d#.6,4f6,32d#.6,32c#.6,32d#.6,32g#.6,8g#.6,32f.6,32g#.6,8a#.6,8g#.6,32d#6,32f6,8d#.6,";

static const char rtttl_tlc_noscrub[] PROGMEM =
  "TLC:d=4,o=6,b=70:32d.5,32a.5,32c.6,4f6,32a.4,32a.5,32c.6,4e6,32e.4,32g#.5,32b.5,4d6,32a.4,32a.5,32c.6,4e6,32d.5,32a.5,32c.6,4f6,32a.4,32a.5,32c.6,4e6,32e.4,32g#.5,32b.5,8d.6,16g.6,32f.6,32e.6,32d.6,8e.6,32d.5,32a.5,32c.6,4f6,32a.4,32a.5,32c.6,4e6,32e.4,32g#.5,32b.5,4d6,32a.4,32a.5,32c.6,4e6,32d.5,32a.5,32c.6,4f6,32a.4,32a.5,32c.6,4e6,32e.4,32g#.5,32b.5,8d.6,16g.6,32f.6,32e.6,32d.6,8e.6,";

static const char rtttl_ymca[] PROGMEM =
  "YMCA:d=4,o=5,b=160:8c#6,8a#,2p,8a#,8g#,8f#,8g#,8a#,c#6,8a#,c#6,8d#6,8a#,2p,8a#,8g#,8f#,8g#,8a#,c#6,8a#,c#6,8d#6,8b,2p,8b,8a#,8g#,8a#,8b,d#6,8f#6,d#6,f.6,d#.6,c#.6,b.,a#,g#";

static const char rtttl_you_the_one[] PROGMEM =
  "YouTheOne:d=4,o=6,b=28:16g#,16a#,c7,16g#,16a#,c7,16f#,16a#,c7,16g#,16a#,32c7,32c#7,8c7,16c7,16f7,16d#7,16c7,8a#,g#,16f,16g#,16c7,8a#,c7,16c7,16g#,g#";

static const char rtttl_bittersweet[] PROGMEM =
  "BitterSweet:d=4,o=5,b=100:8p,32d.6,16p,32f6,16p,32d6,16p,32d#.6,16p,32c6,16p,d#6,32p,32g#6,16p,32d#6,16p,g#6,32p,32g6,16p,32d#.6,16p,g6,8p,32d.6,16p,32f6,16p,32d6,16p,32d#.6,16p,32c6,16p,d#6,32p,32g#6,16p,32d#6,16p,g#6,32p,32g6,16p,32d#.6,16p,g6";

static const char rtttl_countdown[] PROGMEM =
  "FinalCountdown:d=4,o=5,b=125:p,8p,16b,16a,b,e,p,8p,16c6,16b,8c6,8b,a,p,8p,16c6,16b,c6,e,p,8p,16a,16g,8a,8g,8f#,8a,g.,16f#,16g,a.,16g,16a,8b,8a,8g,8f#,e,c6,2b.,16b,16c6,16b,16a,1b";

static const char rtttl_good_bad_ugly[] PROGMEM =
  "GoodBadUgly:d=16,o=5,b=125:a,d6,a,d6,2a,4f,4g,2d,4p,a,d6,a,d6,2a,4f,4g,2c6,4p,a,d6,a,d6,2a,4f,8e,8d,2c,4p,a,d6,a,d6,2a,4g,2d.";

static const char rtttl_knight_rider[] PROGMEM =
  "KnightRider:d=4,o=5,b=125:16e,16p,16f,16e,16e,16p,16e,16e,16f,16e,16e,16e,16d#,16e,16e,16e,16e,16p,16f,16e,16e,16p,16f,16e,16f,16e,16e,16e,16d#,16e,16e,16e,16d,16p,16e,16d,16d,16p,16e,16d,16e,16d,16d,16d,16c,16d,16d,16d,16d,16p,16e,16d,16d,16p,16e,16d,16e,16d,16d,16d,16c,16d,16d,16d";

static const char rtttl_master_house[] PROGMEM =
  "MasterHouse:d=4,o=5,b=100:16a,16a,16a,16a,8e,8p,16a,16a,16a,16a,8e,8p,16a,16a,16a,16a,16a,16g#,16a,16b,8c#6,8a,8e,8p";

static const char rtttl_losing_religion[] PROGMEM =
  "LosingReligion:d=4,o=5,b=63:2p,8b,8c#6,8b,8f#,a.,8a,8a,a,a,a.,8b,8c#6,8b,8f#,a.,8a,8a,a,a.,8b,8c#6,8b,8f#,a.,8a,8a,a,a.,8b,8c#6,8b,8f#,a,a,8a,a,8g#,2g#";

static const char rtttl_flashdance[] PROGMEM =
  "Flashdance:d=4,o=5,b=140:8f,8a,a#.,a#,1p,8p,8f,8a,c.6,8a#,8a#,8f,g,1p,8f,8a,a#,a#,c6,8a#,8a,a#,a#,c6,8f,8f,1f,";

static const char rtttl_ipanema[] PROGMEM =
  "GirlIpanema:d=4,o=5,b=160:g.,8e,8e,d,g.,8e,e,8e,8d,g.,e,e,8d,g,8g,8e,e,8e,8d,f,d,d,8d,8c,e,c,c,8c,a#4,2c";

static const char rtttl_dawsons_creek[] PROGMEM =
  "DawsonsCreek:d=4,o=5,b=125:16d,16e,16e,16e,e,2p,16e,g,g,16f,f,e,d,c,c,16f,f,16p,c,c,16e,e,d,d,2p,d,c,p,16g,g,g,16p,f,e,16p,d,16g,g,g,16p,f,e,16p,d,";

static const char rtttl_big_pimpin[] PROGMEM =
  "BigPimpin:d=4,o=5,b=63:8c6,8p,8a,8p,8g#,16p,8a,16p,8a,8p,8d6,8e6,8d6,16e6,32f6,8e6,32p,16d6,16c#6,8d6,32p,8c6,8p,8a,8p,8g#,16p,8a,16p,8a,8p,8b,8c#6,8d6,16e6,32f6,8e6,32p,16d6,16c#6,8d6,";

static const char rtttl_rendezvous[] PROGMEM =
  "Rendezvous:d=4,o=5,b=80:8g.,16f,8f,8g,8f,8g,8f,8g,8f,16p,16f,16g,16f,16f,16p,16g,16f,16f,16f,16g,16f,16f,16p,8p,a,8a,8g,8f,8d,8a,8g,8a,16p,8p,16g,16f,g,";

static const char rtttl_one_week[] PROGMEM =
  "OneWeek:d=4,o=5,b=112:8f#,8a,8p,8c#,8c#6,16a,16a,8b,16a,8a.,8p,8c#6,16c#6,16c#6,16c#6,16c#6,8d6,16a,16a,16a,8b,8a,8p,8c#6,8c#6,16a,16a,8b,16a,8a,16a,16a,16c#.6,16c#.6,16c#.6,16c#6,8c#6,8d6,8a,16a,8b,8a,";

static const char rtttl_austin_powers[] PROGMEM =
  "AustinPowers:d=4,o=5,b=140:8f6,8a#6,8a#6,8g6,8g6,p,8p,8f6,8a#6,8a#6,8g6,8g6,p,8p,8f6,8a#6,8a#6,8f6,8f6,p,8p,8f6,8a#6,8a#6,8f6,8f6,p,8p,8a#6,8a#6,8g6,8a#6,8a#6,p,8p,8a#6,8a#6,8g6,8a6,8a6,p,8p,8f6,8a#6,8a#6,f6,";

static const char rtttl_brandenburg[] PROGMEM =
  "Brandenburg:d=4,o=5,b=125:8e6,8f#6,16e6,16d6,8e6,16d6,16c#6,8d6,16c#6,16b,16a,16d6,16a,16d6,8b,16a,16g,16f#,16d6,16f#,16d6,8g,16f#,16e,16d,16d6,16e,16d6,16f#,16d6,16g#,16d6,16a,16c#6,16a,16d6,16a,16e6,16a,16f#6,16a,16g6,16a,16a6,8f#6,16e6,16d6,8a,8c#6,d6,";

static const char rtttl_toccata[] PROGMEM =
  "Toccata:d=4,o=5,b=125:a,16d6,16a,16e6,16a,16f6,16a,16d6,16a,16e6,16a,16f6,16a,16g6,16a,16e6,16a,16f6,16a,16g6,16a,16a6,16a,16f6,16a,16g6,16a,16a6,16a,16a#6,16a,16g6,16a,16a6,16a,16f6,16a,16g6,16a,16e6,16a,16f6,16a,16d6,16a,16e6,16a,16c#6,16a,2d6,";

static const char rtttl_around_world[] PROGMEM =
  "AroundWorld:d=4,o=5,b=140:8e6,8a,8c6,8e6,8b,8e,8g,8b,8c6,8f,8a,8c6,8d6,8g,8b,8d6,8e6,8a,8c6,8e6,8b,8e,8g,8b,8c6,8f,8a,8c6,8d6,8g,8b,8d6,8e6,8a,8c6,8e6,8b,8e,8g,8b,8c6,8f,8a,8c6,8d6,8g,8b,8d6,";

// ========== MML Songs ==========

static const char mml_sandstorm[] PROGMEM =
  "MML@V100<e8l32.&er64e8&er64e8e8&er64e8&er64g8g8&gr64g8&gr64g8g8&gr64g8&gr64d8e8&er64e8&er64e8e8&er64e8&er64a8e8&er64e8&er64e8e8&er64e8&er64a8e8&er64e8&er64e8e8&er64e8&er64g8g8&gr64g8&gr64g8g8&gr64g8&gr64d8e8&er64e8&er64e8e8&er64e8&er64g8e8&er64e8&er64e8e8&er64e8&er64g8e8&er64e8&er64e8e8&er64e8&er64g8g8&gr64g8&gr64g8g8&gr64g8&gr64d8e8&er64e8&er64e8e8&er64e8&er64g8e8&er64e8&er64e8e8&er64e8&er64g8e8&er64e8&er64e8e8&er64e8&er64e8e8&er64e8&er64e8e8&er64e8&er64e8e8&er64e8&er64e8e8.&er64<b8.&br64g8.&gr64V120<e4l8e>gbgegbb+egb+bdgbf+egbgegbaegbgegba<e>gbgegbb+egb+bdgbf+egbgegbaegbgegba<e>gbgegbb+egb+bdgbf+egbgegbaegbgegba<e>gbgegbb+egb+bdgbf+egbgegbaegbgegbaegbgegbgegbgegbgegbgegbb+egb+adgbf+egbgegbgegbgegbgegbgegbb+egb+adgbf+egbgegbgegbgegbgegbgegbgegbgegbgc-ege<gb>e<begbge>e<e>e<e>e<e>el16<egb>e<egb>el32<egb>ege<bgegb>egb>eg;";

static const char mml_entertainer[] PROGMEM =
  "MML@v127t160d8&d32e16.c8&c32<a4b16.g4>d8&d32e16.c8&c32<a4b16.g1&g2.g4d8&d32d+16.e8&e32>c4<e16.>c4<e8&e32>c2.c16.d8&d32d+16.e8&e32c16.d8&d32e4<b16.>d4c2.<b2g4g4b4f4>c4<e4d+4d4f+4f+4a4b4<g4a4b2>g4g4>c4<f4>c4<e4>c2<g2b2g4>c2c4c4<a+4>c4<a4>c4<g+4>c4<g4>c2<b4>c4<g4a4b4e8&e32>c4<e16.>c4<e8&e32>c2.c16.d8&d32d+16.e8&e32c16.d8&d32e4<b16.>d4c2.<b2g4g4b4f4>c4<e4d+4d4f+4f+4a4b4<g4a4b2>g4g4>c4<f4>c4<e4>c2<g2b2g4>c2c4c4<a+4>c4<a4>c4<g+4>c4<g4>c2<b4>c4<g4c2.g2g2>c2c2f2f2e2e2e2c2e4e4d+4d4<b2>d4<g4f4e4d2>c2c2c2c2c2c2c2e4c4c4c+4c+4c4c4c4c4c4<a4b4b4>c4<g4c2c4e4<g4>g4<f4>a4<e4>g4<g4>e4<g4>f4c4e4e4g2g4g4>c4<f4>c4<e4d+4d4f+2a4b4g4a4d8&d32d+16.e8&e32>c4<e16.>c4<e8&e32>c2.c16.d8&d32d+16.e8&e32c16.d8&d32e4<b16.>d4c2.c8&c32d16.e8&e32c16.d8&d32e4c16.d8&d32c16.e8&e32c16.d8&d32e4c16.d8&d32c16.e8&e32c16.d8&d32e4<b16.>d4c1;";

static const char mml_baby_shark[] PROGMEM =
  "MML@v127t115<d4.&d16.r32d+8r4.d4.&d16.r32d+8r2.r8>>d8d+8d8d+8d8d+8d8d+8<<g4.&g16.r32>d4.&d16.r32e4.&e16.r32g4.&g16.r32d4.&d16.r32g4.&g16.r32d8r4.>d8.&d32.r64e8.&e32.r64g8g8g8g16g8g16l8gdegggg16g8g16l8gdegggg16g8g16g8g8g8f+4.&f+16.r32d8.&d32.r64e8.&e32.r64g8g8g8g16g8g16l8gdegggg16g8g16l8gdegggg16g8g16g8g8g8f+4.&f+16.r32d8.&d32.r64e8.&e32.r64g8g8g8g16g8g16l8gdegggg16g8g16l8gdegggg16g8g16g8g8g8f+4.&f+16.r32d8.&d32.r64e8.&e32.r64d8d8d8d16d8d16l8ddecccc16c8c16l8cdeeeee16e8e16e8g8g8d4.&d16.r32d8.&d32.r64e8.&e32.r64<g8g8g8g16g16g16g16l8gabgggg16g16g16g16l8gabgggl16ggggg8>e8e8<a4.&a16.r32>d8.&d32.r64e8.&e32.r64<g8g8g8l16ggggl8gabgggl16ggggl8gabgggl16ggggg8>e8e8<a4.&a16.r32t120>d+8.&d+32.r64f8.&f32.r64<g+8g+8g+8l16g+g+g+g+l8g+a+>c<g+g+g+l16g+g+g+g+l8g+a+>c<g+g+g+l16g+g+g+g+g+8>f8f8<a+2.&a+8.&a+64r32.>d+8.&d+32.r64f8.&f32.r64t109l8g+g+g+g+16g+8g+16l8g+d+fg+g+g+g+16g+8g+16l8g+d+fg+g+g+g+16g+8g+16g+8g+8g+8g4.&g16.r32<a+8.&a+32.r64>c8.&c32.r64t115<g+8g+8g+8l16g+g+g+g+l8g+a+>c<g+g+g+l16g+g+g+g+l8g+a+>c<g+g+g+l16g+g+g+g+g+8>f8f8<a+2.&a+8.&a+64r32.g+2.&g+8.&g+32;";

static const char mml_africa[] PROGMEM =
  "MML@t89r1r1r1r1<e8.e8e8e16e8d+8g+8>>c+16<b16g+16b16>c+16<b16g+16b16>c+16<b16g+16f+16g+16f+16g+16>c+16<b8<e8.e8e8e16e8d+8g+8>>c+16<b16g+16b16>c+16<b16g+16b16>c+16<b16g+16f+16g+16f+16g+16>c+16<b8<e8.e8e8e16e8d+8g+8>>c+16<b16g+16b16>c+16<b16g+16b16>c+16<b16g+16f+16g+16f+16g+16>c+16<b8<e8.e8e8e16e8d+8g+8>>c+16<b16g+16b16>c+16<b16g+16b16>c+16<b16g+16f+16g+16f+16g+16>c+16<b8<f+2>f+16f+8.g+8a+16b2&b16d+4e8f+16f+8e8.e8d+16c+4&c+16c+8<b8>d+4&d+16c+16<b8>c+8.<e8e8e16e8d+8g+4f+2a+2g+1>f+8e16e8d+8c+4&c+16c+8<b8>d+4&d+16c+16<b8>c+4&c+16<e8e16e8d+8g+4f+2>f+16f+8.e8f+16g+2&g+16<b8b4.e2g+2f+2>c+4&c+16<e8e16e8d+8g+4f+2>f+16f+8.e8f+16g+2&g+16b16b8.>c+8d+8d+8c+16c+8<b16>c+1&c+2&c+8<f+2>d4d4a4.<b16b2&b16c+2>d16d8d8d8c+4c+16c+16<b8b2&b16c+2d2&d8.c+16<b2.&b8>>c+8c+16c+16c+16c+16d16d8d8d8c+4c+16c+16<b8.b4&b16a16g+8b4&b16>c+16<b8a2;";

// ========== Song Definition Table ==========

static const SongDef songDefs[] PROGMEM = {
    // RTTTL songs
    { rtttl_fiddler,         "Fiddler on the Roof",         FMT_RTTTL },
    { rtttl_larry,           "Larry David Theme",           FMT_RTTTL },
    { rtttl_macarena,        "Macarena",                    FMT_RTTTL },
    { rtttl_who_let_dogs,    "Who Let The Dogs Out",        FMT_RTTTL },
    { rtttl_staying_alive,   "Staying Alive",               FMT_RTTTL },
    { rtttl_small_things,    "All The Small Things",        FMT_RTTTL },
    { rtttl_dammit,          "Dammit",                      FMT_RTTTL },
    { rtttl_oops,            "Oops I Did It Again",         FMT_RTTTL },
    { rtttl_cantina_rtttl,   "Cantina Band (RTTTL)",        FMT_RTTTL },
    { rtttl_genie,           "Genie In A Bottle",           FMT_RTTTL },
    { rtttl_bulls,           "Chicago Bulls Theme",         FMT_RTTTL },
    { rtttl_fill_me_in,      "Fill Me In",                  FMT_RTTTL },
    { rtttl_higher,          "Higher",                      FMT_RTTTL },
    { rtttl_ruff_ryders,     "Ruff Ryders Anthem",          FMT_RTTTL },
    { rtttl_sandstorm_rtttl, "Sandstorm (RTTTL)",           FMT_RTTTL },
    { rtttl_say_my_name,     "Say My Name",                 FMT_RTTTL },
    { rtttl_still_dre_rtttl, "Still D.R.E. (RTTTL)",        FMT_RTTTL },
    { rtttl_forgot_dre,      "Forgot About Dre",            FMT_RTTTL },
    { rtttl_next_episode,    "The Next Episode",            FMT_RTTTL },
    { rtttl_blue,            "Blue (Eiffel 65)",            FMT_RTTTL },
    { rtttl_my_name_is,      "My Name Is",                  FMT_RTTTL },
    { rtttl_slim_shady,      "Real Slim Shady",             FMT_RTTTL },
    { rtttl_careless,        "Careless Whisper",            FMT_RTTTL },
    { rtttl_georgia,         "Georgia On My Mind",          FMT_RTTTL },
    { rtttl_tears,           "Tears In Heaven",             FMT_RTTTL },
    { rtttl_banjos,          "Dueling Banjos",              FMT_RTTTL },
    { rtttl_california,      "California Love",             FMT_RTTTL },
    { rtttl_all_eyez,        "All Eyez On Me",              FMT_RTTTL },
    { rtttl_hit_em_up,       "Hit Em Up",                   FMT_RTTTL },
    { rtttl_mad_at_cha,      "I Ain't Mad At Cha",          FMT_RTTTL },
    { rtttl_mamma_mia,       "Mamma Mia",                   FMT_RTTTL },
    { rtttl_better_off,      "Better Off Alone",            FMT_RTTTL },
    { rtttl_money,           "Money Money Money",           FMT_RTTTL },
    { rtttl_larger_life,     "Larger Than Life",            FMT_RTTTL },
    { rtttl_jurassic,        "Jurassic Park",               FMT_RTTTL },
    { rtttl_mash,            "M*A*S*H",                     FMT_RTTTL },
    { rtttl_muppets,         "Muppet Show",                 FMT_RTTTL },
    { rtttl_phantom_rtttl,   "Phantom of the Opera",        FMT_RTTTL },
    { rtttl_seinfeld,        "Seinfeld",                    FMT_RTTTL },
    { rtttl_sex_city,        "Sex And The City",            FMT_RTTTL },
    { rtttl_simpsons,        "The Simpsons",                FMT_RTTTL },
    { rtttl_golden_girls,    "Golden Girls",                FMT_RTTTL },
    { rtttl_friends,         "Friends",                     FMT_RTTTL },
    { rtttl_jeopardy,        "Jeopardy",                    FMT_RTTTL },
    { rtttl_a_team,          "The A-Team",                  FMT_RTTTL },
    { rtttl_addams,          "Addams Family",               FMT_RTTTL },
    { rtttl_back_future,     "Back To The Future",          FMT_RTTTL },
    { rtttl_zorba,           "Zorba The Greek",             FMT_RTTTL },
    { rtttl_phil_collins,    "Phil Collins",                FMT_RTTTL },
    { rtttl_police,          "The Police",                  FMT_RTTTL },
    { rtttl_poison,          "Poison",                      FMT_RTTTL },
    { rtttl_titanic,         "Titanic",                     FMT_RTTTL },
    { rtttl_tlc_noscrub,     "No Scrubs",                   FMT_RTTTL },
    { rtttl_ymca,            "YMCA",                        FMT_RTTTL },
    { rtttl_you_the_one,     "You're The One",              FMT_RTTTL },
    { rtttl_bittersweet,     "Bittersweet Symphony",        FMT_RTTTL },
    { rtttl_countdown,       "The Final Countdown",         FMT_RTTTL },
    { rtttl_good_bad_ugly,   "The Good The Bad The Ugly",   FMT_RTTTL },
    { rtttl_knight_rider,    "Knight Rider",                FMT_RTTTL },
    { rtttl_master_house,    "Master Of The House",         FMT_RTTTL },
    { rtttl_losing_religion, "Losing My Religion",          FMT_RTTTL },
    { rtttl_flashdance,      "Flashdance",                  FMT_RTTTL },
    { rtttl_ipanema,         "Girl From Ipanema",           FMT_RTTTL },
    { rtttl_dawsons_creek,   "Dawson's Creek",              FMT_RTTTL },
    { rtttl_big_pimpin,      "Big Pimpin",                  FMT_RTTTL },
    { rtttl_rendezvous,      "Rendezvous",                  FMT_RTTTL },
    { rtttl_one_week,        "One Week",                    FMT_RTTTL },
    { rtttl_austin_powers,   "Austin Powers",               FMT_RTTTL },
    { rtttl_brandenburg,     "Brandenburg Concerto",        FMT_RTTTL },
    { rtttl_toccata,         "Toccata",                     FMT_RTTTL },
    { rtttl_around_world,    "Around The World",            FMT_RTTTL },
    // MML songs
    { mml_sandstorm,         "Sandstorm (MML)",             FMT_MML },
    { mml_entertainer,       "The Entertainer",             FMT_MML },
    { mml_baby_shark,        "Baby Shark",                  FMT_MML },
    { mml_africa,            "Africa",                      FMT_MML },
};

static const uint8_t SONG_DEF_COUNT = sizeof(songDefs) / sizeof(songDefs[0]);
