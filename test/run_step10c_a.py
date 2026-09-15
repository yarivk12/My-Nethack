"""Validate and load every Step 10C-A static map/resource body.

Usage: python -B test/run_step10c_a.py RELEASE_DIRECTORY OUTPUT_DIRECTORY [DONOR]

The source gate compares each local map body and directive inventory with the
pinned donor.  The runtime gate then loads every packaged resource in its own
fresh wizard game outside the repository.
"""
from collections import Counter
from pathlib import Path
import re
import subprocess
import sys

from run_step8a_runtime import Game


COMMIT = "17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0"
RESOURCES = {
    "neulev": "neutrality.des",
    "gatetwn": "neutrality.des",
    "out1": "neutrality.des",
    "out2": "neutrality.des",
    "out3": "neutrality.des",
    "out4": "neutrality.des",
    "spire": "neutrality.des",
    "sumall": "neutrality.des",
    "leth-a-1": "neutrality.des",
    "leth-a-2": "neutrality.des",
    "lethe-b": "neutrality.des",
    "leth-c-1": "neutrality.des",
    "leth-c-2": "neutrality.des",
    "leth-d-1": "neutrality.des",
    "leth-d-2": "neutrality.des",
    "lethe-e": "neutrality.des",
    "lethe-f": "neutrality.des",
    "lethe-g": "neutrality.des",
    "lethe-z": "neutrality.des",
    "nkai-a-1": "neutrality.des",
    "nkai-a-2": "neutrality.des",
    "nkai-b": "neutrality.des",
    "nkai-c": "neutrality.des",
    "nkai-z": "neutrality.des",
    "rlyeh": "neutrality.des",
    "lbyrnth": "labr.des",
}
DONOR_PATHS = {
    "neutrality.des": "dnethack-3.4.3/dat/neutrality.des",
    "labr.des": "dnethack-3.4.3/dat/labr.des",
}

# Explicit compatibility decisions for donor identities that are not literal
# local object-table names or statue montypes.  The source gate checks these
# decisions against every donor occurrence so a fallback cannot be silent.
IDENTITY_DECISIONS = {
    "object": {
        "blessed +3 bone viperwhip": {
            "category": 1, "resource": "lethe-f", "local": "viperwhip", "count": 3,
        },
        "trapped metal box": {
            "category": 1, "resource": "lethe-f", "local": "large box", "count": 1,
        },
        "gold amulet of magical breathing": {
            "category": 1, "resource": "lethe-f",
            "local": "amulet of magical breathing", "count": 1,
        },
        "star sapphire": {
            "category": 2, "resource": "lethe-f", "local": "sapphire", "count": 2,
        },
        "flying boots": {
            "category": 3, "resource": "lethe-g",
            "local": None, "count": 1,
        },
        "cursed +12 deep long sword named The Sword of the Deeps": {
            "category": 1, "resource": "lethe-z", "local": "long sword", "count": 1,
            "deferred": True,
        },
    },
    "montype": {
        "dryad": {
            "category": 2, "resource": "lethe-f", "local": "oread", "count": 1,
        },
        "elder priest": {
            "category": 2, "resource": "lethe-g", "local": "high priest", "count": 1,
        },
        "lethe elemental": {
            "category": 2, "resource": "lethe-g",
            "local": "water elemental", "count": 2,
        },
        "god": {
            "category": 3, "resource": "lethe-f", "local": None, "count": 4,
        },
        "gnoll": {
            "category": 3, "resource": "lethe-g", "local": None, "count": 1,
        },
    },
}
CATEGORY_LABELS = {
    1: "exact local equivalent/renamed identity",
    2: "semantically equivalent local adaptation",
    3: "no valid local equivalent",
}


def donor_text(donor, filename):
    return subprocess.check_output(
        ["git", "-C", str(donor), "show", COMMIT + ":" + DONOR_PATHS[filename]],
        text=True,
    ).replace("\r\n", "\n")


def donor_block(text, name):
    start = re.search(
        r"^MAZE:\s*[\"']?" + re.escape(name) + r"[\"']?.*$", text, re.MULTILINE
    )
    assert start, "missing donor resource " + name
    tail = text[start.start():]
    next_level = re.search(r"^MAZE:", tail[1:], re.MULTILINE)
    return tail[: next_level.start() + 1 if next_level else len(tail)]


def donor_operations(block):
    operations = []
    for line in block.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        if line in ("MAP", "WALLIFY", "NOMAP"):
            operations.append((line, line))
            continue
        match = re.match(r"([A-Z_]+)(?:\[\d+%\])?:", line)
        if match:
            operations.append((match.group(1), line))
    return operations


def maps_in_donor(block):
    return [body.rstrip("\n") for body in re.findall(
        r"^MAP\n(.*?)^ENDMAP$", block, re.MULTILINE | re.DOTALL
    )]


def maps_in_local(text):
    return re.findall(
        r"des\.map\(\{[^\n]*map=\[=\[\n(.*?)\n\]=\]\}\)",
        text,
        re.MULTILINE | re.DOTALL,
    )


def local_counts(text):
    return Counter(re.findall(r"\bdes\.([a-z_]+)\(", text))


def donor_quote_entries(block, directive):
    """Return ordered entries using the donor's quote class for a directive."""
    entries = []
    pattern = re.compile(
        r"^" + directive + r"(?:\[\d+%\])?:\s*'\"'\s*,\s*(?P<rest>.*)$"
    )
    for line in block.splitlines():
        match = pattern.match(line.strip())
        if not match:
            continue
        rest = match.group("rest")
        named = re.match(r'"([^"]+)"', rest)
        if named:
            entries.append(("named", named.group(1)))
        elif rest.startswith("random"):
            entries.append(("random", None))
        else:
            raise AssertionError((directive, line))
    return entries


def assert_quote_semantics(local, donor_body, name):
    """Audit every donor quote-class use without treating it as local class data."""
    monster_entries = donor_quote_entries(donor_body, "MONSTER")
    object_entries = donor_quote_entries(donor_body, "OBJECT")
    quote_class = r'class="\""'

    # The donor quote monster class is not registered in this tree. Named
    # Rilmani entries therefore retain their exact identity via id=, while a
    # future donor random-class entry must be represented by the explicit
    # local pool helper rather than one collapsed species.
    assert not any(
        "des.monster" in line and quote_class in line
        for line in local.splitlines()
    ), name + " retains an invalid donor monster quote class"
    named_monsters = [value for kind, value in monster_entries if kind == "named"]
    local_rilmani = re.findall(
        r'des\.monster\(\{[^}\n]*\bid="([^"]+ Rilmani)"[^}\n]*\}\)',
        local,
    )
    assert local_rilmani == named_monsters, name + " Rilmani quote order/identity differs"
    random_monsters = sum(kind == "random" for kind, _ in monster_entries)
    if random_monsters:
        assert "Donor random class '\"' adapted to local Rilmani pool" in local, name
        assert local.count("step10c_random_rilmani()") == random_monsters, name

    # Object class '"' is a valid local object-class selector. Preserve both
    # donor random selection and donor named-object selection in place.
    local_quote_objects = [
        line for line in local.splitlines()
        if "des.object" in line and quote_class in line
    ]
    assert len(local_quote_objects) == len(object_entries), (
        name + " object quote-class count differs"
    )
    named_objects = [value for kind, value in object_entries if kind == "named"]
    local_named_objects = []
    for line in local_quote_objects:
        match = re.search(r'\bid="([^"]+)"', line)
        if match:
            local_named_objects.append(match.group(1))
    assert local_named_objects == named_objects, name + " object quote order/identity differs"


def donor_monster_entries(block):
    entries = []
    pattern = re.compile(
        r"^MONSTER(?:\[(?P<prob>\d+)%\])?:\s*"
        r"(?P<class>'(?:\\'|[^'])*'|random)\s*,\s*(?P<rest>.*)$"
    )
    for line in block.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        match = pattern.match(line)
        if not match:
            continue
        class_token = match.group("class")
        symbol = None if class_token == "random" else class_token[1:-1]
        if symbol is not None:
            symbol = symbol.replace("\\'", "'")
        rest = match.group("rest")
        named = re.match(r'"([^"]+)"', rest)
        selection = "named" if named else "random" if rest.startswith("random") else "other"
        entries.append(
            {
                "symbol": symbol,
                "selection": selection,
                "id": named.group(1) if named else None,
                "prob": int(match.group("prob")) if match.group("prob") else None,
                "line": line,
            }
        )
    return entries


def donor_object_entries(block):
    entries = []
    pattern = re.compile(
        r"^(?P<directive>OBJECT|CONTAINER)(?:\[\d+%\])?:\s*"
        r"(?:'(?:\\'|[^'])*'|random)\s*,\s*(?P<rest>.*)$"
    )
    for line in block.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        match = pattern.match(line)
        if not match:
            continue
        rest = match.group("rest")
        named = re.match(r'"([^"]+)"', rest)
        if named:
            entries.append(
                {
                    "directive": match.group("directive"),
                    "id": named.group(1),
                    "rest": rest[named.end():],
                    "line": line,
                }
            )
    return entries


def donor_statue_montypes(block):
    montypes = []
    for entry in donor_object_entries(block):
        if entry["id"].casefold() != "statue":
            continue
        match = re.match(r"\s*,\s*\([^)]*\)\s*,\s*\"([^\"]+)\"", entry["rest"])
        if match:
            montypes.append((match.group(1), entry["line"]))
    return montypes


def local_named_monsters(repo):
    text = (repo / "include" / "monsters.h").read_text(encoding="utf8")
    names = set()
    for match in re.finditer(r"\bNAMS?\(([^)]*)\)", text, re.DOTALL):
        names.update(value.casefold() for value in re.findall(r'"([^"]+)"', match.group(1)))
    return names


def local_named_objects(repo):
    text = (repo / "include" / "objects.h").read_text(encoding="utf8")
    names = set()
    macros = (
        "OBJ", "WEAPON", "ARMOR", "RING", "AMULET", "TOOL", "FOOD",
        "POTION", "SCROLL", "SPBOOK", "WAND", "GEM", "ROCK", "BALL",
        "CHAIN", "VENOM", "CONTAINER",
    )
    pattern = r"\b(?:" + "|".join(macros) + r")\(\s*\"([^\"]+)\""
    for match in re.finditer(pattern, text):
        names.add(match.group(1).casefold())
    return names


def assert_object_substitution_mechanics(repo, donor, local_by_name):
    """Prove approved object substitutions do not hide a gameplay mismatch."""
    donor_objects = subprocess.check_output(
        [
            "git", "-C", str(donor), "show",
            COMMIT + ":dnethack-3.4.3/src/objects.c",
        ],
        text=True,
    )
    local_objects = (repo / "include" / "objects.h").read_text(encoding="utf8")

    # The donor boots grant FLYING.  The closest local boots grant the
    # materially different LEVITATION property, so the source gate must keep
    # flying boots as an explicit Category 3 no-fallback decision.
    assert re.search(
        r'BOOTS\("flying boots".*?\bFLYING\b', donor_objects, re.DOTALL
    ), "donor flying-boots property audit no longer finds FLYING"
    assert re.search(
        r'BOOTS\("levitation boots".*?\bLEVITATION\b', local_objects,
        re.DOTALL,
    ), "local levitation-boots property audit no longer finds LEVITATION"
    assert 'BOOTS("flying boots"' not in local_objects
    flying_resource = local_by_name["lethe-g"]
    assert not re.search(
        r'^\s*des\.object\(\{[^\n]*\bid="flying boots"',
        flying_resource,
        re.MULTILINE,
    )
    assert not re.search(
        r'^\s*des\.object\(\{[^\n]*\bid="levitation boots"',
        flying_resource,
        re.MULTILINE,
    )

    # Star sapphire and sapphire share the relevant blue hard-gem display
    # family, but are not numerically identical.  The approved adaptation is
    # therefore limited to the two decorative Father Dagon contents, where
    # no later map logic reads the gem identity.
    assert re.search(
        r'GEM\("star sapphire".*?"blue".*?GEMSTONE.*?CLR_BLUE',
        donor_objects,
        re.DOTALL,
    ), "donor star-sapphire gem-family audit failed"
    assert re.search(
        r'GEM\("sapphire".*?"blue".*?GEMSTONE.*?CLR_BLUE',
        local_objects,
        re.DOTALL,
    ), "local sapphire gem-family audit failed"
    father = re.search(
        r'name="Father Dagon".*?\nend\}\)',
        local_by_name["lethe-f"],
        re.DOTALL,
    )
    assert father and father.group(0).count('id="sapphire"') == 2
    assert 'id="star sapphire"' not in local_by_name["lethe-f"]
    print(
        "PASS object substitution mechanics: star sapphire is decorative-only "
        "blue GEM adaptation; flying boots FLYING != local LEVITATION and has no fallback",
        flush=True,
    )


def local_object_ids(local):
    return [
        match.group(1)
        for match in re.finditer(r'des\.object\(\{[^\n]*\bid="([^"]+)"[^\n]*\}\)', local)
    ]


def local_montype_ids(local):
    return [
        match.group(1)
        for match in re.finditer(r'\bmontype="([^"]+)"', local)
    ]


def assert_identity_portability(repo, donor, local_by_name):
    """Audit every named donor identity before relying on runtime discovery."""
    local_monsters = local_named_monsters(repo)
    local_objects = local_named_objects(repo)
    donor_monster_names = Counter()
    donor_object_names = Counter()
    donor_montypes = Counter()
    for name, donor_file in RESOURCES.items():
        block = donor_block(donor_text(donor, donor_file), name)
        for entry in donor_monster_entries(block):
            if entry["selection"] == "named":
                donor_monster_names[entry["id"]] += 1
        for entry in donor_object_entries(block):
            donor_object_names[entry["id"]] += 1
        for montype, _ in donor_statue_montypes(block):
            donor_montypes[montype] += 1

    missing_monsters = {
        identity for identity in donor_monster_names
        if identity.casefold() not in local_monsters
    }
    missing_objects = {
        identity for identity in donor_object_names
        if identity.casefold() not in local_objects
    }
    missing_montypes = {
        identity for identity in donor_montypes
        if identity.casefold() not in local_monsters
    }
    expected_objects = set(IDENTITY_DECISIONS["object"])
    expected_montypes = set(IDENTITY_DECISIONS["montype"])
    assert not missing_monsters, "unclassified missing donor monster identities: " + repr(sorted(missing_monsters))
    assert missing_objects == expected_objects, (
        "donor object identity decision set differs: "
        + repr(sorted(missing_objects))
        + " vs "
        + repr(sorted(expected_objects))
    )
    assert missing_montypes == expected_montypes, (
        "donor statue montype decision set differs: "
        + repr(sorted(missing_montypes))
        + " vs "
        + repr(sorted(expected_montypes))
    )

    for identity, decision in IDENTITY_DECISIONS["object"].items():
        assert decision["category"] in CATEGORY_LABELS, decision
        resource = decision["resource"]
        local = local_by_name[resource]
        marker = "Category " + str(decision["category"]) + ":"
        assert marker in local, (resource, identity, "missing documented decision")
        if decision.get("deferred"):
            for fragment in ('id="long sword"', 'name="The Sword of the Deeps"',
                             'buc="cursed"', "spe=12", "deep=true"):
                assert fragment in local, (resource, identity, fragment)
        elif decision["category"] == 3:
            assert decision["local"] is None, decision
            assert "Category 3: donor " + identity in local, (
                resource, identity, "missing bounded decision"
            )
            assert 'id="levitation boots"' not in local, (
                resource, identity, "category 3 received an unapproved fallback"
            )
        else:
            assert local_object_ids(local).count(decision["local"]) >= decision["count"], (
                resource, identity, decision
            )
        assert donor_object_names[identity] == decision["count"], (
            identity, donor_object_names[identity], decision["count"]
        )
    for identity, decision in IDENTITY_DECISIONS["montype"].items():
        assert decision["category"] in CATEGORY_LABELS, decision
        resource = decision["resource"]
        local = local_by_name[resource]
        if decision["category"] == 2:
            assert local_montype_ids(local).count(decision["local"]) >= decision["count"], (
                resource, identity, decision
            )
        else:
            assert decision["local"] is None, decision
            assert 'montype="' + identity + '"' not in local
            assert "Category 3:" in local, (resource, identity, "missing bounded decision")
        assert donor_montypes[identity] == decision["count"], (
            identity, donor_montypes[identity], decision["count"]
        )

    print(
        "PASS donor identity audit: named monster missing=none; "
        + "missing object decisions=" + str(len(missing_objects))
        + "; missing statue montype decisions=" + str(len(missing_montypes)),
        flush=True,
    )
    for kind in ("object", "montype"):
        for identity, decision in IDENTITY_DECISIONS[kind].items():
            print(
                "  " + kind + " " + identity + " -> "
                + (decision["local"] or "no fallback")
                + " [category " + str(decision["category"]) + ": "
                + CATEGORY_LABELS[decision["category"]] + "]",
                flush=True,
            )


def local_monster_descriptors(local):
    descriptors = []
    previous_nonempty = None
    pattern = re.compile(r"des\.monster\(\{(?P<body>[^}\n]*)\}\)")
    for line in local.splitlines():
        stripped = line.strip()
        match = pattern.search(line)
        if match:
            body = match.group("body")
            class_match = re.search(r'class="((?:\\.|[^"])*)"', body)
            id_match = re.search(r'\bid="([^"]+)"', body)
            descriptors.append(
                {
                    "class": (
                        class_match.group(1).replace('\\"', '"').replace("\\'", "'")
                        if class_match
                        else None
                    ),
                    "id": id_match.group(1) if id_match else None,
                    "prob": (
                        int(re.search(r"if percent\((\d+)\) then", previous_nonempty).group(1))
                        if previous_nonempty and re.search(
                            r"if percent\((\d+)\) then", previous_nonempty
                        )
                        else None
                    ),
                    "line": line,
                }
            )
        if stripped and not stripped.startswith("--"):
            previous_nonempty = stripped
    return descriptors


def local_monster_symbols(repo):
    symbols = set()
    pattern = re.compile(
        r"MONSYM\(\s*\d+,\s*'((?:\\'|[^'])*)'"
    )
    for match in pattern.finditer(
        (repo / "include" / "defsym.h").read_text(encoding="utf8")
    ):
        symbols.add(match.group(1).replace("\\'", "'"))
    return symbols


def assert_monster_class_portability(repo, donor, local_by_name):
    """Reject unadapted donor monster classes and report the full symbol audit."""
    valid = local_monster_symbols(repo)
    inventory = Counter()
    unsupported = Counter()
    statuses = {}
    for name, donor_file in RESOURCES.items():
        donor_body = donor_block(donor_text(donor, donor_file), name)
        entries = donor_monster_entries(donor_body)
        for entry in entries:
            if entry["symbol"] is None:
                inventory["<random-class>"] += 1
            else:
                inventory[entry["symbol"]] += 1
        local_descriptors = local_monster_descriptors(local_by_name[name])
        local_classes = {
            descriptor["class"]
            for descriptor in local_descriptors
            if descriptor["class"] is not None
        }
        assert local_classes <= valid, (
            name + " contains a monster class/display symbol absent from local build: "
            + repr(sorted(local_classes - valid))
        )
        for symbol in sorted({
            entry["symbol"]
            for entry in entries
            if entry["symbol"] is not None and entry["symbol"] not in valid
        }):
            symbol_entries = [entry for entry in entries if entry["symbol"] == symbol]
            named = [entry for entry in symbol_entries if entry["selection"] == "named"]
            random = [entry for entry in symbol_entries if entry["selection"] == "random"]
            if named:
                unsupported[(symbol, "named")] += len(named)
                statuses.setdefault(symbol, set()).add("fixed named species")
            if random:
                unsupported[(symbol, "random")] += len(random)
                statuses.setdefault(symbol, set()).add("true random-class selection")
            assert not any(
                descriptor["class"] == symbol for descriptor in local_descriptors
            ), (name, symbol, "unsupported donor class survived")
            if named:
                expected = [(entry["id"], entry["prob"]) for entry in named]
                names = {entry["id"] for entry in named}
                actual = [
                    (descriptor["id"], descriptor["prob"])
                    for descriptor in local_descriptors
                    if descriptor["id"] in names
                ]
                assert actual == expected, (
                    name + " fixed named species/order/probability changed for class "
                    + repr(symbol)
                )
            if random:
                marker = (
                    "PORTABILITY_10C_A: donor random monster class "
                    + repr(symbol)
                    + " adapted to local pool"
                )
                helper = "step10c_random_monster(" + repr(symbol) + ")"
                assert marker in local_by_name[name], (name, symbol, "missing pool marker")
                assert local_by_name[name].count(helper) == len(random), (
                    name, symbol, "random pool call count differs"
                )
    inventory_text = ", ".join(
        symbol + "=" + str(count) for symbol, count in sorted(inventory.items())
    )
    unsupported_text = ", ".join(
        repr(symbol) + ":" + kind + "=" + str(count)
        for (symbol, kind), count in sorted(unsupported.items())
    ) or "none"
    status_text = ", ".join(
        repr(symbol) + " (" + ", ".join(sorted(kinds)) + ")"
        for symbol, kinds in sorted(statuses.items())
    ) or "none"
    print("PASS monster class/display audit:", inventory_text, flush=True)
    print("  unsupported donor occurrences:", unsupported_text, flush=True)
    print("  unsupported classifications:", status_text, flush=True)


def assert_source_resource(repo, donor, name, donor_file):
    local_path = repo / "dat" / (name + ".lua")
    assert local_path.is_file(), local_path
    local = local_path.read_text(encoding="utf8").replace("\r\n", "\n")
    assert COMMIT in local, name + " is not pinned to the required donor commit"
    assert not re.search(
        r'des\.region\(\{[^\n]*\blit=(?:true|false)\b', local
    ), name + " uses boolean lit in local des.region table form"

    donor_body = donor_block(donor_text(donor, donor_file), name)
    assert maps_in_local(local) == maps_in_donor(donor_body), name + " map body differs"
    assert_quote_semantics(local, donor_body, name)

    donor_ops = Counter(op for op, _ in donor_operations(donor_body))
    local_ops = local_counts(local)
    assert local_ops["map"] == donor_ops["MAP"], name
    assert local_ops["level_init"] == donor_ops["INIT_MAP"], name
    assert local_ops["region"] == donor_ops["REGION"], name
    assert (
        local_ops["stair"] + local.count('type="stair-up"')
        == donor_ops["STAIR"]
    ), name
    assert local_ops["door"] == donor_ops["DOOR"], name
    assert local_ops["trap"] == donor_ops["TRAP"], name
    assert local_ops["gold"] == donor_ops["GOLD"], name
    assert local_ops["drawbridge"] == donor_ops["DRAWBRIDGE"], name
    assert local_ops["altar"] == donor_ops["ALTAR"], name
    assert local_ops["mazewalk"] == donor_ops["MAZEWALK"], name
    assert local_ops["non_diggable"] == donor_ops["NON_DIGGABLE"], name
    assert local_ops["non_passwall"] == donor_ops["NON_PASSWALL"], name
    assert local_ops["wallify"] == donor_ops["WALLIFY"], name
    assert local_ops["teleport_region"] == donor_ops["TELEPORT_REGION"], name
    omitted_objects = local.count("Category 3: donor flying boots")
    assert (
        local_ops["object"] + omitted_objects
        == donor_ops["OBJECT"] + donor_ops["CONTAINER"]
    ), name

    dependent_monsters = sum(
        "monster[" in line for op, line in donor_operations(donor_body)
        if op == "MONSTER"
    )
    assert local_ops["monster"] == donor_ops["MONSTER"] - dependent_monsters, name

    if donor_ops["GEOMETRY"]:
        assert local_ops["map"] == donor_ops["GEOMETRY"], name
    if donor_ops["RANDOM_PLACES"]:
        assert local.count("local place={") == donor_ops["RANDOM_PLACES"], name
    if donor_ops["BRANCH"]:
        assert local.count('type="branch"') == donor_ops["BRANCH"], name
    if donor_ops["PORTAL"]:
        assert local.count("Portal contract:") == donor_ops["PORTAL"], name
    if donor_ops["RANDOM_MONSTERS"]:
        assert "DEFERRED_10C_C: random_monsters()" in local, name
    if donor_ops["FLAGS"]:
        assert (
            local_ops["level_flags"]
            or "step10c_set_level_flags" in (repo / "src/dungeon.c").read_text(
                encoding="utf8")
        ), name
    if donor_ops["NOMAP"]:
        assert "Donor NOMAP marker" in local, name

    forbidden = (
        "place_neutral_features", "mkkamereltowers", "mkminorspire",
        "mkfishingvillage", "mkwell", "mkpluhomestead", "mkpluvillage",
        "mkferrutower", "mkinvertzigg", "mkneuriver", "neuliquify",
    )
    for line in local.splitlines():
        for function_name in forbidden:
            if function_name + "(" in line:
                if name in ("out1", "out2", "out3", "out4") \
                   and function_name == "place_neutral_features":
                    assert line.startswith(
                        "-- Step 10C-C: the engine invokes "
                        "place_neutral_features()"
                    ), (name, function_name, line)
                else:
                    assert line.lstrip().startswith("-- DEFERRED_10C_C"), (
                        name, function_name, line
                    )
    print("PASS source body", name, flush=True)


def assert_packaging(repo):
    props = (repo / "sys/windows/vs/files.props").read_text(encoding="utf8")
    nmake = (repo / "sys/windows/Makefile.nmake").read_text(encoding="utf8")
    props_entries = set(re.findall(r'<Luafiles Include = "([^"]+)"', props))
    for name in RESOURCES:
        filename = name + ".lua"
        assert filename in props_entries, "missing VS package entry " + filename
        assert "$(DAT)" + filename in nmake, "missing nmake entry " + filename
    untouched = subprocess.check_output(
        ["git", "diff", "--name-only", "--", "README.md"],
        cwd=repo,
        text=True,
    ).splitlines()
    assert not untouched, "Step 10 must not modify README.md"
    print("PASS package registration", len(RESOURCES), "resources", flush=True)


def load_resource(release, output, name, has_static_map, directory_name=None):
    game = Game(release, output / (directory_name or name))
    try:
        # WinPTY's current tty path drops lowercase alphabetic keystrokes in
        # this line editor; extended commands are case-insensitive, so use
        # uppercase input to exercise the packaged command itself reliably.
        game.send("#WIZLOADDES\n")
        try:
            game.wait("Load which des lua file?")
        except AssertionError:
            game.send("\x1b")
            game.settle()
            game.send("#WIZLOADDES\n")
            game.wait("Load which des lua file?")
        # File names are case-insensitive on the packaged Windows runtime;
        # uppercase input is needed for the same WinPTY line-editor issue.
        game.send(name.upper() + ".LUA\n", 2)
        game.settle()
        text = game.text()
        assert "Lua error" not in text, (name, text)
        probe = '''
local ox,oy=nh.abscoord(0,0);local walk=0
for x=1,79 do for y=0,20 do
 local m=nh.getmap(x-ox,y-oy)
 if m.typ_name=="room" or m.typ_name=="corr" then walk=walk+1 end
end end
%s
nh.pline("STEP10C_A_OK")
''' % ("assert(walk>0,\"empty generated map\")" if has_static_map else "-- resource intentionally defers its map generator")
        game.lua(probe)
        print("PASS runtime resource", name, flush=True)
    finally:
        game.close()


def main():
    repo = Path(__file__).resolve().parents[1]
    source_only = "--source-only" in sys.argv
    args = [arg for arg in sys.argv[1:] if arg != "--source-only"]
    release = Path(args[0]).resolve() if len(args) > 0 else repo / "binary/Release/x64"
    artifact_root = repo / "_qa"
    output = Path(args[1]).resolve() if len(args) > 1 else artifact_root / "step10c-a-tests" / "x64"
    donor = (Path(args[2]).resolve() if len(args) > 2 else
             artifact_root / "dnethack-donor-pinned")
    assert output != repo and (output == artifact_root or artifact_root in output.parents), \
        "runtime output must be under the project _qa directory"
    assert donor.is_dir(), donor
    if not source_only:
        assert (release / "NetHack.exe").is_file(), release / "NetHack.exe"
    assert sorted(RESOURCES) == sorted(set(RESOURCES)), "duplicate resource name"
    assert_packaging(repo)
    local_by_name = {
        name: (repo / "dat" / (name + ".lua")).read_text(encoding="utf8").replace("\r\n", "\n")
        for name in RESOURCES
    }
    assert_monster_class_portability(repo, donor, local_by_name)
    assert_identity_portability(repo, donor, local_by_name)
    assert_object_substitution_mechanics(repo, donor, local_by_name)
    for name, donor_file in RESOURCES.items():
        assert_source_resource(repo, donor, name, donor_file)
    if source_only:
        print("PASS Step 10C-A source gate", flush=True)
        return
    output.mkdir(parents=True, exist_ok=True)
    for name in RESOURCES:
        local = (repo / "dat" / (name + ".lua")).read_text(encoding="utf8")
        try:
            load_resource(release, output, name, "des.map(" in local)
        except AssertionError as error:
            # WinPTY can race the first extended-command prompt with the
            # startup welcome redraw. Retry only that harness-level failure
            # in a fresh game directory; resource/Lua failures still fail.
            if "Missing Load which des lua file?" not in str(error):
                raise
            load_resource(
                release, output, name, "des.map(" in local,
                directory_name=name + "-fresh-prompt-retry",
            )
    print("PASS Step 10C-A source and packaged runtime gate", flush=True)


if __name__ == "__main__":
    main()
