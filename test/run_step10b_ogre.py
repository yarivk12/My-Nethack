"""Compile the appended ogre mage and its actual native equipment/spell gates.

Run in a VS developer shell, with an output directory under the project _qa directory.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)
source = (repo / "src/makemon.c").read_text(encoding="utf8")
ogre = source.split("    case S_OGRE:\n", 1)[1].split("    case S_TROLL:", 1)[0]
parts = [re.search(r"(?m)^boolean\nstep10b_is_witch\([\s\S]*?^\}", source)[0],
         "static void ogre_equipment(struct monst *mtmp) { int mm=monsndx(mtmp->data);"
         "switch(mtmp->data->mlet) { case S_OGRE:" + ogre + "} }"]
source = (repo / "src/mcastu.c").read_text(encoding="utf8")
parts.append(re.search(r"(?m)^staticfn int\nmith_spell_damage\([\s\S]*?^\}", source)[0])
guard = source.split("mith_castmm(struct monst *caster", 1)[1]
guard = guard[guard.index("    if ((caster->data !="):guard.index("    for (tries")]
parts.append("static int spell_eligible(struct monst *caster, struct monst *target,"
             "struct attack *attack) { int syllable=mith_mon_syllable(caster);"
             + guard + "return M_ATTK_HIT; }")
assert "mtmp->data == &mons[PM_OGRE_MAGE]" in source, "hero spell dice not wired"
hero = (repo / "src/mhitu.c").read_text(encoding="utf8")
assert hero.count("        case AT_MAGC:\n") == 1
route = hero.split("        case AT_MAGC:\n", 1)[1].split("        default: /* no attack */", 1)[0]
parts.append("static int hero_spell_route(struct monst *mtmp, struct attack *mattk,"
             "boolean range2) { int sum[1]={0}, i=0; boolean foundyou=TRUE;"
             "switch(mattk->aatyp) { case AT_MAGC:" + route + "} return sum[0]; }")
(out / "step10b_ogre.h").write_text("\n".join(parts), encoding="utf8")
subprocess.run(["cl", "/nologo", "/std:c11", "/W4", "/WX",
                "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
                "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
                "/I" + str(out), "/Fe:" + str(out / "step10b_ogre.exe"),
                str(repo / "test/test_step10b_ogre.c"),
                str(repo / "src/monst.c"), str(repo / "src/objects.c")], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b_ogre.exe")], cwd=out, text=True)
assert "PASS ogre mage" in result, "missing runtime output"
print(result, end="")

# Reuse the existing full native spell-dispatch harness; its production
# bodies are extracted afresh, never copied from an old build product.
mm = out / "monster-spells"
subprocess.run([sys.executable, "-B", str(repo / "test/run_step9c_elder_mm.py"), str(mm)],
               cwd=repo, check=True)
subprocess.run(["cl", "/nologo", "/std:c11", "/W4",
                "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
                "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
                "/I" + str(mm), "/Fe:" + str(mm / "step10b_ogre_cast.exe"),
                str(repo / "test/test_step10b_ogre_cast.c"),
                str(repo / "src/monst.c"), str(repo / "src/objects.c")], cwd=mm, check=True)
result = subprocess.check_output([str(mm / "step10b_ogre_cast.exe")], cwd=mm, text=True)
assert "PASS ogre mage actual spell dispatch" in result
print(result, end="")
