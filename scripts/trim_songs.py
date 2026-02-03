#!/usr/bin/env python3
"""Trim songs.h to keep only N shortest new favorites for heap testing."""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SONGS_H = ROOT / "include" / "songs.h"

# Read current file
content = SONGS_H.read_text()

# Find all mml_fav_ PROGMEM variable names and their MML string lengths
fav_pattern = re.compile(r'static const char (mml_fav_\w+)\[\] PROGMEM = "([^"]*)"')
fav_vars = {}
for m in fav_pattern.finditer(content):
    var_name = m.group(1)
    mml_len = len(m.group(2))
    fav_vars[var_name] = mml_len

print(f"Found {len(fav_vars)} favorite PROGMEM vars")

# Sort by MML length, keep only the N shortest
N = int(sys.argv[1]) if len(sys.argv) > 1 else 80
sorted_vars = sorted(fav_vars.items(), key=lambda x: x[1])
keep_vars = set(v[0] for v in sorted_vars[:N])
remove_vars = set(v[0] for v in sorted_vars[N:])

print(f"Keeping {len(keep_vars)} shortest, removing {len(remove_vars)} longest")
if remove_vars:
    print(f"Removed songs length range: {sorted_vars[N][1]} - {sorted_vars[-1][1]} chars")

# Remove PROGMEM declarations for removed vars
lines = content.split("\n")
new_lines = []
i = 0
removed_progmem = 0
while i < len(lines):
    line = lines[i]
    skip = False
    for var in remove_vars:
        if f"static const char {var}[]" in line and "PROGMEM" in line:
            skip = True
            break
    if skip:
        removed_progmem += 1
        # Remove preceding blank line
        if new_lines and new_lines[-1].strip() == "":
            new_lines.pop()
        # Skip declaration (possibly multi-line)
        if '";' in line:
            i += 1
        else:
            i += 1
            while i < len(lines) and '";' not in lines[i]:
                i += 1
            i += 1
        # Skip trailing blank
        if i < len(lines) and lines[i].strip() == "":
            i += 1
    else:
        new_lines.append(line)
        i += 1

print(f"Removed {removed_progmem} PROGMEM declarations")

# Remove songDefs entries for removed vars
final_lines = []
removed_defs = 0
for line in new_lines:
    skip = False
    for var in remove_vars:
        pattern = "{ " + var
        if pattern in line and "FMT_" in line:
            skip = True
            break
    if skip:
        removed_defs += 1
    else:
        final_lines.append(line)

print(f"Removed {removed_defs} songDefs entries")

SONGS_H.write_text("\n".join(final_lines))

# Verify
result = "\n".join(final_lines)
entries = re.findall(r'^\s*\{.*FMT_(RTTTL|MML)\s*\}', result, re.MULTILINE)
print(f"Remaining songDefs entries: {len(entries)}")
