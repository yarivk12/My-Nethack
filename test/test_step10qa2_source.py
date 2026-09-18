"""Static QA2 contract for Step 10 special-room and object remediation.

This is intentionally narrow: it protects the exact resource-level boundary
established by QA1/QA1-1 without changing the global des.region default.
Generated semantics are covered by the compiled QA2 probe and the existing
Step 10 integration runners.
"""
from pathlib import Path
import re
import sys


REPO = Path(__file__).resolve().parents[1]

RESOURCES = (
    "neulev", "gatetwn", "out1", "out2", "out3", "out4", "spire",
    "sumall", "leth-a-1", "leth-a-2", "lethe-b", "leth-c-1",
    "leth-c-2", "leth-d-1", "leth-d-2", "lethe-e", "lethe-f", "lethe-g",
    "lethe-z", "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c", "nkai-z",
    "rlyeh", "lbyrnth",
)

# Each tuple is (resource, x1, y1, x2, y2, native room type).
NORMAL_FILL = (
    ("neulev", 16, 6, 19, 8, "barracks"),
    ("neulev", 16, 12, 19, 14, "barracks"),
    ("neulev", 21, 9, 25, 11, "barracks"),
    ("neulev", 27, 7, 30, 9, "barracks"),
    ("neulev", 27, 11, 30, 13, "barracks"),
    ("neulev", 56, 7, 59, 9, "barracks"),
    ("neulev", 56, 11, 59, 13, "barracks"),
    ("neulev", 56, 6, 59, 8, "barracks"),
    ("neulev", 56, 12, 59, 14, "barracks"),
    ("neulev", 32, 9, 35, 11, "weapon shop"),
    ("neulev", 40, 9, 43, 11, "wand shop"),
    ("gatetwn", 32, 9, 36, 11, "armor shop"),
    ("gatetwn", 39, 9, 43, 11, "potion shop"),
    ("gatetwn", 16, 12, 19, 14, "food shop"),
    ("gatetwn", 16, 6, 19, 8, "food shop"),
    ("gatetwn", 56, 12, 59, 14, "food shop"),
    ("gatetwn", 56, 6, 59, 8, "food shop"),
    ("gatetwn", 47, 7, 51, 8, "tool shop"),
    ("gatetwn", 24, 12, 28, 13, "shop"),
    ("gatetwn", 47, 12, 51, 13, "beehive"),
    ("leth-d-1", 13, 9, 21, 11, "zoo"),
    ("leth-d-1", 38, 10, 45, 13, "throne"),
    ("leth-d-1", 38, 15, 45, 18, "throne"),
    ("leth-d-2", 2, 1, 35, 7, "swamp"),
    ("leth-d-2", 13, 9, 21, 11, "morgue"),
    ("leth-d-2", 38, 10, 45, 13, "morgue"),
    ("leth-d-2", 38, 15, 45, 18, "morgue"),
    ("lethe-z", 24, 9, 29, 13, "barracks"),
    ("lethe-z", 59, 1, 63, 5, "morgue"),
    ("rlyeh", 1, 5, 23, 14, "temple"),
    ("rlyeh", 1, 1, 9, 3, "temple"),
    ("rlyeh", 14, 0, 26, 4, "temple"),
    ("rlyeh", 1, 16, 9, 18, "temple"),
    ("rlyeh", 14, 15, 26, 19, "temple"),
    ("rlyeh", 33, 0, 39, 2, "temple"),
    ("rlyeh", 40, 16, 45, 18, "temple"),
)

INTENTIONAL_UNFILLED = (
    ("neulev", 50, 9, 54, 11, "throne"),
    ("leth-c-1", 63, 2, 70, 5, "throne"),
    ("leth-c-2", 63, 2, 70, 5, "morgue"),
    ("nkai-a-1", 1, 1, 75, 19, "morgue"),
    ("nkai-a-2", 1, 1, 75, 19, "morgue"),
    ("nkai-b", 1, 1, 75, 19, "morgue"),
    ("nkai-c", 1, 1, 75, 19, "morgue"),
    ("nkai-z", 1, 1, 75, 19, "morgue"),
)

EXPLICIT_TEMPLES = (
    ("gatetwn", 24, 7, 28, 8, "temple"),
    ("leth-c-1", 67, 7, 73, 10, "temple"),
    ("leth-c-2", 67, 7, 73, 10, "temple"),
    ("lethe-e", 39, 0, 47, 7, "temple"),
    ("lethe-f", 4, 1, 8, 2, "temple"),
)


def region_rows(resource):
    text = (REPO / "dat" / (resource + ".lua")).read_text(encoding="utf-8")
    pattern = re.compile(
        r'des\.region\(\{region=\{([^}]+)\},lit=-?\d+,type="([^"]+)"'
        r'(?:,filled=(\d+))?\}\)'
    )
    rows = []
    for coords, room_type, filled in pattern.findall(text):
        values = tuple(int(value.strip()) for value in coords.split(","))
        rows.append((*values, room_type, None if filled == "" else int(filled)))
    return rows


def find(rows, expected):
    resource, x1, y1, x2, y2, room_type = expected
    return next(
        (row for row in rows[resource]
         if row[:4] == (x1, y1, x2, y2) and row[4] == room_type),
        None,
    )


def main():
    assert len(RESOURCES) == 26 and len(set(RESOURCES)) == 26
    assert all((REPO / "dat" / (name + ".lua")).is_file() for name in RESOURCES)
    rows = {name: region_rows(name) for name in RESOURCES}

    assert len(NORMAL_FILL) == 36
    assert len(INTENTIONAL_UNFILLED) == 8
    assert len(EXPLICIT_TEMPLES) == 5
    assert len({*NORMAL_FILL, *INTENTIONAL_UNFILLED, *EXPLICIT_TEMPLES}) == 49

    for expected in NORMAL_FILL:
        row = find(rows, expected)
        assert row is not None, expected
        assert row[5] == 1, (expected, row)
    for expected in INTENTIONAL_UNFILLED:
        row = find(rows, expected)
        assert row is not None, expected
        assert row[5] in (None, 0), (expected, row)
    for expected in EXPLICIT_TEMPLES:
        row = find(rows, expected)
        assert row is not None, expected
        assert row[5] is None, (expected, row)

    nkai_b = (REPO / "dat/nkai-b.lua").read_text(encoding="utf-8")
    assert nkai_b.count('id="create monster"') == 2
    assert nkai_b.count('id="create monster",coord={44,17},buc="cursed",spe=0') == 1
    assert nkai_b.count('id="create monster",coord={45,18},buc="cursed",spe=0') == 1
    assert 'id="create monster",coord={44,17},buc="cursed",name="0"' not in nkai_b
    assert 'id="create monster",coord={45,18},buc="cursed",name="0"' not in nkai_b

    lethe_z = (REPO / "dat/lethe-z.lua").read_text(encoding="utf-8")
    for x, y, species in (
        (52, 8, "knight"), (61, 12, "wizard"), (65, 7, "rogue"),
        (62, 8, "aligned cleric"), (56, 13, "ranger"),
    ):
        line = next(line for line in lethe_z.splitlines()
                    if f'coord={{{x},{y}}}' in line and 'id="corpse"' in line)
        assert f'montype="{species}"' in line and "name=" not in line, line
        assert "spe=0" in line, line

    parser = (REPO / "src/sp_lev.c").read_text(encoding="utf-8")
    corpse_path = parser[parser.index('if (tmpobj.id == STATUE || tmpobj.id == EGG'):]
    assert 'get_table_str_opt(L, "montype"' in corpse_path
    assert "tmpobj.corpsenm = monsndx(pm)" in corpse_path
    assert 'get_table_int_opt(L, "filled", 0)' in parser

    props = (REPO / "sys/windows/vs/files.props").read_text(encoding="utf-8")
    nmake = (REPO / "sys/windows/Makefile.nmake").read_text(encoding="utf-8")
    for resource in RESOURCES:
        assert f'<Luafiles Include = "{resource}.lua"/>' in props
        assert f"$(DAT){resource}.lua" in nmake

    patchlevel = (REPO / "include/patchlevel.h").read_text(encoding="utf-8")
    assert "#define EDITLEVEL 7" in patchlevel
    print("PASS QA2 source contract: 36 normal-fill, 8 intentional-unfilled, "
          "5 explicit temples, DEF-009/010 parser representations, 26 resources")


if __name__ == "__main__":
    sys.exit(main() or 0)
