# Step 6A/6B: NerfHack content integration and randomized placement

Modified 2026-09-06. NetHack General Public License; see `dat/license`.
The first sections below preserve the Step 6A donor audit and deterministic
playtest record. Step 6B removes that test corridor and applies the final
randomized placement described at the end of this document. Start a **new
game** for the new dungeon topology; existing saves retain their stored
dungeon definitions. Changes remain uncommitted for manual review.

## Baseline

- Repository: `H:\app\mynethack\YarivNetHack`.
- Branch: `phase0/dod-length`.
- Starting HEAD: `4ce6bff74bffe927b8f951fb36986f20683ea5a7`
  (`shops--optimization-step5-complete`). Working tree and index were clean.
- DoD has `base = 200`; Castle retains `base = -1`, hence DoD 200.
- Step 5's deep shop floor, shop/stock weights, and mimic cap are unchanged.
- Complete pre-change working/index patches, status, HEAD, branch and index
  manifest are in sibling directory `../step6a-audit/`. Both patches are empty;
  there were no pre-existing uncommitted Step 5 changes to separate.

## Donor and provenance

Sole donor: https://github.com/elunna/NerfHack, branch `dev`, exact commit
`0cb8781b0929b4617590a3b9fe78f972ef42c25f`, cloned and read before copying.
The clone is in `../step6a-donor-NerfHack`. Original map copyright and historical
attribution comments are retained. Historical variant credits in those files
are donor provenance, not additional donor sources used for this work.

| Feature | Donor files/definitions | Local mapping and adaptation |
| --- | --- | --- |
| Big Room, DL101 | NerfHack `dat/bigrm-18.lua`, complete map and content loops | Historical Step 6A staging name: `dat/bigrm18.lua`. The final local name is `dat/bigrm-14.lua`: copied map, flags, lighting, 15 objects, 6 traps, 28 random monsters. Stairs sample distinct points in the connected arena via `selection.floodfill`, avoiding sealed pillars. No catalogue substitutions. |
| Giant Court, DL102 | `src/mkroom.c`: `mk_zoo_thronemon`, `fill_zoo` GIANTCOURT cases; `src/hack.c` entry message | `dat/giantct.lua` defines one plain court; `src/mkroom.c` adapts the titan ruler, `mkclass(S_GIANT,0)` population, sleeping/hostility, mace, throne, coffer and tinning kits. The existing titan needs no substitution. Greeting adapted in `src/hack.c`. |
| Real Zoo, DL103 | `src/mkroom.c`: `realzoomon`, `fill_zoo` REALZOO cases; `src/hack.c` greeting | `dat/realzoo.lua`, local `realzoomon` and `fill_zoo` in `src/mkroom.c`: same active depth formula and animal choices; sleeping hostile occupants. Keeps local zoo gold as requested. Greeting adapted in `src/hack.c`. |
| Dragon Lair, DL104 | `src/mkroom.c`: `fill_zoo` DRAGONLAIR cases; `src/hack.c` greeting | `dat/drgnlair.lua`, `src/mkroom.c`: local `mkclass(S_DRAGON,0)`, donor scale chance and gold formula without rebalancing. Greeting adapted in `src/hack.c`. |
| Temple entrance, DL105 | `dat/dungeon.lua`: Temple branch/dungeon; `dat/temple-1.lua`: map, `chest_fill`, shrine, monsters, branch region | `dat/dungeon.lua` adds fixed DL105 stair branch and an appended one-floor dungeon. `dat/moloch.lua` copies first map, replacing two ghoul mages with liches and removing the duplicate explicit up stair. The branch region creates the sole return stair. |

### Dependency audit

- **Room representation:** NerfHack has separate REALZOO/GIANTCOURT/DRAGONLAIR
  IDs 17/18/20 among other added types; local IDs at those positions are shops.
  Copying those numbers or shifting SHOPBASE would disturb saved rooms/shops.
  Instead, these are specialized fills of vanilla COURT/ZOO selected by the
  current special-level prototype. No new enum values, fields or registries.
  Local `fill_special_room`, room persistence, first-entry clearing, sounds,
  map overview and bones rules continue to work with those vanilla types.
  Dragon Lair reuses zoo ambience rather than importing donor `has_lair`.
- **Selection guarantee:** fixed special-level definitions bypass probabilistic
  `pick_room`/`mkzoo` selection and random shops on DL101-104. Each DL102-104
  map has exactly one stocked special room, two connected ordinary rooms and
  separate up/down stairs. These are plain rectangular rooms, with no imported
  themed geometry. `src/mklev.c` is untouched.
- **Giant catalogue:** local class includes giant, stone/hill/fire/frost/storm
  giants, ettin, titan and minotaur, subject to local generation eligibility.
  Donor-only hill giant shaman and elder minotaur are omitted by using the
  local class; no replacement catalogue entries. The ruler is a vanilla titan.
- **Zoo catalogue:** mastodon, python, mumak, tiger, panther, jaguar, ape,
  monkey all exist locally. Formula is `rn2(60) + rn2(3 * level_difficulty())`;
  thresholds are >115, >85, >70, >55, >45, >25, >15, otherwise monkey.
  The donor's disabled `#if 0` Jumbo the Elephant code is omitted, not activated
  for the deeper dungeon. Exhausted/genocided animal selections are skipped.
- **Dragon catalogue:** local adult gray, gold, silver, red, white, orange,
  black, blue, green and yellow dragons are available through local `mkclass`.
  Local baby dragons have their usual non-random-generation restrictions;
  disabled shimmering-dragon entries remain disabled. Donor-only shadow/baby
  shadow dragons, fell beast and Wintercloak are not imported (some are
  already ineligible for donor random class generation).
- **Exhaustion:** an exhausted imported class never falls through to
  `makemon(NULL)` and an unrelated random monster. All normal vanilla fills
  keep their existing behavior. Genocide/extinction remain authoritative.
- **Temple named monsters:** aligned cleric (already the local gender-neutral
  name), gargoyle, winged gargoyle, bone devil, ice devil, barbed devil, vrock,
  horned devil and hezrou all exist locally. Two ghoul mages become liches,
  retaining the undead spellcaster role. The 21 class-Z placements use the
  local zombie/ghoul catalogue. The shrine uses the existing priest mechanics.
- **Temple items/features:** eight stocked chests with wax candles, two gold
  calls and two random objects each, plus independent 50% and 25% random-scroll
  calls; ninth locked chest with 50/50 bag of holding or wand of polymorph.
  These objects, unaligned shrine, locked/secret doors, corridor, two spiked
  pits, engraving, non-diggable region and `solidify`/`noflip` flags are local.
  No donor-specific object or trap was found.

### Rewards retained

- Giant Court: ruler receives a mace; royal chest has
  `rn1(50 * level_difficulty(), 10)` gold (10 through 50*difficulty+9), normal
  chest contents, and `spe = 2`; each eligible filler square has a 1/111
  tinning-kit chance. Local and donor normal court reward behavior agree.
- Real Zoo: the donor sets `has_zoo` but does not include REALZOO in its gold
  switch. This adaptation deliberately retains **local zoo floor gold** to
  meet the requested normal zoo reward behavior; this is an explicit departure
  from that donor omission, not an accidental claim of exact copying.
- Dragon Lair: initial gold budget is `1500 * level_difficulty()` (normal
  zoo: 500*difficulty). For each eligible square, `i` is squared `dist2` from
  the first door, or the remaining budget when doorless. If `i >= goldlim`,
  use `5 * level_difficulty()`; subtract `i`, then place `rn1(i,10)` gold.
  This is the actual donor algorithm, **not a promise of three times total
  gold** or a hard cap. Each eligible square independently has 1/20 chance
  of `rnd_class(GRAY_DRAGON_SCALES,YELLOW_DRAGON_SCALES)`, created with
  initialization/artifact flags false. Local scale types/probabilities apply.
  No reward rebalance is included.

## Topology and preservation

The original dungeon entries, special levels and branch definitions retain
their values and order. New special definitions and parent branch are appended
to their lists; the Temple dungeon is appended after the existing dungeons to
preserve their numbers and ledger ranges. This required optional branch is the
only topology addition; DoD length, Castle, Gehennom and endgame are unchanged.
DL105 still uses ordinary generation and gets its third stair through vanilla
`place_branch`. Returning from the Temple targets DoD 105. The new maps have no
bones tags; branch-level bones suppression already protects DL105. No bones
implementation or save format change is necessary.

Shop probabilities/inventory/mimics, XP/XL, monster scaling, catalogues,
artifacts/wishes, global object/trap/terrain rules, vanilla special-room rolls,
late-game content and other ordinary DoD levels are unchanged. No additional
NerfHack systems or production debug/test subsystem are added. Step 6B's random
occurrence rules and caps are intentionally absent.

## Files changed

| File | Reason/symbols |
| --- | --- |
| `dat/dungeon.lua` | Fixed prototypes at 101-104; Temple branch at 105; appended dungeon. |
| `dat/bigrm-14.lua` (`dat/bigrm18.lua` was the historical Step 6A staging name) | Imported NerfHack Big Room 18 in local slot 14; connected stair selection. |
| `dat/giantct.lua` | One COURT room and two connected stair rooms. |
| `dat/realzoo.lua` | One ZOO room and two connected stair rooms. |
| `dat/drgnlair.lua` | One ZOO room and two connected stair rooms. |
| `dat/moloch.lua` | First donor Temple map and minimal entity/stair adaptation. |
| `src/mkroom.c` | `mk_zoo_thronemon`, new local `realzoomon`, named-prototype cases in `fill_zoo`. |
| `src/hack.c` | `check_special_room`: donor-style greetings for the three prototypes. |
| `sys/windows/vs/files.props` | Register five new Lua assets in VS source list. DLB list already includes `*.lua`. |
| `sys/windows/Makefile.nmake` | Lua prerequisites and packaged file list. |
| `sys/unix/Makefile.top` | Lua packaged file list for the Unix build path. |
| `test/test_step6a.lua` | Existing `#wizloadlua` API checks for stairs/connectivity, rooms and rewards. |
| `doc/step6a.md` | This baseline, donor audit, provenance and validation report. |

The Big Room mapping is deliberate. At donor commit
`0cb8781b0929b4617590a3b9fe78f972ef42c25f`, the imported map is NerfHack's
`dat/bigrm-18.lua`; the donor's separate `dat/bigrm-14.lua` is a different map
and was not copied. This repository already had 13 local Big Room slots, so
the donor Big Room 18 is renumbered to the next available local slot,
`dat/bigrm-14.lua`. The `bigrm` entry in `dat/dungeon.lua` therefore uses
`nlevels = 14`, and the normal loader selects the local `bigrm-14.lua` variant.
The renumbering changes only the local variant filename/slot; the imported map
and donor provenance remain Big Room 18.

## Manual playtesting

Use a new wizard game and Ctrl-V to reach 101, 102, 103, 104 and 105.
`#wizwhere` displays the prototypes and fixed branch. Use `#wizmakemap` for
fresh generation of the current test floor. Use the ordinary stairs to descend
or return. At DL105 distinguish the Temple branch stair from the normal stair
to DL106. The Temple has one return stair.

Copy `test/test_step6a.lua` into an isolated playground as `check.lua`, then
`#wizloadlua` -> `check.lua` on each target floor. Run before looting/moving
occupants. For population checks, use a blessed potion of monster
detection and the existing `#wizborn` statistics; Lua map glyphs describe
remembered terrain and cannot reliably enumerate monsters. A missing optional
scale drop in a single generation is valid; scale probability is not forced.

## Validation

Validation commands, complete build logs, source audit and terminal transcripts
are in `../step6a-audit/`. This is an external evidence directory, not a new
production testing framework. Final runtime outcomes are recorded below.

| Check | Result and scope |
| --- | --- |
| Source | PASS. Protected file comparison against starting HEAD; exact fixed entries and room counts; complete named Temple monster audit and donor class/animal comparison. All five assets also listed from both built DLB archives. |
| Release x64 | PASS, zero errors: `MSBuild sys/windows/vs/NetHack.sln /t:Build /p:Configuration=Release /p:Platform=x64 /m`. NetHack, NetHackW, recover and Windows packages built. Rebuilt successfully after final stair/topology adjustment. |
| Release Win32 | PASS, zero errors, same solution and Release configuration with `/p:Platform=Win32`; all corresponding targets/packages built, including final asset adjustment. Existing unrelated compiler warnings remain. |
| Existing regressions | PASS: `test/run_ledger_runtime.ps1` freshly compiled and run for x64 and x86 with each newly built recover.exe. Covers 225 packed pairs, named level decoder, ledger/depth round trips through 3199, internal/standalone recovery and invalid fixtures. Existing baseline `test_depth_range_x64.exe` and `_x86.exe` also passed; those two binaries were not rebuilt because their production source dependencies did not change. |
| Big Room | Final packaged map generated at 101; two connected stairs to 100/102, six traps and donor map/content loops validated. Recreated in wizard mode. Saved and restarted at 101; restored map/link checks passed. Full combat and walking both stair approaches were not tested. |
| Giant Court | Repeated generation, one throne, one royal coffer and correct stairs passed. Example coffer gold: 1108 and 620. Monster detection plus `#wizborn` recorded 60 giant-class occupants including titans; fixed titan-on-throne placement is also source verified. Actual normal stair descent from 102 reached 103. |
| Real Zoo | Repeated generation and one designated room passed. Runtime census: 6 jaguars, 2 panthers, 6 tigers, 2 mumaks, 36 mastodons, 8 pythons (60 total); active donor list source audit includes the less likely apes/monkeys. Example floor gold 26675 and 29050. |
| Dragon Lair | Repeated generation, one designated room and dragon population visible with detection; exact local species selection source verified. Hoard checks passed with 108028 gold/1 scale drop and 89477 gold/3 scale drops. No scale guarantee or rebalance was added. |
| Temple entrance/return | PASS in-game: DL105 had exactly three stairs. Entered the branch at 105 via its actual stair; generated dungeon 9, level 1. Its one up stair returned to DoD105. The separate DoD down stair then reached ordinary DoD106, whose stairs target 105/107. `#wizwhere` displayed the fixed branch and Castle200. |
| Temple content/save | PASS: nine treasure chests, at least eight wax candles, two pits and return link checked. Visible gargoyle/devil/zombie/cleric/lich populations generated. Saved inside Temple, restarted executable, loaded save, rechecked contents/stair, then returned through the up stair. |
| Test-script corrections | Early iterations of the external/runtime checker hit coordinate errors after restore and Lua `__gc` memory warnings after bulk object inspection. The final test subtracts `nh.abscoord(0,0)`, caches map queries and explicitly collects temporary object handles. Subsequent Temple and DL105 checks, reload and traversal did not add errors/warnings. These were checker fixes; no production Lua/save changes. Earlier diagnostics remain in the evidence logs. |
| Whitespace/diff | `git diff --check` PASS. Six tracked files modified, seven files added; no pre-existing modified files, no staged changes and no commit created. Complete Step 6A patch/status are in `../step6a-audit/`. |

The focused runtime work used Release x64. Win32 was build/regression tested,
not separately played. Endgame traversal and combat/balance evaluation were
not part of the runtime pass. Step 6B remains gated on manual playtesting.

Step 6A implementation status: **complete**, left uncommitted for review.

## Step 6B final randomization

Step 6B removes the fixed DL101–105 test corridor and the dedicated
`giantct.lua`, `realzoo.lua`, and `drgnlair.lua` wrappers. Those depths now use
ordinary DoD generation unless selected by the final scheduler. The approved
NerfHack Big Room 18 map (`dat/bigrm-18.lua` in the donor) remains as local
slot 14, `dat/bigrm-14.lua`, and is part of the normal 14-variant Big Room
pool. The donor's own Big Room 14 map is not imported.

During `init_dungeons()` for a new game, `step6b_schedule()` uses the normal
NetHack RNG to choose distinct DoD locations. It creates exactly 3, 4, or 5
total Big Room entries (including every vanilla variant and Big Room 18), one
Giant Court, two or three Real Zoos, one Dragon Lair, and one Temple branch
entrance. General locations are selected from DL30 through DL199, with
room-based features restricted to the ordinary pre-Medusa portion so vanilla
post-Medusa maze generation cannot remove their rooms. Existing special levels,
branch endpoints, Castle DL200, and the normal stair at each selected level are
reserved before placement.

Giant Court, Real Zoo, and Dragon Lair are represented by private persisted
markers in the existing special-level chain. `Is_special()` filters those
markers before map generation, while `makelevel()` requests a vanilla COURT
or ZOO room and `fill_zoo()` applies the approved donor-derived population and
reward behavior. A deterministic room fallback guarantees a selected ordinary
floor receives its designated room. The markers and Temple branch use the
existing save/restore serialization, so locations are chosen once and survive
revisits and save/reload without adding save-format fields or a new count
registry.

The Temple uses the retained adapted `dat/moloch.lua` map, lich substitutions,
shrine, treasure, and return stair. Its branch entrance is rerolled from the
same reserved eligible pool; normal DoD descent remains independent. The
replacement `test/test_step6b.lua` checks generated stair connectivity,
ordinary DoD stair invariants, Temple contents, and retained reward signals
through existing Lua APIs. Donor provenance and the Step 6A audit remain
above; no additional NerfHack content is imported.
