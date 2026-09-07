# Step 8A: Ruins of Moria - historical deterministic validation

> **Historical record.** Step 8A temporarily pinned the branch entrance to
> DoD107. The final Step 8B integration removed that fixture and uses a
> persistent random DL30–199 entrance. See [doc/step8b.md](step8b.md) for the
> authoritative final behavior and validation record.

This is the uncommitted deterministic manual-validation phase. The cumulative
README remains the summary of committed Step 7. No Step 8 commit, tag, or push
is authorized in this phase. Final randomized Moria placement belongs to Step 8B.

## Baseline and donor

- Local branch: `phase0/dod-length`.
- Starting HEAD: `4ab8e05f7a48833f303f11036d3697737b9946a0`.
- Starting index and working tree: clean.
- Donor: https://github.com/UnNetHack/UnNetHack, `master`.
- Pinned donor: `439b8d63d3d1ca78fb08588dd43f61874114b21a`.
- Donor checkout is outside this repository. All donor source inspection uses
  `git show`/`git grep` at that immutable revision.

## Dependency audit (before implementation)

Source paths in this table refer to the pinned donor. The following inventory
was established before implementation; validation is recorded separately below.

| Donor element | Donor source file(s) | Local equivalent? | Required action | Adaptation/deviation |
| --- | --- | --- | --- | --- |
| Six-level upward branch, entry -1, ten resources | `dat/dungeon.def`, `dat/moria.des` | Lua dungeon definitions, special-level variants | Append Moria; entry at branch level 6; reserve temporary DoD107 entrance | Legacy definitions become Lua; no legacy compiler |
| `moria1-1`, Endless Stair remnants | `dat/moria.des` | Maps, lava, bars, branch region, buried objects, engravings | Preserve geometry, gem/lava piles, buried mithril, upstairs and return stair | Legacy selection `&` means **union**, confirmed in `util/lev_comp.y` and `src/spo_lev.c`; use Lua union |
| `moria2-1`, broken bridge | `dat/moria.des` | Lava, holes, corpses, doors, item inventories | Preserve bridge holes, optional lava islands, wizard remains, Durin's Bane and equipment | Existing Lua APIs |
| `moria3-1`, nonpersistent level | `dat/moria.des`, `src/do.c`, `src/drawing.c`, `src/options.c`, `src/restore.c`, `include/obj.h` | Rooms/corridors, migration, level-file deletion, glyph mapping | Seven rooms; regenerate after leaving; migrate monsters; preserve when invocation objects remain; special `#` walls/corridors; centering message/sound | Derive identity from special-level name; avoid new save fields |
| `moria4-1` through `moria4-4`, orc halls | `dat/moria.des` | Maps, lighting, locked doors, class selection | Preserve four equally weighted geometries, independent stair-coordinate shuffles, hordes and loot | Existing Lua APIs |
| `moria5-1`, forest and barracks | `dat/moria.des` | Trees, golem, prisoners, traps, night test | Preserve forest probabilities, bog/ferns, woodman's equipment, bats, deep orcs and dwarves | Add missing terrain and fern mechanics |
| `moria6-1` and `moria6-2`, intact/ruined terminus | `dat/moria.des` | Graveyard, morgue, chests, altar, water, statues, inscriptions | Preserve equally weighted variants, reward classes/containers, night inscription, melon, Watcher and magic lamp | Existing vanilla magic-lamp behavior remains unchanged |
| Deep orc | `src/monst.c`, `src/mon.c` | Orc anatomy/gear; no deep orc | Import definition and reciprocal dwarf grudge | Preserve G_NOGEN, G_GENO, frequency 1; use local monster difficulty field |
| Durin's Bane | `src/monst.c`, `src/monmove.c`, `dat/moria.des` | Demon attacks, uniqueness, gear | Import actual unique definition, fear immunity, whip/shield/wand/potion inventory | Reuse demon art; no unrelated demon changes |
| Watcher in the Water | `src/monst.c`, `src/monmove.c`, `dat/moria.des` | Aquatic movement, wrapping, corpses, unique tracking | Import actual definition and fear immunity; preserve lamp reward | Reuse aquatic monster art |
| Swamp fern, sprout, spore | `src/monst.c`, `src/mondata.c`, `src/monmove.c`, `src/mhitu.c`, `src/mon.c`, `include/mondata.h` | Fungi, gas clouds, growth, explosion attacks | Add three definitions; stationary plant, spore release, poison cloud and bog-dependent regrowth, sprout maturation | Import no unrelated fern species; local gas-cloud API; donor has no Sheol equivalent locally |
| Iron safe | `src/objects.c`, `include/obj.h`, `src/mkobj.c`, `src/apply.c`, `src/lock.c`, `src/pickup.c`, `src/dokick.c`, `src/trap.c` | Containers and lock occupations | Real iron container; always initially locked; stethoscope cracking; cannot force/kick open; fire/water protection | Use existing object fields and container machinery; preserve ordinary boxes |
| Small piece of unrefined mithril | `src/objects.c` | Gem/rock class | Real zero-frequency valuable rock, weight 1, value 10000, damage 3/3, silver appearance | Donor material is MINERAL despite its name; no invented refining system |
| Balin's grave and treasure | `src/dig.c`, `dat/moria.des`, `include/artilist.h` | Grave digging, named corpse, battle axe, sapphire, artifacts | Named dwarf-lord corpse, cursed battle axe, Earthstone artifact | The axe is ordinary in donor despite the source comment "Durin's Axe" |
| Earthstone | `include/artilist.h`, `src/artifact.c`, `src/teleport.c`, `src/bones.c` | Artifact and magic-portal state | Import sapphire artifact and consumed-on-invocation portal behavior | Other portal stones are outside Moria; preserve donor unpaired-portal behavior (hero teleports locally; monsters do not migrate). Existing trap destination sentinel avoids new persistent fields |
| Rare Guest41 memorial | `src/mkroom.c`, `src/dig.c` | Grave/morgue generation | Preserve rare memorial and unused-grave message | Keep local scope to Moria morgue; no unrelated grave loot changes |
| Dead tree (`t`) | `include/rm.h`, `src/dig.c`, `src/dokick.c`, `src/zap.c`, `src/music.c`, `src/display.c` | Living trees, terrain flags | Real blocking/choppable dead tree; faster chopping; kick collapse/raven; no fruit/bees; fire/death interactions | Reuse tree artwork with distinct terrain description/color |
| Muddy swamp/bog (`M`) | `include/rm.h`, `src/hack.c`, `src/trap.c`, `src/mon.c`, `src/monmove.c`, `src/do.c`, `src/dig.c`, `src/zap.c`, `src/engrave.c`, `src/potion.c` | Water, water damage, existing trap state | Mud movement delay, water walking/mud boots/swimming exceptions, wet inventory, rust/gremlin effects, monster delay, boulder fill, freezing/melting/drying, engraving/dipping | Add terrain and trapped-state IDs; use existing fields; no drowning substitution |
| Outdoor sky | `include/rm.h`, `src/dungeon.c`, `src/engrave.c` | Ceiling/surface checks | No ceiling on forest and terminus; use ground/sky descriptions | Derive from Moria special-level identity instead of saving a new sky flag |
| Per-level monster-generation override | `dat/moria.des`, `src/makemon.c`, `src/spo_lev.c` | `rndmonst`, `mkclass`, genocide state | Override chances 50/60/70/90/10/40-or-60%; deep-orc/class-o weights 7:3 (terminus 8:2) | Derive from special-level identity/variant; no donor linked-list save extension |
| All remaining exact monsters and classes | `dat/moria.des` | Hill orc, orc-captain, trolls, dwarf, dwarf lord, human, elf, hobbit, wizard, iron golem, bats, raven | Use local definitions and ordinary class selection | Do not import unrelated class members from UnNetHack |
| All remaining loot and properties | `dat/moria.des` | Teleport scroll, mithril coat, lamp, whip, reflection shield, speed wand, paralysis potion, tools, food, gems, corpses, statues, altar, icebox/chests/boxes | Preserve BUC, enchantment dice, erosion/proofing, burning, burial, quantities and container contents | Named cursed scrolls remain "Word of Recall"; vanilla wishing untouched |
| Ambient sounds | `dat/moria.des`, donor sound machinery | Local ambient sound dispatch | Preserve restless-water and centering sounds, probability 1/200 | Branch-derived dispatch avoids save fields |
| IDs/save/bones/level/recovery | Monster/object/artifact tables, dungeon/terrain definitions | Generated IDs, existing version guard | Advance EDITLEVEL from 2 because stored IDs change; retain structural hardening | No backward-save migration; evaluate every proposed persistent field before adding |
| Display/build/package | Donor descriptions; local tile/data manifests | TTY, Windows tiles, Lua DLB | Suitable art reuse; all ten resources in every manifest and both final DLB files | No donor tileset/framework import |

## Branch ordering and placement contract

| Branch level | Resource | Probability | Display depth with DoD107 entrance |
| --- | --- | --- | --- |
| 6 (entry) | `moria1-1` | 100% | 106 |
| 5 | `moria2-1` | 100% | 105 |
| 4 | `moria3-1` | 100% | 104 |
| 3 | `moria4-1` to `moria4-4` | 25% each | 103 |
| 2 | `moria5-1` | 100% | 102 |
| 1 (terminus) | `moria6-1` or `moria6-2` | 50% each | 101 |

Enter by ascending the branch stair at DoD107, continue upstairs through six
levels, and descend back through the entry level to DoD107. These are ordinary
NetHack logical depths, not custom branch-local display numbers. DoD remains
200 levels and Castle remains DL200. The existing scheduler reserves the
fixed branch endpoint before choosing Step 6/7 features; Lost Tomb stays random
DL30-199. No old deterministic DL101-105 or Lost Tomb DL106 corridor is restored.

## Validation and implementation record

Implementation and automated validation are complete. Entries below describe
executed checks, not approval to commit or proceed to Step 8B.

### Imported definitions and compatibility decisions

- Six new monsters: deep orc, Durin's Bane, Watcher in the Water, swamp fern,
  swamp fern sprout, and swamp fern spore. Attacks, resistances, sizes, movement,
  alignment, corpse behavior and flags follow the pinned donor. Explicit local
  difficulty values are the donor `util/makedefs.c:mstrength` results:
  **9, 20, 28, 14, 8, 2**, respectively. The focused C test recomputes them.
  Sprouts precede adults in the local fungus class; branch-only/unique monsters
  retain `G_NOGEN`, while the adult retains the donor's ordinary-generation
  frequency 1 outside Hell. `G_NOSHEOL` has no local equivalent and is omitted.
- Iron safe is a genuine iron container: weight 900, price 50, probability 10,
  initially locked, with normal trap generation and up to ten contents.
  Stethoscope cracking uses the donor's 5% base chance, with the Rogue dexterity
  bonus. Keys, lock picks, forcing and kicking do not bypass its combination.
  Existing magic lock/open effects remain part of the ordinary box machinery.
  Fire does not destroy it; water does not reach its contents. The tool-class
  probability total must remain 1000: its ten probability points come from
  large boxes (40 to 30). No unrelated donor object probabilities were imported.
- Unrefined mithril is a real zero-frequency gem-class object, weight 1,
  value 10000, damage 3/3, and donor MINERAL material. Its silver appearance
  does not imply a silver weapon or a new refining/crafting system.
- Earthstone is the donor's neutral, nongenerated sapphire artifact (value
  7000), excavated at Balin's grave. Invocation consumes it and creates an
  adjacent magic portal. The Moonstone/Sunstone and their unrelated acquisition
  systems are outside this import. Therefore the Earthstone preserves the
  donor's **unpaired** portal case: local hero teleport; monsters shimmer without
  migrating. There is no functioning multi-stone travel network in this port.
  Destination `(-1,-1)` in the existing trap fields records this portal across
  saves, avoiding the donor's extra global portal-location fields.
- Balin's remains use local `dwarf leader`, the renamed vanilla dwarf-lord
  equivalent. The accompanying cursed battle axe is ordinary, as in the donor;
  the donor comment "Durin's Axe" does not define a separate artifact.
  NetHack 5's pit creation removes the headstone via `unearth_objs`; its
  Moria identity is captured before digging, in a local integer, so the grave
  reward survives that cleanup without adding a save field.
- Dead trees block movement/vision and can be chopped faster, kicked down,
  burned, or toppled by an earthquake. They do not yield fruit or bees; a kick
  can release a raven. Death rays convert living trees to dead trees. The new
  earthquake case is restricted to dead trees, preserving local living-tree
  behavior instead of importing an unrelated earthquake rebalance.
- Bogs are shallow muddy water, not drowning pools. They delay grounded heroes
  and monsters, wet equipment, split gremlins and rust iron golems; swimming,
  amphibious movement, flying, levitation, water walking and mud-boot appearance
  interact as documented by the donor. Boulders fill bogs, cold freezes them,
  melting restores bog terrain, and fire can dry them. Bog water can fill a dug
  pit; generated traps and engraving are excluded. Dipping remains available.
  No bog drawbridge substrate was added: Moria has no drawbridges requiring it.
- Fern release uses the donor's intended sprout/adult probabilities (1/4 and
  1/2), with explicit parentheses fixing its ambiguous ternary expression.
  Cancellation, range and available adjacent space still constrain release.
  Spore death uses the local gas-cloud/region machinery and existing saved
  region lifetime, with bog-dependent plant regrowth. The common death path
  runs after life-saving and detachment, preventing premature regrowth or a
  plant occupying a still-live spore's square.
  The dependency audit also covers `src/exper.c` (spores give zero experience),
  `src/explode.c` (fern vegetation survives explosions), and mutual spore
  aggression against creatures other than plants/spores in `src/mon.c`.
  The imported stationary plants do not issue misleading fleeing messages.
  `explmu` explicitly handles the spore's physical contact blast, including
  donor dexterity avoidance and half-physical damage. The existing elemental
  exploder cases are unchanged; spore death then creates its cloud/regrowth.
- The nonpersistent level migrates remaining monsters and deletes its old
  level file after normal save/free cleanup. Floor, buried and monster-carried
  invocation items protect it; recursive container inspection deliberately
  improves on the donor's shallow scan so an ascension item cannot be lost
  inside a box. Ordinary treasures do not prevent regeneration.
- The rare Guest41 memorial is confined to Moria's morgue. Existing engravings
  prevent duplicates on that level rather than adding a process-global flag.

### Lua conversion and persistent state

All ten resources preserve the pinned `.des` geometry, including ragged map
rows. Local Lua APIs supply room/map generation, doors, stairs, traps, objects,
inventories, burial, class generation, lighting, engravings, graves and altars.
Legacy selection `&` was a union; the Lua port uses selection union `|`.
Random region lighting uses table-form `lit=-1`, since selection-form
`des.region(selection, "random")` is unsupported by NetHack 5.

The four hall variants and two terminal variants are chosen with uniform
`math.random(1,4)` and `math.random(1,2)` at dungeon initialization. Their exact
resource names are saved in the existing special-level chain. This moves
variant selection earlier than the donor's generation-time selection but
preserves weights and lets runtime generation/sky rules identify the variant
without storing a new linked list or level flag. Monster override chances are
50/60/70/90/10/40-or-60%, with deep-orc/class-orc weights 7:3, or 8:2 at the
terminus. Ordinary local class membership supplies the remaining monsters.

Runtime sky, ambient sounds, generation overrides and nonpersistent behavior
derive from the saved resource identity. Normal dungeon depths and ledgers are
unchanged. `EDITLEVEL` advances **2 to 3** because stored monster, object,
artifact, dungeon and terrain IDs change. Existing terrain IDs 0-36 retain
their values; dead tree and bog append at 37 and 38. `TT_SWAMP` and `ICED_BOG`
fit existing fields. No new persistent fields, save migration, or recovery
format extensions were added. Old-epoch saves/bones/checkpoints are rejected by
the existing version guard; start a new Step 8A game.

### Display and builds

TTY has distinct dead-tree and muddy-swamp symbols/descriptions. Moria's barren
level uses `#` for walls and corridors, matching the donor's special display.
Windows art deliberately reuses existing tiles: gas spore for swamp spore,
orc-captain for deep orc, violet fungus for both fern stages, balrog for Durin's
Bane, kraken for Watcher, chest for safe, flint for mithril, tree for dead tree,
and pool for bog. Both male/female monster tiles are represented. Hand-authored
tile source ordinals were adjusted; generated IDs/bitmaps were not hand-edited.

The authoritative manual-build directories are `binary/Release/x64` and
`binary/Release/Win32`, containing `NetHack.exe`, `NetHackW.exe` and `nhdat500`.
Unix, nmake and Visual Studio manifests include all ten Lua resources. Tests
compare actual DLB bytes against the source files, not just directory listings.

### Executed validation

| Check | Result |
| --- | --- |
| Ten-map donor geometry/content contract | PASS: 128 runs per map, with night/day, forest/bog, shield, loot and quantity outcomes. |
| Actual packaged Lua loading | PASS x64: all ten resources through `#wizloaddes`, with monster generation enabled and no panic/fallback. |
| Focused production C functions/databases | PASS final x64/x86, including donor difficulty recomputation, zero spore XP and 10,100 death-helper calls. 2,000 scheduler samples and 700,000 generation samples; fern release, bog effects, invocation-item guard and retained Step 7 light/charge checks. |
| Fresh packaged topology | PASS: eight x64 and three Win32 games. One Moria at107, ordinary101-106 depths; Tomb/Temple remain randomized, Big Rooms3-5, Castle200. All four hall and both terminal variants selected in the x64 sample. |
| Full stair traversal | PASS x64 and Win32 with monster generation enabled: enter107, all six floors up and down, return107; barren floor changes on revisit. |
| Bridge save/reload | PASS x64 and Win32: exact map, hero position, mithril and Magic Candle IDs, then return through the branch to DoD107. |
| Unique boss rewards/save | PASS x64: Durin's Bane and Watcher each born once, killed once, remain extinct after save/reload; exact equipment and uncursed charged magic lamp. |
| Balin grave/save | PASS x64 on the actual terminal: named dwarf-leader corpse, cursed battle-axe, Earthstone; terminal save/reload retains the stone. |
| Live fern lifecycle | PASS x64: actual release, physical contact blast, spore deaths/clouds, surviving plants, no panic. |
| Live terrain interactions | PASS x64: bog freezing/thawing/evaporation, death-ray tree conversion, dead-tree fire and boulder filling. |
| Earthstone invocation/save | PASS x64: consumed stone, persistent unpaired portal, local hero teleport without branch migration. |
| Iron safe occupation/save | PASS x64: actual stethoscope unlocking, mithril contents and unlocked state survive save/reload. |
| Lua allocator regression | PASS x64/x86 with bundled Lua: 1,000 allocating finalizers, strict cap rejection, resize/free accounting, zero warnings. |
| Existing Step 6B/7 live runtime | PASS final x64/Win32: unchanged Lua tests in real randomized Tomb and Temple branches, before/after save, return stairs, empty panic logs. |
| Step 7 donor-content contract | PASS: 64 donor/local runs, four paths, rewards, Shadows/populations; Moloch56x9. |
| Step 7/6 protected source and DLB checks | PASS both packages: shop logic, wishing/magic lamp, Tomb and Big Room provenance resources, structural save/recovery sources. |
| Depth and ledger/recovery | PASS x64/x86 at incarnation `0x05000003`: 225 packed pairs, endpoints, round trips through ledger3199, internal and standalone recovery, bad bounds and old-epoch rejection. |
| Release builds | PASS final x64/Win32, including TTY and Windows tiled executables. Existing compiler warnings remain. |
| Diff and inventory review | PASS: `git diff --check`, new-file whitespace/Python syntax, 84-file inventory, no staged or generated artifacts. |
| DLB byte verification | PASS both packages: all ten Moria resources, current dungeon.lua, tomb-1 present, tomb-2 absent. |

The source guards retain whole-file protection for earlier milestones except
two precisely checked additions: Moria's memorial block in `mkroom.c` and bog
trap exclusion in `mklev.c`. The existing scheduler fixture additionally reserves
Moria107; none of its earlier count/range assertions was removed. Ledger tests
now expect epoch3 rather than2. Existing Step 6B/7 Lua assertions and files
are unchanged. Their object finalizers exposed a pre-existing allocator bug:
Lua returns -1 from its GC-count API inside a finalizer; unsigned conversion
made that look like enormous memory use. `nhl_alloc` now tracks allocated bytes
directly, checks growth against the same memory cap, and permits shrinking and
freeing. A nonpersistent Lua-state counter is initialized before state creation;
no save field or gameplay rule changes. An independent test compiles the exact
allocator with bundled Lua and verifies 1,000 allocating finalizers, deliberate
memory exhaustion, resize/free accounting, and zero warnings. Builds retain pre-existing compiler warnings;
new compilation errors and warnings encountered during porting were corrected.

Runtime fixtures and build logs live outside the repository under
`H:/app/mynethack/step8a-audit`. Wizard traversal clears any monster blocking a
destination stair before teleporting there; it validates topology and runtime
generation, not ordinary combat difficulty. User combat and tile review remain
the purpose of this deterministic phase.

### Manual checklist

1. Start a **new** wizard game with the current x64 or Win32 executable (`-D -u wizard`).
   Use Ctrl-V, enter `107`, and find the extra **upstairs**. `#wizwhere` identifies
   the branch; Ctrl-F reveals the map. Ascending enters Moria at ordinaryDL106.
2. Ascend through Endless Stair remnants (106), Durin's broken bridge (105),
   the regenerating seven-room floor (104), one of four orc halls (103), forest
   and barracks (102), and one of two Doors of Durin endings (101).
3. Inspect lava/gem deposits, buried mithril and coats, bridge holes, named
   cursed Word of Recall scrolls, orc troops/captains, dwarf prisoners, and the
   woodman's equipment. Fight **Durin's Bane** and **Watcher in the Water**;
   inspect Durin's whip/gear and Watcher's uncursed **ordinary magic lamp**.
4. Inspect living/dead forest, possible bogs and fern population. Check chopping,
   kicking and burning dead wood; mud movement/wetting, water-walking or mud
   boots, freezing/thawing and boulder filling. Observe fern spore release,
   explosions/clouds and possible plant regrowth in bogs.
5. Crack an iron safe with a stethoscope; try ordinary looting, carrying,
   dropping and storing contents. Inspect the mithril's name/value/appearance.
   Dig Balin's grave for the corpse, cursed battle axe and Earthstone. Invoke
   the stone and inspect the consumed stone's unpaired local-teleport portal.
6. Leave and revisit the barren floor: its layout and ordinary dropped loot do
   not persist. Required invocation objects, including contained ones, protect
   the floor. Check its `#` display and centering message.
7. Save/reload in the bridge and terminal level, after killing a boss, with new
   items, and after opening a safe or invoking the Earthstone. Continue normally;
   bosses should not reappear as duplicates.
8. In tiles, visually inspect **all six creatures**, **safe and mithril**, and
   **dead tree and bog**. Earthstone uses the existing sapphire tile. Confirm
   reused artwork maps to the correct names instead of shifting later tiles.
9. Descend all six levels; the final downstairs returns to **DoD107**. Supply
   manual validation before Step8B randomization or publication.

The README is intentionally unchanged. This phase has no commit, tag or push.

### Reproducing the automated checks

Run `python test/run_step8a.py OUTPUT_DIRECTORY` from each Visual Studio developer
shell (`x64`, `x86`). It includes the existing Step 7/6 scheduler/light checks,
Moria production-function/database tests and the actual bundled-Lua allocator
regression. Run `test/test_step8a_content.lua PINNED_MORIA_DES dat` with Lua,
and `python test/test_step8a_source.py binary/Release/x64/nhdat500
binary/Release/Win32/nhdat500` for source and package contracts. The existing
`test/test_step7_source.py` accepts the same package arguments.

The packaged Python runners named `run_step8a_runtime`, `variants`, `save`,
`safe`, `earthstone`, `grave`, `boss`, `watcher`, `fern`, `terrain`, and
`regression` take `RELEASE_DIRECTORY NEW_OUTPUT_DIRECTORY`; `topology` also takes
a sample count. They require pywinpty/pyte. Each run copies the release into a
fresh directory and refuses to overwrite a prior fixture. `watcher` uses the
actual terminal resource with flips disabled in that fixture to locate the
boss; terrain/fern tests use isolated arenas. Production Lua has no test hooks.

### Exact changed-file inventory

84 files: 55 modified tracked files and 29 new files. No generated build,
package, save, donor checkout or fixture artifact is included.

```text
dat/dungeon.lua
dat/moria1-1.lua
dat/moria2-1.lua
dat/moria3-1.lua
dat/moria4-1.lua
dat/moria4-2.lua
dat/moria4-3.lua
dat/moria4-4.lua
dat/moria5-1.lua
dat/moria6-1.lua
dat/moria6-2.lua
doc/step8a.md
include/artifact.h
include/artilist.h
include/defsym.h
include/extern.h
include/monattk.h
include/mondata.h
include/monflag.h
include/monsters.h
include/obj.h
include/objects.h
include/patchlevel.h
include/rm.h
include/you.h
src/apply.c
src/artifact.c
src/botl.c
src/cmd.c
src/dig.c
src/display.c
src/do.c
src/dokick.c
src/dungeon.c
src/engrave.c
src/exper.c
src/explode.c
src/hack.c
src/lock.c
src/makemon.c
src/mhitu.c
src/mklev.c
src/mkobj.c
src/mkroom.c
src/mon.c
src/mondata.c
src/monmove.c
src/music.c
src/nhlua.c
src/objnam.c
src/pager.c
src/pickup.c
src/potion.c
src/sounds.c
src/teleport.c
src/trap.c
src/zap.c
sys/unix/Makefile.top
sys/windows/Makefile.nmake
sys/windows/vs/files.props
test/run_step8a.py
test/run_step8a_boss.py
test/run_step8a_earthstone.py
test/run_step8a_fern.py
test/run_step8a_grave.py
test/run_step8a_regression.py
test/run_step8a_runtime.py
test/run_step8a_safe.py
test/run_step8a_save.py
test/run_step8a_terrain.py
test/run_step8a_topology.py
test/run_step8a_variants.py
test/run_step8a_watcher.py
test/test_ledger_runtime.c
test/test_step7_runtime.c
test/test_step7_source.py
test/test_step8a.lua
test/test_step8a_content.lua
test/test_step8a_lua.c
test/test_step8a_runtime.c
test/test_step8a_source.py
win/share/monsters.txt
win/share/objects.txt
win/share/other.txt
```


Final branch remains `phase0/dod-length`; HEAD remains
`4ab8e05f7a48833f303f11036d3697737b9946a0`. The working tree intentionally has
55 modified tracked files and 29 new files, all unstaged. README is unchanged.
No commit, tag or push was performed. Moria remains fixed at DoD107 for user
validation; Step 8B random placement is not implemented.

Final build logs are `build-x64-allocator-final.log` and
`build-Win32-allocator-final.log` in the external audit directory. Final Lua
and native checks are in `focused-x64-allocator.log` and
`focused-x86-allocator.log`; depth/recovery logs are `regression-x64-final.log`
and `regression-x86-final.log`. Passing final packaged fixtures include
`variants-x64-final`, `regression-game-x64-final` and
`regression-game-Win32-final`. The earlier passing mechanics fixtures include
`grave-x64-3`, `fern-x64-4`, `terrain-x64-1`, `watcher-x64-2`, `boss-x64-4`,
`earthstone-x64-2`, `safe-x64-1`, `save-x64-1`, `save-Win32-1`,
`traversal-x64-verified` and `traversal-Win32-verified`.
