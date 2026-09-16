"""Pinned donor, protected source and actual DLB/ZIP freshness checks.

python test/test_step11_resources.py DONOR_THEMERMS RELEASE_DIR [PACKAGE_ZIP]
The donor file must come from the documented immutable commit.
"""
from pathlib import Path
import hashlib
import io
import re
import subprocess
import sys
import zipfile

repo = Path(__file__).resolve().parents[1]
base = "fdc824dfd95e96fbafcbf66177b2fd674818f35b"
donor = Path(sys.argv[1]).read_text(encoding="utf8")
release = Path(sys.argv[2])


def old(path):
    return subprocess.check_output(["git", "show", base + ":" + path], cwd=repo).decode().replace("\r\n", "\n")


def now(path):
    return (repo / path).read_text(encoding="utf8")


def descriptor(text, name):
    start = re.search(r"(?m)^   \{\n      name = ['\"]" + re.escape(name) + r"['\"],", text).start()
    end = text.index("\n   },", start) + len("\n   },")
    return text[start:end]


local = now("dat/themerms.lua")
assert local.startswith(old("dat/themerms.lua")), "ordinary weighted pool changed"
for name in ("Wizard study", "Storeroom vault", "Super Honeycomb", "Dragon hall"):
    expected = descriptor(donor, name)
    if name == "Dragon hall":
        expected = expected.replace("nh.mon_difficulty('black dragon') + 1", "21")
        expected = expected.replace("function loot(x, y)", "local function loot(x, y)")
        expected = expected.replace("selection.circle(hoardctr.x, hoardctr.y, 5, 1)",
                                    "selection.circle(hoardctr.x, hoardctr.y, 5, 1) & floor")
        expected = expected.replace("            colors = {", "            local colors = {")
        expected = expected.replace("'yellow', 'gold'", "'yellow'")
    assert descriptor(local, name) == expected, name
print("PASS four pinned donor bodies; only documented Hall adaptations")

# All pre-existing authored Lua content and stable gameplay identities remain
# byte-equivalent after line-ending normalization. This is a cumulative check
# against the actual audited starting commit, not an obsolete Step 6 quota.
protected = subprocess.check_output(["git", "ls-tree", "-r", "--name-only", base, "dat"], cwd=repo).decode().splitlines()
protected = [p for p in protected if p.endswith(".lua") and p != "dat/themerms.lua"]
protected += ["include/global.h", "include/monsters.h", "include/objects.h",
              "include/artilist.h", "include/rm.h", "include/trap.h", "src/shknam.c",
              "src/shk.c", "src/save.c", "src/bones.c", "src/files.c", "util/recover.c",
              "sys/windows/vs/files.props", "sys/windows/Makefile.nmake"]
for path in protected:
    assert now(path) == old(path), path
mklev = now("src/mklev.c")
old_chain = old("src/mklev.c")
start = 'else if (u_depth > 1 && u_depth < depth(&medusa_level)'
end = 'do_mkroom(COCKNEST);'
normalized = mklev.replace('custom_vanilla(', 'do_mkroom(')
assert normalized[normalized.index(start):normalized.index(end, normalized.index(start))+len(end)] == old_chain[old_chain.index(start):old_chain.index(end, old_chain.index(start))+len(end)]
assert "STEP6B_ROOM_" not in now("src/dungeon.c") + now("src/hack.c") + mklev
for path in (repo / "src").glob("*.c"):
    text = path.read_text(encoding="utf8")
    assert not re.search(r"dilapidated.armory|lemure.pit", text, re.I), path
print(f"PASS {len(protected)} protected files, identities, shop stock/mimic code and deferred inventory")


def resources(data):
    stream = io.BytesIO(data)
    revision, count, _, _, total = map(int, stream.readline().split())
    assert revision == 1 and total == len(data)
    entries = []
    for _ in range(count):
        name, offset = stream.readline().split()
        assert name[:1] == b"n"
        entries.append((name[1:].decode(), int(offset)))
    return {name: data[offset:entries[i+1][1] if i+1 < len(entries) else total]
            for i, (name, offset) in enumerate(entries)}


data = (release / "nhdat500").read_bytes()
packed = resources(data)
lua_count = 0
for name, content in packed.items():
    if name.endswith(".lua") and (repo / "dat" / name).exists():
        assert content.decode().replace("\r\n", "\n") == now("dat/" + name), name
        lua_count += 1
assert "themerms.lua" in packed
if len(sys.argv) > 3:
    with zipfile.ZipFile(sys.argv[3]) as package:
        for name in ("NetHack.exe", "NetHackW.exe", "nhdat500"):
            matches = [n for n in package.namelist() if n.replace("\\", "/").split("/")[-1] == name]
            assert len(matches) == 1, (name, matches)
            assert package.read(matches[0]) == (release / name).read_bytes(), name
    print("PASS ZIP contains exact normal executable, GUI and DLB bytes")
print(f"PASS {lua_count} packaged Lua resources match source, including significant map whitespace")
print("DONOR_SHA256", hashlib.sha256(Path(sys.argv[1]).read_bytes()).hexdigest())
