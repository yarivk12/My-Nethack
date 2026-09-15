# Step 9B: Dragon Caves — final production parent placement

Current production code selects Dragon Caves' parent once from the shared
persistent DL30–199 scheduler. Historical user manual validation used
temporary DoD109; the detailed fixed-parent findings below are historical.
The final compiled closeout matrix passes on x64 and Win32;
[step9.md](step9.md) records the authoritative status.

## Authority and gate

Local branch `phase0/dod-length`; the historical implementation baseline was
`5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`. The combined closeout includes
the approved Step9A–9C implementation, randomized placement and final README
publication.

Donor: `UnNetHack/UnNetHack`, **439b8d63d3d1ca78fb08588dd43f61874114b21a**.
Read the immutable exported files in `../_qa/step9-audit/unnethack-pinned` and use
`git show` from the external donor clone for automated comparisons. No revision
switch or legacy level compiler is used; the combined closeout handles commit,
tag and publication after validation.

## Exact source-derived structure

`dat/dungeon.def` defines a chaotic, mazelike, four-level descending dungeon,
with one fixed layout each: `drgnA`, `drgnB` (town), `drgnC`, `drgnD`.
All maps are in `dat/dragons.des`. There are no alternative map resources.
Temporary local entrance: downward branch stairs on **DoD109**. Map A's donor
Valley portal is excluded; both ordinary internal stairs and return to the
known local parent must work. Map D is terminal, hardfloor and noteleport.

| Map | Source content and randomized elements |
| --- | --- |
| A | Two random stairs; 16 hostile random dragons, four worms, four traps; three gem-class objects, one tool-class object, three random objects; trees, dead trees, pools and bog |
| B | Two stairs restricted to x=0..63; 17 hostile dragons, four worms, four traps; three gems, three random objects, one tool; lit room x67..72/y9..11, open door (66,10), nondiggable x64..75/y6..15 |
| C | Two random stairs; 17 hostile dragons, four worms, four traps; three gems, three random objects, one tool; pools, moat water, bog and trees |
| D | Upstairs (73,18); 22 random hostile dragons plus three explicit chromatic dragons at (1,7), (2,6), (1,6); six worms, four traps; hoard, boulder, 12 random gems, three each tools/weapons/potions/scrolls, five random objects |

D's hoard preserves all ten specified gold stacks: 600+12d100 once,
600+10d100 four times, 400+10d100 once, 200+5d100 three times, 100+5d100 once;
also 2d10 randomly placed 1d100 stacks. Four named potion rolls are 60% each
(gain ability, gain level, full healing, enlightenment), each with independent
80% ring and 40% amulet rolls. Five fixed-position gem rolls remain.

**The apparent dragon shop is not a functioning donor shop.** The string
`"dragon shop"` occurs only in `dat/dragons.des`; it is absent from
`util/lev_main.c:room_types`. `util/lev_comp.y:room_type` warns and substitutes
`OROOM` for an unknown name. Therefore preserve the lit ordinary room, door,
nondiggable bounds and town flag. Do not invent stock, services or a merchant.

## Dependency classification before production changes

Actions: **reuse** = present/equivalent; **extend** = narrow local extension;
**import** = missing required content; **exclude** = unrelated donor-global
behavior. The table records intended implementation, not completed validation.

| Donor element | Donor file(s) | Local equivalent | Required action | Compatibility adaptation |
| --- | --- | --- | --- | --- |
| Four fixed layouts and randomized placement/loot | `dat/dragons.des` | NetHack5 Lua map and selection APIs | import four Lua resources, compare across seeds | Exact geometry, coordinates, class rolls, dice and independent probabilities; no `.des` compiler |
| Four descending levels, chaotic/maze, B town | `dat/dungeon.def` | Persistent dungeon/branch table | extend with one DoD109 branch | Keep the existing early reservation of 108–111; no independent floor roll |
| Donor Gehennom attachment and A Valley portal | `dat/dungeon.def`, `dat/dragons.des` | Optional side-branch stairs | exclude global routing | Return through DoD109; no Valley/Gehennom/invocation change |
| Bog M, dead tree t, tree T, pool P, moat } | `include/rm.h`, `src/hack.c`, `trap.c`, `monmove.c`, `dig.c`, `zap.c`, `potion.c`, `engrave.c` | Step8 bog/dead-tree implementation | reuse | Same pinned donor semantics: mud delay, exceptions, wetting, boulders, freeze/melt/dry and water effects; no new bog type |
| Misnamed dragon shop | `dat/dragons.des`, `util/lev_main.c`, `util/lev_comp.y` | Ordinary lit Lua region | reuse actual compiled donor behavior | Explicitly preserve OROOM fallback; no shop import or global shop changes |
| Ten eligible adult dragon identities, zero-frequency babies | `src/monst.c`, `makemon.c`, `spo_lev.c`, `o_init.c` | Native colored dragons plus missing lava species | extend branch class eligibility; import glowing dragon/baby | Preserve native gray/silver/red/white/orange/black/blue/green/yellow equivalents; add required lava identity; do not add native-only gold dragon to donor branch pool |
| Donor tatzelworm/amphitere/draken/lindworm/sarkany/sirrush/leviathan/wyvern/guivre naming and random color-property reassignment | `src/monst.c`, `o_init.c`, `objnam.c` | Native fixed colored identities | exclude global shuffle/renaming | Names differ deliberately; native attacks/difficulty outside this branch remain authoritative |
| Glowing dragon and baby, light radius1, growth/eggs | `src/monst.c`, `mondata.c`, `include/mondata.h` | Dragon body, light sources, growth pairs and eggs | import exact new definitions and necessary mapping | Isolate ordinary generation; no replacement/rebalance of native gold dragon; fixed lava identity instead of donor global shuffle |
| Lava breath | `include/monattk.h`, `src/zap.c`, `mthrowu.c`, `polyself.c`, `pager.c` | Native ray/fire damage machinery | import lava attack behavior | Fire-resisted damage; reflection stops the jet; no wall bounce; diggable stone wall becomes lit lava; ice walls melt; doors burn; retain donor terrain distinctions |
| Glowing scales/mail | `src/objects.c`, `do_wear.c`, `light.c`, `timeout.c`, `objnam.c` | Native gold-scale light paths and dragon armor | import scales/mail | Donor stone resistance, worn light, radius/BUC behavior, price500/900, AC9/5; no donor-wide armor AC rebalance |
| Three chromatic dragons | `dat/dragons.des`, `src/monst.c`, `muse.c`, `include/youprop.h` | Native dragon/random breath and reflection | import non-unique G_NOGEN, non-genocidable/nopoly species | Full donor attacks, eight resistances, intrinsic reflection; three independent encounters, not a unique boss |
| Chromatic scales/mail | `src/objects.c`, `do_wear.c`, `worn.c`, `muse.c`, `pager.c` | Suit slot and extrinsic properties | import armor and multi-property handling | Eight resistances plus reflection and antimagic when worn; scales AC9/cost1500, mail AC5/cost2400; removal must preserve other sources |
| Scale drops and revival exhaustion | `src/mon.c`, `zap.c`, `mkobj.c`, monster/object definitions | Native corpse traits and mrevived flag | extend imported dragon death/revival paths | Chromatic first death 1/6; first revival 1/20; later none. Preserve ordinary native dragon 1/3 then 1/20 behavior; retain count through corpse traits |
| Corpse rewards, eating and resistance conveyance | `src/monst.c`, `eat.c` | Native corpses and intrinsic selection | reuse native selection with exact imported conveyance mask | No donor-global intrinsic redesign |
| Armor conversion, cancellation, discovery, dragon polymorph, monster armor shedding | `include/obj.h`, `src/read.c`, `zap.c`, `polyself.c`, `mon.c`, `muse.c`, `objnam.c`, `worn.c` | Native scales↔mail and armor/body mapping | extend explicit new mappings | Preserve native contiguous dragon mappings; avoid accidental off-by-one mapping into unrelated monsters/objects |
| Chromatic wish restrictions | `src/objnam.c` | Existing item/monster wish validation | extend only imported chromatic cases | Honor donor restriction outside wizard mode; do not change vanilla lamp/wishing probability or global unidentified-scale rules |
| Random worms, tool/weapon/potion/scroll/gem/ring/amulet loot, boulder, gold, traps | `dat/dragons.des`, `src/monst.c`, `objects.c` | Existing native definitions | reuse | No commented-out hydra/deep dragon/Crassus imports; comments are not active content |
| Branch achievements | `src/do.c` | No donor achievement framework locally | exclude | Optional branch identity derives from current dungeon; no new global achievement system |
| Save/bones/recovery and IDs | Monster/object/dungeon/attack IDs, corpse traits | EDITLEVEL4 combined epoch | audit and validate after imports | Prefer existing persistent state; any required revival-count representation must be documented before changing fields; no migration or extra epoch increment |
| TTY descriptions, Windows tiles, DLB | Local symbols, tile tables and three resource manifests | Existing build pipelines | add names/resources and mapping checks | Document reused art; validate both actual Release packages and binaries |

## Persistent representation decision before production edits

During identifier implementation, the local native Caveman boss was confirmed
to already own `PM_CHROMATIC_DRAGON` and the name `Chromatic Dragon`.
Case-insensitive monster lookup would confuse it with the donor's non-unique
`chromatic dragon`. Use **chromatic cave dragon / PM_CAVE_CHROMATIC_DRAGON**
for the imported species and its map references. Preserve the native unique
boss and `dat/Cav-goal.lua` unchanged. Armor keeps the donor chromatic names.
This explicit naming adaptation avoids changing global monster-name parsing.

Use the existing saved monster `mspare1` as explicitly partitioned state: its
low five bits hold Sheol's 0..17 movement-freeze timer; bits5–6 hold a saturating
0/1/2 imported-dragon revival count. Freeze setters must preserve upper bits,
and revival setters must preserve the low bits. Preserve imported dragons'
corpse/statue monster traits through the existing `oextra.omonst` machinery.
Increment the counter from those saved traits when revived and clear only the
movement restraint. Ordinary native `mrevived` remains its unchanged boolean.
This avoids new fields, struct resizing and a donor-wide revival redesign.
Tests must cover freeze/thaw/teleport with revival bits present, repeated corpse
revival, and save/recovery of both kinds of state. Keep EDITLEVEL4.

Use the existing unused ninth ray/attack slot (`AD_SPC1`) for lava. This keeps
existing attack IDs unchanged; expand derived display mappings for the added
beam and verify all later tile indices. New armor/body conversion extends the
parallel ordered armor/body ranges and adds explicit new-species mappings in
`armor_to_dragon`, preserving every native mapping. The four new armor identities
are fixed and initially known; the donor's globally shuffled unidentified armor
framework is excluded.

## Implementation and current validation gate

The preceding audit and representation decisions were recorded before the
production changes. The four maps, three dragon definitions, four armor types,
lava ray, armor powers/light/conversions, generation restrictions and finite
revival/drop handling are implemented. The completed gate covers full
traversal/return at DoD109, all maps/content probabilities,
dragon/worm population checks, bog/tree checks, chromatic combat/scales and
armor effects, treasure/save/reload, generation restrictions, tiles, both
Release builds/packages and all prior gates. Actual results follow below.

Manual entry is the downward Dragon Caves branch stair on DoD109; follow
A → B → C → D and return upstairs. The interactive mechanics checklist,
changed-file inventory and final test evidence are recorded below.

### Findings fixed during executable validation

- Fresh startup hit `init_dungeon: too many special levels`. The cumulative
  prototype table had a limit of 50. Increase only that transient loader table
  to 128 entries; DoD length, `MAXDUNGEON`, `MAXLINFO` and saved structures remain
  unchanged. Move level/branch bounds checks before their array writes: the
  old checks ran after filling the tables. Release startup and traversal now
  pass on both architectures.
- Although the imported monster has a distinct local name, object-name parsing
  initially consumed the native quest boss prefix in `chromatic dragon scales`.
  Resolve the two exact chromatic armor names before monster-prefix parsing,
  then apply the usual non-wizard restrictions. The Caveman monster and quest
  map remain byte-identical to the baseline.
- The expanded topology exceeds one TTY page. Capture every page in the
  topology runner and retain the terminal model between pages because Windows
  omits unchanged characters. The traversal helper reads the parent entry
  directly instead of waiting for the final page. These are test-harness
  corrections, with unchanged topology assertions.
- The post-release `make_corpse` switch explicitly lists species and had no
  ordinary-corpse cases for the six Step8 monsters, the fifteen Step9A monsters
  and the baby glowing dragon. Add their cases to the existing default corpse
  path; the original `G_NOCORPSE` check still excludes the appropriate species.
  This restores required corpse behavior, including the evil eye's edible
  corpse interaction. No monster definition, corpse probability or reward
  balance is changed. Tests execute the actual complete dispatcher for these
  22 species and the actual cave-dragon drop path over 24,576 deaths, checking
  retained monster traits, first/once-revived rates and exhausted later drops.

### Results recorded so far

- `test_step9b_content.lua`: exact four donor geometries, populations, flags,
  stairs, loot dice and independent chances across 256 seeds per map; PASS.
- `test_step9b_source.py`: all three complete pinned donor monster definitions,
  explicit local naming/generation adaptations, native Caveman/quest boundary,
  preceding source guards, and packaged four-map byte equality; PASS x64.
- `run_step9b.py`: actual armor/body mapping, hero ten-property add/remove,
  monster eight-property add/remove with other sources preserved, lava obstacle
  handling, nine-type breath selection/native eight-type isolation, light,
  and scale chances; PASS x64 and Win32. Additional actual `montraits` tests
  pass x64 through four revivals, preserving identity/HP, clearing frozen feet
  and saturating the saved counter so later scale drops stop.
- `run_step9b_runtime.py`: both Release executables entered at DoD109,
  traversed A/B/C/D, restored entrance and terminal saves, traversed back and
  returned to DoD109; PASS.
- `run_step9b_topology.py`: four fresh games per architecture; exactly one
  four-level Caves at DoD109, Sheol at108, all four temporary depths reserved,
  prior Tomb/Moria varying and collision-free, Big Room counts and Castle200;
  PASS. Hidden Giant Court/Zoo/Dragon Lair marker counts and collisions are
  checked separately by the actual production scheduler fixture, since
  `#wizwhere` intentionally omits those room markers.
- Full x64 and Win32 Release solution builds and cumulative Step6/7/8/9A
  focused/source/depth/ledger/recovery runs; PASS, including the final refresh
  after the armor-name and corpse-dispatch corrections. Final packages match
  all source bytes and generated tile indices pass.

**Step9B gate passed.** Live chromatic/glowing armor powers, both conversions,
light, removal and saves pass. Actual chromatic melee, corpse save/reload and
two native wand revivals/deaths pass. Actual lava melting/obstruction and
silver-dragon reflection pass; the reflection fixture restores its prepared
arena to refresh visibility after level-design terrain edits. The six-level
Sheol round trip still returns to108, and the Win32 bog/tree regression passes.
Both architectures pass the complete corpse/drop and revival function tests.
Proceed to the pinned dNetHack audit; no Step9C production changes preceded
this gate.

Evidence is outside the repository under `../_qa/step9-audit/dragon-*`. Failed
attempts are retained beside their corrected reruns; no failure is counted as
a passing result. README is unchanged and there is no commit/tag/push.

## Manual wizard checklist

1. Start a new EDITLEVEL4 game using `binary/Release/x64/NetHack.exe` or
   `binary/Release/Win32/NetHack.exe` with `-D`; use the corresponding
   `NetHackW.exe` for tiled inspection. From the DoD, Ctrl-V to **109**.
   Ctrl-F (`#wizmap`) reveals the floor; the additional downward branch stair
   leads to **The Dragon Caves**, separate from the ordinary DoD downstairs.
   Confirm the destination in `#overview` or the attributes screen. If ordinary
   DoD110 was entered, return upstairs and use the other down stair.
2. Descend **drgnA → drgnB → drgnC → drgnD**, logical depths110–113 under this
   parent. The layouts may be flipped. B has the lit ordinary room/town flag;
   it intentionally has no merchant. D has no downstairs. Return through all
   upstairs and confirm arrival back at DoD109.
3. Inspect dragons, worms, living/dead trees, pools and muddy bog. Compare
   freeze/thaw, drying, boulder filling and bog movement with Moria. Flying
   dragons should not get stuck in bog.
4. Find the three **chromatic cave dragons** guarding D's hoard. They are
   non-unique, intrinsically reflective, non-genocidable and unavailable to
   ordinary polymorph. The separate Caveman **Chromatic Dragon** is unchanged.
   Inspect the gold piles, gems, tools, weapons, potions and scrolls.
5. Fight a chromatic cave dragon and inspect its corpse/rewards. Scales are a
   chance drop, not guaranteed: chromatic1/6 or glowing1/3 on first death,
   1/20 after one revival, none after two. Repeated revival retains that count.
6. Wizard-wish `chromatic dragon scales`; wear them and inspect all eight
   resistances plus reflection and antimagic. Enchant scales into mail;
   self-zap cancellation to revert mail to scales. Save/reload while worn,
   then remove and confirm the armor's powers disappear. Other independent
   resistance sources must remain. Ordinary chromatic wishes are restricted.
7. Repeat with `glowing dragon scales`: stone resistance and worn light are
   intentional donor properties. Light persists through conversion and saves,
   and stops on removal. Glowing dragon/baby bodies also emit light. Its lava
   breath melts diggable stone walls to lava and ice walls to ice, preserves
   nondiggable stone walls and stops when reflected.
8. Save/reload on A and D, before and after combat, with new armor and corpses.
   Continue traversal after each restore. Inspect TTY names and tile selection.

## Artwork and file inventory

The baby/adult glowing dragon reuse native baby/adult gold-dragon art; the
chromatic cave dragon reuses the Caveman boss art. Glowing armor reuses gold
armor art; chromatic armor reuses silver armor art. Four lava beam tiles reuse
the corresponding fire beam tiles. Male/female reused frames intentionally
match. Generated name/index checks and bitmap inspection show no shifted or
corrupt tiles; these visual substitutions still need the user's in-game review.

Step9B changes are attributable to:

- Resources: `dat/drgnA.lua`, `drgnB.lua`, `drgnC.lua`, `drgnD.lua`, and
  `dat/dungeon.lua`.
- Definitions: `include/{dgn_file,display,extern,monattk,mondata,monsters,obj,
  objects,youprop}.h`.
- Mechanics: `src/{artifact,do_wear,dungeon,light,makemon,mon,mondata,monmove,
  mthrowu,muse,objnam,polyself,teleport,worn,zap}.c`. The shared frozen-feet
  representation also adjusts `src/uhitm.c`.
- Packaging/artwork: the three existing build manifests, `win/share/{monsters,
  objects,other}.txt` and `win/share/tilemap.c`.
- Tests: `test/*step9b*`; the prior topology/traversal helpers, explicit donor
  boundary guards and the Step9A freeze-state test are extended without
  removing assertions.
- Documentation: this file, `step9.md`, and `step9-playtest.md`; the corpse
  finding is also cross-referenced from `step9a.md`.
