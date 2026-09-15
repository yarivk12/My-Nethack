"""Run exactly 1,000 native production Outlands generations."""
from pathlib import Path
import json
import os
import subprocess
import sys

exe = Path(sys.argv[1]).resolve()
out = Path(sys.argv[2]).resolve()
out.mkdir(parents=True, exist_ok=True)
totals = {k: 0 for k in ("instances", "kamerel", "spire", "fishing", "well",
                          "river", "village", "ziggurat", "ferrumach",
                          "homestead_attempts", "homestead_successes")}
payload_keys = ("river_cells", "semantic_monsters", "semantic_objects",
                "deep_one", "deeper_one", "amm_kamerel", "hudor_kamerel",
                "sharab_kamerel", "plumach_rilmani", "ziggurat_wizard",
                "ziggurat_knight", "ziggurat_cultist", "mirrors", "robes",
                "khakkhara", "reflection_amulets", "rakuyo", "chests",
                "puddles", "pools", "moats", "doors", "altars", "barracks",
                "courts", "shops")
totals.update({key: 0 for key in payload_keys})
rows = []
for index, level in enumerate(("out1", "out2", "out3", "out4")):
    env = os.environ.copy()
    env.update(NETHACK_STEP10C_C_TEST="1", NETHACK_OUTLANDS_LEVEL=level,
               NETHACK_OUTLANDS_COUNT="250",
               NETHACK_OUTLANDS_SEED=str(100000 * (index + 1)))
    run = subprocess.run([str(exe)], cwd=exe.parent, env=env,
                         capture_output=True, text=True)
    (out / f"{level}.stdout").write_text(run.stdout, encoding="utf-8")
    (out / f"{level}.stderr").write_text(run.stderr, encoding="utf-8")
    assert run.returncode == 0, (level, run.returncode,
                                 run.stdout[-2000:], run.stderr[-2000:])
    lines = [line for line in run.stdout.splitlines()
             if line.startswith("OUTLANDS|")]
    assert len(lines) == 1, (level, run.stdout[-2000:])
    row = dict(field.split("=", 1) for field in lines[0].split("|")[1:])
    assert row.pop("level") == level
    row = {key: int(value) for key, value in row.items()}
    assert row["instances"] == 250
    rows.append({"level": level, **row})
    for key, value in row.items(): totals[key] += value

assert totals["instances"] == 1000
for key in ("kamerel", "spire", "fishing", "well", "river", "village",
            "ziggurat", "ferrumach", "homestead_attempts",
            "homestead_successes"):
    assert totals[key] > 0, (key, totals)
for key in ("river_cells", "semantic_monsters", "deep_one", "plumach_rilmani",
            "ziggurat_wizard", "ziggurat_knight", "ziggurat_cultist",
            "mirrors", "robes", "khakkhara", "reflection_amulets", "rakuyo",
            "puddles", "pools", "moats", "doors", "barracks"):
    assert totals[key] > 0, (key, totals)
(out / "results.json").write_text(json.dumps({"levels": rows, "totals": totals},
                                               indent=2), encoding="utf-8")
print("PASS 1000 native Outlands instances (250 each out1-out4):", totals)
