# Phase 2, Step 19: Equipment and high-level monsters

Step 19 expands the persistent catalogue by 20 object types, 14 monster
types, and three artifacts. `EDITLEVEL` changes once, from 14 to 15. Start
a new game; saves and bones from the previous catalogue are incompatible.
There is no migration or second boss-spawn persistence record.

## Equipment

Gain Intelligence and Gain Wisdom rings use native signed attribute bonuses,
including cancellation and draining. Carrying rings sum their enchantments
and multiply finalized native capacity by `(100 + 5 * total spe) / 100`,
using signed arithmetic and the existing capacity boundary. All three rings
have one socket and generation weight 1. The amulet of power grants the existing energy
regeneration property and has probability 60/1000, funded by reducing change,
restful sleep, and strangulation to 95 each.

Passwall is a level-6 Escape spell granting 50-149 turns of the existing wall
passing property. Repair Armor is a level-3 Matter spell repairing exactly
one erosion point on worn armor: random below Basic, selectable at Basic and
above. Menu cancellation and no eligible target leave equipment untouched.
The Hand of Vecna is excluded. Both books have probability 20, funded by ten
points each from knock, wizard lock, confuse monster, and slow monster.
The bone sacrificial knife resembles a knife until identified. Its primary
wielded sacrifice multiplier is 3/2, or 1/2 when cursed; same-race sacrifice
handling is unchanged. Its generation weight is 10 and it shares the former knife
generation allocation equally with ordinary knives.

## Dragons and golems

The canonical mapping in `mondata.c` retains native Gray-through-Yellow
offsets and explicitly maps custom dragon identities. Shimmering, Deep,
Razor, Filth, Shadow, and Celestial dragons each have scales and mail.
Only Shimmering also has a baby and egg/growth family. Scale drops retain
the native 1/3 normal and 1/20 revived rules.

| Dragon | Scales | Mail |
| --- | --- | --- |
| Shimmering | Displacement | Displacement |
| Deep | Drain Resistance | Drain Resistance |
| Razor | FAST | FAST |
| Filth | Sickness Resistance | Sickness Resistance |
| Shadow | Infravision | Infravision, Sleep and Drain Resistance |
| Celestial | Flying | Flying, Sleep and Shock Resistance |

Shadow uses the existing radius-2 dynamic darkness implementation. Shimmering
uses existing monster displacement. Celestial's sonic attack is replaced
by 8d6 magic-missile breath. Deep and Filth corpses are poisonous and convey
poison resistance; Shadow selects sleep or poison, Celestial sleep or shock.
Shimmering and Razor convey no corpse intrinsic.

All scales-to-mail conversions retain enchantment, BUC, erosion/proofing,
Quality, affixes, sockets, gems, and Phase-1 metadata. Conversion immediately
activates the mail's secondary powers. Duplicate property checks occur when
adding a property, so a socket made redundant by conversion survives.

Ordinary generation is restricted to the main DoD with these actual-depth
minima: baby Shimmering 20; Shimmering 30; Deep and Filth 45; Ruby Golem 55;
Sapphire Golem 60; Shadow 70; Crystal Golem 75; Razor and Celestial 80;
Diamond Golem 85. Explicit creation and legitimate polymorph are separate.

Gemstone golems retain Hack'EM claw/breath attacks, HP, resistance and
reflection behavior. They leave no corpse. Ruby/Sapphire drop 1d3 matching
gems, Diamond 1d2 diamonds, and Crystal 1d3 weighted valuable gems, with
duplicates allowed and at most one T4 gem per Crystal death.

## Bosses and artifacts

Nightmare, Beholder, and Vecna are placed at actual DoD depth 40, 50, and 80
respectively, on the first eligible newly generated procedural level with a
valid room square. Special/protected levels, junctions, Medusa, and Castle
defer placement. Native unique birth state prevents repeat generation.
Ordinary random creation cannot select these bosses.

Nightmare reflects intrinsically and never starts with Nighthorn. Eligible
normal deaths can produce +0 cursed Nighthorn despite having no corpse.
Vecna selects Eye or Hand once; an existing selected artifact suppresses the
reward without rerolling. Destructive corpse-suppressing deaths suppress
rewards. Normal artifact uniqueness and bones fallback remain authoritative.

Nighthorn carries the exact historical uncursed/blessed horn algorithm from
the parent of NetHack commit `43d331c4ebe7bcd30a7cab7d4228957e283584c2`,
the April 2020 horn nerf. Its cursed behavior remains current NetHack 5.
Eye grants carried cold resistance, telepathy and half spell damage without
astral vision. Eye and worn Hand use EvilHack death-magic targeting,
immunity, probabilities, cooldown and alignment/Luck consequences.

The Hand binds in the glove slot and survives polymorph, theft and ordinary
destruction. Worn powers include hungerless regeneration, half physical
damage, sickness resistance, and Strength 25. Melee adds 8-12 cold damage
against nonresistant targets. Its donor 1/8 frost explosion uses the actual
area explosion, including collateral and hero damage, with 4d6 unarmed,
2d6 ordinary weapon, and no explosion for artifact weapons. Enchantment
uses the special-armor +5 threshold; a destructive result leaves the Hand
intact, grants no enchantment and consumes the scroll normally.

The zero-generation mundane mummified hand has ordinary removable/enchantable
glove behavior and no artifact powers. It and all three artifacts are excluded
from Phase-1 enhancements and Forge/Salvage; normal wishes exclude the three
artifacts and the mundane hand. Bones can produce the proper mundane bases.

## Donor pins and local adaptations

| Source | Revision | Authority |
| --- | --- | --- |
| SLASH'EM | `fbd743a6081f4a447b9fc2dc545f20c3bc603330` | Deep Dragon, Nightmare, Nighthorn metadata |
| SpliceHack | `8d70ade6f015c7894c171af691393327983173fe` | Razor and Filth Dragons |
| EvilHack | `c444f6a3ab1e9f16d0676961dba86f628e91c6ba` | Shadow/Celestial Dragons, Vecna, Eye, Hand |
| Hack'EM | `cebe2f36e1908e82665c01a30a9eef2f108d07d9` | Gemstone golems and Beholder |

Resistance bits that do not fit native monster resistance storage use existing
resistance helpers and polymorph properties. Native boolean resistances replace
donor percentage resistance handling. Beholder hero petrification uses the
five-turn countdown; monster petrification uses native immediate resolution
with the donor activation gate because native monsters have no delayed
petrification timer. Donor-only sonic/ultravision, enchantment-to-hit gating,
and global traitor/flanking systems are not introduced.

## Validation

The native fixtures live in `test/test_step19*.c`, included in the established
Step-15 engine harness. Source catalogue checks are in `test_step19_source.py`.
`run_step19_source.py` runs exact-delta mutation guards and the unchanged
historical source gates through an explicit Step-19 projection. This retains
earlier persistence and catalogue assertions against their historical inputs.

Run:

```text
py -3 -B test/test_step19_source.py
py -3 -B test/run_step19_source.py
py -3 -B test/run_step15.py --out _qa/step19-step15
STEP17_ONLY=1 py -3 -B test/run_step13.py --out _qa/step19-step17
```

Build `sys/windows/vs/NetHack.sln`, target `NetHack`, with Release/x64.
The solution target also builds the normal data-generation dependencies.

Completed validation on 2026-10-06:

- Five Step-19 source catalogue tests passed.
- Exact-delta guards rejected 138 mutations; historical source gates passed.
- The Step-15 native suite, all added Step-19 fixtures, and 1,000-level corpus
  passed. The corpus had 104 selections, 103 placements, one no-candidate case,
  zero invalid placements and zero multiple placements.
- Step-17 native integration, wishes, object/level/bones persistence, and
  explicit epoch-15 acceptance/epoch-14 rejection passed.
- Normal Release/x64 solution target built `binary/Release/x64/NetHack.exe`
  and regenerated `nhdat500`. Existing inaccessible symbol files required a
  fresh `SymbolsDir` under `_qa/step19-release`; no source workaround was needed.
- `git diff --check` passed. Existing compiler warnings remain in unrelated
  code; no unresolved Step-19 blocker remains.

### Dragon armor wish parser correction (2026-10-06)

Named dragon armor wishes exposed two legacy monster-to-object offset
calculations in `readobjnam`: custom monster order differs from armor order.
The new parser fixture reproduced Glowing scales resolving to Celestial scales
against the unchanged production code. Both scales and scale-mail parsing now
use the existing `dragon_armor_type` mapping. Native ordering, Chromatic wish
restrictions, catalogue identities and `EDITLEVEL` 15 remain unchanged.

`test_step19_wishes.c` exercises the real parser with 72 cases: all ten native
families, Glowing, Chromatic and the six Step-19 families, each as scales and
scale mail in ordinary and wizard modes. It asserts exact object identities
(the existing native fallback range for ordinary Chromatic wishes), blessed
status and explicit enchantment. Wizard cases use -2, including Shimmering
scales; ordinary cases use -1 because native wish rules may reduce larger
enchantment magnitudes randomly.

The matrix and Step-15/19 native fixtures passed, including the runner's
1,000-level corpus. Five catalogue tests, all historical source guards and
140 Step-19 mutation rejections passed. Step-17 wish/integration, persistence,
bones and epoch acceptance/rejection checks also passed. The normal Release/x64
`NetHack` target built successfully as 5.0.0-15, and `git diff --check` passed.
