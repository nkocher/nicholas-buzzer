#!/usr/bin/env python3
"""Scrape MML data from favorites.txt URLs and produce a review report."""

import json
import re
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from difflib import SequenceMatcher

import requests
from bs4 import BeautifulSoup

ROOT = Path(__file__).resolve().parent.parent
FAVORITES = ROOT / "favorites.txt"
OUTPUT_JSON = ROOT / "scraped_songs.json"
OUTPUT_REPORT = ROOT / "scrape_report.txt"
STR_BUF_SZ = 6144

# All existing song titles (from songs.h songDefs[] + hardcoded melodies in main.cpp)
EXISTING_TITLES = [
    # RTTTL songs
    "Fiddler on the Roof", "Larry David Theme", "Macarena", "Who Let The Dogs Out",
    "Staying Alive", "All The Small Things", "Dammit", "Oops I Did It Again",
    "Cantina Band (RTTTL)", "Genie In A Bottle", "Chicago Bulls Theme", "Fill Me In",
    "Higher", "Ruff Ryders Anthem", "Sandstorm (RTTTL)", "Say My Name",
    "Still D.R.E. (RTTTL)", "Forgot About Dre", "The Next Episode",
    "Blue (Eiffel 65)", "My Name Is", "Real Slim Shady", "Careless Whisper",
    "Georgia On My Mind", "Tears In Heaven", "Dueling Banjos", "California Love",
    "All Eyez On Me", "Hit Em Up", "I Ain't Mad At Cha", "Mamma Mia",
    "Better Off Alone", "Money Money Money", "Larger Than Life", "Jurassic Park",
    "M*A*S*H", "Muppet Show", "Phantom of the Opera", "Seinfeld",
    "Sex And The City", "The Simpsons", "Golden Girls", "Friends", "Jeopardy",
    "The A-Team", "Addams Family", "Back To The Future", "Zorba The Greek",
    "Phil Collins", "The Police", "Poison", "Titanic", "No Scrubs", "YMCA",
    "You're The One", "Bittersweet Symphony", "The Final Countdown",
    "The Good The Bad The Ugly", "Knight Rider", "Master Of The House",
    "Losing My Religion", "Flashdance", "Girl From Ipanema", "Dawson's Creek",
    "Big Pimpin", "Rendezvous", "One Week", "Austin Powers",
    "Brandenburg Concerto", "Toccata", "Around The World",
    # MML songs
    "Sandstorm (MML)", "The Entertainer", "Baby Shark", "Africa",
    "All Summer Long", "Smooth Criminal", "My Way", "Come As You Are",
    "Heart-Shaped Box", "Lithium", "Don't Look Back In Anger", "Fireflies",
    "You'll Be In My Heart", "Phantom Overture (MML)", "Creep", "Maple Leaf Rag",
    "Closing Time", "Every Breath You Take", "Today",
    "Bullet With Butterfly Wings", "Vivaldi Winter", "Pure Imagination",
    "River Flows In You", "Sweet Child O' Mine", "Now We Are Free",
    "More Than A Feeling", "Just What I Needed", "Despacito", "Feel Good Inc.",
    "Firework", "Final Countdown (MML)", "Grease Mashup", "Margaritaville",
    "Kiki's Delivery Service", "Make A Man Out Of You", "Whiskey In The Jar",
    "Man Or Muppet", "Rape Me", "Laura's Theme", "Wannabe", "Soviet Anthem",
    "Linger", "Fast Car", "Under The Sea", "Wonderwall", "Avril 14th",
    "99 Luftballons", "Gimme! Gimme! Gimme!", "Piano Man", "Paranoid",
    "Holding Out For A Hero", "Holy Diver", "Winner Takes It All",
    "Colors Of The Wind", "Aerodynamics", "Macintosh Plus", "Karma Police",
    "Mii Plaza", "Changes",
    # Hardcoded melodies
    "Like a Prayer", "La Cucaracha", "Axel F", "The Godfather",
    "Die Forelle", "Tetris", "Nokia", "Fur Elise",
    "Pink Panther", "Super Mario", "Take On Me", "Pac-Man",
    "Ode to Joy", "Coffin Dance", "Keyboard Cat",
    "Still D.R.E.", "Funkytown",
]


def normalize_title(t):
    """Lowercase, strip punctuation/articles for fuzzy matching."""
    t = t.lower()
    t = re.sub(r'\(.*?\)', '', t)  # remove parenthetical
    t = re.sub(r'\[.*?\]', '', t)  # remove brackets
    t = re.sub(r'[^a-z0-9 ]', '', t)  # strip punctuation
    t = re.sub(r'\b(the|a|an)\b', '', t)
    return ' '.join(t.split())


def title_matches(t1, t2, threshold=0.75):
    """Check if two titles are similar enough to be duplicates."""
    n1, n2 = normalize_title(t1), normalize_title(t2)
    if n1 == n2:
        return True
    return SequenceMatcher(None, n1, n2).ratio() >= threshold


def fetch_archeage(url, session):
    """Extract MML from archeagemmllibrary.com page."""
    resp = session.get(url, timeout=15)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, 'html.parser')

    textareas = []
    post = soup.select_one('.post-content, .entry-content, article')
    if post:
        textareas = post.find_all('textarea')
    if not textareas:
        textareas = soup.find_all('textarea')

    if not textareas:
        return None, False

    multi_track = len(textareas) > 1
    mml = textareas[0].get_text(strip=True)
    return mml, multi_track


def fetch_mabimml(url, session):
    """Extract melody MML from mabimml.net page."""
    resp = session.get(url, timeout=15)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, 'html.parser')

    # Look for textareas or pre blocks with MML
    textareas = soup.find_all('textarea')
    for ta in textareas:
        text = ta.get_text(strip=True)
        if 'MML@' in text:
            # Take first track only (before any comma-separated tracks)
            return text, len(textareas) > 1

    # Try to find MML in plain text - look for Melody sections
    text = soup.get_text()
    # Find MML@ patterns
    mml_matches = re.findall(r'MML@[^;]*;', text)
    if mml_matches:
        multi = len(mml_matches) > 1
        return mml_matches[0], multi

    return None, False


def normalize_mml(mml):
    """Ensure MML@ prefix and ; suffix."""
    if not mml:
        return None
    mml = mml.strip()
    # Some entries have multiple tracks separated by comma-newline pattern
    # Take only the first track (up to first ,MML@ or just the whole thing)
    tracks = re.split(r',\s*MML@', mml)
    if len(tracks) > 1:
        mml = tracks[0]
        if not mml.endswith(';'):
            mml += ';'

    if not mml.startswith('MML@'):
        mml = 'MML@' + mml
    if not mml.endswith(';'):
        mml += ';'
    return mml


def fetch_song(entry, session):
    """Fetch a single song, return result dict."""
    uid = entry['uid']
    title = entry['title']
    url = entry['url']

    result = {
        'uid': uid,
        'title': title,
        'url': url,
        'mml': None,
        'multi_track': False,
        'flags': [],
        'error': None,
    }

    try:
        if 'archeagemmllibrary.com' in url:
            mml, multi = fetch_archeage(url, session)
        elif 'mabimml.net' in url:
            mml, multi = fetch_mabimml(url, session)
        else:
            result['error'] = f"Unknown site: {url}"
            result['flags'].append('FETCH_ERR')
            return result

        if mml is None:
            result['error'] = "No MML found on page"
            result['flags'].append('FETCH_ERR')
            return result

        mml = normalize_mml(mml)
        result['mml'] = mml
        result['multi_track'] = multi
        if multi:
            result['flags'].append('MULTI')

        if mml and len(mml) >= STR_BUF_SZ:
            result['flags'].append('OVER_BUF')

    except Exception as e:
        result['error'] = str(e)
        result['flags'].append('FETCH_ERR')

    return result


def check_duplicates(results):
    """Flag duplicates against existing songs and within favorites."""
    existing_normalized = [(t, normalize_title(t)) for t in EXISTING_TITLES]

    for r in results:
        if r['mml'] is None:
            continue

        # Check against existing songs
        for existing_title, _ in existing_normalized:
            if title_matches(r['title'], existing_title):
                r['flags'].append(f"DUP:{existing_title}")
                break

    # Check internal duplicates within favorites
    for i, r1 in enumerate(results):
        if r1['mml'] is None:
            continue
        for j, r2 in enumerate(results):
            if j <= i or r2['mml'] is None:
                continue
            if title_matches(r1['title'], r2['title']):
                r1['flags'].append(f"INTERNAL_DUP:{r2['uid']}")
                r2['flags'].append(f"INTERNAL_DUP:{r1['uid']}")


def generate_report(results):
    """Generate human-readable scrape report."""
    lines = []
    lines.append("=" * 70)
    lines.append("SCRAPE REPORT — favorites.txt MML songs")
    lines.append("=" * 70)
    lines.append("")

    # Stats
    total = len(results)
    ok = sum(1 for r in results if r['mml'] and not any(f.startswith(('FETCH_ERR', 'DUP:', 'INTERNAL_DUP:', 'OVER_BUF')) for f in r['flags']))
    flagged = sum(1 for r in results if r['flags'])
    errors = sum(1 for r in results if any(f == 'FETCH_ERR' for f in r['flags']))
    dups = sum(1 for r in results if any(f.startswith('DUP:') for f in r['flags']))
    internal_dups = len(set(r['uid'] for r in results if any(f.startswith('INTERNAL_DUP:') for f in r['flags'])))
    multi = sum(1 for r in results if any(f == 'MULTI' for f in r['flags']))
    over_buf = sum(1 for r in results if any(f == 'OVER_BUF' for f in r['flags']))

    lines.append(f"Total URLs:          {total}")
    lines.append(f"Clean (no flags):    {ok}")
    lines.append(f"Flagged:             {flagged}")
    lines.append(f"  Fetch errors:      {errors}")
    lines.append(f"  Existing dups:     {dups}")
    lines.append(f"  Internal dups:     {internal_dups}")
    lines.append(f"  Multi-track:       {multi}")
    lines.append(f"  Over buffer:       {over_buf}")
    lines.append("")

    # Flagged songs
    if flagged:
        lines.append("-" * 70)
        lines.append("FLAGGED SONGS")
        lines.append("-" * 70)
        for r in results:
            if not r['flags']:
                continue
            flag_strs = []
            for f in r['flags']:
                if f.startswith('DUP:'):
                    flag_strs.append(f"[DUP] matches \"{f[4:]}\"")
                elif f.startswith('INTERNAL_DUP:'):
                    flag_strs.append(f"[INTERNAL_DUP] matches {f[13:]}")
                elif f == 'MULTI':
                    flag_strs.append("[MULTI] multi-track (using track 0)")
                elif f == 'OVER_BUF':
                    mml_len = len(r['mml']) if r['mml'] else 0
                    flag_strs.append(f"[OVER_BUF] {mml_len} chars (max {STR_BUF_SZ})")
                elif f == 'FETCH_ERR':
                    flag_strs.append(f"[FETCH_ERR] {r['error']}")
            lines.append(f"\n  {r['title']} ({r['uid']})")
            lines.append(f"    URL: {r['url']}")
            for fs in flag_strs:
                lines.append(f"    {fs}")
        lines.append("")

    # Clean songs
    clean = [r for r in results if r['mml'] and not r['flags']]
    lines.append("-" * 70)
    lines.append(f"CLEAN SONGS ({len(clean)})")
    lines.append("-" * 70)
    for r in clean:
        lines.append(f"  {r['title']} ({len(r['mml'])} chars)")

    lines.append("")
    return "\n".join(lines)


def main():
    # Read favorites
    entries = []
    with open(FAVORITES) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            entries.append(json.loads(line))

    print(f"Loaded {len(entries)} favorites")

    # Fetch in parallel
    session = requests.Session()
    session.headers['User-Agent'] = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)'
    results = []

    with ThreadPoolExecutor(max_workers=8) as pool:
        futures = {pool.submit(fetch_song, e, session): e for e in entries}
        done = 0
        for future in as_completed(futures):
            done += 1
            result = future.result()
            results.append(result)
            status = "OK" if result['mml'] else f"ERR: {result['error']}"
            print(f"  [{done}/{len(entries)}] {result['title']}: {status}")

    # Sort by original order
    uid_order = {e['uid']: i for i, e in enumerate(entries)}
    results.sort(key=lambda r: uid_order.get(r['uid'], 999))

    # Check duplicates
    check_duplicates(results)

    # Save JSON
    with open(OUTPUT_JSON, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"\nSaved {OUTPUT_JSON}")

    # Generate and save report
    report = generate_report(results)
    with open(OUTPUT_REPORT, 'w') as f:
        f.write(report)
    print(f"Saved {OUTPUT_REPORT}")

    # Print report
    print("\n" + report)


if __name__ == '__main__':
    main()
