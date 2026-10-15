My-NetHack modification notice, added 2026-10-15.
Local project integration/modifications; original authorship is retained.
Recorded project change dates (Git author dates): 2026-10-15.
See doc/license-compliance.md and doc/my-nethack-modifications.json.
Existing copyright, attribution, license and warranty notices still apply.

# My-NetHack — Step 22: Exotic Artifact Properties Expansion
# Target: GPT-6.1 Sol | Reasoning Effort: Medium

## 1. OBJECTIVE AND AUTHORIZATION

Implement, integrate, and fully validate the Step 22 Exotic Artifact
Property Engine for My-NetHack.

The implementation must support all 18 approved artifact properties
across 17 property families, including combat integration, activation,
native lifecycle behavior, persistence, user interfaces, and regression
coverage.

This is an implementation task, not another gameplay-design exercise.
Proceed autonomously through source inspection, implementation,
integration, testing, defect correction, and documentation.

Step 22 is an ENGINE-ONLY milestone.

Use controlled, test-only artifact grants to validate every property,
including multiple simultaneous sources and different activation modes.

Do not assign these properties to production artifacts, introduce new
playable artifacts, or modify the approved powers of the 23 Step 21
artifacts.

Production artifact binding, activation conditions, acquisition,
distribution, rarity, and monster/pet usage remain reserved for a
separate, user-approved gameplay-design stage.

The expected Step 22 outcome is a complete, validated, production-ready
property engine that can subsequently receive approved artifact
bindings without architectural redesign.

Do not stop after analysis or deliver a partial framework as completion.
Continue through actual implementation and native runtime validation.


## 2. AUTHORITATIVE SPECIFICATION

The authoritative gameplay and implementation specification is:

My-NetHack_Exotic_Artifact_Expansion_Scope_v2.1.md

Title:
My-NetHack — Exotic Artifact Properties Expansion
FINAL Consolidated Gameplay and Implementation Scope (v2.1)
Dated October 14, 2026.

This specification incorporates:
- All 18 approved property contracts.
- Historical Issues 95–134 and their supersession decisions.
- Review decisions Q1–Q27.
- Follow-up gameplay approvals F1–F6.
- Technical resolutions T1–T8.
- Cross-property event ordering.
- Native interaction and persistence requirements.
- Mandatory implementation and verification matrices.

All approved gameplay values, probabilities, thresholds, exceptions,
target eligibility rules, event semantics, and native-behavior
requirements in v2.1 are binding.

The summary contracts below emphasize critical implementation
requirements. They do not replace or weaken the full specification.

The complete v2.1 document must be available in the Codex task or
local workspace. If it is missing, locate the exact authoritative
file or report the missing prerequisite before implementation.
Do not reconstruct the specification from older versions.

Authority order:
1. Current explicit user instructions and approved gameplay decisions,
   including supersessions and F1–F6.
2. The final v2.1 consolidated specification.
3. Approved Step 21 gameplay and historical project invariants.
4. Current local repository source and executable tests as evidence
   of actual implementation behavior.
5. Pinned donor implementations as technical references.
6. Otherwise applicable native NetHack 5.0 behavior.

Donor mechanics do not override approved gameplay decisions.

Do not import superseded mechanics such as Mirror Image decoys,
shared duplicate-property proc rolls, universal activation modes,
mana-powered digging/transmutation, or old property probabilities.


## 3. VERIFIED IMPLEMENTATION BASELINE

The Step 21 baseline has already been independently verified.

Local authoritative repository:
E:\Codex\My_Nethack

Branch:
phase1/equipment-enhancement

Verified HEAD:
e61a290dece24a4dbaaaa2f05747da15dc96e36f

Verified EDITLEVEL:
17

Verified project state:
- Step 21 is complete.
- All 23 approved Step 21 artifacts are implemented.
- doc/step21.md records successful implementation and validation.
- Native gameplay and artifact tests passed.
- Historical source/mutation checks passed.
- Seven resource tests passed.
- Historical spell and portal regressions passed.
- Production all-23 artifact inspection and save/reload passed.
- The repository has one worktree.
- Tracked files were unchanged at the time of verification.
- Existing untracked files were preserved.

GitHub status:
The corresponding remote branch is five local commits behind, at
Step 20 commit 7813481f7.

This discrepancy is explained by unpushed local development commits.
It is not an unresolved Step 21 implementation issue.

Execution requirements:

- Use the verified local repository as the implementation authority.
- Perform a lightweight consistency check that HEAD, branch, working
  tree, and Step 21 state have not materially changed.
- Do not repeat the completed baseline investigation unnecessarily.
- Do not replace local code with the outdated GitHub version.
- Do not synchronize GitHub as a prerequisite.
- Do not reset, rebase, merge, force-push, discard user changes,
  or overwrite existing untracked files.
- Do not publish remote changes without explicit authorization.

If the local baseline has changed since verification, evaluate the
actual differences and continue when safe. Escalate only a genuine
incompatible baseline or destructive-operation conflict.

Preserve the completed Step 21 implementation and its historical
regression guarantees.


## 4. STEP 22 SCOPE BOUNDARY

IN SCOPE

Implement:
- Reusable typed artifact-property definitions and grant semantics.
- Independent activation and processing per artifact source.
- Artifact-specific activation-condition support.
- Source-specific and target-specific persistent state.
- All seven offensive properties.
- All four defensive properties.
- All seven utility properties.
- Native combat, spell, trap, terrain, corpse, inventory, and command
  integration.
- Save/reload and legal level-transition handling.
- Accurate source-specific #inspect information.
- Controlled test-only grants with configurable activation and
  actor-permission fixtures.
- Deterministic native gameplay verification.
- T1–T8 technical implementations and runtime validation.
- Historical regression preservation.
- Step 22 documentation and completion evidence.

OUT OF SCOPE

Do not:
- Add production artifact assignments.
- Create new named or spawnable artifacts.
- Modify the approved powers of Step 21 artifacts.
- Change artifact names, item bases, appearance, rarity, pricing,
  acquisition, gifts, wishes, or distribution.
- Invent monster/pet activation permissions for future artifacts.
- Add these properties to ordinary Forge affix generation.
- Modify Forge recipes, sockets, Essence, or artifact compatibility
  without separate approval.
- Introduce unrelated characters, skills, monsters, branches, quests,
  races, roles, or graphics.
- Import unrelated donor gameplay.
- Rebuild existing native systems when a safe, targeted integration
  can satisfy the approved contracts.

Existing Step 21 artifacts remain unchanged in normal gameplay.

Controlled test grants may exercise combinations, different activation
conditions, weapon attribution, and actor permissions without implying
approval of those configurations for production.

Artifact-specific binding data must be cleanly separable from the
shared property engine.

Do not label the entire player-facing artifact expansion as complete
until the separate production artifact-binding stage has been approved
and implemented.


## 5. GLOBAL PROPERTY ENGINE CONTRACT

Implement shared typed definitions for the 18 properties rather than
18 disconnected, source-specific implementations.

Separate:
1. Property definition and behavior.
2. Artifact instance and property grant.
3. Per-property activation condition.
4. Actor permission.
5. Eligible gameplay event.
6. Per-source independent trigger and execution.
7. Persistent resource/status ownership.
8. Inspection and gameplay disclosure.

Each artifact defines the conditions under which each of its granted
properties is active.

Do not impose universal offensive=wielded, defensive=worn, or
utility=carried requirements.

An artifact may grant multiple properties with different activation
conditions. Different artifacts may grant the same property while
using different activation conditions.

The engine must support later explicitly approved hero, monster,
or pet activation permissions without granting those permissions
universally during Step 22.

For each eligible event:
- Snapshot the identities of then-active candidate sources.
- Evaluate captured sources in stable deterministic order.
- Revalidate source existence, activation, and eligibility before
  each individual roll.
- Revalidate current target/effect eligibility before mutation.
- Exclude newly acquired or newly activated sources until a later event.
- Roll independently for every still-eligible artifact.
- Roll different applicable properties independently.
- Execute each successful effect using its approved semantics.
- Preserve earlier effects and native state changes.
- Stop only when the event/target becomes ineligible or an approved
  exception requires stopping.

Never collapse multiple sources into one shared probability roll.

The sole explicitly approved cross-artifact SUCCESS cap is D2
Reactive Teleportation: one successful teleport per low-HP episode.

Other limitations arise naturally from state:
- No remaining wielded weapon to disarm.
- No remaining eligible inventory object to steal.
- No surviving monster to modify.
- No remaining spell after absorption.
- No item destruction remaining to conserve.
- No trap remaining to disarm.
- One preservation application per corpse.
- One distinct muted ability per monster.

These are not permissions to replace independent source rolls with
shared property activation.

Classify combat events so primary attacks, Frenzy damage, elemental
chain hops, and Counterstrike attacks cannot recursively activate
the new property system.

Use native world-turn timing rather than hero command counts,
monster attack counts, or real elapsed time.

Speed-related extra actions must not accelerate timers or recharge.

Maintain native equipment, artifact uniqueness, polymorph, lifesaving,
combat attribution, XP, loot, conduct, resistance, mitigation, shop,
and protected-progression behavior unless an approved property
explicitly overrides it.

Never operate on stale monster/object pointers after death,
polymorph, inventory movement, or transfer.


## 6. REQUIRED PROPERTY CONTRACTS

The full mechanics and exceptions remain authoritative in v2.1.
The requirements here identify the implementation-critical behavior.

### 6.1 Offensive Properties

O1A — Disarming
- 20% independent trigger per eligible landed primary melee hit.
- Remove the surviving defender's wielded weapon using native
  unwield/ownership cleanup.
- Drop it on the monster tile or nearest valid adjacent tile.
- No direct hero transfer.
- Exclude cursed/welded equipped weapons and protected progression
  objects; ordinary unprotected artifacts may be disarmed.
- Resolve after surviving-monster damage/Executioner, after O1B,
  and before O2.
- No disarm after an actual killing blow.

O1B — Enhanced Steal
- 20% independent trigger per eligible landed primary melee hit.
- Uniformly sample one eligible monster inventory entry.
- Transfer the whole selected stack.
- Eligible equipped and unequipped objects include ordinary artifacts.
- Cursed equipped objects are excluded; cursed unequipped objects
  remain eligible.
- Recursively reject an entire top-level container if it contains
  any protected nested progression object (T1).
- If whole-stack transfer increases the hero's current encumbrance
  category, place the intact stack at the hero's feet.
- Preserve BUC, object identity, equipment cleanup, container
  ownership, artifact uniqueness, shop billing, and native hazards.
- Independent successful sources can take different entries.
- Resolve before O1A and O2, only against surviving defenders.

O2 — Polymorphic Strike
- 20% independent trigger per eligible landed primary melee hit.
- Use native polymorph machinery.
- Replacement species must satisfy both:
  new.mlevel <= current.mlevel
  new.difficulty <= current.difficulty
- Preserve native resistance, genocide/extinction, legal generation,
  identity, equipment, and transformation constraints.
- Unique, quest-critical, and protected bosses/NPCs remain immune.
- Construct a finite, native-weighted legal candidate pool (T2).
- An empty legal pool produces a safe no-op.
- Multiple successful sources may polymorph the same defender
  consecutively; every attempt uses the actual current form.
- Genuine species change clears U7 mute but preserves O3 suppression.
- No donor pig-only or instant-kill fallback.

O3 — Anti-Regeneration
- 20% independent trigger per eligible landed primary melee hit.
- Suppress ALL automatic monster HP regeneration for five world turns,
  including intrinsic, native, and passive-equipment regeneration.
- Do not suppress intentional healing, lifesaving, resurrection,
  reanimation, Pw recovery, or unrelated cooldowns.
- One refreshable status per monster; successful reapplication resets
  the five-turn duration.
- Preserve through species change, save/reload, and level migration.
- Off-level recovery must award automatic HP only for unsuppressed
  elapsed ticks (T3).
- New application occurs after O2 and U7.

O4 — Frenzy
- 20% independent trigger per eligible landed primary melee hit.
- Each successful source adds one guaranteed physical damage tick.
- Additional ticks continue at 45% each, maximum three per activation.
- Every successful artifact gets its own bounded sequence.
- Freshly roll ordinary physical damage and applicable native physical
  bonuses/mitigation for each tick.
- No extra to-hit, contact, poison, elemental, special riders,
  offensive property procs, or duplicate kill/XP pipeline.
- Complete Frenzy before O5 eligibility evaluation.

O5 — Executioner
- Requires a landed primary hit causing positive actual HP damage.
- Qualify after native primary/Frenzy/primary elemental damage.
- Ordinary monsters: HP <=25% maximum, independent 20% roll.
- Eligible noncritical uniques: HP <=15%, independent 10% roll.
- Riders and critical/progression targets are immune.
- Successful execution is fatal WITHOUT applying armor/DR again (F4).
- Preserve native lifesaving, kill attribution, loot, and XP.
- Revalidate surviving targets after lifesaving and between sources.
- Do not bypass the native death pipeline.

O6 — Elemental Chain Damage
- 30% independent trigger per eligible artifact source.
- Qualifying intrinsic artifact element: fire, cold, shock, or acid.
- Source must have an actual qualifying native elemental damage
  resolver for the eligible strike.
- Establish eligibility and rolls before potentially lethal primary
  damage.
- A physical killing blow may launch committed chains.
- Select uniformly among an artifact's qualifying elements.
- Every successful source independently rolls its own native
  qualifying elemental damage base (F1).
- Do not fabricate another elemental hit on the original monster.
- Apply legitimate primary elemental damage exactly once.
- Original target is hop 0.
- Chain through up to three additional hostile monsters using legal
  8-way adjacency.
- No branching, repeated monster within a chain, blocked corners,
  solid barriers, or friendly conduits.
- Each next victim is selected uniformly among legal neighbors.
- Hop damage: floor(base_damage * 0.75^hop), minimum 1 for positive
  base damage.
- Full corresponding resistance terminates that chain.
- Secondary hops cause elemental HP damage only; no new procs,
  special statuses, inventory destruction, or Executioner.
- Independent chains may overlap secondary victims.
- A committed chain continues from the original impact location
  after primary-target death where valid.
- Preserve native per-victim kills/XP/loot attribution.
- No automatic Forge-element inclusion.

### 6.2 Defensive Properties

D1 — Counterstrike
- 33% independent trigger after each qualifying hostile adjacent
  monster melee ATTEMPT, hit or miss.
- Hero must survive and possess an eligible attack-capable weapon.
- Every successful source produces one genuine native weapon
  to-hit counterattack.
- A wielded granting weapon counterattacks using itself (F5).
- Main-hand and offhand grants independently use their respective
  weapons; one proc does not replay a dual-wield sequence.
- Nonweapon grant weapon selection remains binding-specific and must
  be explicitly configurable, not globally guessed.
- Preserve native weapon effects, passives, and attribution.
- No secondary property recursion.
- Revalidate adjacency, attacker survival, hero survival, weapon
  availability, and disabling consequences before each riposte.
- Native lifesaving from incoming damage may permit Counterstrike;
  lifesaving during a riposte sequence stops later ripostes.
- Attacker death alone does not cancel previously qualified D3/D2.

D2 — Reactive Teleportation
- Trigger after positive actual HP damage from a deliberately hostile
  direct monster melee hit.
- Hero must survive at <=25% of the active HP maximum after native
  reversion/lifesaving and again when the effect executes.
- Exclude spells, ranged attacks, hazards, DoT, friendly attacks,
  and other unapproved causes.
- Each active source makes a guaranteed native same-level teleport
  attempt in deterministic sequence.
- If a source fails, later sources may attempt.
- First completed teleport ends processing and disarms the global
  low-HP episode.
- Completed same-square teleport counts as success.
- Controlled-target cancellation is failure; retain native fallback
  semantics for invalid destinations.
- Rearm only when active HP rises strictly above 25%.
- Swapping artifacts cannot bypass the episode restriction.
- Respect native teleport legality and destination constraints.
- Resolve after D3.
- Identified #inspect shows Armed/Disarmed and rearming requirements.

D3 — Emergency Summoning
- Same original-damage and current-survival/low-HP eligibility as D2.
- Each ready source rolls 1d3 existing, difficulty-appropriate tame
  monsters using approved native adjustment and NO_MINVENT.
- Place guardians near the hero's pre-teleport location.
- Partial placement succeeds; zero placement starts no window.
- Per-source 30-world-turn window begins only when at least one
  guardian is successfully placed.
- No refresh/resummon during the window.
- Early deaths do not restart or shorten the window.
- Native expiry death uses normal corpse/loot handling and grants
  no hero kill XP.
- A guardian saved from expiry by native lifesaving permanently
  detaches from temporary status and remains an ordinary tame pet.
- Maintain stable per-instance source identity, monster membership,
  saved deadline, exactly-once expiration, and detachment (T4).
- Preserve behavior through transfer, drop, polymorph, migration,
  cloning, off-level time, save/reload, and subsequent summon windows.
- Process overdue off-level expiry before guardian actions.
- Identified #inspect shows Ready/Active/Recovering, linked living
  count, and relevant remaining window.

D4 — Spell Absorption
- 20% independent roll per active source at COMMITTED hostile monster
  spell cast time.
- Evaluate before hit/miss, reflection, Magic Resistance, and
  irreversible spell effects (F2).
- Includes genuine directed hero spells even when they would miss.
- Includes area spells whose initial footprint contains the hero.
- Includes mixed hostile/caster-beneficial casts; absorption cancels
  the complete cast, including caster benefits.
- Excludes nonspell rays/wands, breath, existing hazards, solely
  caster-directed buffs, spells solely targeting others, and
  indirect summons that merely create nearby enemies.
- First successful absorption atomically cancels the entire spell,
  including resulting clouds and status effects.
- Restore Pw once, capped at hero maximum:
  listed level: max(1, level) * ONE d6
  unlisted level: SIX independent d6 rolls (6d6)
- Listed level zero is not an unlisted spell.
- Do not infer spell level from attack damage or caster level.
- Preserve current Step 21 spell reflection/redirection behavior,
  including the Mirrored Mask.
- No double cancellation, duplicated casts, or duplicate Pw recovery.

### 6.3 Utility Properties

U1 — Universal Trap Disarming
- Existing #untrap automatically uses artifact-assisted disarming
  for eligible targets when at least one active source exists.
- Preserve native target selection, discovery/inspection, and
  applicable inspection costs.
- Include known floor/door/container hazards, appropriate magical
  traps, holes/trapdoors, and approved matching self-release.
- Exclude portals, vibrating squares, mandatory connectors,
  and protected progression hazards.
- Per-source success:
  max(20, 100 - 5 * floor((DL - 1) / 5))
- DL1–5 = 100%; DL76–80 = 25%; DL81–200 = 20%.
- All eligible artifact rolls share one native disarm action.
- Assisted failure is safe and allows future retries.
- Invalid/canceled attempts are free; native deliberate inspection
  still has its ordinary cost.
- Self-release must clear native restraints and vision/flight effects
  without removing unrelated restraint.
- Without an active U1 grant, ordinary native #untrap remains unchanged.

U2 — Wall Consumption
- Explicit #invoke using full native Wand of Digging-style behavior.
- Preserve horizontal multi-tile, upward, and downward digging,
  native risks, hardness, terrain, and shop liability.
- Per artifact: 5 maximum charges, initially full.
- Recharge +1 per 100 world turns below full.
- No recharge progress banks at full.
- Further spending while recharging must not reset progress.
- Confirmed use costs one charge and one native action even when
  digging is ineffective but valid.
- Direction cancellation and zero-charge use are free.
- No hero Pw cost.
- Save/reload and equipment/ownership changes preserve resources.
- Identified #inspect reports charge count and recharge progress.

U3 — Progressive Corpse Preservation
- Explicit #apply of the selected artifact.
- Select exactly one eligible fresh corpse in inventory, on the
  hero's tile, or on an adjacent reachable floor tile.
- Corpses inside containers must first be removed.
- Freshness eligibility by native effective age:
  cursed <20, uncursed <40, blessed <60 world turns.
- Reject already-preserved, persistently rotten, or irreversibly
  tainted corpses.
- Preserve one separated unit from a stack.
- One successful application costs one turn and adds one point to
  that artifact's independent proficiency.
- Rank I: 0–19 prior successes, 10x slowdown.
- Rank II: 20–59 prior successes, 20x slowdown.
- Rank III: 60+ prior successes, 30x slowdown.
- The corpse captures the multiplier available BEFORE awarding
  the new progress point.
- Preserve its captured multiplier and one-time lock permanently.
- Ordinary ice adds another 2x slowdown; iceboxes pause aging.
- Persist fractional effective-age remainder without rounding away
  partial aging (T5).
- Reconcile age exactly once through storage, floor, ice, transfer,
  stack separation, off-level time, and save/reload.
- Forbid incompatible state merges and age laundering.
- Preserve native rot/taint, species-specific eating hazards,
  revival, zombification, and icebox timer behavior.
- Ordinary decaying corpses, trolls, zombifiable corpses, and acid
  blobs may qualify; Riders, lizards, and lichens are excluded.
- Failed/invalid/canceled attempts cost no turn/progress.
- Corpse #inspect visibly reports preservation and multiplier.
- Artifact #inspect reports rank and successful-preservation count.

U4 — Consumable Conservation
- Applies to committed native q/quaff-potion or r/read-scroll use.
- Includes ALL potions and scrolls, even high-power items.
- Per-source chance by objects[otyp].oc_cost:
  0–100: 30%
  101–200: 20%
  201+: 10%
- Independent sources roll until one conserves the item.
- The native magical effect must execute exactly once.
- Precisely one ORIGINAL unit otherwise consumed remains.
- Preserve identity, quantity, BUC, dilution, identification,
  ownership, and shop records.
- Cancellation before consumption commitment is ineligible.
- Cancellation of a later target picker after commitment remains
  eligible under the native consumption boundary.
- Every committed use of unpaid merchandise incurs normal native
  use billing exactly once, even when conserved.
- The retained merchandise remains separately unpaid; later
  purchase/return follows native shop semantics.
- Single-unit stacks must work without deleting/recreating a
  phantom item.
- Use an atomic native committed-consumption integration (T6).
- Never duplicate magical effects, physical units, or bill entries.

U5 — Environmental Transmutation
- Explicit #invoke with map-cursor target.
- Chebyshev range <=5, visible terrain, unobstructed LOS.
- Monsters do not obstruct LOS.
- Ordinary pool/moat/bog/puddle water -> ice: 1 charge.
- Ice -> remembered native original terrain: 1 charge.
- Ordinary lava pool -> solid traversable floor: 2 charges.
- Exclude Water Plane WATER, protected walls, drawbridges,
  quest/branch connectors, and other protected terrain.
- Converted terrain remains until a valid world action changes it.
- Preserve exact original water subtype through native external
  melting, explicit reversal, and save/reload (T7).
- Preserve preexisting ice's native thaw history.
- Atomically update map state, occupancy, objects, traps,
  visibility, pathing, and provenance.
- Clean stale provenance after unrelated legitimate changes.
- Per artifact: 10 charges, initially full.
- Recharge +1 per 10 world turns below full.
- No banked recharge at full or progress reset after further use.
- Invalid/blocked/canceled/no-op/insufficient-charge attempts are
  free; successful conversion costs one native action and charges.
- Identified #inspect reports charges and recharge progress.

U6 — Anti-Theft Ward
- Every individually active source guarantees 100% prevention of
  ordinary monster-initiated theft.
- Protect gold, wielded/worn gear, unequipped inventory, ordinary
  artifacts, seductive theft, and theft-associated removal.
- Preserve ownership, equipment state, worn effects, and BUC.
- Do not block genuine scripted quest/progression transfers.
- Do not turn this into universal item preservation, elemental
  inventory immunity, waterproofing, or curse protection.
- Activation is specific to the granting artifact, not a universal
  direct-carry requirement.

U7 — Ability Mute
- Independent 25% chance per eligible landed primary melee hit.
- Select uniformly among DISTINCT eligible special abilities.
- Canonical attack identity: delivery method plus special effect.
- Deduplicate repeated equivalent attack slots and handler paths.
- AD_SPEL, AD_CLRC, and AD_PSON are distinct complete casting channels;
  muting one prevents every spell through that channel.
- Include eligible active effects, contact passives, innate
  teleportation, standalone nonspell summoning, transformations,
  and other discrete supernatural/special-AI actions (F3).
- Canonical special-AI identity: special-action category plus effect.
- Audit every native/imported dispatch and monster-versus-monster
  execution path (T8).
- Suppress only the selected capability.
- Mixed physical/special attacks retain physical damage.
- Pure special attacks may be suppressed completely.
- Preserve unrelated AI, movement, innate resistance, and normal
  attack behavior.
- Ignore monster Magic Resistance and unrelated status saves.
- Initial duration: 2d3 world turns.
- At most ONE distinct ability is muted per monster.
- Further successes refresh the same mute using:
  remaining = max(current_remaining, new_2d3)
- Do not shorten, add durations, or select a second simultaneous mute.
- Eligible noncritical uniques may be affected.
- Riders, quest-critical bosses, and protected NPCs remain immune.
- Monsters without eligible abilities receive no fallback penalty.
- Genuine species change immediately clears existing mute.
- Preserve the struck form's ORIGINAL passive-contact consequences
  for the triggering hit.
- Living cockatrice passives may be muted for subsequent contacts;
  cockatrice corpses remain independently dangerous.
- Persist ability identity and timer across save/level transitions.


## 7. MANDATORY CROSS-PROPERTY EVENT ORDERING

The implementation must preserve v2.1 Section 6's relative ordering.

PRIMARY HERO MELEE

1. Snapshot landed-hit sources, original impact square, contacted
   monster form, pre-existing mute/passive state, and O6 prelethal
   eligibility/resolver information.
2. Apply native physical damage and defenses.
3. Resolve Frenzy physical bonus ticks.
4. Resolve legitimate primary elemental contribution once and
   retain independent committed O6 secondary-chain data.
5. Evaluate Executioner on the surviving target.
6. Resolve Enhanced Steal.
7. Resolve Disarming.
8. Resolve consecutive eligible Polymorphic Strike effects.
9. Resolve Ability Mute on the final surviving form.
10. Resolve Anti-Regeneration on the surviving current form.
11. Execute committed elemental secondary chains, preserving their
    after-primary-death behavior.

Native passive-contact processing may require engine-specific hooks;
the contacted form and pre-existing mute state must remain authoritative
for the original contact.

A successful roll is not permission to modify a dead or transformed
target through stale state.

HOSTILE MONSTER MELEE

1. Resolve incoming native damage, consequences, active HP pool,
   possible reversion, and lifesaving.
2. Record original qualifying positive HP damage and low-HP survival.
3. Process eligible D1 Counterstrikes independently.
4. Recheck current survival and active HP <=25%.
5. Process ready D3 sources and guardian placement.
6. Recheck eligibility and process D2 attempts until first success.

A retaliatory kill does not invalidate prior emergency qualification.
Healing above 25% cancels pending emergencies. Passive damage caused
by the counterattack cannot newly qualify D2/D3.

HOSTILE MONSTER SPELL

1. Classify an actual committed spell and its initial target/footprint.
2. Evaluate each eligible D4 source before irreversible effects.
3. After absorption, end the entire canceled cast.
4. Without absorption, continue the existing native/Step 21 reflection,
   redirection, MR, and spell-effect pipeline unchanged.

All other ordering constraints in v2.1 remain binding.


## 8. TECHNICAL INTEGRATION GATE: T1–T8

T1–T8 are approved scope-level resolutions, NOT previously implemented
or tested code.

Step 22 must implement and validate them against the actual local
post-Step-21 source.

Do not treat their presence in the scope as implementation evidence.

Before significant modifications to each affected subsystem, identify
the actual code ownership, native hooks, persistence representations,
and relevant existing tests.

Pay particular attention to the five highest-risk integrations:

D4 — SPELL INTERCEPTION
Establish a real cast-commitment interception boundary capable of
classifying directly hero-affecting, initial-area, and mixed spells
before any irreversible effects.

Audit all relevant monster spell channels and unusual species-specific
casting paths. Prevent partial cancellation, hazard creation, duplicate
Pw credit, and interference with Step 21 reflection.

U7 — ABILITY REGISTRY
Build an explicit canonical catalogue for normal attack riders,
pure special attacks, contact passives, AD_SPEL/AD_CLRC/AD_PSON channels,
and innate/special-AI actions.

Trace each eligible capability to ALL native and imported execution
handlers. A missing execution-path mapping is a validation failure.

Do not use global monster cancellation or mutable species descriptors
as a shortcut.

U3 — CORPSE AGING
Establish a persistent fractional native-effective-age representation.

Ensure deterministic freshness admission, preservation multipliers,
ice/icebox handling, transfers, merges/splits, and save/reload cannot
reset age or corrupt persistent rot/taint.

Keep native revival and zombification clocks independent.

U4 — CONSUMPTION TRANSACTION
Locate the real native q/r consumption commitment boundary.

Preserve exactly one original otherwise-consumed unit while allowing
the native effect and unpaid-use billing to execute once each.

Correctly handle target-selection cancellation occurring after
consumption commitment.

D3 — GUARDIAN LIFECYCLE
Establish stable source and guardian identity plus saved world-turn
deadlines, independent of inventory position and transient pointers.

Handle early death, migration, polymorph, cloning, off-level expiry,
native death/loot, and permanent lifesaving detachment exactly once.

The other three resolutions remain mandatory:

T1 — Recursively exclude protected nested containers from O1B
     eligibility BEFORE uniform inventory selection.

T2 — Use a finite native-weighted O2 polymorph candidate set,
     honoring current form ceilings and empty-set safe no-op.

T3 — Suppress all automatic HP regeneration, including equipment,
     with correct on/off-level elapsed-time reconciliation.

T7 — Persist U5-generated ice's exact water subtype through reverse
     transformation and external native melting, with safe atomic
     terrain transitions.

T8 — Fully audited ability registry and dispatch coverage.

Use the complete normative T1–T8 register in v2.1 Section 8.6.

A technical source gap must be investigated and resolved when possible
without changing approved gameplay.

If the source demonstrably cannot satisfy an approved requirement,
report the precise conflict and evidence. Do not silently weaken the
contract, omit supported cases, or substitute a different mechanic.


## 9. PERSISTENCE, OWNERSHIP, AND NATIVE LIFECYCLE

Implement persisted state wherever required by the approved mechanics.

At minimum, ensure:

- Individual source artifact identity and activation/grant state.
- U2/U5 charge counts and fractional recharge progress.
- U3 artifact proficiency and corpse preservation/age metadata.
- U5 terrain provenance.
- D2 global low-HP Armed/Disarmed episode state.
- D3 per-source summon windows, guardian links, expiry, and
  lifesaving detachment.
- O3 monster suppression status.
- U7 selected ability identity and duration.
- Native world-turn catch-up and save/reload invariants.

Do not duplicate timers, credit full-capacity recharge, reset progress
through item movement, or give extra elapsed time on reload.

Use existing native serialization and timer conventions when suitable.

Audit the actual local persistent structures before choosing fields.

Advance EDITLEVEL deliberately if the new serialized format is
incompatible. Do not assume 18 merely because Step 21 uses 17.

Backward migration of older development saves is not required under
the established project policy unless a newer authoritative policy
explicitly changes that requirement.

New saves must roundtrip correctly. Document whether Step 22 requires
starting a new game.

Use native fatal/lifesaving, monster ownership, and item transaction
pipelines. Avoid stale IDs/pointers, duplicated objects, double-death
processing, and inconsistent shop inventory state.


## 10. INSPECTION AND USER-VISIBLE BEHAVIOR

Respect existing identification and #inspect conventions.

For identified artifacts, expose applicable property activation rules,
chance, duration, and state.

Specific required displays include:
- D2 Armed/Disarmed and rearming condition.
- D3 readiness, linked guardians, and remaining window.
- U2/U5 charges and recharge progress.
- U3 artifact proficiency and application progress.
- Preserved corpse marker and captured multiplier.
- Relevant existing Step 21 invocation cooldowns.

Do not invent invocation cooldowns for passive properties.

Do not leak unidentified artifact powers or unrelated hidden monster
abilities.

Normal gameplay must not expose controlled test-only grants or menus.


## 11. VERIFICATION REQUIREMENTS

A compilation PASS or static source check does not establish gameplay
correctness for Step 22.

Use deterministic native fixtures and actual runtime validation.

Create a traceable property verification matrix with:
- Property ID and scenario.
- Active-source/activation configuration.
- Relevant production code hook.
- Test command and deterministic seed where applicable.
- Expected behavior and prohibited behavior.
- Actual observed result.
- Multi-source coverage.
- Cross-property coverage.
- Save/reload or level-transition evidence.
- PASS / FAIL / BLOCKED / NOT RUN / N/A status.

Implement tests alongside the affected systems and rerun relevant
regressions after corrections.

MANDATORY T1–T8 EVIDENCE

T1:
Nested and deeply nested protected objects; uniform selection after
eligibility filtering; item identity and ownership integrity.

T2:
Empty and nonempty candidate sets; native weighting; repeated
current-form polymorph; legal bounds; immunity and equipment state.

T3:
Automatic regeneration on/off level, passive equipment effects,
remaining-timer reconciliation, save/reload, and intentional healing
remaining unaffected.

T4:
Partial/zero guardian placement, early death, artifact transfer,
off-level expiration before guardian actions, migration, cloning,
lifesaving, permanent detachment, reload, and no expiry XP.

T5:
Cursed 19/20, uncursed 39/40, blessed 59/60 age boundaries;
10x/20x/30x ranks; fractional remainders; ice and icebox;
split/merge/transfer; rot; independent revival/zombification.

T6:
Single-unit and stacked consumption, high-power scrolls/potions,
precommit versus postcommit cancellation, independent source rolls,
retained identity, unpaid repeated-use charges, purchase/return,
and exactly-once magical effects.

T7:
Each ordinary water subtype, preexisting ice, native external melt,
save/reload provenance, occupied terrain, protected connectors,
and invalid/canceled transitions with no resource consumption.

T8:
Distinct ability registry, repeated attack deduplication, full spell
channel suppression, innate summon/teleport/transform handlers,
passive contact, monster-versus-monster dispatch, imported handlers,
polymorph clearing, and nonshortening timer refresh.

CROSS-PROPERTY HARD GATES

Verify at least:
- O4 + O6 + O5 ordering and thresholds.
- O1B + O1A + O2 ownership and equipment changes.
- O2 + U7 mute clearing, with O3 suppression retained.
- O6 + O6 independent chain bases and propagation after primary death.
- D1 + D3 + D2 original-versus-current low-HP qualification.
- D4 + Step 21 reflection/redirect behavior.
- U1/U5 preservation of quest and progression connectors.
- U3 native freshness/revival interactions.
- U4 exactly-once effect and billing.
- Source removal/activation during a pending event.
- Two or more active sources with different activation conditions.
- Independent source state across save/reload and equipment changes.

Run the full individual-property and cross-property requirements in
v2.1 Sections 9, 9.1, and 9.2, not merely the abbreviated cases above.

HISTORICAL REGRESSIONS

Preserve and validate relevant behavior from:
- All 23 Step 21 artifacts and their approved overlap decisions.
- Step 21 invocation, inspection, spell interaction, and persistence.
- Steps 16–17 Forge, affixes, sockets, quality, and Essence.
- Steps 18A/18B equipment and magical recipe progression.
- Step 19 equipment, dragon/golem behavior, bosses, and artifacts.
- Step 20 high-level monsters, special attacks, and source guards.
- 200-level DoD, Castle/Medusa placement, branch links, portals,
  and protected quest/progression content.

Run:
- Focused native deterministic suites.
- Actual production-code gameplay fixtures.
- Native save/reload and off-level/expiry roundtrips.
- Appropriate long-run randomized combat, corpse, item, and terrain
  stress tests.
- Historical source/mutation assertions.
- Windows Release x64 console and GUI builds.
- Existing tile/glyph/rendering verification.
- Packaging and applicable donor/license checks.

Record any established historical ancillary failures separately.
No newly introduced regression may be disguised as historical.

Do not weaken valid tests or remove meaningful assertions to obtain
a passing result.

Every T1–T8 contract requires actual appropriate native runtime
evidence. Static review alone is insufficient.

Do not claim complete verification with known new FAIL/BLOCKED cases,
missing supported ability handlers, or unexecuted required scenarios.


## 12. EXECUTION POLICY

Resolve repository-discoverable implementation details autonomously.

Inspect relevant callers, native handlers, saved structures, established
test harnesses, and current project conventions before editing the
corresponding subsystem.

Use the smallest safe architecture that satisfies the contracts,
preserving integration coherence across all 18 properties.

Follow the existing engineering milestone sequence where useful:

P0 — Confirm the verified baseline and source/compatibility hooks.
P1 — Implement offensive engine and O1A/O1B/O2–O6.
P2 — Implement defensive D1–D4.
P3 — Implement utility U1–U7.
P5 — Integrate, validate, document, and close Step 22.

P4 in the v2.1 engineering sequence is production artifact binding.
It remains deferred and is NOT authorized as part of Step 22.

The implementation sequence is flexible except where ordering,
persistence, or compatibility requires a specific dependency.

Do not stop at P0 or produce only an implementation plan.

Continue through all in-scope work and correct failures discovered
during relevant testing.

Do not ask for ordinary implementation choices such as internal field
names, source locations, helper selection, fixtures, or representation
when current code and approved semantics provide sufficient evidence.

Escalate only a material unresolved conflict involving approved
gameplay, persistence behavior, destructive operations, inaccessible
prerequisites, or changes outside authorized scope.

Do not invent new gameplay mechanics to resolve engine complexity.

A genuine blocker affecting one subsystem does not justify silently
declaring other unimplemented properties complete. Finish independent
safe work where possible and record the remaining blocker accurately.


## 13. DOCUMENTATION AND LOCAL GIT HANDLING

Update:
- doc/step22.md.
- The current project roadmap/phase document.
- README milestone status.
- Relevant technical and testing documentation.

The Step 22 implementation record must include:
- Actual implementation baseline and final HEAD.
- The 18-property engine architecture.
- Activation/grant design and source-specific state.
- Native integration hooks and donor provenance.
- Save format and EDITLEVEL decisions.
- T1–T8 implementation evidence.
- Full verification matrix and counts.
- Exact reproducible commands/seeds for relevant tests.
- Historical regression status.
- Known limitations and unresolved conflicts, if any.
- Explicit declaration that artifact bindings remain deferred.

Respect existing local changes and untracked files.

Do not synchronize or push to GitHub as an incidental step.

If the established project milestone policy calls for a local commit
or annotated tag, create it only after reviewing the exact intended
changes and satisfying applicable validation gates.

Never stage unrelated user files, discard existing changes, rewrite
history, or claim publication that did not occur.


## 14. DEFINITION OF DONE

Step 22 qualifies for implementation sign-off only when ALL of the
following are demonstrated:

1. All 18 approved property contracts are implemented in real
   production gameplay code.

2. Artifact activation conditions and actor permissions are modeled
   separately from property behavior.

3. Controlled test grants demonstrate different activation modes,
   multiple independent sources, correct per-source state, and the
   approved global D2 exception.

4. All T1–T8 technical resolutions have been implemented and passed
   the relevant native runtime validation gates.

5. Combat, spell, trap, corpse, inventory, and terrain integrations
   preserve the complete approved v2.1 behavior.

6. Cross-property event ordering and native lifecycle invariants hold.

7. Required persistent state survives supported save/reload,
   transfers, world-turn catch-up, and level transitions.

8. All relevant Step 21 and earlier historical regression gates pass,
   with historical ancillary exceptions distinguished from new defects.

9. Release x64 console/GUI builds, relevant tile checks, and packaging
   checks pass as required by current project policy.

10. The complete verification matrix, source attribution, documentation,
    compatibility notes, and project status updates are present.

11. No production artifact assignment, new production artifact, or
    unauthorized change to the 23 Step 21 artifacts has been introduced.

12. The later artifact-binding stage is documented explicitly as a
    separate gameplay approval, not silently implemented.

13. There are no concealed new FAIL/BLOCKED requirements or supported
    mechanics left untested while claiming full sign-off.

Distinguish:
A. Implementation completeness.
B. Native validation completeness.
C. Historical regression status.
D. Deferred production artifact bindings.
E. Any genuine blocker or residual risk.

If an environmental or technical blocker prevents completion, report
the affected contract and evidence precisely. Do not characterize
partial implementation or source-only verification as full PASS.

FINAL RESPONSE

Provide a concise engineering closeout containing:
- Verified starting and final local commit/branch.
- Implemented property engine and affected areas.
- EDITLEVEL/save compatibility decision.
- Property and T1–T8 validation results.
- Historical regression/build results.
- Documentation and local commit/tag status.
- Any unresolved blockers.
- Clear confirmation that production artifact binding is deferred.

Do not describe the entire player-facing artifact expansion as
released until the separate binding/distribution stage is approved
and completed.

Proceed with Step 22 implementation, integration, validation,
and documentation now.

---

## Implementation record (2026-10-15)

The original approved request above is preserved. The implementation,
architecture, scenario verification matrix, reproducible commands and delivery
status are recorded in [step22_implementation.md](step22_implementation.md).
The native capability audit is [step22_ability_registry.md](step22_ability_registry.md).

The authoritative starting local HEAD is
`e61a290dece24a4dbaaaa2f05747da15dc96e36f` on
`phase1/equipment-enhancement`. The final Step 22 commit is its new direct
child, identified by annotated tag `step22-exotic-artifact-engine-complete`
and message `Implement and validate Step 22 exotic artifact property engine`.
No amend, pull, push or history rewrite was performed. The verified
Step 21 baseline and its 23 artifacts were preserved. GitHub was behind local
development, so synchronization was unnecessary.

All 18 property engines are integrated. The current save epoch is
`EDITLEVEL 18`, requiring a new game. Production artifact bindings,
acquisition, rarity and distribution remain deferred to separate approval.
The linked implementation record distinguishes implementation, native
validation, historical regression results and deferred catalogue work.
