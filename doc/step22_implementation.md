My-NetHack modification notice, added 2026-10-15.
Local project integration/modifications; original authorship is retained.
Recorded project change dates (Git author dates): 2026-10-15.
See doc/license-compliance.md and doc/my-nethack-modifications.json.
Existing copyright, attribution, license and warranty notices still apply.

# Step 22 exotic artifact engine: implementation and verification

Status: Step 22 engine implementation and required native validation complete.
All 18 property groups and all eight T1-T8 technical groups pass their native
scenario fixtures. Historical regression gates, production save/relaunch,
resource checks and Release packaging pass. No known in-scope implementation
or validation blocker remains. Production bindings remain deferred. The
original request in `doc/step22.md` is preserved, with this record appended.

## Baseline, persistence and delivery scope

The implementation starts from the local Step 21 commit `e61a290dece24a4dbaaaa2f05747da15dc96e36f`, on branch `phase1/equipment-enhancement`. The completed milestone is the new direct child commit identified by the annotated tag `step22-exotic-artifact-engine-complete`, with commit message `Implement and validate Step 22 exotic artifact property engine`. Resolve its exact SHA with `git rev-parse step22-exotic-artifact-engine-complete^{commit}`. Step 21 was not amended; no history rewrite, pull or push was performed. The baseline includes all 23 approved Step 21 artifacts and its validation record. GitHub's Step 20 view reflected five unpushed local development commits, not missing Step 21 code. Existing unrelated local files, including `.codegraph/`, were preserved and excluded from the milestone commit. The approved Step 22 scope and original request are included as milestone documentation.

The gameplay authority is [the consolidated v2.1 scope](My-NetHack_Exotic_Artifact_Expansion_Scope_v2.1.md). This milestone implements the engine for 18 properties. Production artifact assignments, acquisition, rarity, activation conditions and monster/pet permissions remain deferred to a separately approved catalogue. The production artifact catalogue is unchanged. Controlled native fixtures grant properties through `STEP15_TEST` helpers; these fixture grants are not game distribution.

`EDITLEVEL` is 18 because object, hero and monster persistent state changed. Use a new game for this epoch. There is no claim that epoch-17 saves migrate to epoch 18. Existing native save and level codecs carry the new POD fields, while fixture reload checks verify selected object identities, deadlines, fractional ages and guardian links.

## Engine structure and native integration

`include/exotic.h` defines property identities, per-property actor permissions and activation modes, source snapshots, object state and defensive event state. `include/exotic_combat.h` carries primary contact snapshots, copied striking-object data, committed chains and capability identities. `src/exotic.c` supplies shared enumeration, eligibility, protected-object/actor checks and resource accounting. Its property modules are `exotic_combat.inc`, `exotic_defense.inc`, `exotic_utility.inc` and `exotic_items.inc`.

Sources are artifact instances identified by native object ID. An event captures its then-active candidates in deterministic order. Each source is resolved again before its own roll, so removal or deactivation prevents later participation and newly activated sources wait for another event. Actor permission is explicit for hero, monster and pet ownership. Direct carry, nested carry, wielded, assigned worn slot, manual and invoked modes are represented independently for each property. The engine does not infer production assignments from these modes.

The native integrations preserve the existing action, damage, equipment and persistence pipelines:

- `uhitm.c` and `dokick.c` own hero melee contacts, native physical calculations, per-strike snapshots and post-damage effects. Captured species and pre-existing mute state preserve the triggering contact's passives. Frenzy uses fresh physical rolls; committed chain paths retain the impact square after primary death. Deferred intrinsic elemental resolution preserves the approved combined ordering.
- `mhitu.c` wraps incoming attempted melee subattacks, including initial engulf contact, for Counterstrike and emergency qualification. Counterstrike runs before summoning, which runs before teleportation. Ongoing digestion is distinguished from a fresh contact.
- `mcastu.c` intercepts eligible committed hostile spells before targeting misses and Step 21 defenses. `region.c` tests a newly computed cloud footprint before creating the region. Existing hazards do not acquire a new absorption opportunity.
- `teleport.c` returns native teleport completion, including same-square completion, cancellation and random fallback. The saved D2 episode flag is global to the hero and rearms above 25% active HP.
- `mon.c`, `dog.c`, `makemon.c` and `allmain.c` maintain regeneration suppression, guardian membership, native cloning, saved deadlines and expiry. Migrating guardians receive legal arrival coordinates before native expiry death and loot handling.
- `trap.c`, `lock.c`, `apply.c`, `dig` entrypoints, `eat.c`, `pickup.c`, `read.c`, `potion.c`, `invent.c`, `shk.c` and the terrain paths integrate utility actions, custody transitions, consumption commitment and shop accounting. Saved water provenance supports explicit thaw and native external melting.
- U7 selects one discrete capability identity on one monster. Native dispatch boundaries suppress that identity while retaining ordinary physical components and unrelated capabilities. The source map is [step22_ability_registry.md](step22_ability_registry.md).

The implementation reuses authoritative local native code at the Step 21 baseline. The external donor register in the scope provided research leads, not wholesale imports for this milestone. No new external donor source import is asserted by this report. Existing native and previously imported code retain their existing license/provenance context.

## Evidence register

Logs are under `_qa/step22/`. The following are distinct executions; their case counts overlap and must not be added into a single coverage total.

| Evidence | Observed result and limit |
| --- | --- |
| `final-complete-build.log`, `final-complete-runtime.log` | Full native fixture run reached `PASS Step 15 native runtime fixtures`, including Step 15-21 regressions and all Step 22 suites. Includes final U7 configuration substitutions, retained shapechanger identity, independent innate handlers and off-level regeneration rounding correction. |
| `closure-fourth-build.log`, `closure-fourth-runtime.log` | Final combined historical plus all-Step-22 native run completed with exit 0 and the completion marker. Adds 1,000 kick/natural paired cases, dual-source/interrupted retaliation, polymorph HP/rearm and sequential teleport attempts, 300 genuine beam+Mask casts and real horizontal/up/down/shop digging. Supersedes earlier runtime runs for final validation. |
| `milestone-final-build.log`, `milestone-final-runtime.log`, `milestone-source.log` | Commit-finalization reruns pass the full native historical plus Step 22 suite and frozen source/historical mutation gates. No engine or test behavior changed during finalization. The complete intended diff and file inventory are preserved locally as `milestone-complete.diff` and `milestone-inventory.json`. |
| `expanded-twentyfirst-build.log`, `expanded-twentyfirst-runtime.log` | Expanded Step 22 run completed. Its capability sweep reports 408 native monster tables; the later imported sweep supersedes that count. |
| `registry-imported-build.log`, `registry-imported-runtime.log` | Completed sweep over 531 compiled native monster tables: 450 distinct-slot selections, 171 pure and 279 mixed suppressions, and 165 real spell/gaze/breath/spit/projectile entrypoint cancellations. This is table/dispatch evidence, not proof of every live individual configuration. |
| `status-combat-first-build.log`, `status-combat-first-runtime.log` | 400 actual monster-versus-monster contacts, seeds `232001..232200`: living cockatrice stoning and gelatinous paralysis occur in controls and selected mute suppresses them without global cancellation. Completed fixture marker present. |
| `defense-order-first-build.log`, `defense-order-first-runtime.log` | Actual Aesculapius healing cancels pending emergencies above threshold; acid passive alone cannot create incoming-hit qualification; lethal riposte preserves D3-before-D2 ordering; source invalidation and newly activated-source exclusion. Completed fixture marker present. |
| `historical-source-closeout.log` | PASS for 49 reviewed production deltas and 247 rejected mutation probes, followed by unchanged Step 21 and inherited historical source/mutation gates. Re-freeze and rerun after any further production edit. |
| `release-package-build-5.log` | Final-source Release x64 console, GUI, recover, utilities, data and ZIP package completed successfully. Both executables and `vspackage/nethack-500-win-x64.zip` were produced. |
| `resources-final.log`, `elder-mm-final.log`, `step10b3_3-final.log` | Seven tile/resource checks pass, plus native Elder monster-target casting and Silver Key/Necronomicon portal regressions. |
| `full-save-accepted.log`, `full-save-tour.log` | PASS: all 23 production identities and inspection screens, original object IDs/state, Book page/cooldown, Saddle toggle, all three permanent lights and full native save/relaunch. Before/after JSON and terminal transcripts are preserved in the matching directories. The tour completes every historical assertion using the final production build. |

The full native run additionally records all 22 Step 21 additions through acquisition, inspection, native application/invocation, source removal, Forge exclusion, level reload and selected level transitions. Those checks complement the historical baseline's Silver Key coverage. The final production tour completes all 23 inspection contracts and then native save/relaunch. Earlier unsuccessful driver runs are retained as diagnostics and are not counted as completed runs.

### Property matrix: executed coverage only

Every PASS below records observed native fixture results. The rows group
related scenarios; they are not an assertion that every possible gameplay
cross-product was sampled. Final scope review closed the explicit additional
scenarios listed in the closure record below.

| Property | Executed evidence | Remaining qualification |
| --- | --- | --- |
| O1A Disarming | PASS: actual native unwield/drop, cursed-welded rejection, ordinary artifact eligibility and unique identity (`final-runtime.log`, seeds `231301..231600`). | No claim of exhaustive inventory/attack cross-product. |
| O1B Enhanced Steal | PASS: uniform top-level entry sampling across unit/10,000-unit heterogeneous stacks, capacity-driven floor transfer, ownership, cursed worn/unworn eligibility, ordinary artifacts and nested progression protection; 1,200 seeds `230001..231200`. | Scenario coverage is bounded by the fixture catalogue. |
| O2 Polymorphic Strike | PASS: finite candidate bounds, empty/bounded candidate handling and source snapshots in combat fixtures (`1..500`, `220001..220400` aggregate combat seed ranges). | Do not infer every eligible species/form combination was sampled. |
| O3 Anti-Regeneration | PASS: native regeneration, elapsed-time catch-up and deliberate healing controls; capability/poly state fixtures. | No claim of an exhaustive equipment-regeneration source matrix. |
| O4 Frenzy | PASS: paired actual `hmon` damage; Healer/Rogue/barehand/improper/improvised cases; Rakuyo, two-weapon minimum skill, mounted dual wield and two-handed strength attribution; consumed-source snapshots. Paired suites report 360 and 240 cases (`225001..225560`, `227001..227360`). Final closure adds 1,000 paired actual kicks and polymorphed natural contacts, pure-element/zero-physical exclusions, sleep state and non-idempotent teleport destination controls (`234001..235500`). | Counts are fixture cases, not distinct production builds. |
| O5 Executioner | PASS: exact ordinary/optional-unique 25%/15% boundaries, seeded 20%/10% outcomes, positive-damage gate, DR bypass, native lifesaving and XP; 200 seeds `228001..228200`. | Protected-actor classification remains governed by actual native progression roles. |
| O6 Elemental Chain | PASS: native primary killing blow continuation, secondary inventory survival, exact three-hop falloff/minimum, no fourth hop, resistance termination, friendly-conduit and diagonal-corner exclusions (`221001..221100`, `226001..226600`). | No broad elemental-inventory effects are inferred from HP-only chain tests. |
| D1 Counterstrike | PASS: native hostile adjacent miss attempts, independent retaliation, weapon tools, no recursion, native healing and passive interruption. Final closure adds 200 paired exact main/offhand source events plus native paralysis and lifesaving stopping later ripostes before their rolls (`235001..237500`). | Weapon attribution, one-contact RNG and multiple successful sources are checked directly against native retaliation. |
| D2 Reactive Teleportation | PASS: same-square native completion, controlled cancellation, saved global episode across source swaps, strict rearm above 25%, and emergency ordering. Final closure uses actual polymorph damage at 25% despite healthy human HP; a canceled first source permits a completed second source, and actual rehumanization rearms. | Targeting uses native window callbacks because native cancellation correctly clears the canned-input queue. |
| D3 Emergency Summoning | PASS: native `NO_MINVENT` placement, zero/partial placement, independent deadlines, early death, clone membership, acquired loot, expiry without hero XP, permanent lifesaving detachment, and overdue migration after legal placement. | Native codecs preserve source windows, guardian membership and detached survivors. Production all-23 save/relaunch also passes. |
| D4 Spell Absorption | PASS: all 27 directed named native spells; 600 actual unlisted cold casts with displaced misses/fumble controls (`227001..227300`); initial cloud-footprint cancellation (`228001..228100`); 900 two-source listed-zero/listed-12/unlisted-six-die energy cases. Final closure adds 300 genuine `buzzmu` casts with worn Mirrored Mask, exact 6d6 restoration and native ray/RNG ordering (`238001..238300`). | Absorbed casts do not enter ray/Mask RNG; failed absorption reaches native ray defenses. |
| U1 Universal Trap Disarming | PASS: depth boundaries and 5,400 deterministic one/two/three-source sequential-roll cases; native box/door inspection and discovery behavior, floor traps, safe failures, self-release and protected/unreachable rejection. | No claim that every authored map's connector was exercised in gameplay. |
| U2 Wall Consumption | PASS: charge grant, direction cancellation, actual multi-tile digging stopping at hard rock, valid ineffective charge, downward hardfloor pit/self-restraint and legal hole, upward falling-rock HP risk, protected portal, zero-charge free rejection and no Pw cost. Native shop wall destruction records exact liability and starts unpaid shopkeeper pursuit. | Shared source/recharge/inspection and lifecycle checks establish independent charge/progress state. |
| U3 Corpse Preservation | PASS: BUC freshness boundaries, exclusions/rot, proficiency boundaries 20/60, native decay timers, once-only marker, unit splitting, fractional ice/icebox custody, revival separation and level reload. Also 1,000 seeded custody transitions, seed `230001`. | Custody stress is not a long-run whole-game fuzz run. |
| U4 Consumable Conservation | PASS: independent cost tiers, original unit/stack identities, actual potion healing once, actual charging-scroll late cancellation, initial cancellation, repeated unpaid fees surviving return, and native level reload. | No claim that every potion/scroll effect was individually driven. |
| U5 Environmental Transmutation | PASS: four water subtypes, explicit/external thaw, permanent ice, lava conversion, invalid/protected rejection, saved provenance and 2,000 freeze/thaw stress cycles. | No claim of every occupant/trap/terrain permutation. |
| U6 Anti-Theft Ward | PASS: ordinary theft ward paths and per-source activation/removal checks in utility/source fixtures. | Full scripted-transfer classification is a source audit obligation; this printed runtime summary does not prove every script. |
| U7 Ability Mute | PASS: deduplication, nonshortening refresh, polymorph clearing, original-contact passive semantics, spell-channel cancellation, mixed physical retention, 531-table imported sweep, innate dispatch and actual monster-versus-monster contacts. Final run also passes SEDUCE=0/1 substitutions, prior-hit stun variants and retained vampire shapechanger capabilities in ordinary wolf form. | Native configuration and dispatch evidence is recorded separately from the source audit map. |

The combined O4/O5/O6 Fire Brand fixture also passed 300 paired seeds `229001..229300`: post-Frenzy/intrinsic-element execution thresholds and identical precommitted HP-only chain hops after native execution death.

### Traceability, activation and prohibited effects

All rows use actual native artifacts with test-only grants, not production
catalogue bindings. `sources` iterates all 18 properties through direct and
nested carrying, with explicit hero/monster/pet permission, worn-slot,
wielded/invoked and source-removal/new-activation controls. Property fixtures
use independent direct sources unless the row requires a striking weapon or
explicit manual selection. `lifecycle`, `terrain` and `items` reload actual
native codecs; `sources` checks independent resources and read-only inspection.
The property matrix above supplies observed outcomes and deterministic seeds.

| IDs | Production hook / native operation | Focused suite | Expected / prohibited | Multiple sources and interactions | Persistence evidence |
| --- | --- | --- | --- | --- | --- |
| O1A/O1B | `ex_onhit`, native unwield/drop and inventory transfer | combat | One legal ownership transfer; no protected nested object or duplicate ID | Independent filtering/rolls; steal before disarm and polymorph | Native object identity/ownership assertions; lifecycle grant reload |
| O2 | `ex_onhit`, finite native-species selection / `newcham` | combat | Both current-form ceilings; no unbounded retry or protected transformation | Repeated same-contact transforms; U7 clears and O3 survives form change | Monster status and source reload in lifecycle |
| O3 | `mon_regen`, native `mon_catchup_elapsed_time` | combat | Five world turns; no automatic HP healing or retroactive catch-up | Independent refresh; deliberate healing remains native | Absolute deadline reload and expired catch-up control |
| O4 | Native primary contact snapshot / physical-roll helper | combat | At most three physical ticks/source; no repeated riders/contact/procs | Paired native damage and combined O4/O5/O6 | Snapshots contain copied striking data, not saved dangling pointers |
| O5 | Post-primary/Frenzy/elemental `ex_onhit` phase | combat | Exact thresholds; fatal damage bypasses repeated DR; one native death | Independent source rolls, lifesaving and subsequent chains | Native death/XP/drop assertions |
| O6 | Precommitted native elemental resolver and HP-only hop executor | combat | Three hops, resistance termination; no inventory/status/proc cascades | Independent two-source bases; primary-death continuation | Source/form/impact snapshot identity checks |
| D1 | `mhitu` attempted subattack / native one-weapon retaliation | defense, defense-order | Hit or miss can qualify; no recursive or full dual-wield replay | Source attribution; healing/passive interruption; D3/D2 ordering | Event state is transient; saved source configuration in lifecycle |
| D2 | Defensive event end / native `scrolltele` result | defense, defense-order | Positive incoming melee HP loss and low-HP survivor; one completed escape/episode | Failed sources may continue; D3 uses original position | Global disarm flag, native hero codec and rearm |
| D3 | Defensive event end / `MM_EDOG` + `NO_MINVENT` / native death | defense, lifecycle | Source-specific 30 turns; no top-up, expiry XP or illegal-position death | Independent groups; clone, lifesaving detachment and D2 ordering | Source IDs/windows, membership, migration and acquired loot reload |
| D4 | `castmu`, `buzzmu`, initial cloud creation | defense | Whole eligible committed spell canceled before reflection/MR; one Pw restoration | Sequential sources stop on first success; Step 21 defense ordering | Stateless per-cast decision; source configuration saved |
| U1 | Native floor/door/box `#untrap` entrypoints | utility | Native discovery/confirmation/turn accounting; safe failed rolls | 5,400 one/two/three-source formula cases; protected anchors | No new persistent trap format; native level handling retained |
| U2 | Selected artifact `#invoke`, native `zap_dig` | utility, sources | One selected source; canceled/empty free, valid ineffective spends | Independent 5-charge pools and 100-turn partial recharge | Lifecycle charge/progress reload; drop/transfer clocks |
| U3 | Selected manual corpse action, native age/timer custody hooks | items, corpse-stress | One separated fresh corpse/point; no revival-clock or merge laundering | BUC/rank boundaries, independent proficiency, 1,000 custody transitions | Fractional age, original ID, marker and native timers reload |
| U4 | Native `q`/`r` commitment and `useup` transaction | items | Effect once, original unit retained, no clone or unpaid-use forgiveness | Independent tier rolls stop on first retention; late cancellation | Native retained-stack and per-use bill reload |
| U5 | Selected map cursor, native ice/melt/lava transition | utility, terrain | Range/LOS/charges; exact thaw provenance, no automatic melt timer | Independent pools; protected U1/terrain anchors | All four water subtypes through native save/load; 2,000 cycles |
| U6 | Native theft/seduction/gold/equipment guards | utility, sources | Active ordinary theft blocked; no broad damage immunity or blocked mandatory script | Per-source activation/removal and ordinary effect controls | No separate timer/state beyond saved source grants |
| U7 | Capability catalogue plus actual native dispatch boundaries | combat, registry, status-combat | One uniform distinct capability; no global cancellation or initial-contact immunity | Multiple refreshes, O2 form changes, mixed physical and unrelated powers | Selected identity/species/deadline reload in lifecycle |

For a focused reproduction use `python test/run_step22_native.py --label
UNUSED-LABEL --suite SUITE` with a suite named above. `combat` also executes
the configuration and innate fixtures included by its translation unit.
`all` runs every Step 22 suite; omitting `--suite` also runs historical native
Step 15-21 regressions. No row treats a source audit alone as runtime PASS.

### T1-T8 implementation evidence

| Closure | Evidence observed | Scope of conclusion |
| --- | --- | --- |
| T1 Protected nested contents | O1B native theft fixtures preserve nested protected containers and whole-stack identities. | Observed native ownership and eligibility cases passed. |
| T2 Finite polymorph candidates | Combat fixtures check finite bounds and safe empty/bounded candidate handling. | Finite selection behavior checked; exhaustive species weighting is not claimed. |
| T3 Automatic HP restoration | Native regeneration/catch-up suppression and healing controls passed. | Tested HP restoration paths retain the automatic/deliberate distinction. |
| T4 Guardian lifetime and ownership | Source removal, zero/partial groups, early death, clone counts, native death/loot, no expiry XP, lifesaving detachment, codec reload and overdue migration passed. | Migration expiry uses legal placement before death. Production all-23 save also passes. |
| T5 Fractional corpse age | Freshness, split/marker, native timers, ice/icebox fractions, level reload and 1,000 custody transitions passed. | Tested transitions preserve fractional age and revival separation. |
| T6 Consumption commitment | Actual `q` and `r`, late target cancellation, retained original identities and repeated unpaid fees passed. | No extra magical effect or cloned retained unit in these fixtures. |
| T7 Terrain provenance | Four water subtypes through native reload/external melt and 2,000 freeze/thaw cycles passed. | Exact original subtype and permanent-ice semantics checked for tested terrain. |
| T8 Capability dispatch | All 531 compiled monster tables, 450 distinct-slot selections, 165 real dispatch cancellations, native innate powers and 400 real monster-versus-monster contacts passed. | Final configuration substitutions and retained shapechanger capability tests also pass. |

## Reproduction and evidence boundaries

Run the Windows native fixture driver from the repository root. It finds MSBuild through `vswhere`, builds the actual Release x64 console engine with `STEP15_TEST=true`, directs intermediates into `_qa/step22`, and runs with `NETHACK_STEP15_TEST=1`, `STEP21_TRANSITIONS=1` and deterministic fixture seeds. These are reproduction commands supported by the checked-in driver, rather than a reconstruction of every original terminal invocation:

```powershell
python test/run_step22_native.py --label closure-fourth
python test/run_step22_native.py --label expanded-twentyfirst --suite all
python test/run_step22_native.py --label registry-imported --suite registry
python test/run_step22_native.py --label status-combat-first --suite status-combat
python test/run_step22_native.py --label defense-order-first --suite defense-order
python test/run_step22_source.py
python test/run_step22_release.py
python test/run_step22_save.py binary/Release/x64 _qa/step22/full-save-accepted --state-only
python test/run_step22_save.py binary/Release/x64 _qa/step22/full-save-tour
python test/test_step20_tiles.py --step21
```

Use a fresh label to preserve existing evidence logs. The source gate's `--freeze` is an explicit final-review action, not part of an ordinary test run. Its accepted hashes must be reconciled after any further production edit. Historical test expectations remain unchanged; only exact reviewed Step 22 source deltas are projected to the Step 21 baseline for historical reads. The gate inlines historical Python source-test subprocesses so that this projection is inherited, while historical Git baseline reads retain their original behavior.

The native runtime driver's final completion marker is `PASS Step 15 native runtime fixtures`. A build log alone, partial PASS output or an old run against earlier source is insufficient evidence for final completion.

## Final scenario closure and diagnostic findings

The accepted production save/relaunch and final U7 configuration checks are
complete. A final scope audit identified additional explicit scenarios:
kick/natural Frenzy, simultaneous defensive sources and interruption,
polymorph HP/reversion rearm, absorption with the Mirrored Mask and genuine
spell beams, and upward/downward digging. All passed in the final combined
`closure-fourth` native run. The closure tests changed only fixtures, so the
accepted frozen engine source and final production Release build remain exact.

The final matrix contains 18 PASS property groups and eight PASS technical
groups. These overlap and are not added into a unique scenario count. There
are zero known in-scope FAIL, BLOCKED or NOT RUN groups. Artifact binding is
DEFERRED by explicit scope, not an engine validation failure. Optional GUI
manual playthrough is NOT RUN and is not represented by build/resource PASS.

Earlier fixture failures remain in the QA directory. The corpse command
fixture had overwritten `corpsenm` without recalculating native corpse weight;
the changed random stream exposed an overburdened hero and correct native
action rejection. It now uses `set_corpsenm` for weight and timer consistency.
The sequential teleport fixture now supplies scoped native position callbacks,
because a canceled native `getpos` correctly clears all queued canned input.
Neither fix changes game behavior or weakens the original assertions.

An earlier diagnostic fixture fault came from driving a status/damage path without the real spell initialization context. The replacement fixtures use the actual spell path and the later execution logs above completed. That harness finding does not prove the exact cause of the user's screenshot, and this report does not assert an active production crash. No controlled production crash reproduction establishing that screenshot's cause is recorded here.

The native randomized combat, trap, corpse and terrain tests are deterministic
and bounded, with seeds/counts above. They are not an unlimited fuzz guarantee.
Console gameplay and resource checks complement the GUI Release build; an
interactive GUI playthrough is not claimed. Production artifact bindings and
their later balance/distribution validation remain explicitly deferred.
