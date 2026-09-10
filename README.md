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
tagged `step8-ruins-of-moria`. Step 9A–9C adds Sheol, Dragon Caves and
Mithardir. Their production parents now use the shared persistent scheduler,
each selecting one distinct legal Dungeons of Doom depth from DL30–199;
DL108–111 have no fixed Step 9 reservation. Step 9D was canceled. The final
architecture/runtime/package closeout has passed on x64 and Win32: the native
fresh-topology fixtures, focused regressions, donor/package checks, tile checks
and non-PTY Mithardir suite all pass. The exact commit, annotated tag and
atomic publication are recorded below after the final Release rebuild.

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

The combined Step 9 save epoch is `EDITLEVEL = 4`. Mithardir adds saved hero
syllable counters/timers, Word knowledge/cooldowns and slab-generation state,
plus scoped object material, size and property metadata. Pre-Step 9 saves and
bones are unsupported; there is no migration. The frozen DoD length, ledger
capacity and ordinary depth semantics remain unchanged.

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

## Step 9A–9C: FINAL PRODUCTION PARENT PLACEMENT

All three branches are implemented and have passed their historical manual
validation at temporary fixed parents. The production code now removes those
fixed placements: each branch is selected once from the shared persistent
DL30–199 scheduler, with collision avoidance against the existing Step 5–8
reservations and against the other two Step 9 parents.

### Step 9A: Sheol

Donor: UnNetHack, pinned commit
`439b8d63d3d1ca78fb08588dd43f61874114b21a`. One optional descending branch has
6–8 levels and four Lua resources: procedural frozen passages, the middle
map, the palace entrance and the Executioner's palace. Imported content
includes 15 monster definitions/dependencies, cold fern and weeping-angel
behavior, blue-slime movement freezing, white-naga frost spit, ice traps,
opaque ice walls, transparent crystal ice walls, fire/digging interactions,
the crystal pick, and the Executioner encounter and donor rewards.

Sheol was manually validated at the **temporary fixed DoD108** parent. Its
production parent is now randomized persistently in DL30–199. It returns
through ordinary branch stairs; the donor's Valley shortcut is excluded. See
[Step 9A](doc/step9a.md).

### Step 9B: Dragon Caves

Donor: the same UnNetHack commit,
`439b8d63d3d1ca78fb08588dd43f61874114b21a`. One optional four-level descending
branch uses all four donor maps, dragon/worm populations, bogs, trees, traps
and hoard tables. It adds baby/adult glowing dragons and non-unique chromatic
cave dragons, lava breath and terrain interactions, glowing/chromatic scales
and mail, worn powers/light, armor conversions, and finite scale drops across
corpse revival. The native Caveman Chromatic Dragon remains unchanged. The
donor's apparent dragon shop compiles as an ordinary room, preserved here.

Dragon Caves was manually validated at the **temporary fixed DoD109** parent.
Its production parent is now randomized persistently in DL30–199. See
[Step 9B](doc/step9b.md).

### Step 9C: Mithardir

Donor: dNetHack (`Chris-plus-alphanumericgibberish/dnethack`), pinned commit
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. One optional ten-level branch
contains Elshava and its eight shops/services, three Wastes floors, three
Last Spire maps and three generated Catacombs floors. Eight Lua resources
include the DoD approach. Directed portals, the Last Spire shortcut, white-dust
storms, shallow-water freezing/thawing, terrain effects and generated terminal
rooms preserve the selected donor content with documented native adaptations.

The import adds 22 monster definitions and 36 object types, including Alabaster
creatures, Eladrin forms, First Wraithworm, Aspect of The Silence, living armor
and masks, sized/material-specific equipment, six ceramic syllables and three
Word slabs. Syllable effects, Word study/powers, boss encounters, rewards and
shop services persist through saves. The Second and Third Keys of Chaos work
as ordinary keys; they add no endgame gate. Donor-global quests, roles, sanity,
insight and ascension requirements are excluded.

Mithardir was manually validated through the **temporary fixed DoD110**
approach portal. Its production parent and DoD approach map are now rebased
together and randomized persistently in DL30–199. See
[Step 9C](doc/step9c.md) for provenance, mechanics, adaptations and test
evidence.

### Scope and checkpoint validation

Step 9D, Neutral Quest / Lost Cities, was **canceled before production
integration** and is not part of this milestone. Its [read-only audit](doc/step9d.md)
is historical only. **DL111 is not reserved for Step 9**; the existing scheduler
can use it for earlier enrichment. DL108–110 are also ordinary candidates when
otherwise eligible. Steps 5–8 occurrence rules and randomized placement remain
intact, with Castle at DL200. The combined Step 9 `EDITLEVEL` is 4.

User manual validation of 9A, 9B and 9C passed at the historical fixed parents.
This closeout's donor/source boundary checks, scheduler contract, prior Step
7/8 source checks, Python syntax checks and `git diff --check` pass. Fresh
native production topologies pass eight samples on both x64 and Win32; the
focused Step 9A/9B suites and the non-PTY Mithardir native/comparison suite
also pass on both architectures. The final Release/package and tile gates are
recorded in the closeout report.
Publication also preserves significant ASCII map padding with scoped Git
whitespace rules and a source guard for whitespace outside map literals.

See the [closeout record](doc/step9.md) and
[wizard verification findings and fixes](doc/step9-playtest.md). The final
publication is made only after the exact staged delta and Release artifacts
are rechecked.

## Validation through Step 8

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
