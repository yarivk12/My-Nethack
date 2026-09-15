"""Run the compile-gated native topology probe against fresh game processes."""
from pathlib import Path
import json
import os
import subprocess
import sys


def parse(line):
    fields = line.strip().split("|")
    assert fields[0] == "TOPOLOGY", line
    return {key: int(value) for key, value in (item.split("=", 1)
                                               for item in fields[1:])}


release, output = sys.argv[1:3]
count = int(sys.argv[3]) if len(sys.argv) > 3 else 8
exe = Path(release).resolve()
out = Path(output).resolve()
out.mkdir(parents=True, exist_ok=True)
results = []

for sample in range(1, count + 1):
    env = os.environ.copy()
    env["NETHACK_STEP9_TOPOLOGY_TEST"] = "1"
    run = subprocess.run([str(exe)], cwd=exe.parent, env=env,
                         capture_output=True, text=True)
    (out / f"sample-{sample}.stdout").write_text(run.stdout, encoding="utf-8")
    (out / f"sample-{sample}.stderr").write_text(run.stderr, encoding="utf-8")
    assert run.returncode == 0, (sample, run.stdout, run.stderr)
    lines = [line for line in run.stdout.splitlines()
             if line.startswith("TOPOLOGY|")]
    assert len(lines) == 1, (sample, run.stdout, run.stderr)
    row = parse(lines[0])
    assert all(30 <= row[name] <= 199
               for name in ("sheol", "dragon", "mithardir", "neutral")), row
    assert len({row["sheol"], row["dragon"], row["mithardir"],
                row["neutral"]}) == 4, row
    assert row["step9_count"] == 3 and row["step9d_count"] == 0, row
    assert row["collisions"] == 0 and row["castle_count"] == 1, row
    assert row["chalv2"] == row["mithardir"], row
    assert row["neulev"] == row["neutral"], row
    assert row["dungeons"] <= 18, row
    assert row["neutral_floors"] == 7 and row["lost_floors"] == 13, row
    assert row["alternates"] == 4 and 2 <= row["dispensary_parent"] <= 6, row
    assert row["max_depth"] == row["neutral"] + 18, row
    results.append(row)

(out / "results.json").write_text(json.dumps(results, indent=2),
                                   encoding="utf-8")
print(f"PASS native fresh topology: {count} real init_dungeons samples; "
      "Step9A-C plus Neutral distinct DL30-199 parents, Castle200, "
      "two Step10 dungeons and internal Dispensary")
