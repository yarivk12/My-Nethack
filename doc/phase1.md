# Phase 1 — Equipment Enhancement

## Status and baseline

Phase 0 — Dungeons of Doom Expansion is complete and frozen through Step 12.
The exact baseline is the annotated tag `phase0-dod-expansion-complete`, which
points to commit `216bf60903cbde8b3ef1de33aeb7368a09ba6719`, the final commit on
the preserved historical branch `phase0/dod-length`.

Phase 1 is active on `phase1/equipment-enhancement`. Steps 13 and 14, the
Step 15A forge foundation, and Step 15B recipe transactions are complete through
the Phase 1 audit and closeout. Step 15C deterministic equipment-state
inheritance is complete and validated; [Step 15](step15.md) records its separate
native, production persistence and x64 package evidence. Step 15D gemstone
affixing remains deferred. The finalized step documents govern
current semantics; [the Phase 1 audit](phase1-audit.md) records validation and
remaining limitations through Step 15B.

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

## Milestones

The original planning boundaries have been refined by finalized Step 15:

1. **Step 13 — Unified Enhancement Engine + initial properties + quality**
2. **Step 14 — Depth-scaled random enhanced equipment generation**
3. **Step 15A/B — Forge foundation and recipe transactions**
4. **Step 15C — Deterministic equipment-state inheritance** (complete and validated)
5. **Step 15D — Gemstone affixing** (deferred)
6. **Step 16+ — Later combinations and progression** (not implemented)

Optional later extensions may include dragon-scaled armor, monster-essence
concepts, and explicitly designed artifact interactions.

These milestone boundaries may be refined by later design work without changing
the overall Phase 1 objective.

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
