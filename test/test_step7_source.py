"""Source/packaged-data regression contracts; run from any directory.

python test/test_step7_source.py [binary/Release/x64/nhdat500 ...]
"""
from pathlib import Path
import io
import re
import subprocess
import sys
from step9c_source_projection import project
from test_step10b_source import project as step10b_project

repo=Path(__file__).resolve().parents[1]
base="9191de7079624e94acdde19d07814a0f03ce9450"
def old(path):
    return subprocess.check_output(["git","show",base+":"+path],cwd=repo).decode().replace("\r\n","\n")
def now(path):
    return (repo/path).read_text(encoding="utf-8")
for path in ["src/mklev.c","src/mkroom.c","src/shknam.c","include/dungeon.h",
             "include/global.h","src/save.c","src/restore.c","src/bones.c",
             "dat/bigrm-14.lua"]:
    current = project(path, step10b_project(path, now(path)))
    if path == "src/mklev.c":
        # Step 9 adds an ice-trap branch to the trap selector. Normalize only
        # these exact additions; retain the whole-file Step 5 shop contract.
        additions = [
            ('''    case ICE_TRAP:
        if (!In_sheol(&u.uz))
            kind = NO_TRAP;
        break;
''', ''),
            ('if (!Inhell || In_sheol(&u.uz))', 'if (!Inhell)'),
            ('''    } else if (In_sheol(&u.uz) && !rn2(3)) {
        kind = ICE_TRAP;
    } else if (Inhell && !In_sheol(&u.uz) && !rn2(5)) {''',
             '''    } else if (Inhell && !rn2(5)) {'''),
        ]
        for after, before in additions:
            assert current.count(after) == 1
            current = current.replace(after, before)
        # Only the new bog's trap exclusion differs; all shop generation,
        # probabilities, room scheduling and other bytes remain protected.
        before = "    if (tm && is_pool_or_lava(tm->x, tm->y))"
        after = "    if (tm && (is_pool_or_lava(tm->x, tm->y)\n               || IS_BOG(levl[tm->x][tm->y].typ)))"
        assert current.count(after) == 1
        current = current.replace(after, before)
    if path == "src/mkroom.c":
        # Step 8 adds this exact, Moria-only memorial block. Keep the original
        # whole-file protection for every other byte, including Step 6 rooms.
        memorial = '''                else if (moria_level(&u.uz) == 6 && !rn2(1000)) {
                    struct engr *ep;
                    boolean exists = FALSE;

                    for (ep = head_engr; ep; ep = ep->nxt_engr)
                        if (!strncmp(ep->engr_txt[actual_text], "Guest41,", 8))
                            exists = TRUE;
                    if (!exists)
                        make_grave(sx, sy,
                            "Guest41, Wherever you are, I hope you're doing fine");
                }
'''
        assert current.count(memorial) == 1
        current = current.replace(memorial, "")
    assert current==old(path),path
def function(text,name):
    matches=re.findall(r"(?m)^(?:staticfn )?(?:const )?\w+(?: \*)?\n"
                       +name+r"\([\s\S]*?^\}",text)
    assert len(matches)==1,name
    return matches[0]
for path,name in [("src/apply.c","dorub"),("src/potion.c","djinni_from_bottle"),
                  ("src/zap.c","makewish")]:
    assert function(now(path),name)==function(old(path),name),name
for path,pattern,expected in [
    ("include/monsters.h",r'MON\(NAM\("([^"\n]+)"\)',
     {"shadow", "deep orc", "swamp fern", "swamp fern sprout",
      "swamp fern spore", "Durin's Bane", "Watcher in the Water",
      "arctic fern spore", "evil eye", "chillbug", "dark Angel",
      "weeping angel", "weeping archangel", "arctic fern sprout",
      "arctic fern", "white naga hatchling", "white naga", "blue slime",
      "ice golem", "crystal ice golem", "Executioner", "Punisher",
      "baby glowing dragon", "glowing dragon", "chromatic cave dragon",
      "crystal ooze", "deep one", "deeper one", "deepest one", "selkie", "seal",
      "oceanid", "yurian", "Coure Eladrin", "Noviere Eladrin", "Bralani Eladrin",
      "mote of light", "water dolphin", "singing sand", "living mirage", "wraithworm",
      "Alabaster elf", "Alabaster elf-elder", "sentinel of Mithardir", "Alabaster mummy",
      "first wraithworm", "aspect of The Silence", "ogre mage",
      "plumach rilmani", "ferrumach rilmani", "cuprilach rilmani",
      "argenach rilmani", "aurumach rilmani", "amm kamerel",
      "hudor kamerel", "sharab kamerel", "ara kamerel", "argentum golem",
      "living doll", "living lectern", "parasitized doll",
      "bestial dervish", "ethereal dervish", "flashing lake",
      "frosted lake", "smoldering lake", "sparkling lake", "blood shower",
      "many-taloned thing", "deep blue cube", "pitch black cube",
      "prayerful thing", "hemorrhagic thing", "many-eyed seeker",
      "voice in the dark", "tiny being of light", "man-faced millipede",
      "mirrored moonflower", "crimson writher", "radiant pyramid", "Kuker",
      "lurking one", "small goat spawn", "goat spawn", "giant goat spawn",
      "blessed", "mouth of the goat", "apprentice witch", "witch",
      "coven leader", "The Good Neighbor", "Hmnyw-Pharaoh", "migo worker",
      "migo soldier", "migo philosopher", "migo queen", "byakhee",
      "dark young", "deep dweller", "deminymph", "gnoll ghoul", "gug",
      "Illurien of the Myriad Glimpses", "nightgaunt", "oread",
      "minotaur priestess", "priest of an unknown god", "shoggoth",
      "star spawn", "Shattered Ziggurat cultist",
      "Shattered Ziggurat knight", "Shattered Ziggurat wizard",
      "hunting horror", "blasphemous lurker", "alhoon", "Center of All",
      "Father Dagon", "Mother Hydra", "Great Cthulhu", "witch's familiar"}),
    ("include/objects.h",r'TOOL\("([^"\n]+)"',{"magic candle", "crystal pick"})]:
    before=set(re.findall(pattern,old(path))); after=set(re.findall(pattern,now(path)))
    assert after-before==expected and before<=after,path
assert not (repo/"dat/tomb-2.lua").exists()
dungeon=now("dat/dungeon.lua")
assert dungeon.count('name = "The Lost Tomb"')==2
assert 'levels = { { name = "tomb-1", bonetag = "Z", base = 1 } }' in dungeon
assert 'tomb-2' not in dungeon
assert 'base = 200' in dungeon
for path in ["sys/unix/Makefile.top","sys/windows/Makefile.nmake","sys/windows/vs/files.props"]:
    assert "tomb-1.lua" in now(path) and "tomb-2" not in now(path)
assert "#define EDITLEVEL 5" in now("include/patchlevel.h") # Step 10 save epoch
print("PASS source: protected Step 5/6, vanilla wishing/lamp rub, exact Step 7/8/9 additions, classic one-level Tomb/manifests, Step 10 save epoch")

for arg in sys.argv[1:]:
    data=Path(arg).read_bytes(); stream=io.BytesIO(data)
    revision,count,_,_,total=map(int,stream.readline().split())
    assert revision==1 and total==len(data)
    entries=[]
    for _ in range(count):
        name,offset=stream.readline().split()
        assert name[:1]==b'n'
        entries.append((name[1:].decode(),int(offset)))
    assert not any("tomb-2" in name for name,_ in entries)
    for name in ["tomb-1.lua","moloch.lua","bigrm-14.lua","dungeon.lua"]:
        indices=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(indices)==1,(arg,name)
        i=indices[0]; end=entries[i+1][1] if i+1<len(entries) else total
        actual=data[entries[i][1]:end].replace(b"\r\n",b"\n")
        assert actual.decode()==now("dat/"+name),(arg,name,"stale packaged resource")
    print("PASS packaged resource bytes match final sources:",arg)
