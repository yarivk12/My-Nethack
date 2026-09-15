"""Source contract for the Step 10C-C Outlands generator bridge."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
mkroom = (ROOT / "src" / "mkroom.c").read_text(encoding="utf-8")
mkmaze = (ROOT / "src" / "mkmaze.c").read_text(encoding="utf-8")
extern = (ROOT / "include" / "extern.h").read_text(encoding="utf-8")
runtime = (ROOT / "test" / "test_step10c_c_runtime.c").read_text(encoding="utf-8")

required = (
    "place_neutral_features", "mkkamereltowers", "mkminorspire",
    "mkfishingvillage", "mkwell", "mkpluhomestead", "mkpluvillage",
    "mkferrutower", "mkinvertzigg", "mkneuriver", "neuliquify",
)
for name in required:
    assert re.search(r"\n%s\s*\(" % name, mkroom), name

assert not re.search(r"\nmkferrufort\s*\(", mkroom)
assert "extern void place_neutral_features(void);" in extern

bridge = re.search(
    r"if \(load_special\(protofile\)\) \{(?P<body>.*?)dmonsfree\(\);",
    mkmaze, re.S,
)
assert bridge, "missing makemaz post-load bridge"
body = bridge.group("body")
assert body.count("place_neutral_features();") == 1
for proto in ("out1", "out2", "out3", "out4"):
    assert f'!strcmp(protofile, "{proto}.lua")' in body
assert sorted(re.findall(r'!strcmp\(protofile, "([^"]+)\.lua"\)', body)) == [
    "out1", "out2", "out3", "out4"
]

dispatch = re.search(
    r"\nplace_neutral_features\s*\(void\)\s*\{(?P<body>.*?)\n\}",
    mkroom, re.S,
)
assert dispatch, "missing dispatcher"
body = dispatch.group("body")
ordered = (
    "!rn2(30)", "mkkamereltowers();", "!rn2(16)",
    "mkfishingvillage();", "else if (!rn2(16))", "mkminorspire();",
    "else if (!rn2(16))", "mkfishingvillage();", "!rn2(8)",
    "mkneuriver();", "!rn2(8)", "mkpluvillage();", "!rn2(16)",
    "mkinvertzigg();", "!rn2(8)", "mkferrutower();", "!rn2(3)",
    "rnd(4) + rn2(4)", "mkpluhomestead();",
)
pos = -1
for token in ordered:
    pos = body.find(token, pos + 1)
    assert pos >= 0, token
assert "mkferrufort" not in body

river = re.search(r"\nmkneuriver\s*\(void\)\s*\{(?P<body>.*?)\n\}", mkroom, re.S)
fishing = re.search(
    r"\nmkfishingvillage\s*\(void\)\s*\{(?P<body>.*?)\n\}", mkroom, re.S
)
assert river and "neuliquify(" in river.group("body")
assert fishing and "mkfishinghut(left);" in fishing.group("body")
assert fishing and "mkwell(left);" in fishing.group("body")
assert 'OUTLANDS_NOTE("river-cell");' in mkroom
assert "struct outlands_payload" in runtime
assert "collect_payload(payload)" in runtime
for field in ("semantic_monsters", "semantic_objects", "deep_one", "plumach_rilmani",
              "ziggurat_wizard", "mirrors", "rakuyo", "puddles", "moats"):
    assert field in runtime, field

for name in ("out1", "out2", "out3", "out4"):
    text = (ROOT / "dat" / f"{name}.lua").read_text(encoding="utf-8")
    assert "DEFERRED_10C_C: place_neutral_features()" not in text
    assert "DEFERRED_10C_D:" not in text
assert "step10c_post_load_content(&u.uz)" in mkmaze

print("PASS Step 10C-C source/bridge/dispatch contract")
