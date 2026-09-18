"""B1/ogre source boundary gate and exact projection for historical tests."""
from pathlib import Path
import subprocess

BASE = "48fe150af4a087fd2f4ff576b43c1c96cc08c6fe"


from step13_source_projection import project as step13_project

def project(path, text):
    text = step13_project(path, text)
    if path == "include/monsters.h":
        start = text.index("    /* Step 10B append-only branch content.")
        end = text.index("    /*\n     * mons_init()", start)
        addition = text[start:end]
        # Native declaration tests verify every field; this enforces the
        # append-only Step 10B block and preserves every old declaration.
        assert addition.count("    MON(") == 73 and 'MON(NAM("ogre mage")' in addition
        for name in ("plumach rilmani", "ferrumach rilmani", "cuprilach rilmani",
                     "argenach rilmani", "aurumach rilmani", "amm kamerel",
                     "hudor kamerel", "sharab kamerel", "ara kamerel",
                     "argentum golem", "living doll", "living lectern",
                     "parasitized doll", "bestial dervish", "ethereal dervish",
                     "flashing lake", "frosted lake", "smoldering lake",
                     "sparkling lake", "blood shower", "many-taloned thing",
                     "deep blue cube", "pitch black cube", "prayerful thing",
                     "hemorrhagic thing", "many-eyed seeker",
                     "voice in the dark", "tiny being of light",
                     "man-faced millipede", "mirrored moonflower",
                     "crimson writher", "radiant pyramid", "Kuker",
                     "lurking one", "small goat spawn", "goat spawn",
                     "giant goat spawn", "blessed", "mouth of the goat",
                     "apprentice witch", "witch", "coven leader",
                     "The Good Neighbor", "Hmnyw-Pharaoh", "migo worker",
                     "migo soldier", "migo philosopher", "migo queen",
                     "byakhee", "dark young", "deep dweller", "deminymph",
                     "gnoll ghoul", "gug", "Illurien of the Myriad Glimpses",
                     "nightgaunt", "oread", "minotaur priestess",
                     "priest of an unknown god", "shoggoth", "star spawn",
                     "Shattered Ziggurat cultist",
                     "Shattered Ziggurat knight",
                     "Shattered Ziggurat wizard", "hunting horror",
                     "blasphemous lurker", "alhoon", "Center of All",
                     "Father Dagon", "Mother Hydra", "Great Cthulhu",
                     "witch's familiar"):
            assert f'MON(NAM("{name}")' in addition
        text = text[:start] + text[end:]
    if path == "include/objects.h":
        start = text.index("/* Step 10B3-1 append-only object extension.")
        end = text.index("#if defined(OBJECTS_DESCR_INIT)", start)
        addition = text[start:end]
        assert addition.count("OBJECT(OBJ(") == 21
        assert "MARKER(FIRST_STEP10B_OBJECT, SICKLE)" in addition
        assert "MARKER(LAST_STEP10B_OBJECT, LIFELESS_DOLL)" in addition
        text = text[:start] + text[end:]
    if path == "include/artilist.h":
        start = text.index("    /* Step 10B3-2: neutral keys")
        end = text.index("#if !defined(ARTI_ENUM)", start)
        text = text[:start] + text[end:]
    if path == "src/restore.c":
        familiar_bones_remap = '''        if (ghostly && mtmp->data == &mons[PM_WITCH_S_FAMILIAR]
            && mtmp->mspare1) {
            unsigned mapped_witch_id;

            if (lookup_id_mapping((unsigned) mtmp->mspare1,
                                  &mapped_witch_id))
                mtmp->mspare1 = (long) mapped_witch_id;
            else
                mtmp->mspare1 = 0L;
        }
'''
        assert text.count(familiar_bones_remap) == 1
        text = text.replace(familiar_bones_remap, "")
    replacements = {
        "include/global.h": (
            "#define MAXDUNGEON 18 /* Step 10 capacity; topology is registered separately */",
            "#define MAXDUNGEON 16 /* current maximum number of dungeons */"),
        "include/you.h": (
            "    Bitfield(sum_entered, 1);      /* Step 10: entered Sum of All (Center) */\n"
            "    /* 6 free bits; sum_entered owns the first formerly free event bit. */",
            "    /* 7 free bits */"),
        "include/rm.h": (
            "    Bitfield(lethe, 1);        /* Step 10: amnesiac water, not new terrain */\n", ""),
        "src/mklev.c": ("    svl.level.flags.lethe = 0;\n", ""),
        "include/patchlevel.h": ("#define EDITLEVEL 8", "#define EDITLEVEL 4"),
        "include/hack.h": (
            "\n/* Step 10 keeps serialized IDs append-only.  obj.oartifact is a char;\n"
            " * use the signed-char limit even on ports where plain char is unsigned. */\n"
            "typedef char artifact_id_must_fit_saved_char[(NROFARTIFACTS <= 127) ? 1 : -1];\n", ""),
    }
    if path == "include/global.h":
        text = text.replace(
            "\n/* Controlled return values for dormant Step 10B Neutral selector branches. */\n"
            "#define STEP10B_NEUTRAL_QUADRUPED (-10)\n"
            "#define STEP10B_CENTER_NEUTRAL 1\n"
            "#define STEP10B_CENTER_LOST_CITIES 2\n"
            "#define STEP10B_KEY_SECOND 1\n"
            "#define STEP10B_KEY_THIRD 2\n"
            "#define STEP10B_KEY_ORDINARY 3\n"
            "#define STEP10B_CTHULHU_GAS_RADIUS 2\n"
            "#define STEP10B_CTHULHU_CLOUD_SIZE 5\n"
            "#define STEP10B_CTHULHU_GAS_DAMAGE 30\n"
            "#define STEP10B_CTHULHU_GAS_TTL 30\n", "")
        start = text.index("\n/* Step 10B4 behavior context")
        end = text.index("#define MAXLEVEL", start)
        text = text[:start] + text[end:]
    if path == "src/mklev.c":
        hook = "    step10c_set_level_flags(&u.uz);\n"
        assert text.count(hook) == 1
        text = text.replace(hook, "")
    if path in replacements:
        after, before = replacements[path]
        assert text.count(after) == 1, (path, "missing/duplicate B1 change")
        text = text.replace(after, before)
    return text


if __name__ == "__main__":
    repo = Path(__file__).resolve().parents[1]
    for path in ("include/global.h", "include/you.h", "include/rm.h",
                 "src/mklev.c", "include/patchlevel.h", "include/hack.h",
                 "include/monsters.h", "include/objects.h", "include/artilist.h",
                 "src/save.c", "src/restore.c", "src/bones.c", "util/recover.c",
                 "README.md"):
        # Freeze the verified Step 12 checkpoint; older IDs also have donor gates.
        old = subprocess.check_output(["git", "show", "f27b7f20fd90da8dcb7aa0e90ee85f1af1123444:" + path], cwd=repo)
        old = old.decode("utf8").replace("\r\n", "\n")
        current = (repo / path).read_text(encoding="utf8")
        expected = old.replace("#define EDITLEVEL 6", "#define EDITLEVEL 8")
        assert project(path, current) == project(path, expected), path
    assert "PM_OGRE_MAGE" in (repo / "src/mcastu.c").read_text(encoding="utf8")
    assert "PM_OGRE_MAGE" in (repo / "src/mhitu.c").read_text(encoding="utf8")
    print("PASS B1/ogre exact scope; old IDs, save routing and README unchanged")
    print("Complete Step 10B 73-monster/21-object append blocks project cleanly")
