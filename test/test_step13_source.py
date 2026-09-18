"""Step 13 source/persistence integration contract; native runtime is separate."""
from pathlib import Path
import re
R=Path(__file__).resolve().parents[1]
def read(p): return (R/p).read_text(encoding='utf8')
def body(p,name):
 s=read(p);m=re.search(r'(?m)^'+name+r'\([^\n]*[\s\S]*?^\}',s)
 assert m,(p,name)
 return m[0]
obj=read('include/obj.h');hdr=read('include/enhance.h')
for typ,field in [('uint32','o_enh_props'),('uint32','o_enh_known'),('uint8','o_enh_quality'),('uint8','o_enh_flags')]:
 assert re.search(r'\b'+typ+r'\s+'+field+r'\s*;',obj)
 assert f'obj->{field} != otmp->{field}' in body('src/invent.c','mergable')
assert 'unsigned long obranch_props;' in obj
assert re.search(r'#define EDITLEVEL\s+7\b',read('include/patchlevel.h'))
for i,name in enumerate(['FIRE','COLD','SHOCK','TRUEFLIGHT','WARNING','SEARCHING','STEALTH','CUMBERSOME']):
 assert int(re.search(r'#define OEP_'+name+r'\s+(0x[0-9a-f]+)U',hdr)[1],16)==1<<i
for name in ['enhancement_hit_bonus','enhancement_damage_bonus','enhancement_quality_bonus','enhancement_visible_props']:
 code=body('src/enhance.c',name)
 assert not re.search(r'\b(rnd|rn2|pline|You|enhancement_set)\(',code),name
 assert not re.search(r'->o_enh_\w+\s*(?:=(?!=)|\|=|&=|\+\+)',code),name
assert 'enhancement_weapon_effects' not in body('src/weapon.c','dmgval')
for file,name in [('src/artifact.c','artifact_exists'),('src/artifact.c','mk_artifact'),('src/do_name.c','oname')]:
 assert 'enhancement_strip_for_artifact' in body(file,name)
for file,name in [('src/uhitm.c','hmon_hitmon'),('src/mhitu.c','hitmu'),('src/mhitm.c','mdamagem'),('src/mthrowu.c','thitu_enhanced'),('src/mthrowu.c','ohitmon_enhanced')]:
 assert body(file,name).count('enhancement_weapon_effects(')==1,(file,name)
assert 'enhancement_weapon_effects' not in read('src/dothrow.c')
assert 'enhancement_worn_off' in body('src/worn.c','setworn')
assert 'enhancement_worn_off' in body('src/worn.c','setnotworn')
assert '*otmp = *obj;' in body('src/mkobj.c','splitobj')
assert '*otmp = cg.zeroobj;' in body('src/mkobj.c','mksobj')
assert 'sizeof *d_##dt' in read('src/sfstruct.c')
assert 'sizeof(struct obj)' in read('src/version.c') or 'sizeof (struct obj)' in read('src/version.c')
for file in ['src/makemon.c','src/shknam.c','src/sp_lev.c']:
 assert not re.search(r'\benhancement_set\(',read(file)),file
print('PASS Step 13 representation, separate namespaces, pure queries, artifact/attack/wear hooks, epoch and no acquisition')
