# Step 12: recurring Library

## Architecture and compatibility

Library is stable `CUSTOM_LIBRARY = 8`, appended after Dragon Hall (7).
It uses the Step 11 categorical registry with explicit weight **300/10,000**
on ordinary eligible Dungeons of Doom **DL5–199**. All existing descriptor
weights and bounds, selector partition logic, protected-map/branch exclusions,
and the one-custom-room-per-level invariant remain unchanged. There is no quota,
cooldown or consumed bit. A successful selection still requires a valid host;
selection probability is not a guarantee that every level has such a host.

A narrow classic adapter selects a joined, stair-free ordinary room without
subrooms. It requires at least **12 eligible floor squares**. Eligibility means
in-bounds `ROOM` terrain belonging to the room, excluding irregular edges,
`occupied()` features/traps/invocation positions and squares within one step of
any room door. The same predicate is used by every Library placement stage.
Prepopulated hosts must have no existing chest and more monster-unoccupied
eligible squares than the depth-band chest cap, so neither native chest
contents nor lack of a guaranteed-monster square can leak into generation.
Chests must have distinct squares; Library monsters additionally require no
chest and no existing monster. Loose loot excludes chests but permits monsters.
Normal floor terrain is retained. Native room lighting lights the room and its
bordering walls/doors.

The existing `THEMEROOM` classification carries the room until first entry.
Ordinary filling excludes Library; `fill_special_room()` invokes its native
payload once and clears `needfill`. The existing entry/discovery path says
**`You enter a library!`**, discovers the room, and changes `rtype` to `OROOM`.
Saved `custom_id` and `orig_rtype` remain intact. Shops and vanilla special rooms
can coexist in separate rooms; their generation and billing code is unchanged.

There is **no core LIBRARY rtype, Lua map, donor dependency, new level flag,
terrain, object or monster definition**. Theme is a local variable in
`fill_library()`, selected once and passed to monster creation. Only the
resulting normal objects/monsters and existing room metadata survive generation.
No theme field or theme serialization exists.

`save_room()`/`rest_room()` already call `Sfo_mkroom()`/`Sfi_mkroom()` for the
existing structure, which already includes the unsigned-byte `custom_id`.
Neither that structure nor its codec changes. ID 8 fits the existing field;
**no EDITLEVEL or save-epoch bump is needed or made**. Existing Step 11 saves
retain their layout. No backwards gameplay support for loading new Library
levels in an older executable is promised.

## Rules

Every band uses raw DoD `u.uz.dlevel`, never `level_difficulty()`.

| Depth | Shaman | Gnomish | Lich | Forbidden |
|---|---:|---:|---:|---:|
| 5–29 | 50% | 50% | 0% | 0% |
| 30–59 | 30% | 45% | 25% | 0% |
| 60–99 | 10% | 30% | 45% | 15% |
| 100–149 | 0% | 15% | 55% | 30% |
| 150–199 | 0% | 5% | 50% | 45% |

Shaman uses Kobold Shaman/Orc Shaman, Lich uses Lich/Demilich, and Forbidden
uses Mind Flayer/Master Mind Flayer, each 50/50. Gnomish uses only Gnomish Wizard.
A selected `G_GONE` species falls back only to its same-theme partner; an
exhausted theme skips the attempt without rerolling. Monsters use native
inventory and hostility/alignment initialization, `MM_ASLEEP | MM_NOGRP`, and
start asleep. Target occupancy is checked before `makemon()`; there is no
`MM_ADJACENTOK` or location fallback. One themed monster is attempted first,
then remaining eligible unoccupied non-chest squares receive the extra rolls.

| Depth | Guaranteed chests | Extra chest / monster chance | Chest cap | Items/chest | Locked | Trapped |
|---|---:|---:|---:|---:|---:|---:|
| 5–59 | 1 | 1/4 | 4 | 1–3 | 20% | 5% |
| 60–99 | 2 | exactly 1/3 | 4 | 2–4 | 35% | 10% |
| 100–199 | 2 | 2/5 | 5 | 2–5 | 50% | 15% |

Guaranteed chests use uniform rank selection among remaining eligible squares,
with no bounded retries. Extras use a fixed coordinate scan until the cap.
All chests are created with `mksobj_at(CHEST, x, y, FALSE, FALSE)`, bypassing
native contents and native lock/trap initialization. All chests are placed
before any is populated. Each independently generated chest item is:

- 25% native random scroll;
- 40% native random spellbook;
- 20% miscellaneous magic: exactly 33% native random wand, 33% ring, 34% amulet;
- 10% utility: 50% magic marker / 50% magic whistle;
- 5% premium utility: 50% magic lamp / 50% bag of holding.

These are fixed integer thresholds; the 33/33/34 subtable uses a 100-way roll.
There is no guaranteed item type or Library-specific rarity/level weighting.
Items use normal native initialization. `add_to_container()` attaches them;
`weight()` recomputes each completed chest. Independent 100-way rolls set
`olocked` and `otrapped` after stocking. There are no chest-related map traps.

After monsters, each eligible non-chest square has exactly **1/20** chance of
one loose native random scroll/spellbook, chosen **50/50**. Monster occupancy
does not suppress this roll. All normal custom-room completion bookkeeping
then runs unchanged.

## Validation

The existing Step 11 native harness is extended, not replaced. The
`test_step12_library.h` diagnostic-only include tests private Library helpers
and supplies scripted Library RNG decisions; native object/monster generation
retains its real RNG. Production expands `LIBRARY_RN2` directly to `rn2` and
contains neither the script state nor these tests.

- Exhaustive production selector enumeration checks all old eligibility rules,
  all weights, Library boundaries, exclusions, permutations and one selection.
- Complete theme and loot decision tables are enumerated; density uses the
  precise denominators 4/3/5. Same-theme selection, genocide/extinction fallback
  and exhaustion are covered for every pool.
- All 10,000 independent lock/trap pairs are tested at each depth boundary;
  freshly uninitialized native chests have no contents, locks or traps.
- 400 deterministic native fixtures exercise depth boundaries, minimum/cap
  chest counts, minimum/maximum content counts, 12-square acceptance/11-square
  rejection, prohibited terrain, full lighting, single-theme filling, exhausted
  themes, repeated-fill prevention, occupied monster targets and loose-loot/
  monster coexistence. They also reject pre-existing chests and completely
  monster-occupied hosts. Another 40 fixtures exhaust the 20 loose-loot chance
  outcomes crossed with both item-type outcomes, proving exactly 5% and 50/50.
- Focused tests retain all seven Step 11 rooms and Step 5 shop-type/billing
  checks, plus Library with shops and vanilla zoos. Library level/bones codecs
  and restored/revisited entry behavior run in every depth band.

Final validation on 2026-09-18:

```powershell
python test/run_step11.py _qa/step12/diag _qa/step12/focused-verified
python test/run_step11_save.py binary/Release/x64 _qa/step12/save-library-final 8
python _qa/step12/source_contracts.py
python _qa/step12/natural.py
```

All passed. The focused runner includes selector, Library tables/fixtures,
shop types, excluded levels/branches, metadata, accepted bones, all eight
features, recurrence, billing, coexistence, clean/partial generation failures,
and Library level save/restore/bones and entry/revisit checks in all five bands.
The production-game test saves, restores, visits another recurring Library,
returns, saves/restores again, and restores the actual `recover.exe` output.

The additional natural sample uses seeds **120001–120020**, DL5–195, with no
wizard forcing: **3,820 levels**, **3,523 Library-eligible opportunities**,
**94 selections and 94 completions**, **zero double emissions**. This is a
placement sanity check; exact probabilities are established by enumeration.
Evidence is in `_qa/step12/natural/results.json` and individual seed logs.

The source-contract audit checks the unchanged selector and vanilla generation
chain against the starting HEAD, plus 12 unchanged protected files covering
room layout, save version/codecs, object/monster/terrain definitions and shops.
It also checks the empty chest path, native monster flags and target occupancy,
absence of relocation/map traps, raw-depth use, transient theme and loose loot.

The authoritative normal build passed:

```powershell
MSBuild.exe sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64
```

The local wrapper `_qa/step12/build_normal.py` normalizes duplicate `Path`/`PATH`
environment names, routes symbols to `_qa/step12/normal-symbols`, and records
`build-normal.log`. The build required access to existing intermediate build
directories. Final result: **0 errors**, seven warnings in unchanged `mkmap.c`,
`u_init.c`, and `win/win32/mswproc.c`. The diagnostic executable is separate in
`_qa/step12/diag`; normal `NetHack.exe` excludes the STEP11_TEST entry point.
ZIP bytes for `NetHack.exe`, `NetHackW.exe`, and `nhdat500` match Release output.

Final production `NetHack.exe` SHA256:
`3f560a5b29e05405de871213a7419bd97dea80795e537256d07c7ed85bd295fc`.

`git diff --check` passes. No specification deviation or compatibility bump was
required. Existing uncommitted Step 5/11 instrumentation was preserved; this
change is uncommitted and unpublished. Human gameplay review was not performed.
