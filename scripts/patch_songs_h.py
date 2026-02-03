#!/usr/bin/env python3
"""Patch songs.h: remove replaced songs, append new favorites, widen type."""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SONGS_H = ROOT / "include" / "songs.h"
SCRAPED = ROOT / "scraped_songs.json"

# UIDs to skip entirely
SKIP_UIDS = {
    "mab-13779", "mab-8512", "mab-18710", "mab-10299", "mab-8847",
    "mab-1212", "mab-3510", "mab-18589", "mab-868",
    "aa-benny-hill-theme-3",
    "aa-harder-better-faster-stronger-2",
}

# Songs being replaced (var name patterns to remove from PROGMEM + songDefs)
REMOVE_VARS = {"mml_entertainer", "rtttl_muppets", "rtttl_simpsons"}
REMOVE_TITLES = {"The Entertainer", "Muppet Show", "The Simpsons"}

def uid_to_varname(uid):
    name = uid.replace("-", "_")
    if name[0].isdigit():
        name = "s_" + name
    return "mml_fav_" + name

def escape_c_string(s):
    return s.replace("\\", "\\\\").replace('"', '\\"')

def escape_title(title):
    return title.replace('"', '\\"').replace("'", "\\'")

def main():
    with open(SCRAPED) as f:
        songs = json.load(f)

    approved = [s for s in songs if s["uid"] not in SKIP_UIDS and s["mml"] is not None]
    print(f"Approved songs: {len(approved)}")

    content = SONGS_H.read_text()
    lines = content.split("\n")

    # === Remove replaced PROGMEM declarations ===
    new_lines = []
    i = 0
    removed_progmem = 0
    while i < len(lines):
        line = lines[i]
        # Check if this line starts a PROGMEM declaration for a removed var
        skip = False
        for var in REMOVE_VARS:
            if f"static const char {var}[]" in line and "PROGMEM" in line:
                skip = True
                break
        if skip:
            removed_progmem += 1
            # Skip this line and the next line (the string content)
            # Also skip trailing blank line
            # Pattern: empty line, declaration, string, empty line
            # Or: declaration + string on same line, then empty
            if i > 0 and new_lines and new_lines[-1].strip() == "":
                new_lines.pop()  # remove preceding blank line
            # Check if declaration and string are on same line
            if '";' in line:
                # single line PROGMEM
                i += 1
                # skip trailing blank
                if i < len(lines) and lines[i].strip() == "":
                    i += 1
            else:
                # multi-line: skip until we see the closing ";
                i += 1
                while i < len(lines) and '";' not in lines[i]:
                    i += 1
                i += 1  # skip the closing line
                # skip trailing blank
                if i < len(lines) and lines[i].strip() == "":
                    i += 1
        else:
            new_lines.append(line)
            i += 1

    print(f"Removed {removed_progmem} PROGMEM declarations")

    # === Remove replaced songDefs entries ===
    final_lines = []
    removed_defs = 0
    for line in new_lines:
        skip = False
        for var in REMOVE_VARS:
            pattern = "{ " + var + ","
            if pattern in line:
                skip = True
                break
        # Also check by title match
        if not skip:
            for title in REMOVE_TITLES:
                if f'"{title}"' in line and "FMT_" in line:
                    skip = True
                    break
        if skip:
            removed_defs += 1
        else:
            final_lines.append(line)

    print(f"Removed {removed_defs} songDefs entries")

    # === Find insertion point: before the closing }; of songDefs ===
    content = "\n".join(final_lines)

    # Find the last FMT_MML or FMT_RTTTL line before "};"
    # Insert new entries before "};"
    songdefs_end = content.rfind("};")
    if songdefs_end == -1:
        print("ERROR: Could not find end of songDefs array")
        return

    # Find the actual "};" that closes songDefs (it's the first one after
    # the last FMT_ entry)
    # Let's find it by looking for the pattern: FMT_MML/RTTTL entries then };
    lines = content.split("\n")
    insert_idx = None
    for i in range(len(lines) - 1, -1, -1):
        if lines[i].strip() == "};":
            # Check if previous non-empty lines have FMT_ in them
            for j in range(i - 1, max(i - 5, -1), -1):
                if "FMT_" in lines[j]:
                    insert_idx = i
                    break
            if insert_idx is not None:
                break

    if insert_idx is None:
        print("ERROR: Could not find songDefs closing brace")
        return

    print(f"Inserting new entries at line {insert_idx}")

    # === Generate new PROGMEM declarations ===
    new_progmem = []
    new_progmem.append("")
    new_progmem.append("// ========== Favorites MML Songs ==========")

    for s in approved:
        var = uid_to_varname(s["uid"])
        mml = escape_c_string(s["mml"])
        new_progmem.append("")
        new_progmem.append(f'static const char {var}[] PROGMEM = "{mml}";')

    # === Generate new songDefs entries ===
    new_defs = []
    for s in approved:
        var = uid_to_varname(s["uid"])
        title = escape_title(s["title"])
        var_padded = var.ljust(45)
        title_padded = f'"{title}"'.ljust(55)
        new_defs.append(f"    {{ {var_padded}, {title_padded}, FMT_MML }},")

    # === Insert PROGMEM declarations before songDefs array ===
    # Find "static const SongDef songDefs[]" line
    songdefs_start = None
    for i, line in enumerate(lines):
        if "songDefs[]" in line and "SongDef" in line:
            songdefs_start = i
            break

    if songdefs_start is None:
        print("ERROR: Could not find songDefs array start")
        return

    # Insert PROGMEM declarations before songDefs array declaration
    result_lines = lines[:songdefs_start] + new_progmem + [""] + lines[songdefs_start:insert_idx] + new_defs + lines[insert_idx:]

    # === Widen SONG_DEF_COUNT type ===
    result = "\n".join(result_lines)
    result = result.replace(
        "static const uint8_t SONG_DEF_COUNT",
        "static const uint16_t SONG_DEF_COUNT"
    )

    SONGS_H.write_text(result)
    print(f"Written updated songs.h")

    # Count entries
    count = result.count("FMT_MML") + result.count("FMT_RTTTL")
    print(f"Total songDefs entries: {count}")

if __name__ == "__main__":
    main()
