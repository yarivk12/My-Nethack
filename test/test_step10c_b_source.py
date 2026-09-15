"""Static contract for Step 10C-B topology and scheduler integration."""
from pathlib import Path
import re


repo = Path(__file__).resolve().parents[1]
dungeon = (repo / "dat/dungeon.lua").read_text(encoding="utf-8")
source = (repo / "src/dungeon.c").read_text(encoding="utf-8")

assert dungeon.count('name = "Neutral Quest"') == 1
assert dungeon.count('name = "The Lost Cities"') == 1
assert 'name="Neutral Quest", base=30, range=170, branchtype="portal"' in dungeon
assert 'name = "Neutral Quest"' in dungeon and 'base = 8' in dungeon
assert 'name = "The Lost Cities"' in dungeon and 'entry = 2' in dungeon
assert 'name = "The Dispensary"' not in dungeon

for proto in ("gatetwn", "out1", "out2", "out3", "out4", "spire",
              "sumall", "lbyrnth", "leth-a-1", "lethe-b", "leth-c-1",
              "leth-d-1", "lethe-e", "lethe-f", "lethe-g", "lethe-z",
              "nkai-a-1", "nkai-b", "nkai-c", "nkai-z", "rlyeh"):
    assert re.search(r'name\s*=\s*"' + re.escape(proto) + r'"', dungeon), proto

assert 'name = "The Lost Cities"' in dungeon
assert 'name="The Lost Cities", chainlevel="sumall", base=0, direction="down"' in dungeon
assert "step10c_internal_branch" in source
assert "step10c_select_alternates" in source
assert "step10c_internal_depth" in source
assert "neutral_approach" in source
assert "step6b_rebase_level(neutral_approach, dlevel)" in source
assert "step6b_rebase_branch(neutral)" in source
assert "STEP10C_DISPENSARY_LEVEL" in source
assert "rn2(5) + 2" in source
assert "111" not in source[source.index("step6b_schedule"):
                           source.index("#endif /* !SFCTOOL */")]
assert re.search(
    r'if \(svn\.n_dgns > MAXDUNGEON\)\s*'
    r'panic\("init_dungeons: too many dungeons"\)', source
)
scheduler_start = source.index("\nstep6b_schedule(void)\n{")
scheduler = source[scheduler_start:
                   source.index("#endif /* !SFCTOOL */", scheduler_start)]
assert scheduler.index("Missing Lost Tomb branch") < scheduler.index(
    "neutral->end1.dlevel = (xint16) dlevel"
)
previous_start = source.index("\nprev_level(boolean at_stairs)\n{")
previous = source[previous_start:
                  source.index("\nearth_sense(void)\n{", previous_start)]
assert "if (at_stairs && stway)" in previous
assert "newlevel.dnum = stway->tolev.dnum" in previous
assert "newlevel.dlevel = stway->tolev.dlevel" in previous

for name, destinations in {
    "gatetwn.lua": ("out1",),
    "out1.lua": ("gatetwn", "out2"),
    "out2.lua": ("out1", "out3"),
    "out3.lua": ("out2", "out4"),
    "out4.lua": ("out3", "spire"),
    "spire.lua": ("out4",),
}.items():
    text = (repo / "dat" / name).read_text(encoding="utf-8")
    for destination in destinations:
        assert re.search(r'type\s*=\s*"portal"[^\n]*name\s*=\s*"'
                         + destination + r'"', text), (name, destination)

assert "README.md" not in {p.name for p in repo.glob("README.md.step10c-b")}
print("PASS Step 10C-B source topology/scheduler contract")
