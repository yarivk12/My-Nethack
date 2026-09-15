"""Check actual Release enum dumps against every generated glyph/tile entry.

Usage: py -3 -B test/test_step10b_generated.py [NetHack.exe ...]
This does not replace species/mechanics tests or launch a game.
"""
from pathlib import Path
import re
import struct
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
binaries = [Path(p).resolve() for p in sys.argv[1:]] or [
    repo / "binary/Release/x64/NetHack.exe",
    repo / "binary/Release/Win32/NetHack.exe",
]
previous = None
for exe in binaries:
    dump = subprocess.check_output([str(exe), "--dumpenums"], text=True, cwd=exe.parent)
    assert "enum monnums" in dump and "AFTER_LAST_ARTIFACT" in dump, "missing enum output"
    if previous is not None:
        assert dump == previous, "architecture enum mismatch"
    previous = dump
    values = {}
    for name in ("NUMMONS", "NUM_OBJECTS", "AFTER_LAST_ARTIFACT", "MAX_GLYPH"):
        matches = re.findall(r"\b" + name + r"\s*=\s*(\d+)", dump)
        assert len(matches) == 1, name
        values[name] = int(matches[0])
    assert values["MAX_GLYPH"] == 22 * values["NUMMONS"] + 2 * values["NUM_OBJECTS"] + 250
    assert values["NUM_OBJECTS"] - 1 <= 32767
    assert values["AFTER_LAST_ARTIFACT"] - 1 <= 127
    print("PASS Release enum dump", exe, values)

generated = (repo / "src/tile.c").read_text(encoding="utf8")
count = int(re.search(r"int total_tiles_used = (\d+)", generated)[1])
assert count < 32767
table = generated.split("glyph_map glyphmap[MAX_GLYPH] = {", 1)[1].split("};", 1)[0]
rows = re.findall(r"NO_CUSTOMCOLOR, NO_CUSTOMCOLOR,\s+(\d+), 0 \},\s*/\* \[(\d+)\]", table)
assert len(rows) == values["MAX_GLYPH"]
assert [int(g) for _, g in rows] == list(range(values["MAX_GLYPH"]))
assert all(0 <= int(t) < count for t, _ in rows)
bitmap = (repo / "win/win32/tiles.bmp").read_bytes()
assert bitmap[:2] == b"BM"
offset = struct.unpack_from("<I", bitmap, 10)[0]
width, height = struct.unpack_from("<ii", bitmap, 18)
bits = struct.unpack_from("<H", bitmap, 28)[0]
stride = ((width * bits + 31) // 32) * 4
assert len(bitmap) >= offset + stride * abs(height)
assert width % 16 == 0
assert ((count + width // 16 - 1) // (width // 16)) * 16 <= abs(height)
print("PASS all", len(rows), "glyph indices/tile bounds;", count,
      "tiles; complete bitmap", width, abs(height), bits, "bpp")
