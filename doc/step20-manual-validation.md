# Step 20 manual test and validation plan

Scope: the complete Deep-Dungeon Monster Expansion, including the Vorpal
fatal-damage and Priestess crystal-sword corrections. This is an execution
plan, not a record that the following manual cases have passed. Record results
against the final local working tree and final Release x64 executable.

## Preparation and evidence

1. Record the commit, `git status --short`, executable hash, build date,
   configuration, and `EDITLEVEL`. Include uncommitted Step 20 changes in the
   implementation under test. Do not substitute the pushed branch.
2. Use an isolated wizard game and fresh save directory. Start the final
   `binary/Release/x64/NetHack.exe` with `-D`; confirm wizard commands are
   enabled. Use `#wizgenesis` to create named monsters, `#wizwish` for equipment,
   `#wizlevelport` for depths, `#wizmap` for visibility, and `#wizwhere` to
   distinguish branches and special levels. Keep ordinary play separate from
   fixtures which force otherwise rare conditions.
3. Prepare a clear room and corridor, a closed door and wall, stairs, a portal,
   an existing trap, and protected terrain. Use existing level fixtures or a
   disposable wizard test level. Do not modify production level definitions.
4. Record each case as PASS, FAIL, BLOCKED, or NOT RUN with seed, initial state,
   coordinates, HP/status/equipment before and after, turn/action count, and
   messages. On failure preserve the save and shortest reproduction.
5. Where exact RNG outcomes, internal flags, or action counts cannot be observed
   reliably in ordinary play, use the existing native diagnostic harness with
   controlled RNG or a debugger. Manual repetition alone does not prove exact
   probabilities, absent attack slots, or a single enhancement opportunity.
6. Compare donor-inherited fields with the pinned source, not a wiki or the
   current upstream branch: SLASH'EM `fbd743a6081f4a447b9fc2dc545f20c3bc603330`,
   EvilHack `c444f6a3ab1e9f16d0676961dba86f628e91c6ba`, SpliceHack
   `8d70ade6f015c7894c171af691393327983173fe`, dNetHack
   `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`, and UnNetHack
   `439b8d63d3d1ca78fb08588dd43f61874114b21a`.

## Catalogue and ordinary generation

For every row, inspect the generated definition and verify name, glyph,
level/speed/AC/MR, alignment, size, weight, nutrition, sound, color, complete
ordered attacks, resistances, body/behavior flags, and eligibility. Explicit
Step 20 overrides take priority over donor values.

| Monster | Glyph | Min DoD depth | Level / speed / AC / MR | Genocide / polymorph |
|---|:---:|---:|---|---|
| Vampire Mage | V | 50 | 20 / 14 / -4 / 50 | yes / yes |
| Deepest One | h | 50 | 30 / 15 / -5 / 70 | yes / yes |
| Drider | s | 55 | 14 / 15 / 2 / 15 | yes / yes |
| Astral Deva | A | 60 | 18 / 18 / -6 / 90 | no / no |
| Shoggoth | P | 60 | 18 / 15 / -5 / 25 | yes / yes |
| Death Knight | L | 65 | 17 / 9 / -4 / 45 | yes / yes |
| Hound of Tindalos | d | 70 | 14 / 12 / 2 / 0 | no / yes |
| Planetar | A | 70 | 29 / 16 / -10 / 80 | no / no |
| Vorpal Jabberwock | J | 80 | 20 / 12 / -2 / 50 | yes / yes |
| Neothelid | w | 80 | 32 / 12 / 2 / 60 | no / no |
| Gug | Y | 85 | 15 / 18 / 5 / 15 | yes / yes |
| Giant Shoggoth | P | 85 | 36 / 20 / -10 / 50 | yes / yes |
| Void Dragon | D | 90 | 25 / 9 / -10 / 20 | no / no |
| Priestess of Ghaunadaur | s | 95 | 18 / 15 / 2 / 10 | no / no |
| Alhoon | h | 95 | 26 / 9 / -6 / 90 | yes / no |
| Solar | A | 95 | 39 / 16 / -10 / 80 | no / no |
| Juggernaut | q | 100 | 30 / 9 / 7 / 0 | yes / yes |
| Elder Brain | h | 130 | 30 / 12 / 0 / 60 | no / no |

- G01: At 49/50, 54/55, 59/60, 64/65, 69/70, 79/80, 84/85, 89/90,
  94/95, 99/100, and 129/130, test the gate for each affected species.
  Below minimum it fails; at minimum it opens subject to native eligibility
  and difficulty. Eligibility does not guarantee a particular random draw.
- G02: Repeat representative boundary checks in Mines, Quest, Gehennom, and
  another branch. Ordinary selection must exclude all 18. Confirm an ordinary
  random-generation-capable main-DoD special level remains eligible.
- G03: At DoD 150 and 199, confirm no added maximum-depth exclusion. Preserve
  native minimum/maximum difficulty filtering, even when it excludes an older
  Step 20 species from these deeper samples. Vary player level and verify the
  normal native formula rather than a Step 20 override.
- G04: Inspect frequency 1, no group flags, and no donor unique/no-generation
  or branch flags that defeat ordinary eligibility. Sample actual creation
  and distinguish native summons from an unintended group multiplier.
- G05: Genocide each eligible species in an isolated game; it must disappear
  from normal selection. Confirm explicit non-genocidable species remain
  protected. Exercise species and class genocide where applicable.
- G06: Test native polymorph against the table, with deterministic fixtures
  for forms that random polymorph rarely chooses. Confirm explicit wizard
  creation below minimum and in another branch still works. Exercise legal
  native summoning and scripted creation without an ordinary-generation gate.
- G07: Record at least 10,000 seeded ordinary selections at each of DoD 60,
  80, 100, 130, 150, and 199: seed, player level, native difficulty bounds,
  species counts, no-candidate count, and non-Step-20 count. No ineligible
  species may appear; existing native monsters must remain represented.
  There is no required exact percentage distribution.

Drider difficulty validation: verify stored difficulty 16 and its use by native
selection. The pinned donor already has ranged Web and assigns 16. Current
native `mstrength()` would calculate 18 for the adapted three-slot table:
ranged capability (weapon) 1, AC 1, active slots 3, strong weapon slots 2,
poison 2, giving `14 + 9/2`. It does not calculate 17 or add another ranged
bonus for external Web dispatch. The prior literal 17 was not a calculated
result. Test the chosen donor-compatible value directly; do not require all
donor-authored difficulty values to equal the native heuristic.

## Range-2 attacks and action scheduling

Run R01-R07 for Hound and Elder Brain, against both the hero and another
monster. Use ordinary `dochug` scheduling as well as the direct combat path.

- R01: Place the target at each offset (2,0), (0,2), (2,1), (1,2), (2,2),
  then mirrored offsets. Clear line of effect permits exactly one Hound
  `AT_REACH2/AD_DRLI/2d6`, or exactly two Elder `AT_REACH2/AD_DRIN/1d4`
  attacks, in order. Observe native hit and miss outcomes for each slot.
- R02: At distance 3 the special drain sequence is unavailable. A wall or
  closed door between legal distance-2 squares blocks it. Opening the door
  restores eligibility. Check diagonal obstruction using native geometry.
- R03: Trace a complete scheduled attack action. Hound must not also execute
  physical tentacle or attack-array spellcasting; Elder must not execute mage
  or clerical slots. No extra item use or second melee action may accompany
  the distance-2 sequence. Compare Speed 12 energy consumption.
- R04: Give the victim drain resistance, then remove it for Hound. Compare
  native life-drain results. For Elder, test bare head and native protective
  helmet outcomes with controlled RNG, including a miss on either slot.
- R05: Use a cockatrice/Medusa, acid/corrosion passive, and another native
  contact-retaliation target. At distance 2 the attacker must receive no
  touch petrification, brain-ingestion death, or contact passive retaliation.
- R06: Move adjacent. The full array runs normally: Hound drain, physical
  tentacle, spell; Elder drain, drain, mage, clerical spell. Each designated
  drain occurs once. Restore native contact/passive behavior for adjacent
  contact attacks. Use surviving targets so death does not truncate slots.
- R07: Exercise invisible/displaced targets, cancelled attacker, fleeing or
  scared attacker, hero on a steed, and target death/lifesaving during the
  sequence. Native legality must remain intact without extra attacks.

## Shared Web behavior

- W01: At a legal native ranged distance, let Drider target hero and monster
  separately. Force one hit and one miss. There is one ordinary hit roll,
  no direct HP damage, and the attempt consumes the whole attack action,
  including miss and illegal-placement outcomes. No subsequent weapon/bite
  sequence, spell, or bonus action occurs.
- W02: A wall, closed door, invalid alignment, and out-of-range target must
  fail native ranged targeting. Test invisible and displaced targets to
  ensure an apparent target is not treated as an automatic hit.
- W03: A successful legal placement creates one native WEB on the victim's
  square and immediately applies native interaction. Check a susceptible
  target and a naturally web-resistant/escaping target. Use native struggle,
  removal, and escape behavior. No surrounding web field is created.
- W04: Repeat with an existing trap, portal, stairs, and protected terrain
  under the victim. Original feature and state survive unchanged. No status
  or substitute terrain is added when a Web cannot be placed.
- W05: Adjacent Priestess successful kick invokes the same placement and
  interaction. Its ordinary 1d1 kick remains distinct from Web's zero direct
  damage. A missed or cancelled kick does not create a Web; distant Priestess
  cannot Web-kick. Repeat W03-W04 through this caller.
- W06: Adjacent Drider uses its two weapon attacks and poison bite normally.
  Both weapon slots use the same wielded weapon; no offhand attack appears.

## Vorpal Jabberwock regression and applicability

- V01: Use a qualifying, unarmored water dolphin with 200 HP and native
  physical mitigation. Force a successful claw and its beheading trigger
  through the complete monster damage path. Without lifesaving it must die,
  not survive at 50 HP. Inspect the actual death result, not just the fatal
  flag, message, or pre-mitigation damage.
- V02: Repeat V01 with a worn amulet of life saving. It must consume the
  amulet and survive through native lifesaving. Control later attacks so
  a subsequent independent hit cannot obscure this outcome. Also exercise
  the complete hero-target path with applicable physical mitigation.
- V03: Suppress the trigger and confirm ordinary claw damage still undergoes
  ordinary physical mitigation. Bites cannot trigger this property.
- V04: Force qualifying claws separately and together. Each successful claw
  gets one independent 1/40 roll; misses do not. Inspect/control the RNG bound
  rather than claiming probability from a small manual sample.
- V05: Repeat on headless, amorphous, and noncorporeal targets and a cancelled
  attacker. Ordinary applicable damage remains, but no beheading occurs.
- V06: Hero wielding Vorpal Blade is protected; carrying it without wielding
  is not protection. Helmet alone must not become beheading immunity.
- V07: Attack Vorpal Jabberwock with Vorpal Blade and compare native
  Jabberwock-family offensive behavior. Verify shared applicability remains
  correct for other targets and no general critical-hit system was added.

## Species combat and retained abilities

Use both resisted and unresisted targets where applicable. Verify exact dice
and order in the table/fixtures; use playtesting to confirm observable effects.

| Case | Species | Required observation |
|---|---|---|
| M01 | Vampire Mage | Claw DRLI 2d8, bite DRLI 1d8, mage 2d6; flight, unbreathing, regeneration, sleep/poison/drain defense; existing vampire transformations work and return to the right species. |
| M02 | Deepest One | Physical claws 3d6 twice, bite 5d6; swimming/underwater survival, cold/poison defense; no added casting, offhand, promotion, or entourage machinery. |
| M03 | Astral Deva | Weapon PHYS 3d12, weapon STUN 1d4, cleric 3d4; flight, see invisible, native lawful peacefulness and death resistance. |
| M04 | Death Knight | Two weapon PHYS 3d8, cold touch 3d6; native undead/unbreathing/regeneration and fire/cold/sleep/poison defense; no corpse raising or covetous teleport behavior. |
| M05 | Planetar | Two weapon PHYS 4d4, gaze BLND 3d6, claw PHYS 2d8, mage 4d6; native gaze blindness and only native secondary stun; lawful peacefulness, flight, regeneration, see invisible, listed elemental/death defenses. |
| M06 | Solar | Two weapon PHYS 5d4, gaze BLND 5d6, claw PHYS 5d8, mage 5d6; same native gaze and celestial defenses; no added vorpal property. |
| M07 | Neothelid | Acid breath 6d6, two physical tentacles 4d4, two DRIN tentacles 2d4, digestion 6d6; native helmets, engulfing, acid defense, swimming, acidic flesh. |
| M08 | Gug | Weapon PHYS 2d6, two claws PHYS 1d6, hug/holding 1d12, bite PLYS 3d6, theft claw 1d6; cold defense; no imported fear or extra-arm weapon system. |
| M09 | Shoggoth | Claw PHYS 4d8, corrosion touch, acid engulf 4d8, passive corrosion; verify resolved corrosion tuples against donor; regeneration, flow under doors, listed defenses, no splitting after weapon hits. |
| M10 | Giant Shoggoth | Two claws PHYS 5d10, two corrosion touches, passive corrosion, acid engulf 8d10; same retained traits, native teleport capability, no splitting. |
| M11 | Priestess | Two weapon PHYS 1d10, bite CORR 3d8, kick PHYS 1d1 plus Web, cleric 0d8, passive acid 2d12; sleep defense, always hostile. |
| M12 | Alhoon | Weapon PHYS 1d10, cold touch 5d6, two DRIN tentacles 2d1, mage 0d0; native helmets, undead/flight/regeneration/see invisible, listed elemental/drain/death defenses; no Book hunting or covetous warp. |
| M13 | Juggernaut | Single butt PHYS 8d8, gigantic thick-hide animal; no invented charge, guaranteed knockback, trample, boulder/door demolition, or siege behavior. |

For Planetar/Solar, attack using otherwise valid ordinary weapons below +4.
For all three celestials and Alhoon, exercise native death-ray/spell paths
appropriate to hero and monster targets. Do not infer death resistance from
MR or a few resisted random rolls.

## Elder Brain stationarity

- S01: Spawn at a fixed square and approach, retreat, break line of sight,
  and circle obstacles for multiple normal action turns. Coordinates never
  change through voluntary pathfinding. Speed remains 12.
- S02: At distance 2 observe both drain slots. Adjacent, allow both caster
  slots to function. This specifically detects a stationary implementation
  which silently prevents the monster from taking turns.
- S03: Injure/scare it and provide teleport wand, teleport scroll, cursed gain
  level potion, digging wand, and other native escape options. Exercise
  item selection with controlled RNG; it must not initiate relocation.
  Place stairs, escape traps, and polymorph traps nearby and test again.
- S04: Exercise native self-teleport and spell selection wherever legal.
  No self-initiated move is allowed. Source-audit alternate self-relocation
  paths that ordinary gameplay cannot force reliably.
- S05: Externally teleport/relocate it using a legal native effect. It may
  arrive elsewhere, then remains stationary there while retaining actions.
  Verify no change to other stationary or ordinary mobile monsters.

## Equipment, enhancements, and corpse policy

- E01: Force Priestess armor-selection rolls across all ten outcomes.
  Preserve donor branches: 9-10 crystal plate mail, 6-8 cloak of protection,
  1-5 omit unsupported consort suit. Every outcome also supplies one ordinary
  crystal sword. Exclude later unrelated loot when evaluating this branch.
- E02: Trace crystal sword construction through normal object generation.
  Assert the existing enhancement creation hook runs exactly once and that
  eligibility/exclusion rules remain unchanged. Test an eligible enhanced
  outcome and a plain outcome. No artifact conversion or donor-only object
  properties are permitted. A plain sword alone does not prove no double roll.
- E03: Force Astral Deva shield roll outcomes: one of four reflection, three
  of four standard shield. Planetar and Solar always receive reflection.
  All shields are uncursed and erosion-proof; no added shield enchantment roll.
- E04: Force each celestial sword enchantment 0, 1, 2, 3. Every sword is
  blessed, erosion-proof, ordinary, and subject only to existing enhancement
  generation. No automatic Sunsword, Demonbane, or other artifact.
- E05: Check Death Knight's permitted martial loadout and Alhoon's donor
  weapon probability/quarterstaff branch. Alhoon must not create Magicbane.
  Drider retains only permitted donor items/substitutions; no drow family.
- E06: Count weapon inventory and actual attacks for repeated WEAP species.
  Slots reuse the wielded weapon, never an extra offhand or bonus inventory.
- C01: Kill each species with a corpse-preserving method. Astral Deva,
  Planetar, Solar, Void Dragon, Alhoon, Juggernaut, and Elder Brain leave no
  corpse. Check native vampire/undead behavior for Vampire Mage/Death Knight.
- C02: Eat permitted corpses under controlled initial intrinsic state and
  RNG. No permanent intrinsic is granted, including teleport/teleport control
  from innate capabilities. Specifically test Deepest poison resistance,
  Priestess sleep resistance, and Gug Strength. Preserve normal nutrition,
  poisonous Drider, acidic Shoggoths/Neothelid, harmful/temporary effects,
  and applicable undead handling. Repeat through native tin handling where
  a corpse can be tinned.

## Void Dragon and Step 19 protection

- D01: Verify cold breath 4d6, physical bite 3d8, physical claw 4d4, exactly
  one disintegration claw 2d4, in that order. Use native disintegration
  resistance and worn-equipment layers to verify native consequences, not
  a damage-only substitute. Check cold/stone/disintegration defense,
  flight, tunneling, unsolid and unbreathing traits.
- D02: Confirm no corpse, scales, baby, egg/hatching membership, DSM, or
  Forge recipe. Test APIs/mappings directly as well as repeated kills.
- D03: Enumerate every native and Step 19 dragon family: baby-to-adult,
  adult-to-baby where applicable, egg/hatching, scales, DSM, armor conversion,
  wish parser, polymorph/breed mapping, and Forge mapping. Compare logical
  results against completed Step 19, not raw enum numbers. Void must be
  outside every applicable contiguous/index arithmetic range.
- D04: Check native and Step 19 dragon minimum depths, corpse intrinsics,
  scale-drop probabilities, and Scales-to-DSM conversion remain unchanged.
  Force probability branches; do not infer rates from a small kill sample.
- D05: Run Step 19 Nightmare, Beholder, Vecna/rewards (Eye, Hand,
  Sacrificial Knife), Nighthorn, and gemstone-golem scenarios. Verify behavior,
  exact object identities, and rewards remain unchanged after enum insertion.

## Integration, compatibility, and completion

1. Run `py -3 -B test/test_step20_source.py` and
   `py -3 -B test/run_step15.py --out _qa/step20-validation`. Retain build,
   runtime, and corpus logs; inspect individual Step 20 fixture results.
   Repeat affected tests after fixes. Run relevant existing Step 19,
   enhancement, vampire, polymorph/genocide, object lookup, and persistence
   suites using their documented runners. Do not weaken historical guards.
2. Run source/definition checks against pinned donors and resolved semantic
   tuples, plus catalogue class-contiguity/index checks. Verify no rejected
   candidates, new objects/artifacts, save fields, donor-only systems, or
   unrelated gameplay changes entered the final diff. Existing Storm Giant
   remains unchanged.
3. Confirm `EDITLEVEL` is exactly 16, with only the 15-to-16 bump. Save and
   restore a fresh Step 20 game containing representative monsters and
   enhanced equipment; native recovery/bones paths should preserve identities
   where applicable. Pre-Step-20 save migration is not required.
4. Build `sys/windows/vs/NetHack.sln`, target `NetHack`, with
   `/p:Configuration=Release /p:Platform=x64`. Locate MSBuild via the installed
   Visual Studio tools. Serialize builds sharing generated headers. A
   diagnostic executable is not the authoritative production-build proof.
5. Smoke-test the final production executable and its generated resources,
   monster names/glyphs/tiles, wishes, level transitions, and normal game start.
   Run `git diff --check`. Verify `doc/step20.md` and the active roadmap match
   the implemented design and include sampling/build evidence.
6. Record final results by case ID. List any blocked/not-run cases explicitly
   with reason. Only mark Step 20 complete when the required automated gates,
   generation sampling, documentation, and authoritative Release x64 build
   pass, or the specification's genuine external build blocker is documented.
