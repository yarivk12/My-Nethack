# Step 20: Deep-Dungeon Monster Expansion

Step 20 adds 18 species to the main Dungeons of Doom using existing combat,
equipment, traps, and enhancement systems. It introduces no new objects,
attack enums, save fields, permanent corpse intrinsics, or donor subsystems.
The monster catalogue changes require `EDITLEVEL 16` (Step 19 was 15).

## Sources and precedence

The Step 20 specification overrides conflicting donor fields. Otherwise the
primary donor is authoritative; native mechanics provide the implementation.
Pinned sources are SLASH'EM `fbd743a6081f4a447b9fc2dc545f20c3bc603330`,
EvilHack `c444f6a3ab1e9f16d0676961dba86f628e91c6ba`, SpliceHack
`8d70ade6f015c7894c171af691393327983173fe`, dNetHack
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`, and UnNetHack
`439b8d63d3d1ca78fb08588dd43f61874114b21a`.
The first three pins follow Step 19; dNetHack follows Step 9C and UnNetHack
follows Step 9A. Local inspected copies are under `_qa/step19-donors` and
`_qa/step20-donors`.

## Catalogue and eligibility

All have frequency 1, no groups, no maximum depth, and no donor branch,
unique, or no-generation restriction. Ordinary generation checks actual
main-DoD depth through the existing Step 19 gate; native difficulty and
weighted selection remain active. Explicit creation is not depth-gated.
Genocide and ordinary polymorph eligibility are independent of generation.

| Species | Donor | Glyph | Minimum DL | Genocide | Polymorph |
|---|---|:---:|---:|:---:|:---:|
| Vampire Mage | SLASH'EM | V | 50 | yes | yes |
| Deepest One | SLASH'EM | h | 50 | yes | yes |
| Drider | EvilHack | s | 55 | yes | yes |
| Astral Deva | SLASH'EM | A | 60 | no | no |
| Shoggoth | SLASH'EM | P | 60 | yes | yes |
| Death Knight | dNetHack | L | 65 | yes | yes |
| Hound of Tindalos | dNetHack | d | 70 | no | yes |
| Planetar | SLASH'EM | A | 70 | no | no |
| Vorpal Jabberwock | UnNetHack | J | 80 | yes | yes |
| Neothelid | EvilHack | w | 80 | no | no |
| Gug | dNetHack | Y | 85 | yes | yes |
| Giant Shoggoth | SLASH'EM | P | 85 | yes | yes |
| Void Dragon | SpliceHack | D | 90 | no | no |
| Priestess of Ghaunadaur | dNetHack | s | 95 | no | no |
| Alhoon | EvilHack | h | 95 | yes | no |
| Solar | SLASH'EM | A | 95 | no | no |
| Juggernaut | SLASH'EM | q | 100 | yes | yes |
| Elder Brain | dNetHack | h | 130 | no | no |

## Resolved attack arrays

Notation omits `AT_` and `AD_`; unused trailing slots are `NO_ATTK`.
Ordering is significant. Repeated weapons use the normally wielded weapon.

| Species | Ordered tuples: attack / damage / dice |
|---|---|
| Vampire Mage | CLAW/DRLI/2d8; BITE/DRLI/1d8; MAGC/SPEL/2d6 |
| Deepest One | CLAW/PHYS/3d6; CLAW/PHYS/3d6; BITE/PHYS/5d6 |
| Drider | WEAP/PHYS/2d8; WEAP/PHYS/2d4; BITE/DRST/1d4; separate ranged Web action |
| Astral Deva | WEAP/PHYS/3d12; WEAP/STUN/1d4; MAGC/CLRC/3d4 |
| Shoggoth | CLAW/PHYS/4d8; TUCH/CORR/0d0; ENGL/ACID/4d8; NONE/CORR/0d0 |
| Death Knight | WEAP/PHYS/3d8; WEAP/PHYS/3d8; TUCH/COLD/3d6 |
| Hound of Tindalos | REACH2/DRLI/2d6; TENT/PHYS/4d4; MAGC/SPEL/0d6 |
| Planetar | WEAP/PHYS/4d4; WEAP/PHYS/4d4; GAZE/BLND/3d6; CLAW/PHYS/2d8; MAGC/SPEL/4d6 |
| Vorpal Jabberwock | BITE/PHYS/3d10; BITE/PHYS/3d10; CLAW/PHYS/3d10; CLAW/PHYS/3d10 |
| Neothelid | BREA/ACID/6d6; TENT/PHYS/4d4; TENT/PHYS/4d4; TENT/DRIN/2d4; TENT/DRIN/2d4; ENGL/DGST/6d6 |
| Gug | WEAP/PHYS/2d6; CLAW/PHYS/1d6; CLAW/PHYS/1d6; HUGS/PHYS/1d12; BITE/PLYS/3d6; CLAW/SITM/1d6 |
| Giant Shoggoth | CLAW/PHYS/5d10; CLAW/PHYS/5d10; TUCH/CORR/0d0; TUCH/CORR/0d0; NONE/CORR/0d0; ENGL/ACID/8d10 |
| Void Dragon | BREA/COLD/4d6; BITE/PHYS/3d8; CLAW/PHYS/4d4; CLAW/DISN/2d4 |
| Priestess of Ghaunadaur | WEAP/PHYS/1d10; WEAP/PHYS/1d10; BITE/CORR/3d8; KICK/PHYS/1d1 with Web hook; MAGC/CLRC/0d8; NONE/ACID/2d12 |
| Alhoon | WEAP/PHYS/1d10; TUCH/COLD/5d6; TENT/DRIN/2d1; TENT/DRIN/2d1; MAGC/SPEL/0d0 |
| Solar | WEAP/PHYS/5d4; WEAP/PHYS/5d4; GAZE/BLND/5d6; CLAW/PHYS/5d8; MAGC/SPEL/5d6 |
| Juggernaut | BUTT/PHYS/8d8 |
| Elder Brain | REACH2/DRIN/1d4; REACH2/DRIN/1d4; MAGC/SPEL/0d6; MAGC/CLRC/2d8 |

The Hound's donor vampiric reach uses native life drain. Corrosion uses
native CORR rather than inventing numerical armor damage. Gug holding,
paralysis and theft use native HUGS, PLYS and SITM. Elder Brain uses native
spell sound rather than the donor quest-nemesis sound and its unrelated
quest scripting. No generalized psionics, sanity, factions, wards, sonic
attacks, racial equipment, percentage defenses, or special celestial
weapon-enchantment thresholds are imported.

## Combat and movement integration

At Chebyshev distance exactly 2, Hound and Elder Brain require `clear_path`
and execute only their REACH2 slots. The native hit/effect handlers retain
misses, drain resistance, helmet handling and target-specific behavior.
The special action returns before items, movement and extra spell dispatch.
Distance-2 attacks suppress touching/brain-ingestion petrification and
contact passives; adjacency retains the full native sequence.

Drider uses `lined_up`/`linedup` targeting and one ordinary d20 hit roll.
The attempt consumes the attack action even on miss or rejected placement.
It inflicts no direct damage. The Priestess invokes the same Web helper
after a successful uncancelled kick. That helper accepts ordinary room or
corridor squares without a trap or stairway, constructs one native WEB,
and invokes `dotrap` or `mintrap`. Native escape/removal remains unchanged.

Elder Brain retains speed 12 and uses `M3_STATIONARY` to suppress `m_move`.
Defensive/miscellaneous item selection excludes voluntary stair/trap escape,
digging escape, teleportation and cursed gain-level relocation. Forced
relocation remains available. No global stationary-monster AI is added.

Vorpal Jabberwock claws independently roll 1/40 on successful hits. The
shared body predicate rejects headless, amorphous and noncorporeal targets;
cancellation and a hero wielding Vorpal Blade suppress beheading. Its species
also joins Vorpal Blade's offensive Jabberwock-family check.

The mid-development correction makes a triggered claw's fatal damage equal
to current HP. The existing fatal flag bypasses knockback and ordinary
physical mitigation in monster/hero damage paths, then uses native death
and lifesaving handling. It does not inflate the damage margin. Ordinary
claws continue through normal mitigation.

## Equipment and legacy integration

Celestials receive blessed erosion-proof ordinary long swords, +0 through
+3. All receive proofed shields; Astral Deva uses reflection 25% of the time,
Planetar and Solar always do. Artifact conversion is disabled.
Death Knight receives the donor runesword and plate mail. Alhoon retains
its donor ordinary athame/quarterstaff branch without automatic artifacts.
Priestess uses one `rnd(10)`: 9-10 crystal plate, 6-8 protection cloak,
otherwise no supported branch armor. The excluded consort's suit is omitted.
Every branch grants the existing crystal sword with `mongets`, which calls
`mksobj(TRUE,FALSE)` and the constructor's single enhancement opportunity.
There is no second Step 20 roll or custom object property.
Drider's dedicated donor equipment is excluded dark-elven gear; no
unapproved substitute loadout or probability is invented.

Existing Deepest One, Gug, Shoggoth and Alhoon identities are relocated or
redefined rather than duplicated. Legacy Deepest equipment, extra offhand,
promotion and entourage behavior is removed. Alhoon no longer inherits
the old dNetHack key artifact, special defenses or mind-flayer brain blast.
Vampire Mage joins the native vampire transformation predicates and choices.
Celestials join native death resistance checks. Corpse intrinsic selection
explicitly excludes Step 20, including inherent teleport conveyance; Gug
does not acquire the native giant strength-conveyance flag.

Void Dragon is outside native contiguous dragon ranges and explicit Step 19
armor/baby mappings, has no egg flag, and produces no corpse or scales.
Step 19 native/custom armor mapping fixtures remain part of the runtime run.

## Drider difficulty decision

The stored value is **16**, matching pinned EvilHack `src/monst.c`.
The former 17 was a literal in the temporary catalogue construction script,
not a local calculation or documented Step 20 override. The donor already
includes ranged Web, so its existence does not justify adding one.

Local `src/mondata.c:mstrength` estimates **18**: base 14 plus integer
`(1 ranged + 1 AC + 3 active slots + 2 strong weapon slots + 2 poison) / 2`.
WEAP already satisfies `mstrength_ranged_attk`; the external Web action
does not add another increment. This diagnostic heuristic does not rewrite
the table. `montooweak`/`montoostrong` compare the stored `difficulty` consumed
by `rndmonst_adj`. Neither mechanism supports 17. The specification's donor
precedence therefore selects 16, with a native runtime assertion protecting it.

## Validation

Native fixtures live in `test/test_step20.c`, included by the Step 15 runner.
They cover definitions/eligibility, depth boundaries, six deterministic
10,000-selection samples, range-2 obstruction and contact exclusion, Web
placement, direct stationary movement, Vorpal dispatch, full damage/death/
lifesaving and cancelled-claw paths, and Priestess equipment branches.
The fatal-mitigation regression was verified to fail with its guard removed;
the Priestess test failed before the sword was restored.

Run `py -3 -B test/test_step20_source.py` and
`py -3 -B test/run_step15.py --out _qa/step20-step15`.
The runner includes directly affected Step 19 fixtures and a 1,000-level
generation corpus. Logs remain under the named output directory.

The [manual validation plan](step20-manual-validation.md) covers all 18
monsters, scheduled actions, item/spell relocation, corpse effects,
equipment, generation, Step 19 compatibility and Release acceptance.
It records planned checks, not claims that those manual checks have run.

### Automated results, 2026-10-07

- Three Step 20 source tests passed.
- Step 15/19/20 native fixtures passed, including the new full damage-path
  beheading and Priestess equipment regressions.
- The separately built Step 13 focused combat runner passed its affected
  native hero/monster melee, enhancement, mitigation and knowledge fixtures.
- The 1,000-level corpus produced 140 selections, 138 placements, two
  no-candidate cases, zero invalid placements and zero multiple placements.
- Seeded generation retained ordinary native monsters at every sampled
  depth. Samples use seed `202000 + depth` and player level 30. Difficulty
  filtering can exclude an otherwise depth-eligible monster at high depths.

| DoD DL | Samples | Non-Step-20 selections | No selection |
|---:|---:|---:|---:|
| 60 | 10000 | 9466 | 0 |
| 80 | 10000 | 8409 | 0 |
| 100 | 10000 | 6788 | 0 |
| 130 | 10000 | 4498 | 0 |
| 150 | 10000 | 4438 | 0 |
| 199 | 10000 | 2214 | 0 |

Detailed species counts are in `_qa/step20-step15/runtime.log`; focused
combat logs are in `_qa/step20-combat`. The manual plan remains unexecuted.

The authoritative `NetHack` target of `sys/windows/vs/NetHack.sln` built
successfully with Release/x64 and regenerated `binary/Release/x64/nhdat500`.
The executable is `binary/Release/x64/NetHack.exe`; the build log is
`_qa/step20-release/build.log`. The initial sandbox attempt could not update
existing dependency build-state files; the authorized external-sandbox rerun
succeeded. Existing unrelated `mkmap.c` uninitialized-variable warnings remain.
`git diff --check` passed.
