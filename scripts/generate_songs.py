#!/usr/bin/env python3
"""Generate PROGMEM C code from scraped_songs.json for songs.h."""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
INPUT = ROOT / "scraped_songs.json"
OUTPUT = ROOT / "scripts" / "generated_songs.h.fragment"

# Songs to skip
SKIP_UIDS = {
    # mabimml fetch errors
    "mab-13779", "mab-8512", "mab-18710", "mab-10299", "mab-8847",
    "mab-1212", "mab-3510", "mab-18589", "mab-868",
    # Internal dup: second Benny Hill
    "aa-benny-hill-theme-3",
    # Internal dup: generic title version
    "aa-harder-better-faster-stronger-2",
}

# Songs that replace existing ones (uid -> existing title to remove)
REPLACE_EXISTING = {
    "aa-entertainer-amateur": "The Entertainer",
    "aa-coffin-dance": "Coffin Dance",
    "aa-the-muppet-show-theme": "Muppet Show",
    "aa-the-simpsons-main-title-theme": "The Simpsons",
}


def uid_to_varname(uid):
    """Convert uid to a valid C variable name."""
    name = uid.replace("-", "_")
    # Ensure it starts with a letter
    if name[0].isdigit():
        name = "s_" + name
    return "mml_fav_" + name


def escape_c_string(s):
    """Escape a string for C."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def escape_title(title):
    """Escape title for C string."""
    return title.replace('"', '\\"').replace("'", "\\'")


def main():
    with open(INPUT) as f:
        songs = json.load(f)

    # Filter
    approved = []
    for s in songs:
        if s["uid"] in SKIP_UIDS:
            continue
        if s["mml"] is None:
            continue
        approved.append(s)

    print(f"Generating code for {len(approved)} songs")
    print(f"Songs replacing existing: {list(REPLACE_EXISTING.values())}")

    lines = []
    lines.append("")
    lines.append("// ========== Favorites MML Songs ==========")
    lines.append("")

    # PROGMEM declarations
    for s in approved:
        var = uid_to_varname(s["uid"])
        mml = escape_c_string(s["mml"])
        lines.append(f'static const char {var}[] PROGMEM = "{mml}";')
        lines.append("")

    # songDefs entries
    lines.append("// --- songDefs entries for favorites ---")
    lines.append("// Paste these into the songDefs[] array:")
    lines.append("// BEGIN_FAVORITES_DEFS")
    for s in approved:
        var = uid_to_varname(s["uid"])
        title = escape_title(s["title"])
        # Pad for alignment
        var_padded = var.ljust(45)
        title_padded = f'"{title}"'.ljust(55)
        lines.append(f"    {{ {var_padded}, {title_padded}, FMT_MML }},")
    lines.append("// END_FAVORITES_DEFS")

    # Write output
    with open(OUTPUT, "w") as f:
        f.write("\n".join(lines) + "\n")

    print(f"Written to {OUTPUT}")
    print(f"\nExisting songs to REMOVE from songDefs[]:")
    for uid, title in REPLACE_EXISTING.items():
        print(f"  - \"{title}\" (replaced by {uid})")


if __name__ == "__main__":
    main()
