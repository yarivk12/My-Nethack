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
cures are mapped to current local status cures without attribute restoration
in the utility-group matrix below.
The Duster copies source beatitude and creates exactly one selected ordinary
supply; its donor worn requirement is explicitly overridden by the scope.

## Verification record

- Native regression fixture added for artifact cooldown inspection.
- RED: Release x64 test executable failed at the expected missing
  `Cooldown: 37 turns remaining` assertion. Log:
  `_qa/step21/red/runtime.log`.
- GREEN: `_qa/step21/green-runtime.log`, full native fixture exit 0 including
  Step 19 and Step 20. Cooldown inspection committed as `51aa5450d`.
- First artifact group: `_qa/step21/group1-red-runtime.log` fails on missing
  Lily registration; `group1-active-runtime.log` passes carried properties,
  real reflection resolution, invocation, native acquisition and uniqueness.
- Equipment group: `_qa/step21/group2-red-runtime.log` fails on missing Cloak;
  `group2-coverage-runtime.log` passes Cloak carried sleep resistance and worn
  free action/MR/MC3, Ring carried sickness resistance and worn free action/MR,
  Tlaloc property removal orders and worn water protection, amulet worn
  reflection/breathing, and native acquisition for all seven additions.
- `test_step19_source.py`: 5 passed. `test_step20_source.py`: 4 passed.
- Full persistence, protected terrain, detailed summon/teleport controls and
  final production build acceptance remain pending.

Seven additional designs now have gameplay implementations: Void Lily, Earth
Crystal, Cloak of the Unheld One, Ring of P'hul, Mask of Tlaloc,
Ehecailacocozcatl, and Storm Whistle. All final validation matrices remain open.
No applicable unexecuted case is classified as PASS or N/A.

The latter five use inspected dNAO definitions above, the previously pinned
SLASH'EM `fbd743a6081f4a447b9fc2dc545f20c3bc603330` (`include/artilist.h`,
Candle/Whistle definitions), and Hack'EM
`cebe2f36e1908e82665c01a30a9eef2f108d07d9` (Ring definition).
Approved adaptations remove donor role-only acquisition, Storm Whistle Warning,
Tlaloc Half Spell Damage and amulet Energy Regeneration/other unapproved powers.
The Cloak invokes native hero `tele()` regardless of its equipment state.
The amulet and whistle create one native elemental with initialized pet state.

## Utility group implementation and donor reconciliation

Shirt, Talisman, Duster and Lyre gameplay is implemented. Native fixture
`group3-coverage-runtime.log` passes capacity BUC rounding/noncompounding,
Talisman slot properties, Shirt opaque/transparent covering and blind reading,
Duster six supplied types across three beatitudes, nonvisible pet tameness,
and Lyre single native migration with absent-source and sleeping controls.
Six actual transition entry points and full persistence remain unexecuted.

Shirt donor: pinned dNAO `src/read.c` at `arti_mandala` and
`src/apply.c` at `use_unicorn_horn`. Local mapping:

| Condition/prerequisite | Donor behavior | Local mapping |
|---|---|---|
| Direct-carried or worn uncovered shirt | Reading permitted | Native read selection |
| Worn under opaque body armor | Reading blocked | No action or cure |
| Worn under transparent glass armor | Reading permitted | Crystal plate mail exception |
| Blind | Mandala reading permitted | Native timed blindness remains a possible cure |
| Sickness | Eligible cure | `make_sick`, all local sickness types |
| Blindness above face cream duration | Eligible cure | Timed component only, cream preserved; ongoing engulfing blindness excluded |
| Hallucination, vomiting, confusion, stun | Eligible cures | Native timed condition setters |
| Deafness, slime, petrification, lost attributes | No donor cure | Not added to the cure list |
| Blessed/uncursed | Random subset, `rn2(d(2,4/2))` cures | Same native random subset algorithm |
| Cursed | Uniform sickness/blindness/confusion/stun/hallucination | Same five outcomes; added duration `rnd(100)` except native sickness formula |

Local permanent condition sources are not erased. Native capacity/read
prerequisites still apply. The artifact does not add attribute restoration,
attribute bonuses or an enchantment-cap exception.

Talisman donor: pinned notdnethack `src/artifact.c`, HEALING (line 10701),
uses `(missing_HP + 1) / 2`, choosing polymorph HP when appropriate. This is
half missing HP, not half maximum HP. The scope's donor-formula precedence
is followed using the identical existing local HEALING implementation:
80/100 HP becomes 90/100. Sickness, slime and blindness above face cream
are removed with native setters. Native life saving remains consumptive.

Independent review found native food initialization can produce a pair.
Duster retrieval now forces quantity one and recomputes weight before native
inventory/floor placement. Lyre changes only the proximity term for tame
monsters in shared `keepdogs`, retaining all other migration restrictions.

## Permanent lights

Lantern, Candle and Glitterstone use native object light sources at fixed
radii 4/3/2. Creation attaches a source even while the object is temporarily
unlocated; native location resolution occludes contained objects. There is no
fuel timer and invocation `age` is untouched by lighting. Native destruction
releases the source. Applying the permanent candle cannot consume it into the
Candelabrum. Candle warning/cold resistance and fire-elemental invocation,
Glitterstone energy regeneration and native BUC recharge are registered.

Lantern revelation matches native circle bounds and source line of sight,
then requires current hero vision. It overrides only monster invisibility.
Native trap discovery and secret-door conversion run after vision updates and
hero actions. Invisible-monster glyphs refresh when coverage is lost too.
Independent review found null camera-flash and saved-identifier source hazards;
the query now excludes those before dereferencing object pointers.

`group4-red-runtime.log` fails at missing light registration;
`group4-coverage-runtime.log` passes the full native regression fixture,
including fixed BUC radii, no fuel timers, floor/drop behavior, lit-room
invisible-monster entry/exit, container occlusion, blindness, destruction and
camera null-source control. Full light save/restore, trap/door and opaque-wall
matrices still need execution. Fourteen new artifacts now have implementations;
the original Silver Key remains preserved, and Step 21 remains incomplete.
