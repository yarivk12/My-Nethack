"""Native Step 11 validation. Run against an explicit diagnostic x64 build.

Probability corpus declared before sampling: game seeds 110001..111000;
each game generates ordinary eligible DoD levels 1..14 and 40..57, in order.
This supplies shallow vault opportunities and deep mixed-backend opportunities.
No wizard or forcing during probability runs. Four isolated process directories
allow parallel execution without sharing saves, bones, RNG, or Lua state.
"""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
from collections import Counter
import argparse
import json
import math
import os
import shutil
import subprocess

NAMES = ["Giant Court", "Real Zoo", "Dragon Lair", "Wizard Study",
         "Storeroom Vault v1", "Super Honeycomb", "Dragon Hall", "Library"]
PACKAGE_FILES = ("NetHack.exe", "nhdat500", "symbols.template", "sysconf.template",
                 "nethackrc.template", "Guidebook.txt", "opthelp", "license")


def prepare(release, target):
    target.mkdir(parents=True, exist_ok=True)
    for name in PACKAGE_FILES:
        shutil.copy2(release / name, target / name)
    (target / "sysconf").write_text("WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n")
    return target / "NetHack.exe"


def run(exe, log, **variables):
    env = {k: v for k, v in os.environ.items()
           if not (k.startswith(("STEP11_", "NETHACK_STEP", "NETHACK_OUTLANDS"))
                   or k in ("CUSTOMROOM", "SHOPTYPE", "THEMERM", "THEMERMFILL"))}
    env.update(NETHACK_STEP11_TEST="1", NETHACKOPTIONS="!news,!legacy,!tutorial,!tips")
    env.update(variables)
    result = subprocess.run([str(exe)], cwd=exe.parent, env=env,
                            capture_output=True, timeout=60)
    output = (result.stdout + result.stderr).decode(errors="replace").replace("\r", "")
    log.write_text(output)
    if variables.get("STEP11_PARTIAL_FAILURE"):
        assert result.returncode == 86 and "PARTIAL_ERROR|attempts=1|rooms=1" in output, output[-3000:]
    else:
        assert result.returncode == 0, (log, result.returncode, output[-3000:])
        assert "PASS" in output, (log, "no native assertions executed")
    return output


def rows(text, prefix):
    return [dict((k, int(v)) for k, v in
                 (field.split("=", 1) for field in line.split("|")[1:]))
            for line in text.splitlines() if line.startswith(prefix + "|")]


def focused(release, out):
    exe = prepare(release, out / "game")
    run(exe, out / "selector.log", STEP11_MODE="selector")
    run(exe, out / "shop-types.log", STEP11_MODE="shop-types")
    run(exe, out / "library.log", STEP11_MODE="library")
    run(exe, out / "excluded.log", STEP11_MODE="excluded")
    run(exe, out / "metadata.log", STEP11_MODE="metadata")
    run(exe, out / "accepted-bones.log", STEP11_MODE="bones", CUSTOMROOM="4")
    run(exe, out / "library-bones.log", STEP11_MODE="bones", CUSTOMROOM="8")
    for feature in (4, 5):
        run(exe, out / f"escape-{feature}.log", STEP11_MODE="escape", CUSTOMROOM=str(feature))
    for feature in range(1, 9):
        for shop in [None, "g", {1: "t", 2: "z", 3: "z", 6: "b", 8: "z"}.get(feature)]:
            if shop is None and (out / f"feature-{feature}-none.log").exists():
                continue
            variables = dict(STEP11_MODE="forced", CUSTOMROOM=str(feature))
            if shop: variables["SHOPTYPE"] = shop
            log = out / f"feature-{feature}-{shop or 'none'}.log"
            output = run(exe, log, **variables)
            levels = rows(output, "LEVEL")
            assert sum(r["emissions"] for r in levels) == 3
            assert all(r["selected"] in (0, feature) for r in levels)
            assert output.count("PASS actual savelev/getlev") == 9
            coexist = rows(output, "COEXIST")
            if shop == "g": assert any(r["shops"] for r in coexist)
            elif shop:
                assert any(r["vanilla"] if feature == 8 else r["samebase"] for r in coexist)
            if feature == 8:
                assert output.count("You enter a library!") == 3
    run(exe, out / "clean-failure.log", STEP11_MODE="forced", CUSTOMROOM="4",
        STEP11_CLEAN_FAILURE="1")
    run(exe, out / "partial-failure.log", STEP11_MODE="forced", CUSTOMROOM="4",
        STEP11_PARTIAL_FAILURE="1")
    for lo, hi in [(30, 59), (60, 99), (100, 149), (150, 195)]:
        output = run(exe, out / f"library-band-{lo}.log", STEP11_MODE="forced", CUSTOMROOM="8",
                     STEP11_MIN_DL=str(lo), STEP11_MAX_DL=str(hi))
        assert sum(r["emissions"] for r in rows(output, "LEVEL")) == 3
        assert output.count("You enter a library!") == 3
    print("PASS focused selector, all features, recurrence, coexistence, level/bones codecs, clean and partial failures")


def wilson(successes, total, z=3.2):
    """Conservative simultaneous intervals (z=3.2), declared before runs."""
    if not total: return [0, 1]
    p = successes / total
    mid = (p + z*z / (2*total)) / (1 + z*z/total)
    radius = z * math.sqrt(p*(1-p)/total + z*z/(4*total*total)) / (1 + z*z/total)
    return [max(0, mid-radius), min(1, mid+radius)]


def probability(release, out, games):
    assert games == 1000, "Change the predeclared corpus only in a documented confirmation batch"
    corpus = {"seeds": [110001, 111000], "depths": [[1, 14], [40, 57]],
              "wizard": False, "forcing": False, "interval_z": 3.2,
              "placement_family_alpha": 0.05, "features": len(NAMES),
              "cluster": "fresh game process"}
    (out / "corpus.json").write_text(json.dumps(corpus, indent=2))
    executables = [prepare(release, out / f"worker-{i}") for i in range(4)]
    def worker(index):
        result = []
        for seed in range(110001 + index, 111001, 4):
            output = run(executables[index], out / f"seed-{seed}.log",
                         STEP11_MODE="natural", STEP11_SEED=str(seed))
            result.extend(rows(output, "LEVEL"))
        return result
    with ThreadPoolExecutor(max_workers=4) as pool:
        data = [row for batch in pool.map(worker, range(4)) for row in batch]
    (out / "levels.json").write_text(json.dumps(data))
    assert all(row["emissions"] <= 1 for row in data)
    results = {}
    for feature, name in enumerate(NAMES, 1):
        eligible = [r for r in data if r[f"e{feature}"]]
        selected = [r for r in eligible if r["selected"] == feature]
        complete = [r for r in selected if r["emissions"] == 1]
        recurrence = Counter(r["seed"] for r in complete)
        distribution = Counter(recurrence.values())
        distribution[0] = games - len(recurrence)
        n, s, k = len(eligible), len(selected), len(complete)
        # Bonferroni one-sided exact bound when no placement failures.
        lower = (0.05/len(NAMES))**(1/s) if s and k == s else wilson(k, s)[0]
        results[name] = dict(eligible=n, selected=s, attempts=sum(r["attempts"] for r in selected),
                             completed=k, failures=s-k, selection_rate=s/n,
                             appearance_rate=k/n, placement_rate=k/s if s else 0,
                             selection_interval=wilson(s,n),
                             placement_simultaneous_lower=lower,
                             recurrence=dict(sorted(distribution.items())))
        by_game = {seed: [0, 0] for seed in range(110001, 111001)}
        for row in eligible:
            by_game[row["seed"]][0] += 1
            by_game[row["seed"]][1] += row["selected"] == feature
        p = s/n
        se = math.sqrt(games/(games-1) * sum((sel-p*opp)**2
                       for opp, sel in by_game.values())) / n
        results[name]["game_cluster_selection_interval"] = [max(0,p-3.2*se),p+3.2*se]
        independent_games = sum(sel > 0 for opp, sel in by_game.values())
        results[name]["independent_first_selected_games"] = independent_games
        if k == s:
            # Also bound success for the first selected occurrence in each
            # independent game; this avoids treating repeated rooms within a
            # game as independent placement evidence.
            results[name]["first_per_game_placement_lower"] = (0.05/len(NAMES))**(1/independent_games)
        assert wilson(s, n)[0] <= 0.03 <= wilson(s, n)[1], results[name]
        assert k/s >= 0.98, results[name]
    results["overall"] = dict(levels=len(data), any_custom=sum(r["emissions"] for r in data),
                              doubles=0, vanilla_attempts=sum(r["vanilla_attempt"] != 0 for r in data),
                              vanilla_placements=sum(r["vanilla_placements"] for r in data))
    (out / "results.json").write_text(json.dumps(results, indent=2))
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("release", type=Path)
    parser.add_argument("out", type=Path)
    parser.add_argument("--phase", choices=["focused", "probability"], default="focused")
    parser.add_argument("--games", type=int, default=1000)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    if args.phase == "focused": focused(args.release.resolve(), args.out.resolve())
    else: probability(args.release.resolve(), args.out.resolve(), args.games)
