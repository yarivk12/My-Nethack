# My-Nethack

## Private NetHack 5.0 expansion

This repository is a private NetHack 5.0 expansion based on the upstream
NetHack 5.0 source. Development is staged as small, reviewable milestones.
The implementation branch is `phase0/dod-length`; the GitHub-visible README is
kept current with every project commit.

## Current state

The completed project includes the frozen dungeon-depth foundation, the Step 5
shop milestone, the Step 6/7 NerfHack dungeon enrichment, and the Step 8
Ruins of Moria integration. Step 8 is committed as `Add Ruins of Moria` and
tagged `step8-ruins-of-moria`; this README is updated with every project
commit.

## Frozen structural baseline

Commit `64db689a1` (`Expand dungeon depth and harden ledger capacity`), tagged
`expanded-dod-200-baseline`, established the structural foundation:

- Dungeons of Doom has exactly 200 levels, with the Castle at DL200.
- Physical depth, ledger, save, restore, and recovery identifiers use the
  widened signed-depth handling.
- Runtime ledger capacity is audited through 3199.
- Deep-level save/restore, recovery, Castle, Gehennom, and Vlad's Tower paths
  were validated on Windows x86 and x64.

This structural baseline is frozen. Later milestones do not change dungeon
length, ledger capacity, or ordinary depth semantics. Step 7 added no
save-format fields; its `EDITLEVEL` advance to 2 was required because stored
monster, object, and dungeon IDs changed. Step 8 also adds no save-format
fields; `EDITLEVEL` advances to 3 because stored monster, object, artifact,
dungeon and terrain IDs changed.

## Step 5: shop optimization

Commit `4ce6bff74` (`shops--optimization-step5-complete`) completed the shop
milestone:

- Shop creation keeps its existing eligibility rules and uses
  `max(vanilla 3/depth, 15%)`; the 15% minimum therefore starts at DL20.
- Random shop-type weights are general 30, armor 5, scroll 1, potion 1,
  weapon 1, food 1, ring 15, wand 10, tool 10, spellbook 25, and health-food 1.
- General and specialized shop-stock distributions were updated to the
  approved weighted tables.
- Shop-stock mimic creation is capped at 10%.
- Global `RANDOM_CLASS` generation and unrelated gameplay remain unchanged.

## Step 6: NerfHack dungeon enrichment

Commit `9191de707` (`Add initial NerfHack dungeon enrichment`), tagged
`step6b-nerfhack-dungeon-enrichment`, imported the approved NerfHack `dev`
content. Big Room 18 from NerfHack is preserved locally as `dat/bigrm-14.lua`,
the next available local Big Room slot. The extended Dungeons of Doom keeps:

- 3–5 Big Rooms per game, with a hard maximum of 5;
- exactly 1 Giant Court;
- 2–3 Real Zoos;
- exactly 1 Dragon Lair; and
- exactly 1 optional Temple of Moloch branch.

These features use randomized persistent placement in DL30–199. The former
deterministic DL101–105 validation placement was removed after testing. Castle
DL200 and normal branch depth/display semantics remain unchanged.

## Step 7: classic NerfHack Lost Tomb

The sole donor is NerfHack `dev` at pinned commit
`0cb8781b0929b4617590a3b9fe78f972ef42c25f`. Only the classic
`dat/tomb-1.lua` map was imported; `tomb-2.lua` and unrelated NerfHack systems
were not imported.

Step 7 adds exactly one optional one-level branch named **The Lost Tomb**.
Its DoD entrance is selected by the existing persistent scheduler at a random
depth from DL30 through DL199, after avoiding Step 6 reservations. The classic
map preserves its geometry, secret paths, locked and trapped chests, pits,
treasure, wax candles, undead population, Shadow population, guardian, and
reward selection logic. The branch uses normal saved topology and ordinary
return-stair/depth behavior.

Shadow is a genuine `G_NOGEN` monster with the imported undead, non-corporeal,
invisible, wall-passing, light-sensitive, resistance, movement, and attack
properties. It is explicitly created by the Tomb and excluded from ordinary
random generation.

Magic Candle is a normal magical `TOOL_CLASS` object. It can be generated and
handled through normal inventory, apply, light, extinguish, drop, pickup, and
container paths. When lit it provides permanent radius-3 light without a fuel
timer. Attaching it to a Candelabrum produces ordinary finite candle fuel.
Vanilla magic-lamp behavior and wishing remain unchanged.

Adding the monster, object, and dungeon definitions advances `EDITLEVEL` to 2;
older saves and bones are rejected by the normal version gate. A four-newline
Moloch map repair restored the intended 56×9 evaluated map without changing its
gameplay design.

## Step 8: Ruins of Moria

Step 8 imports the Ruins of Moria from the UnNetHack `dev` donor at pinned
commit `439b8d63d3d1ca78fb08588dd43f61874114b21a`. It adds one six-level upward
branch represented by ten Lua maps: the Endless Stair, broken bridge,
regenerating seven-room floor, four orc halls, forest and barracks, and the
intact or ruined Doors of Durin terminus.

The import preserves the donor's monsters (including deep orcs, Durin's Bane,
Watcher in the Water and the swamp fern lifecycle), objects (iron safes and
unrefined mithril), terrain (dead trees, muddy bogs and outdoor sky), Balin's
grave, the Earthstone artifact and portal, loot, engravings, sounds, map
geometry and generation tables. Existing magic-lamp and wishing behavior is
unchanged. No save-format fields were added; the new stored IDs advance
`EDITLEVEL` to 3.

Exactly one Moria branch is placed per game. Its DoD entrance is selected once
by the persistent scheduler at a random legal depth from DL30 through DL199,
with a distinct reservation that avoids all special levels, branch entrances,
Big Rooms, Giant Court, Real Zoos, Dragon Lair, Temple of Moloch and Lost Tomb.
The temporary deterministic DL107 placement used during Step 8A manual
validation was removed before release, and no per-floor random roll remains.
The six branch floors occupy the entrance depth minus one through minus six
and return to the saved DoD entrance; Castle remains DL200.

## Validation

The final Step 8 change set contains focused source/content/runtime tests,
topology smoke tests, and packaging checks. Validation included:

- 2,000 scheduler samples on x64 and x86, with 168 distinct legal Tomb depths
  and no Step 6 reservation collisions;
- donor/local Lua content comparisons across all secret-path and reward cases;
- Shadow and Magic Candle database, lighting, Candelabrum, shop-charge, and
  vanilla magic-lamp checks;
- fresh packaged topology games confirming one Tomb, one Temple, 3–5 Big Rooms,
  Castle DL200, and varying DL30–199 placement;
- Tomb traversal, save/reload, lit Magic Candle persistence, and container
  cleanup checks;
- x64 and Win32 Release builds and DLB verification;
- x64/x86 depth-range and ledger/recovery regressions; and
- Moria's ten-map content, source, runtime, randomized topology, traversal and
  save/reload checks on x64 and Win32; and
- `git diff --check` and final source audits with no generated artifacts.

Detailed provenance, compatibility notes, test commands, and the complete
changed-file inventory are in [doc/step8b.md](https://github.com/yarivk12/My-Nethack/blob/phase0/dod-length/doc/step8b.md).

## Milestone policy

Every project commit updates this README with the resulting project state and
is published to the corresponding GitHub branch. Gameplay changes require the
same focused validation and regression coverage described above.
