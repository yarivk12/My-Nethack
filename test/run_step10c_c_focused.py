"""Exercise deterministic real-dispatch paths for every Outlands feature."""
from pathlib import Path
import json
import os
import subprocess
import sys


exe = Path(sys.argv[1]).resolve()
out = Path(sys.argv[2]).resolve()
out.mkdir(parents=True, exist_ok=True)
keys = ("kamerel", "spire", "fishing", "well", "river", "village",
        "ziggurat", "ferrumach", "homestead_attempts",
        "homestead_successes")
expected = {
    100000: (0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    100001: (0, 0, 1, 1, 0, 0, 0, 0, 0, 0),
    100002: (0, 0, 0, 0, 1, 0, 0, 1, 0, 0),
    100003: (0, 0, 0, 0, 0, 1, 0, 0, 0, 0),
    100006: (0, 0, 0, 0, 0, 1, 0, 0, 4, 4),
    100011: (0, 0, 0, 0, 0, 0, 1, 0, 0, 0),
    100018: (0, 1, 0, 0, 0, 0, 0, 1, 0, 0),
    100032: (0, 0, 1, 1, 0, 0, 0, 1, 0, 0),
    100034: (1, 0, 0, 0, 0, 0, 0, 0, 0, 0),
}


def run_seed(seed, repetition):
    env = os.environ.copy()
    env.update(NETHACK_STEP10C_C_TEST="1", NETHACK_OUTLANDS_LEVEL="out1",
               NETHACK_OUTLANDS_COUNT="1", NETHACK_OUTLANDS_SEED=str(seed),
               NETHACK_OUTLANDS_TRACE="1")
    run = subprocess.run([str(exe)], cwd=exe.parent, env=env,
                         capture_output=True, text=True)
    stem = f"seed-{seed}-run-{repetition}"
    (out / (stem + ".stdout")).write_text(run.stdout, encoding="utf-8")
    (out / (stem + ".stderr")).write_text(run.stderr, encoding="utf-8")
    assert run.returncode == 0, (seed, run.returncode, run.stderr[-2000:])
    lines = [line for line in run.stdout.splitlines()
             if line.startswith("OUTLANDS_SAMPLE|")]
    assert len(lines) == 1, (seed, run.stdout[-2000:])
    row = dict(field.split("=", 1) for field in lines[0].split("|")[1:])
    assert int(row.pop("seed")) == seed
    assert int(row.pop("sample")) == 0
    result = tuple(int(row[key]) for key in keys)
    assert result == expected[seed], (seed, result, expected[seed])
    # Dispatch is paired with real generated payload, not just a callback.
    assert int(row["semantic_monsters"]) > 0
    assert int(row["river_cells"]) > 0 if result[4] else True
    if result[0]:
        assert (int(row["amm_kamerel"]) + int(row["hudor_kamerel"]) +
                int(row["sharab_kamerel"]) + int(row["mirrors"])) > 0
    if result[1]:
        assert (int(row["robes"]) + int(row["khakkhara"]) +
                int(row["reflection_amulets"])) > 0
    if result[2] or result[3]:
        assert (int(row["deep_one"]) + int(row["rakuyo"]) +
                int(row["pools"])) > 0
    if result[5] or result[8] or result[9]:
        assert int(row["plumach_rilmani"]) > 0
    if result[6]:
        assert (int(row["ziggurat_wizard"]) + int(row["ziggurat_knight"]) +
                int(row["ziggurat_cultist"])) > 0
        assert int(row["chests"]) + int(row["altars"]) > 0
    if result[7]:
        assert int(row["barracks"]) > 0
    return dict(zip(keys, result))


results = {}
for seed in expected:
    first = run_seed(seed, 1)
    second = run_seed(seed, 2)
    assert first == second, (seed, first, second)
    results[str(seed)] = first

assert any(not any(row.values()) for row in results.values())
assert any(sum(bool(row[key]) for key in keys[:-2]) >= 3
           for row in results.values())
for key in keys:
    assert any(row[key] for row in results.values()), key
(out / "results.json").write_text(json.dumps(results, indent=2), encoding="utf-8")
print("PASS deterministic focused Outlands dispatch paths:", ", ".join(keys))
