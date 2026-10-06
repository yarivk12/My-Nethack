# Phase 1 — Equipment Enhancement

## Status and baseline

Phase 0 — Dungeons of Doom Expansion is complete and frozen through Step 12.
The exact baseline is the annotated tag `phase0-dod-expansion-complete`, which
points to commit `216bf60903cbde8b3ef1de33aeb7368a09ba6719`, the final commit on
the preserved historical branch `phase0/dod-length`.

Phase 1 remains active on `phase1/equipment-enhancement`. Steps 13 and 14,
Steps 15A through 15D, Steps 16A through 16C, Step 17, Step 17.5, Step 17.6,
and Steps 18A and 18B are complete. Their implementation records contain
the step-specific behavior and validation evidence: [Step 15](step15.md),
[Step 16A](step16a.md), [Step 16B](step16b.md), [Step 16C](step16c.md),
[Step 17](step17.md), [Step 17.5](step17.5.md), [Step 17.6](step17.6.md), and
[Step 18A](step18a.md) and [Step 18B](step18b.md). The
[Phase 1 audit](phase1-audit.md) is retained as a historical audit with scope
and evidence through Step 15B; later step records supersede its milestone
status.

Phase 2 begins with [Step 19](step19.md), which adds ordinary equipment,
high-level dragons and golems, three procedural bosses and their artifacts.
It preserves the Phase-1 enhancement model, including existing socket and
affix state across dragon armor conversions. Its persistent catalogue uses
`EDITLEVEL 15`; the completed Phase-1 milestone used 14.

## Objective

Introduce a unified enhancement framework for weapons and armor, followed by
enhanced loot generation, forging, gemstone affixing, advanced properties, and
late-game equipment progression.

## Scope

The initial Phase 1 scope includes:

- melee weapons;
- launchers;
- thrown weapons;
- ammunition;
- armor slots;
- generic per-object enhancement metadata;
- quality;
- object properties;
- enhanced equipment generation;
- forging;
- gemstone affixing;
- later advanced/endgame equipment progression.

The following are not part of the initial Phase 1 baseline:

- enhancement systems for unrelated non-weapon/non-armor item classes;
- artifact fusion;
- wholesale transplantation of another variant's complete equipment system;
- migration or backward compatibility for old save/bones formats.

## Architectural principles

Phase 1 design is constrained by the following principles:

- Build one target-neutral generic enhancement model rather than independent
  systems for random properties, forging, gemstones, and later progression.
- Acquisition mechanisms should reuse common enhancement semantics instead of
  implementing duplicate combat or worn-effect logic.
- Keep existing imported `obranch_*` metadata conceptually separate from
  generic equipment enhancement metadata unless later current-HEAD analysis
  demonstrates that a refactor is genuinely preferable.
- Use fixed-width representations for new persistent masks/fields where
  relevant rather than depending on native `long` width.
- Artifact interaction must be deliberate; ordinary enhancement must not
  accidentally multiply artifact power.
- Donor probability curves and power values must not be copied blindly into
  My-Nethack's 200-level DoD.

The shared representation and lifecycle contract are defined in
[Step 13](step13.md), with the finalized catalog and acquisition rules in
[Step 14](step14.md).

## Persistence policy

My-Nethack is not deployed and has no existing player save/bones population
requiring preservation. Therefore:

- backward compatibility with pre-Phase-1 save files is not required;
- migration of old save files is not required;
- backward compatibility or migration for old bones files is not required;
- preservation of the current serialized object representation is not a
  constraint;
- Phase 1 may make a clean persistent-object-format change when justified;
- saves and bones created by the resulting new implementation must still
  function correctly within that implementation.

## Current milestones

The following implementation milestones are complete:

1. **Step 13: Unified Enhancement Engine, initial properties, and Quality**
2. **Step 14: Depth-scaled random enhanced equipment generation**
3. **Steps 15A and 15B: Forge foundation and recipe transactions**
4. **Step 15C: Deterministic equipment-state inheritance**
5. **Step 15D: Gemstone affixing**
6. **Step 16A: Offensive affixes**
7. **Step 16B: Defensive and armor affixes**
8. **Step 16C: Utility and tool affixes**
9. **Step 17: Forge and Affix System**
10. **Step 17.5: Wished Item Enhancement Generation**
11. **Step 17.6: Forge Menu Terminology**
12. **Step 18A: Weapon and Armor Forge Progression Chains**
13. **Step 18B: Magical Equipment and Tools Forge Recipes**

The current Forge has 47 exact formulas: 23 from Step 18A and 24 from Step 18B.
Output types are deterministic. Enchantments inherit from supported sources;
consumable charges never inherit. Magic Flute and Magic Harp receive their
native fresh randomized `4..8` charges. Step 18B changes no persistence format
or EDITLEVEL.

The original planning label that grouped later combinations and progression
under Step 16+ is historical and has been superseded by the completed Step 16A
through Step 18B records above. Further Phase 1 extensions, such as
dragon-scaled armor, monster-essence concepts, or explicitly designed artifact
interactions, remain possible future work and are not part of these completed
milestones.

These milestones refine the overall Phase 1 objective without changing it.

## Historical Step 13 entry requirement

Step 13 required a then-current-HEAD architecture/design audit. Earlier
enhancement research remains useful background, but it was performed against
an earlier repository snapshot and must not substitute for inspection of the
current Phase 1 codebase.

The audit must establish, from the then-current repository:

- the current object model and relevant existing metadata;
- all relevant weapon/armor lifecycle paths;
- enhancement/property/quality representation;
- eligible equipment categories;
- artifact policy;
- the generation model;
- persistence behavior for the new format;
- merge/split and identification implications;
- combat and worn-property integration surfaces;
- a concrete verification contract.

The audit is an entry requirement, not an implementation detail to be assumed
in advance.
