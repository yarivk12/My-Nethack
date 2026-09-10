"""Production Step 9 parent-placement contract, independent of donor exports."""
from pathlib import Path

repo = Path(__file__).resolve().parents[1]
dungeon = (repo / "dat/dungeon.lua").read_text(encoding="utf-8")
source = (repo / "src/dungeon.c").read_text(encoding="utf-8")

for branch, kind in [
    ("Sheol", 'direction="down"'),
    ("The Dragon Caves", 'direction="down"'),
    ("Mithardir", 'branchtype="portal"'),
]:
    assert f'name="{branch}", base=30, range=170, {kind}' in dungeon

assert "Step9 parent branches use the shared persistent scheduler" in source
assert "for (dlevel = 108; dlevel <= 110; ++dlevel)" not in source
assert "step6b_rebase_branch(sheol)" in source
assert "step6b_rebase_branch(dragon_caves)" in source
assert "step6b_rebase_branch(mithardir)" in source
assert 'strcmp(slev->proto, "chalv2")' in source
assert "step6b_rebase_level(mithardir_approach" in source
assert "111" not in source[source.index("step6b_schedule"):source.index("#endif /* !SFCTOOL */")]

print("PASS Step 9 parent branches use one shared persistent DL30-199 scheduler")
