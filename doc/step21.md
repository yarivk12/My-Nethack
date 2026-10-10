# Step 21 implementation record

Status: in progress. This document is not a completion claim.

Authority: [approved scope](Step21_Artifact_Expansion_FINAL_Implementation_Scope.md).
Baseline: `7813481f7`, completed Step 20. Existing tracked sources were clean
when work started. The supplied scope and `.codegraph/` were untracked.

## Audit findings

- Silver Key already exists once, based on `UNIVERSAL_KEY`, with carried
  energy regeneration, teleport control and polymorph control. Its explicit
  portal handler bypasses ordinary invocation cooldowns. Preserve it.
- Artifact identities use `obj.oartifact` (signed char), generated enum and
  `NROFARTIFACTS`-sized tracking arrays. Capacity must be checked after expansion.
- Native `mk_artifact` supports base-object conversion and aligned gifts;
  imported role-exclusive acquisition must not be retained. Wishes still use
  native rarity and artifact-history checks.
- Existing `MASK` and `JACKET` bases are available. Do not duplicate them.
- Pickup grants properties before linking inventory. Removal callbacks must
  exclude the departing object. A rescan which assumes completed pickup is wrong.
- Reflection combat resolution in `ureflects` needs integration as well as
  property bits for carried/face sources. Worn grants need exact slot checks.
- Native energy regeneration already implements original SpliceHack's fast
  every-turn behavior. It does not need a second additive regeneration clock.
- Forge recipe eligibility rejects artifact ingredients in `forge` selection;
  complete consumption-path and runtime coverage remains pending.

## Cooldown field inventory

| Mechanism | Authoritative state | Knowledge required | Clock |
|---|---|---|---|
| Ordinary artifact invocation | `obj.age` | Known and seen object, discovered artifact | Absolute `svm.moves` deadline |
| Invoked property reactivation | `obj.age` | Same | Deadline established on deactivation |
| Stoning enhancement | `o_stoning_remaining` | Identified affix | Active-level turns |
| Purifying enhancement | `o_purification_remaining` | Identified affix | Active hero turns |
| Silver Key travel | None | No cooldown display | No cooldown |
| Necronomicon reading | None | No cooldown display | No invocation cooldown |
| Lamps, candles, corpses, eggs | Fuel/age/lifecycle timers | No cooldown display | Not a use cooldown |

Inspection reads these clocks without changing them. Existing enhancement
impact descriptions remain available. Newly implemented active mechanics must
be added to this inventory before being considered validated.

## Inspected donor sources

| Repository | Immutable revision | Inspected files |
|---|---|---|
| `NullCGT/SpliceHack` | `b3a1427a43fd6ecc6088129e72ac594a1d3e531f` (Spl-R-1.1.0) | `include/artilist.h`, `src/artifact.c`, `src/allmain.c` |
| `Chris-plus-alphanumericgibberish/dNAO` | `d512426550448e944f381469e498423b656f2e74` | `include/artilist.h`, `src/artifact.c`, `src/read.c`, `src/apply.c`, `src/spell.c`, `src/dog.c`, `src/attrib.c`, `src/mthrowu.c`, `src/mcastu.c` |

Original SpliceHack `SUMMONING` calls
`create_critters(rnd(10), NULL, TRUE)`. Its energy regeneration uses a native
per-turn random gain of `1..((WIS+INT)/15+1)`; the current local engine retains
that formula and its existing magical-breathing interaction. The derivative
card-drop invocation is not used.

dNAO Shirt reading permits blindness and direct-carried use. Opaque body armor
blocks reading only when the shirt is worn underneath it. Its unicorn-horn
cures need mapping to current local status cures without attribute restoration.
The Duster copies source beatitude and creates exactly one selected ordinary
supply; its donor worn requirement is explicitly overridden by the scope.

## Verification record

- Native regression fixture added for artifact cooldown inspection.
- RED: Release x64 test executable failed at the expected missing
  `Cooldown: 37 turns remaining` assertion. Log:
  `_qa/step21/red/runtime.log`.
- GREEN and subsequent artifact, acquisition, persistence and final build
  acceptance remain pending until executed and recorded below.

All 22 added artifact implementations and their required validation matrices
remain open. No applicable unexecuted case is classified as PASS or N/A.
