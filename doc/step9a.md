# Step 9A: Sheol (deterministic validation phase)

Current checkpoint status: automated gate and user manual validation **PASS**.
Sheol remains temporarily fixed at DoD108. The user authorized the combined
9A–9C checkpoint commit, cumulative README update and branch push; final
randomization/tagging remains deferred. The implementation-phase restrictions
and results below are historical; [step9.md](step9.md) records current status.

## Baseline and authority

- Local branch: `phase0/dod-length`.
- Starting HEAD: `5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`.
- Initial working tree/index: clean.
- Donor: `UnNetHack/UnNetHack`, immutable revision
  `439b8d63d3d1ca78fb08588dd43f61874114b21a` (the Step 8 donor).
- Source inspected using `git show`/`git grep` and a `git archive` of that
  revision, extracted outside this repository in `../step9-audit`.
- No README update, commit, tag, push, or final randomized Step 9 placement
  belongs to this phase. Step 9A must pass before Step 9B production work.

## Donor dependency audit before production edits

Actions use these classifications: **equivalent** (reuse), **extend** (narrow
local extension), **import** (absent), **exclude** (unnecessary donor global
system). This is an implementation checklist, not a claim of completed tests.

| Donor element | Donor file(s) | Local equivalent | Required action | Compatibility adaptation |
| --- | --- | --- | --- | --- |
| Six to eight descending levels, uniform length; middle at 2, palace at N-1/N | `dat/dungeon.def` | Lua dungeon topology, persisted branches | extend: optional Sheol at DoD108, preserve 6/7/8 weights | Exclude donor Gehennom attachment; no Vlad/Valley/invocation change |
| `sheolfil`, `sheolmid`, `palace_f`, `palace_e` | `dat/sheol.des` | Lua special levels | import all four resources | Convert `.des`; no legacy compiler |
| Voronoi terrain and Bezier passage generator, open first-floor alternative 1/3, lower narrow passages, cloud chance 1/5, stair-space/reachability rejection | `src/mksheol.c`, `src/spo_lev.c` | level-init C dispatch | import generator, expose Lua init style | Preserve donor procedural construction; document any safety fixes discovered by seed tests |
| Middle-map geometry, eight shuffled monster classes, two fire wands, locked hut, lighting | `dat/sheol.des` | map, shuffle, region, monster/object APIs | equivalent: convert exactly | Preserve local class membership; unrelated donor-global class additions excluded |
| Middle portal to Valley | `dat/sheol.des` | magic portals and ordinary stairs | exclude donor-global Valley shortcut | Preserve ordinary connected descent/return; no second Valley entrance |
| Palace maps, exact gates, loot rooms, nondiggable bounds, stair/arrival regions | `dat/sheol.des` | Lua maps/regions | equivalent: convert exact coordinates and loot | Ordinary saved special levels |
| Executioner and seven boss chests, two crystal picks, magic marker, grave rolls | `dat/sheol.des`, `src/monst.c`, `src/makemon.c` | unique-monster state, containers, Cleaver | import boss definition and inventory | Existing artifact uniqueness; blessed proofed Cleaver +0..5 and cloak of magic resistance; no invented rewards |
| Ice wall and crystal ice wall (`U`/`Y`) | `include/rm.h`, `src/vision.c`, `src/display.c`, `src/drawing.c`, `src/dig.c`, `src/zap.c`, `src/hack.c`, `src/dokick.c`, `src/dbridge.c`, `src/monmove.c`, `src/dogmove.c` | obstruction, terrain symbols, wall digging | import two real terrain IDs and complete interactions | Ordinary ice blocks vision; crystal ice is transparent but blocks movement; fire converts both to ice; magic digging cannot remove them; crystal pick required for crystal walls |
| Crystal pick | `src/objects.c`, `include/obj.h`, `src/dig.c`, `src/apply.c`, `src/invent.c`, `src/shk.c`, `src/hack.c` | pick-axe weapon-tool and skill | import: weight80/value500/12d-small,10d-large base damage; zero generation frequency | Use existing pick-tool paths, double digging effort; retain shop pick restrictions; no probability-table changes |
| Cold/ice floor trap | `include/trap.h`, `src/trap.c`, `src/mklev.c`, `src/drawing.c` | trap dispatch, cold item damage | import ice trap | Hero 4d4 cold and potion damage; monster 2d4 with cold resistance; Sheol random-trap priority 1/3, forbid fire traps there |
| Chillbug | `src/monst.c`, `src/monmove.c`, `src/worn.c` | xan class, fleeing, HP/status state | import definition and group AI | Group retreat below half HP, coordinated reattack, adjacent HP/status sharing, fast nonfleeing regeneration and fleeing AC bonus; do not globally alter other monsters |
| Blue slime | `src/monst.c`, `src/mhitu.c`, `src/mhitm.c`, `src/uhitm.c`, `src/hack.c`, `src/teleport.c`, `src/polyself.c`, `src/zap.c`, `src/dothrow.c` | physical attacks, trapped-state handling | import freeze attack and movement-only restraint | Preserve flight/flame/amorphous/water exceptions, pool freezing, thaw/teleport behavior; do not substitute paralysis |
| White naga and hatchling | `src/monst.c`, `src/mthrowu.c`, `src/dothrow.c`, `src/mondata.c`, `src/mon.c` | naga eggs/growth and spit | import cold spit, definitions, egg/growth mapping | Preserve donor frost projectile/freezing behavior and resistance/corpse definitions |
| Ice and crystal ice golems | `src/monst.c`, `src/makemon.c`, `src/dog.c` | golems, breath, HP dispatch | import exact attacks/resistances, HP130/160, no polymorph | Existing random breath implementation; donor taming restriction |
| Dark Angel | `src/monst.c`, `src/makemon.c` | angel/minion/equipment infrastructure | import hostile angel definition and required inventory handling | Use local minion extension conventions |
| Punisher | `src/monst.c`, `src/mcastu.c`, `src/monmove.c` | stationary flag, wizard/cleric spells | import exact stationary/no-regeneration statue and ten-spell dispatcher | Reuse nine existing spell effects, add punishment selection; summon rejection 2/3; exclude unrelated punishment-scroll spawning |
| Sheol-specific random pool, doubled Sheol weights, exclusions | `src/makemon.c`, `src/monst.c` | weighted reservoir sampling and class generation | extend using pinned per-species eligibility | Keep NetHack5 difficulty architecture and all non-Sheol existing generation unchanged; do not port global dragon randomization/renaming |
| Evil eye | `src/monst.c`, `src/mhitu.c`, `src/mhitm.c`, `src/apply.c`, `src/eat.c`, `src/uhitm.c`, `include/mondata.h` | gaze, luck, mirror, luckstone | import Sheol pool dependency | Preserve luck drain, mirror/corpse/luckstone interactions; exclude global summon-table addition |
| Weeping archangel (and directly required weeping behavior) | `src/monst.c`, `src/mon.c`, `src/mhitu.c`, `src/mhitm.c`, `src/makemon.c`, `src/apply.c`, `src/attrib.c`, `src/worn.c`, `src/muse.c` | gaze, teleport, monster movement, attributes | import Sheol pool dependency, quantum locking, mental gaze, energy/life drain and level teleport | Preserve branch-local behavior, population limit7 and uniqueness of tracked state; no invocation redesign |
| Arctic fern, sprout and spore | `src/monst.c`, `src/mon.c`, `src/mhitu.c`, `src/mondata.c` | Step8 fern lifecycle | extend existing implementation for cold variant | Preserve cold explosion, frozen-ground regrowth and maturation; no duplicate fern framework |
| All shared monsters/classes and ordinary randomized loot | `dat/sheol.des`, `src/monst.c`, `src/objects.c` | native NetHack5 definitions | equivalent for existing species/classes | Global donor species/dragon names, difficulty changes, unrelated objects and artifact frameworks excluded unless a direct branch dependency requires them |
| Floor flags, water/cloud, lights, trees, teleport restriction, hardfloor | `dat/sheol.des`, `src/spo_lev.c`, `src/mksheol.c` | existing terrain/flags | reuse; derive Sheol identity from dungeon name | No new branch-identity save field |
| Branch entry/exit messages | `src/do.c` | ordinary arrival messages | extend for cold-side-branch transition | Exclude donor achievements/endgame connections |
| IDs, saves, bones, checkpoints/recovery | `include/you.h`, `include/monst.h`, save/restore code | version guard and hardened serialization | audit every stored addition; combined Step9 EDITLEVEL4 | One uncommitted compatibility epoch; no migration; document new fields only if unavoidable |
| TTY/Windows tiles, DLB, builds | terrain/monster/object symbols, local manifests and tile tables | Step8 tile reuse/package process | add resources and correctly indexed display entries | Record artwork substitutions and verify both Release packages |

## Structure and manual target

Temporary parent: **DoD108**, downward branch stairs to Sheol level 1.
The donor chooses N=6,7,8 with equal probability. Level 1 is `sheolfil`, level 2
is `sheolmid`, levels 3 through N-2 use `sheolfil`, level N-1 is `palace_f`, and
level N is `palace_e`. Follow stairs down to the Executioner and upstairs back
to DoD108. The middle-level Valley portal is excluded to respect local topology.

## Implemented content and compatibility details

All four maps and the donor C filler generator are imported. The 15 monster
definitions retain the pinned donor attacks, resistances, flags and sizes;
NetHack5 names, explicit difficulty and minion allocation use the local ABI.
The source comparison checks each complete definition. Added species are:
arctic fern spore/sprout/adult, evil eye, chillbug, dark Angel, weeping angel,
weeping archangel, white naga hatchling/adult, blue slime, ice golem, crystal ice
golem, Executioner and Punisher.

The crystal pick is a normal zero-frequency pick tool, not an artifact. Ice
venom supports white-naga freezing spit. The cold trap, two wall types, actual
fire/digging behavior, cold fern lifecycle, group AI, Punisher spells, evil-eye
luck/mirror/corpse interactions and quantum-locking behavior use narrow local
extensions. The Executioner's Cleaver uses existing artifact uniqueness.

Deliberate adaptations and exclusions:

- Sheol is an optional DoD108 side branch. Exclude its donor Valley portal and
  all donor Vlad, Gehennom and invocation routing changes.
- Preserve the native NetHack5 dragon family in the shared random pool.
  UnNetHack's global dragon renaming/randomized identities are outside Sheol's
  scope. Missing unrelated global species are not imported merely because they
  occur in a broad class selector. No Sheol-specific eligible species is omitted.
- Restrict the supporting ordinary weeping angel to Sheol generation; do not
  add its donor global encounter distribution. Archangel birth limit is seven.
- Reuse native spell and monster-level-teleport infrastructure. Do not add the
  donor-wide post-invocation changes to weeping-angel attacks.
- Preserve existing Moria fern machinery, with arctic cold effects added to it.
  The same spore hostility and immunity handling applies across imported ferns.
- The local Lua map API requires legal x=1 for the one-cell full-frame marker.
  Middle-level placement uses x=1,y=2, equivalent to the donor's relative offset.
- Ice restraint uses the existing saved `u.uspare1` and monster `mspare1` fields;
  ordinary traps and paralysis remain independent. It is a movement timer,
  not inability to attack or use tools. Engulfing, teleportation, appropriate
  transformation/flight and fire release it as applicable.

## Save, IDs and display

Combined Step9 **EDITLEVEL4** replaces the committed EDITLEVEL3 guard. New
monster/object/terrain/trap/dungeon and display IDs require fresh games. There
are no new serialized fields and no save migration. Hero/monster spare fields
hold persistent ice restraint. Save, restore, bones and recovery structure
definitions remain unchanged; old-version checkpoints are rejected.

Both Windows tile builds include the new names at verified generated glyph
indices. Reused artwork: arctic spore → gas spore; arctic plants → violet fungus;
evil eye → floating eye; chillbug → xan; dark/weeping angel → Angel; archangel →
Archon; white nagas → guardian nagas; blue slime → green slime; ice golems →
glass golem; Executioner → Croesus; Punisher → stone golem; crystal pick →
pick-axe; ice venom → acid venom; ice walls → vertical wall; ice trap → fire
trap. These are visual substitutions, not monster or terrain substitutions.
The generated bitmap was inspected for corruption; in-game tiled readability
and distinguishing these reused images remain on the human inspection list.

## Validation results

External evidence is under `../step9-audit`; no fixture or build result is part
of the source diff. The evolving whole-project report is
[step9-playtest.md](step9-playtest.md).

| Check | Result / evidence |
| --- | --- |
| Four Lua maps vs pinned `.des` | PASS: 256 seeds/map, exact geometry and content/conditional checks |
| Native generator vs donor | PASS: 192 identical seeded upper/middle/lower outputs on each architecture (`generator-x64-2`, `generator-Win32-1`) |
| Production mechanics/database fixtures | PASS x64/Win32: freeze states/exceptions, pool conversion, trap damage/resistance/items, golem HP, birth limits, Sheol generation and chillbug AI |
| Complete Sheol traversal | PASS x64 six levels (`sheol-traversal-x64-7`), Win32 six levels (`sheol-traversal-Win32-2`), actual stairs both ways and correct DoD108 return |
| Filler and terminal saves | PASS on both traversal architectures |
| Fire, ordinary/magic/crystal digging | PASS actual x64 terrain arena (`sheol-terrain-x64-4`), including nondiggable palace crystal and altered-terrain save/restore |
| Blue-slime combat | PASS actual contact, movement restraint, save/restore and teleport release (`sheol-freeze-x64-3`) |
| Executioner | PASS actual melee damage, unique birth, alive save, death/equipment drops, seven chests/two picks/marker and dead save (`sheol-boss-x64-2`) |
| Final fresh topology | PASS four games each architecture: one DoD108 Sheol; prior Tomb/Moria placements vary, Castle200 and all temporary reservations preserved (`sheol-final-topology-*`) |
| Prior Tomb/Temple and Moria content | PASS actual branches/save/return and all ten Moria variants (`prior-branches-x64-2`, `prior-moria-maps-x64-1`) |
| Cumulative focused/source/depth/ledger/recovery | PASS both architectures (`sheol-regressions-x64-3`, `sheol-regressions-Win32-3`); earlier milestone assertions retained |
| Final Release solution builds | PASS x64 and Win32 (`build-sheol-gaze-*`), console and tiled binaries |
| DLB packages | PASS both: all four Sheol resources and dungeon bytes match source; all ten Moria maps and classic Tomb retained |
| Tile mappings | PASS new text entries, generated glyph references and 2,405-tile bitmap capacity (`sheol-tiles.log`) |
| Whitespace / scope | `git diff --check` PASS; README, save structs, shop tables and endgame boundaries checked unchanged |

The topology reader's initial left-margin assumption was corrected to accept
the TTY's indented columns; existing successful game captures were revalidated
without replacing or changing them. The final whole-project pass will repeat
the affected checks after all four branches coexist. The additional complete
six-level Moria round trip also passed (`prior-moria-traversal-x64-1`), returning
to its sampled DoD176 parent. **Step9A's gate passed; Step9B may proceed.**

## Files attributable to Step9A

- Maps: `dat/{sheolfil,sheolmid,palace_f,palace_e}.lua`, `dat/dungeon.lua`.
- Definitions: `include/{botl,defsym,extern,mcastu,monattk,mondata,monflag,
  monsters,objects,patchlevel,rm,sp_lev,trap,youprop}.h`.
- Runtime: `src/{apply,attrib,botl,cmd,dig,display,do,dog,dothrow,dungeon,eat,
  exper,explode,hack,insight,makemon,mcastu,mhitu,mklev,mkmap,mon,mondata,
  monmove,mthrowu,muse,nhlua,polyself,shk,sp_lev,teleport,trap,uhitm,vision,
  worn,zap}.c`.
- Build and artwork: `sys/unix/Makefile.top`, `sys/windows/Makefile.nmake`,
  `sys/windows/vs/files.props`, `win/share/{monsters,objects,other}.txt`.
- Tests: new `test/*step9a*`; compatibility updates in `run_step7.py`,
  `run_step8a_runtime.py`, `test_ledger_runtime.c`, `test_step7_runtime.c`,
  `test_step7_source.py`, `test_step8a_source.py`.
- Documents: this file, `step9.md` and `step9-playtest.md`.

## Manual wizard checklist

Subsequent Step9B death-path validation found missing imported species in the
post-release build's explicit corpse switch. The 15 Sheol species now enter
the ordinary corpse path, with donor `G_NOCORPSE` exclusions preserved. Actual
dispatcher tests cover every entry; see `step9b.md` and `step9-playtest.md`.

1. Start a **new** wizard game with the current x64 or Win32 Release binary.
   Use Ctrl-V to level-teleport to **108**. `#wizwhere` identifies the downward
   **Sheol** branch stair. Ordinary DoD stairs are also present; use the Sheol
   branch stair, confirmed by the cold arrival message and dungeon overview.
2. Descend `sheolfil` → `sheolmid` → filler levels → `palace_f` → `palace_e`.
   Total length is 6–8; the palace entrance is penultimate and Executioner's
   palace is last. Return upstairs through every connection to DoD108.
3. Inspect irregular ice/water/cloud passages. Compare opaque ice walls and
   transparent crystal ice. Fire melts both; magical digging removes neither.
   Ordinary pick works on ordinary ice only; wish for a `crystal pick` to test
   crystal ice, including the nondiggable palace enclosure.
4. Trigger an ice trap with and without cold resistance; check cold damage and
   vulnerable potions. Meet the new cold monsters. Blue slime and white naga
   can restrain movement while still allowing attacks and tools. Check Frozen,
   teleport/thaw release and a save while restrained.
5. Observe chillbug retreat/group behavior, Punisher spellcasting, cold fern
   spores, evil-eye luck/mirror interactions, and weeping angels while observed
   versus unobserved. Avoid treating wizard invulnerability as normal balance.
6. Fight the Executioner. Inspect its equipment drops and the seven chests,
   including two crystal picks and a marker. Save/reload both before and after
   the kill; the boss must not respawn. Also save/reload on an ordinary filler.
7. Inspect the reused tiles listed above in NetHackW; TTY terrain descriptions
   and the actual collision/vision behavior must agree with their identities.
