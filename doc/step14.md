# Step 14 — Depth-Scaled Natural Equipment Enhancements

## Scope and generation boundary

Step 14 extends the existing Step 13 engine and its four persistent fields.
Eligible objects are actual weapons, conventional weapon-class ammunition and
armor, including ordinary magical equipment. Artifacts, every tool, every gem
(including sling stones/flint), and all other classes are excluded. There is no
crafting, forging, gemstone system, reroll command or player upgrade interface.

Creation defaults to `ENH_CONTEXT_NONE`. Native random floor callers use
`enhancement_mkobj`/`enhancement_mkobj_at`; specific normal shop stock uses
`enhancement_mksobj_at`, including replacement imported stock. Natural monster
callers use `enhancement_makemon`, which sets `MM_NATURAL`. `makemon` scopes
`ENH_CONTEXT_MONSTER` only around original `m_initweap`/`m_initinv`, restoring the
previous scope before wearing or delivering migrating inventory. Groups and
entourages propagate the flag. Wandering-monster creation in `allmain.c` opts in;
native Astral `create_mplayers` scopes its separately constructed equipment.

The final `mksobj` event calls `enhancement_created`, which does nothing without
an explicit scope. Synchronous wrappers always restore their previous scope.
Containers inherit their original creation scope. Current native `boxiprobs`
contains no weapon/armor classes, so its contents stay ordinary without changing
that table; normal supply-chest random equipment uses the opted-in random path.
This is creation provenance, not an inventory/ownership/position heuristic.

Wishes, starting inventory, explicit Lua/special-level objects, scripted rewards,
artifact gifts, polymorph, wizard creation and player-created/summoned monsters
do not opt in. Native random room/maze filling uses the opted-in paths even when
the enclosing level also contains authored content. Restore, migration and bones
never activate creation scope. Only original constructors attempt acquisition;
movement, merging, splitting and ownership changes cannot repeat the attempt.

## Effective depth and exact probabilities

Depth is `max(1, min(199, depth(&u.uz)))`, the absolute effective dungeon depth.
Branch-local `u.uz.dlevel` and `level_difficulty()` are not probability inputs.

All four approved contexts use the same table. Percentages after the gate are
conditional on gate success.

| Depth | Gate | Standard / Fine / Exceptional | No property / Has property | One / Two properties |
|---|---:|---|---|---|
| 1–29 | 6% | 65 / 30 / 5 | 50 / 50 | 85 / 15 |
| 30–59 | 10% | 50 / 40 / 10 | 40 / 60 | 80 / 20 |
| 60–99 | 16% | 40 / 40 / 20 | 30 / 70 | 70 / 30 |
| 100–149 | 22% | 30 / 40 / 30 | 20 / 80 | 60 / 40 |
| 150–199 | 28% | 20 / 40 / 40 | 10 / 90 | 50 / 50 |

| Depth | T1 | T2 | T3 | T4 |
|---|---:|---:|---:|---:|
| 1–29 | 70% | 15% | 10% | 5% |
| 30–59 | 55% | 20% | 15% | 10% |
| 60–99 | 40% | 25% | 20% | 15% |
| 100–149 | 25% | 30% | 25% | 20% |
| 150–199 | 10% | 35% | 30% | 25% |

One gate roll is followed by independent quality and property-presence rolls.
Standard plus no property rerolls **both** latter values until valid, without
rerolling the gate, promoting quality or forcing properties. Thus accepted
quality/presence marginals differ from raw table probabilities. Quality-only
Fine/Exceptional and Standard with properties are valid.

If properties are present, roll count, then independently roll each slot's tier.
Select uniformly among valid members of that tier. If none remain, fall down one
tier at a time; never reroll upward or renormalize. Slot two excludes slot one's
exact identity. T1 exhaustion is an impossible-state diagnostic. Catalog metadata
drives selection rather than permanent individual rarity weights.

## Final weapon catalog

| Tier | Property | Prefix | Suffix | Elemental bonus |
|---|---|---|---|---|
| T1 | Fire I | Smoldering | of Embers | 1d4 fire |
| T1 | Cold I | Chilled | of Rime | 1d4 cold |
| T1 | Shock I | Sparking | of Static | 1d4 shock |
| T1 | Trueflight | Trueflight | of Trueflight | +2 ranged hit |
| T2 | Fire II | Blazing | of the Inferno | 3d4 fire |
| T2 | Cold II | Glacial | of the Blizzard | 3d4 cold |
| T2 | Shock II | Thunderous | of the Tempest | 3d4 shock |
| T3 | Fire III | Cataclysmic | of Hellfire | 5d6 fire |
| T3 | Cold III | Stygian | of Absolute Zero | 5d6 cold |
| T3 | Shock III | Voltaic | of Heaven's Wrath | 5d6 shock |
| T4 | Primordial | Primordial | of the Elements | separate 5d6 fire, cold, shock |

Every eligible weapon can receive elemental properties. Trueflight requires
native launcher, weapon-ammunition, missile or spear classification (the native
spear skill includes javelins); arbitrary throwable melee weapons do not qualify.
Distinct tiers of one element, mixed elements, and Primordial with any distinct
eligible property coexist. There are at most two properties, never exact duplicates.

## Final armor catalog

Every armor slot supports each property, except a base item's existing native
property is excluded. Filtering uses native property IDs and secondary native
armor powers, not item-name matching.

| Tier | Native property | Prefix | Suffix |
|---|---|---|---|
| T1 | Searching | Keen | of Searching |
| T1 | Warning | Watchful | of Warning |
| T1 | Stealth | Silent | of Stealth |
| T2 | Fire Resistance | Emberward | of Fire Resistance |
| T2 | Cold Resistance | Frostward | of Cold Resistance |
| T2 | Shock Resistance | Stormward | of Shock Resistance |
| T2 | Poison Resistance | Venomward | of Poison Resistance |
| T3 | Speed | Swift | of Speed |
| T3 | Regeneration | Renewing | of Regeneration |
| T3 | Displacement | Shifting | of Displacement |
| T3 | Slow Digestion | Sustaining | of Slow Digestion |
| T4 | Magic Resistance | Arcane | of Magic Resistance |
| T4 | Reflection | Mirrored | of Reflection |

Distinct properties coexist, including Magic Resistance + Reflection. Native
`u.uprops[].extrinsic` slot masks govern hero stacking, blocking, redundant
sources, intrinsic interactions, removal and Speed limits. Monsters use existing
resistance, speed, regeneration, displacement, magic-resistance and reflection
mechanics. Searching, Warning, Stealth and Slow Digestion remain hero-only.

## Quality, combat and stacks

Standard/Fine/Exceptional give 0/1/2 bonuses: melee/direct-thrown hit and physical
damage, launcher ranged hit, fired-ammunition physical damage, armor AC.
Trueflight from either or both launcher/ammunition is capped at +2. Elemental
components from both sources add and roll independently. Native elemental
resistances suppress only their matching components. Elements are added after
physical armor, Strength/skill/native-enchantment processing and existing
Anarchic/Concordant doubling. They are never part of physical `dmgval` dice.

Confirmed-hit integration uses `hmon_hitmon`, `hitmu`, `mdamagem`,
`thitu_enhanced` and `ohitmon_enhanced`; shade contact and zero-physical early
returns account for elemental equipment. Hero and monster paths share calculators.

An arrow/bolt/dart/shuriken stack receives one result at creation, shared by every
member. Spears, daggers, javelins and boomerangs use ordinary weapon eligibility.
Split copies all four fields. Merging requires exact actual, known, quality and
flags equality. Splits and movement never initiate enhancement generation.

## Knowledge, naming and valuation

Knowledge never gates mechanics. Full identification reveals all properties and
quality. Elemental use reveals only visibly effective components; Primordial is
known when any of its components manifests. Fully resisted effects stay hidden.
Trueflight never identifies just from a hit. Existing Step 13 passive observations
are retained; native resistance, speed, healing, displacement and reflection events
can reveal a unique attributable worn source. Redundant intrinsic or extrinsic
sources prevent attribution. Magic Resistance and Slow Digestion use normal ID.

Quality precedes the enhancement prefix. A single known property uses its prefix;
two use higher-tier prefix and lower-tier suffix. Equal tiers use catalog priority:
Fire, Cold, Shock, Trueflight, Primordial for weapons; the armor table's row order
for armor. Naming is independent of generation order. Examples:
`exceptional primordial long sword of the Inferno`,
`cataclysmic long sword of Embers`,
`cataclysmic long sword of Heaven's Wrath`.

Property surcharges are T1 +50%, T2 +100%, T3 +200%, T4 +400%, added to base
value and then multiplied by the existing 1.0/1.1/1.2 quality factor. T2+T3 is
4x and T4+T4 is 9x before quality. Shops use actual state, preserving native
enchantment/pricing/billing behavior. BUC, native `spe`, material, erosion and
erosion-proofing are not modified by this generator.

## Transformations and save epoch

A real base-type change clears quality, properties, knowledge and flags. A no-op
same-type change preserves state. No transformation rerolls. Artifact conversion
also strips generic state. Other changes (BUC, spe, repair, ownership, containers,
split/compatible merge) preserve state.

The four Step 13 field widths and 112-byte x64 object layout remain unchanged.
Bit 7 is retired; existing low property identities are retained; bits 8–24 add
the new catalog. Actual and known masks use the same mapping. **EDITLEVEL 7 → 8**
explicitly rejects obsolete semantics through the established version mechanism.
There is no migration or new codec. Save/restore, level reload, bones, recovery,
monster/hero/floor/container/migration and bill chains retain the same fields.

## Deterministic and statistical validation

The extended Step 13 diagnostic harness tests every object-table recipient,
native secondary-property filtering, all armor properties/slots, exact component
dice and native RNG tails, nine combat pipelines, same-source and cross-source
stacking, resistance, Anarchic/Concordant independence and zero-physical shades.
It tests canonical/partial naming, observation, every legal pair's price, split,
merge, transformations, artifacts, billing, native level/bones and version gates.

50,000 independent-reference fixed-seed traces verify generation output and the
next native RNG draw, including reroll behavior and downward-only fallback. Depth
boundaries and out-of-range clamps have exact constant checks. A branch fixture
sets local level 2 to absolute depth 150 and compares original creation with the
depth-150 generator. Creation fixtures exercise ordinary constructors, flagged
and unflagged monsters, later monsters, floor/shop wrappers and container creation.

The fixed-seed corpus uses 200,000 long swords per band, seeds 140000–140004
(one continuous native RNG stream per band), totaling 1,000,000 objects:

| Depth | Enhanced | Observed gate | Standard / Fine / Exceptional | Zero / One / Two properties | Selected T1 / T2 / T3 / T4 |
|---|---:|---:|---|---|---|
| 1 | 12019 | 6.0095% | 5822 / 5305 / 892 | 3070 / 7686 / 1263 | 7082 / 1548 / 1056 / 526 |
| 30 | 19882 | 9.9410% | 7435 / 9895 / 2552 | 4956 / 11885 / 3041 | 9934 / 3610 / 2711 / 1712 |
| 60 | 31744 | 15.8720% | 10090 / 14393 / 7261 | 6527 / 17641 / 7576 | 13357 / 8014 / 6712 / 4710 |
| 100 | 44120 | 22.0600% | 11519 / 18597 / 14004 | 6603 / 22417 / 15100 | 13272 / 15553 / 13840 / 9952 |
| 150 | 56054 | 28.0270% | 10253 / 22929 / 22872 | 4557 / 25629 / 25868 | 7705 / 26938 / 25115 / 17607 |

Quality/property columns are accepted results after rejection sampling; selected
tiers include downward fallback (especially duplicate Primordial in slot two).
They must not be compared directly to raw pre-rejection tier/quality percentages.
Deterministic tables and reference-RNG tests are authoritative.

Native object-codec coverage is 5,345,280 finalized legal active-mask × all
class-relevant knowledge masks × three qualities × two flags, plus 27,648 retained
low-byte knowledge cases and nested containers. Batches use native object disposal
to bound memory. The full-game fixture checks seven ownership chains through
save/restore and real checkpoint recovery, not just a synthetic memory copy.

## Commands and evidence

All current enhancement/native gates, the focused Step 11/12 suite, save/recovery,
and the authoritative x64 Release/package validation passed. Production warnings
remain the pre-existing unused `trop`/GUI `fmt` parameters and potentially
uninitialized `mkmap.c` coordinates; no new enhancement warning remains.

- `python test/test_step13_source.py` and `python test/test_step14_source.py`.
- `python test/run_step13.py --out _qa/step14-diagnostic`; build/runtime logs in that directory.
- `C:/Python311/python.exe test/run_step13_save.py _qa/step14-diagnostic/bin _qa/step14-enhanced-save`.
- `C:/Python311/python.exe test/run_step11_save.py binary/Release/x64 _qa/step14-production-save 4`.
- `MSBuild.exe sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /m`; log `_qa/step14-release-build.log`.
- `python test/test_step11_resources.py _qa/step13-donors/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip` checks exact EXE/GUI/DLB ZIP bytes and 186 Lua resources.
- Existing donor regression logs in `_qa/step14-regressions`: 377568 weapon dice/RNG, 800 coatings, 20200 defense dispatch, 180 Deep One equipment combinations, 28672 fey loadouts, 4032 handedness, 864864 clothing-size and 1689120 service-price comparisons.
- `python test/run_step11.py _qa/step14-step11/bin _qa/step14-step11-focused --phase focused` passed the existing selector, all eight features, recurrence, coexistence, level/bones codecs, clean failure and partial failure checks. The isolated executable was compiled with `STEP11_TEST=true` from current source; its build log is `_qa/step14-step11-build.log`.
- Historical source gates use frozen exact Step 14 hunks in `test/step14_historical_changes.json` before the existing Step 13 projection. Tests never regenerate this manifest; all other bytes remain protected.

Two older cumulative source gates already disagree with the pre-task HEAD:
Step 7 `src/mklev.c` against its ancient checkpoint and Step 8A `include/dungeon.h`.
Projected current and pre-task HEAD bytes are identical for both failing files.
These pre-existing failures are not weakened or disguised. Step 9 and Step 10
source gates and current enhancement gates pass.

No intentional gameplay deviation. The two older source-gate failures above are
pre-existing baseline mismatches and are not weakened or disguised. The
pre-existing untracked `.codegraph/` index is preserved.

## Step 14 status

Step 14 is complete on `phase1/equipment-enhancement`. This document records the
finalized implementation and current validation evidence. Step 15 is the next
development step; no Step 15 work is included here.

## Changed-file inventory

- Engine/representation: `include/enhance.h`, `include/hack.h`, `include/patchlevel.h`, `src/enhance.c`.
- Original generation: `src/allmain.c`, `src/makemon.c`, `src/mklev.c`, `src/mkmaze.c`, `src/mkobj.c`, `src/mkroom.c`, `src/mplayer.c`, `src/shknam.c`.
- Combat/native properties/names: `src/mhitm.c`, `src/mhitu.c`, `src/mondata.c`, `src/monmove.c`, `src/mthrowu.c`, `src/muse.c`, `src/objnam.c`, `src/uhitm.c`, `src/worn.c`.
- Current validation: `test/run_step13.py`, `test/test_step13_runtime.c`, `test/test_step13_source.py`, `test/test_step14_combat.c`, `test/test_step14_generation.c`, `test/test_step14_names.c`, `test/test_step14_persistence.c`, `test/test_step14_source.py`.
- Historical validation maintenance: `test/run_step9c_defense.py`, `test/step13_source_projection.py`, `test/step14_historical_changes.json`, and epoch assertions in `test/test_step{7,8a,9a,10b,10b3_1,10b3_4,10b4,10b5,10qa2}_source.py`.
- Documentation: `doc/step13.md`, `doc/step14.md`.
