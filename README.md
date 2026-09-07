# My-Nethack

## Private NetHack 5.0 expansion

This repository is a private NetHack 5.0 expansion based on upstream NetHack
5.0. Development is staged as small, reviewable milestones. The implementation
branch is `phase0/dod-length`, and this README is updated whenever project
progress is committed.

## Current state

The completed project includes the frozen dungeon-depth foundation, Step 5 shop
optimization, and Step 6/7 NerfHack dungeon enrichment. The Step 7
implementation is committed as `a3b0ec624` (`Add classic NerfHack Lost Tomb`)
and tagged `step7-classic-lost-tomb`; later documentation commits keep this
summary current.

## Frozen structural baseline

Commit `64db689a1` (`Expand dungeon depth and harden ledger capacity`), tagged
`expanded-dod-200-baseline`, established the structural foundation:

- Dungeons of Doom has exactly 200 levels, with the Castle at DL200.
- Physical depth, ledger, save, restore, and recovery identifiers use widened
  signed-depth handling.
- Runtime ledger capacity is audited through 3199.
- Deep-level save/restore, recovery, Castle, Gehennom, and Vlad's Tower paths
  were validated on Windows x86 and x64.

This structural baseline is frozen. Later milestones do not change dungeon
length, ledger capacity, or ordinary depth semantics. Step 7 adds no
save-format fields; its `EDITLEVEL` advance to 2 is required because stored
monster, object, and dungeon IDs changed.

## Step 5: shop optimization

Commit `4ce6bff74` (`shops--optimization-step5-complete`) completed the shop
milestone:

- Shop creation preserves its eligibility rules and uses a 15% minimum
  shop-attempt chance from depth 20 onward.
- Random shop-type weights are general 30, armor 5, scroll 1, potion 1,
  weapon 1, food 1, ring 15, wand 10, tool 10, spellbook 25, and health-food 1.
- General and specialized shop-stock distributions use the approved weighted
  tables; shop-stock mimic creation is capped at 10%.
- Global `RANDOM_CLASS` generation and unrelated gameplay remain unchanged.

## Step 6: NerfHack dungeon enrichment

Commit `9191de707` (`Add initial NerfHack dungeon enrichment`), tagged
`step6b-nerfhack-dungeon-enrichment`, imported approved NerfHack `dev` content.
NerfHack Big Room 18 is preserved locally as `dat/bigrm-14.lua`, the next
available local Big Room slot. The extended Dungeons of Doom keeps:

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
`0cb8781b0929b4617590a3b9fe78f972ef42c25f`. Only classic `dat/tomb-1.lua` was
imported; `tomb-2.lua` and unrelated NerfHack systems were not imported.

Step 7 adds exactly one optional one-level branch named **The Lost Tomb**. Its
DoD entrance is selected by the existing persistent scheduler at a random
depth from DL30 through DL199, after avoiding Step 6 reservations. The classic
map preserves its geometry, secret paths, locked and trapped chests, pits,
treasure, wax candles, undead and Shadow populations, guardian, and reward
selection logic.

Shadow is a genuine `G_NOGEN` monster with its imported undead, non-corporeal,
invisible, wall-passing, light-sensitive, resistance, movement, and attack
properties. It is explicitly created by the Tomb and excluded from ordinary
random generation.

Magic Candle is a normal magical `TOOL_CLASS` object. It works through normal
inventory, apply, light, extinguish, drop, pickup, and container paths. Lit
Magic Candles provide permanent radius-3 light without a fuel timer. Attaching
one to a Candelabrum produces ordinary finite candle fuel. Vanilla magic-lamp
behavior and wishing remain unchanged.

Adding the monster, object, and dungeon definitions advances `EDITLEVEL` to 2;
older saves and bones are rejected by the normal version gate. A four-newline
Moloch map repair restored the intended 56×9 evaluated map without changing its
gameplay design.

## Validation

The final Step 7 change set includes focused source/content/runtime tests,
topology smoke tests, packaging checks, x64 and Win32 Release builds, x64/x86
depth-range and ledger/recovery regressions, and `git diff --check`. Scheduler
tests covered 2,000 samples per architecture, 168 distinct legal Tomb depths,
and no Step 6 reservation collisions. Fresh packaged games confirmed one Tomb,
one Temple, 3–5 Big Rooms, Castle DL200, and varying DL30–199 placement.
Tomb traversal, save/reload with a lit Magic Candle, Candelabrum conversion,
Shadow exclusion, and final DLB contents were also validated.

Detailed provenance, compatibility notes, test commands, and the complete
Step 7 changed-file inventory are in [doc/step7.md](https://github.com/yarivk12/My-Nethack/blob/phase0/dod-length/doc/step7.md).

## Milestone policy

Every project commit updates this README with the resulting project state and
is published to the corresponding GitHub branch. Gameplay changes require the
focused validation and regression coverage documented above.
