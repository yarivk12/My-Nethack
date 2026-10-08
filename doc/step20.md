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

The continuation validation on 2026-10-07 completed the plan using actual
production wizard gameplay, controlled native fixtures, authenticated donor
comparisons, and source guards. The final matrix records 78 PASS, zero FAIL,
zero BLOCKED, and zero NOT RUN: 68 parent/admin cases and 10 separately bounded
subcases. A subcase never substitutes for its parent. Step 20 qualifies for
full validation sign-off against the current uncommitted working tree at
HEAD `2f067f4094eba86e520dd07b7e35a6eff1fcd6ad`.

Detailed commands, hashes, case evidence, limitations, and final Git state are
in `_qa/step20-continuation-20261007/validation-report.md`, `final-matrix.json`,
and `final-provenance.json`. Previous independent evidence under
`_qa/step20-independent-20261007` remains unchanged. No commit, tag, or push
was performed during this continuation.

### Confirmed validation corrections

The post-release native `make_corpse` switch omitted the newly active species.
Eligible monsters could die without ever reaching ordinary corpse creation.
Explicit dispatch cases now preserve native probability and `G_NOCORPSE`
handling. Vampire Mage instead follows native vampire undead conversion and
produces an old HUMAN corpse when the native corpse chance succeeds. Lich
handling and intentional no-corpse species remain intact. The tracked native
regression exercises 32 seeded actual deaths for each species, with native
corpse-policy initialization. Controlled consumption covers complete raw meals
and actual tinning/opening, normal nutrition rounding, poison/acid/temporary
harms, undead taint, and absence of added permanent intrinsics or Gug Strength.

Neothelid stored difficulty is **36**, matching pinned EvilHack. The former
literal 37 had no documented override or local derivation; local `mstrength`
is 39 for the explicitly adapted attack array. The attacks remain unchanged.
Drider remains **16**, with local diagnostic heuristic 18 as explained above.

Tile resource order omitted active Step 19/20 entries, so enum insertion could
display unrelated artwork. `win/share/monsters.txt` and `objects.txt` now
follow current native catalogue order and conditional reservations, retaining
all original pixel bodies and palettes. Missing entries use exact existing
art, not new graphics. Focused checks cover both sexes of all 531 monsters,
all 568 objects, all 36 dragon armor mappings, all 13,069 glyphs, and every
423,680 source RGB pixel in the regenerated bitmap. The GUI executable embeds
that exact bitmap. No object definitions or gameplay identities were added.

The Step 19 epoch test retains historical 15 after reversing only exact
reviewed Step 20 deltas; current Step 20 independently requires 16. The source
projection is anchored to immutable accepted commits, preserves all earlier
guards, and rejects unreviewed edits. The Step 17 native persistence assertion
uses compiled `EDITLEVEL` and rejects the preceding epoch, preserving native
incompatibility without adding old-save migration.

The disposable runner copies required ignored icon/record resources and uses
isolated build directories. Diagnostic crashes were traced to uninitialized
TTY callbacks and stale monster-grid pointers during fixture cleanup. Native
cleanup and initialized callbacks fixed the harness; production breath and
combat paths were not bypassed. Earlier failed logs remain preserved.

### Executed results

- Step 19 source tests: 5 passed; Step 20 source tests: 4 passed.
- Historical source runner: passed exact follow-up, Step 20, Step 19, Step 18B,
  Step 17 and Step 13 through 16C guards, including mutation rejection.
- Step 17 native runtime/persistence suite: passed, including epoch 16 header,
  epoch 15 rejection, restore/recovery/bones and enhancement paths.
- Step 15/19/20 native suite: passed, including fatal Vorpal mitigation,
  native lifesaving, ordinary cancelled claws, Priestess equipment, corpse
  dispatch, and Step 19 bosses/rewards/dragon mappings.
- Fresh isolated controlled build and runtime: exit 0,
  `QA20 COMPLETE failures=0`; full geometry, scheduling, Web, Vorpal,
  retained abilities, stationarity, equipment, consumption and dragon cases.
- All five donor pins authenticated; 247 semantic and 7 integration checks
  passed. Native Elder Brain MM caster dispatch retains native eligibility
  misses; this is not a claim of successful MM spell effects.
- 60,000 seeded ordinary-generation selections passed. The 1,000-level corpus
  produced 137 selections, 135 placements, two no-candidate cases, zero invalid
  placements and zero multiple placements.

| DoD DL | Samples | Non-Step-20 selections | No selection |
|---:|---:|---:|---:|
| 60 | 10000 | 9466 | 0 |
| 80 | 10000 | 8409 | 0 |
| 100 | 10000 | 6788 | 0 |
| 130 | 10000 | 4498 | 0 |
| 150 | 10000 | 4438 | 0 |
| 199 | 10000 | 2214 | 0 |

Sampling uses seed `202000 + depth`, player level 30; native stored-difficulty
filtering remains active. Additional controlled player-level 1/30 tests check
each selection against native difficulty bounds at depths 60/150/199.

The authoritative `sys/windows/vs/NetHack.sln` target `NetHack` built with
Release/x64 after the gameplay corrections and again after final resource
regeneration. `NetHackW`, `recover`, `tilemap`, and `tile2bmp` also built.
Final production normal startup, checkpoint/recovery/restore and DoD level
transitions passed. Actual production gameplay additionally covers creation,
combat, Web struggle/escape, corpses, eating/tinning, swimming and door flow.
GUI resource correctness is proven by mappings, exact pixels and embedded DIB,
not a claim of an executed interactive GUI session.

Seven current resource tests passed. Two older ancillary checker failures are
preserved separately: Step 9A rejects pre-existing Essence artwork and Step
10B uses a stale glyph-count formula predating Forge. Neither is claimed to
pass; current resource proof uses native enum counts, complete bounds and exact
pixels. Existing unrelated `mkmap.c` warnings remain. `git diff --check` passed.

The [validation plan](step20-manual-validation.md) retains its reproducible
case instructions and records the executed evidence layers and final totals.
