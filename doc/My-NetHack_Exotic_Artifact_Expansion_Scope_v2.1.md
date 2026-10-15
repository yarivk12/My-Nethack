My-NetHack modification notice, added 2026-10-15.
Local project integration/modifications; original authorship is retained.
Recorded project change dates (Git author dates): 2026-10-15.
See doc/license-compliance.md and doc/my-nethack-modifications.json.
Existing copyright, attribution, license and warranty notices still apply.

# My-NetHack - Exotic Artifact Properties Expansion
## FINAL Consolidated Gameplay and Implementation Scope (v2.1)

**Updated and consolidated:** 14 October 2026  
**Supersedes:** v1.0 and v2.0 consolidated scopes. Integrates Q1-Q27, historical Issues 95-134, and follow-up gameplay approvals F1-F6 plus technical closures T1-T8 on 14 October 2026.  
**Project:** My-NetHack, custom NetHack 5.0, 200-level Dungeon of Doom  
**Required implementation baseline:** Step 21 reported complete and validated at `e61a290de`, save epoch / `EDITLEVEL=17`; these remain reported baseline facts, not newly verified implementation status.  
**Source inspected during this review:** `H:/app/mynethack/YarivNetHack`, branch `phase0/dod-length`, Step 11 commit `7fa6dd11a`. This checkout is not the required Step 21 baseline.  
**Scope status:** Gameplay mechanics consolidated, including follow-up approvals for independent Elemental Chain damage, cast-time Spell Absorption, expanded Ability Mute, DR-bypassing Executioner, and renamed D1 Counterstrike. The reviewed T1-T8 technical issues have normative engineering resolutions, but their code feasibility and runtime correctness have NOT been verified against post-Step-21 HEAD. **Artifact selection, activation conditions, monster/pet usability, distribution, and source-level implementation remain unassigned or unverified.** This is a scope and verification contract, not a claim of implementation or successful tests.

> **Governing principle:** An artifact defines when each of its properties is active. Each active artifact independently evaluates applicable events, makes its own trigger rolls, and executes its effects. The only approved cross-artifact *success* cap is Reactive Teleportation's one successful teleport per low-HP episode. Natural effect-state constraints remain in force. Do not resurrect superseded universal activation requirements or shared-roll rules.

---

## At-a-glance approved property register

| ID | Property | Approved essential gameplay | Multiple active sources |
| --- | --- | --- | --- |
| O1A | Disarming | 20% on eligible primary melee hit; remove wielded weapon to ground | Independent rolls until no eligible wielded weapon |
| O1B | Enhanced Steal | 20%; uniform eligible inventory entry; whole stack; capacity-aware | Independent rolls and transfers of different entries |
| O2 | Polymorphic Strike | 20%; native transform bounded by current form level **and** difficulty | Multiple consecutive transformations allowed |
| O3 | Anti-Regeneration | 20%; block natural HP regeneration for 5 turns | Independent rolls; one refreshable condition |
| O4 | Frenzy | 20%; 1-3 extra physical ticks, 45% continuation | Every success gives its own extra ticks |
| O5 | Executioner | Ordinary <=25% HP / 20% roll; eligible uniques <=15% HP / 10% roll | Each surviving eligible target gets independent source roll |
| O6 | Elemental Chain | 30%; intrinsic fire/cold/shock/acid; at most 3 8-way hops, 75% falloff | Every success has its own chain, even after original death |
| D1 | Counterstrike | 33% on hostile adjacent melee attempt hit **or miss** | Every successful source counterattacks |
| D2 | Reactive Teleportation | Guaranteed attempt if surviving qualifying hostile melee leaves HP <=25% | Each source can try after failures; **one success per low-HP episode** |
| D3 | Emergency Summoning | 1d3 tame guardians; 30-turn window; normal expiry death/loot; lifesaving survivor becomes ordinary pet | Independent sources; window starts only if at least one guardian is placed |
| D4 | Spell Absorption | 20%; fully cancel eligible spell; known level: max(1, level) x one d6; unlisted level: 6d6 Pw | Independent rolls until first absorbs |
| U1 | Universal Trap Disarming | Active source supersedes ordinary disarming; known floor/door/container traps; self-release; safe failure | Independent rolls in one-turn disarm; native deliberate inspection retained |
| U2 | Wall Consumption | `#invoke` full native digging; 5 charges, +1/100 turns | Independently charged manual artifact sources |
| U3 | Corpse Preservation | Fresh-only, one corpse per `#apply`; 10x/20x/30x aging slowdown; ranks at 20/60 successes | Independent artifact proficiency; corpse one-time lock and visible preservation |
| U4 | Consumable Conservation | `q`/`r`; native base-cost 30%/20%/10%; all potions/scrolls | Independent rolls until first conservation |
| U5 | Environmental Transmutation | Range-5 terrain cursor; water<->ice, lava->floor; 10 charges, +1/10 turns | Independently charged manual artifact sources |
| U6 | Anti-Theft Ward | 100% prevention of ordinary monster theft, including equipment/gold | Each active ward guarantees prevention; event ends once blocked |
| U7 | Ability Mute | 25%; one delivery-method-plus-effect ability, initial 2d3 turns; ignores MR | Independent rolls; refresh uses max(remaining, new 2d3) |

**All activation conditions are specified by each artifact's individual design, not by this table or by property category.**

---

## 1. Purpose, authority, scope, and exclusions

### 1.1 Scope inventory

The expansion comprises **17 families / 18 independently defined properties**:

- **Offensive (7):** O1A Disarming; O1B Enhanced Steal; O2 Polymorphic Strike; O3 Anti-Regeneration; O4 Frenzy; O5 Executioner; O6 Elemental Chain Damage.
- **Defensive (4):** D1 Counterstrike; D2 Reactive Teleportation; D3 Emergency Summoning; D4 Spell Absorption.
- **Utility (7):** U1 Universal Trap Disarming; U2 Wall Consumption (rechargeable native Wand of Digging effect); U3 Progressive Corpse Preservation; U4 Consumable Conservation; U5 Environmental Transmutation; U6 Anti-Theft Ward; U7 Ability Mute.

These are **artifact-exclusive properties**, not ordinary randomly generated Forge affixes. O1A and O1B are distinct effects despite belonging to one original proposal family.

### 1.2 Authority and baseline

In descending priority: (1) explicit user-approved gameplay decisions in this review, including later reversals; (2) approved Step 21 behavior and overlap choices; (3) actual current local My-NetHack source at implementation time; (4) pinned donor source for implementation technique; (5) otherwise native NetHack 5.0 semantics. Donor behavior does not override user-approved changes.

The original 11 October v0.1 review document proposed defaults that have since been replaced. Do **not** accidentally implement its original 10%-15% proc suggestions, 20-turn Anti-Regeneration, Mirror Image decoys, mana-powered wall/terrain effects, automatic corpse carry aura, exclusions for powerful scrolls, simple 15%/4-turn Ability Mute, or its global one-roll-per-property rule.

The v2 review inspected Step 11 commit `7fa6dd11a`, including native timers, combat, corpse handling, spells and untrapping. Its findings support feasibility and the native-behavior examples in this scope, but do not verify Step 21 integration. The earlier code/source survey also included a public repository branch that did **not** expose the completed local Step 21 commit. Before coding, check the **actual local post-Step-21 HEAD** and its verified artifacts, `doc/step21.md`, save epoch, test status and any intervening code. Do not assume an old public branch equals current local production state.

### 1.3 Explicitly outside this approval

- No assignment of any new property to any named existing or new artifact; no decisions on artifact names, spawn probabilities, item bases, artifact count, acquisition, price, alignment, role, gift/wish behavior or distribution.
- No changes to any of the 23 completed Step 21 artifacts, their existing powers, overlap decisions, invocation cooldowns, acquisition, or save behavior, unless specifically approved in an artifact-binding pass.
- No artifact-property entries in ordinary equipment affix generation, Forge Add/Reroll/Extract/Imprint, Essence recipes, sockets, salvage, random quality or ordinary item wishes. Later independent Forge compatibility for artifacts, including elemental chain interaction, requires separate approval.
- No new character skills, races, roles, monsters, quests, branches or graphics systems as an incidental consequence of adding these properties. D3 summons use existing monster types.
- No unauthorized user-visible gameplay changes disguised as technical fixes. Source gaps must be recorded, not silently answered by invention.

### 1.4 Definitions

- **Source:** A particular artifact instance granting a property. A source can contribute only while its own *artifact-designed* activation condition is met.
- **Eligible primary melee hit:** Each separately resolved successful direct hero melee strike satisfying the property's hit/target contract. Main-hand, offhand, unarmed, kick and polymorph melee strikes are separate opportunities, even within one player action, when the artifact design grants the property to that strike. A mere landed hit does not bypass post-damage survivor or eligibility rules. Actor permissions for monster-held artifacts are assigned later per artifact, not globally by property.
- **Secondary damage:** Frenzy bonus ticks, elemental chain secondary hops, and Counterstrike counterattacks. These do not themselves trigger new on-hit offensive properties. D1 weapon sources use the weapon that actually granted their own successful Counterstrike; non-weapon sources require explicit weapon selection in the later artifact-binding catalogue.
- **Natural termination:** A later successful effect cannot change an object/event/monster that no longer exists or has become ineligible. This is *not* a shared proc roll or artificial activation cap.
- **Refreshable status:** One timer per target, never additive stacking. O3 refreshes to 5 world turns; U7 retains the greater of its current remaining duration and a newly rolled 2d3 world turns.
- **Protected progression content:** Mandatory quest/branch/invocation objects, bosses, scripted transfers and terrain anchors as specifically applicable to the property. Audit native checks rather than assuming every unique/ordinary artifact is progression-critical.

---

## 2. Global artifact activation, coexistence and event rules

### 2.1 Activation is designed per artifact, per property

Every artifact's design must specify the exact condition that makes **each** of its granted properties active: wielded, worn in a particular slot, directly carried, contained-and-carried (if deliberately designed), invoked/applied, or another explicitly approved condition. One artifact may require different conditions for different properties; two artifacts with the same property may use different conditions.

**No universal rule** says all offensive properties require their source to be wielded, all defensive properties require the source to be worn, or all utility properties require direct carrying. In particular, the earlier default direct-carrying rule for U6, the weapon-only source restriction for U7, and the proposed directly-carried requirement for U4 have been **superseded**. Their event semantics remain: U7 acts on qualifying melee hits, U4 on successful `q`/`r` consumption, and U6 on theft attempts. A source need not itself be the striking weapon unless its own artifact design says so. Properties intrinsically requiring a wielded counterattack (D1) or explicit manual activation (U2/U3/U5) keep those functional prerequisites.

**Actor permissions:** Whether monsters or pets may activate a granted property is an explicit decision for each artifact during artifact design. Neither a universal hero-only rule nor automatic monster symmetry is approved. The hero-facing contracts below do not assign monster permissions; U7/O3 statuses already applied to a monster continue to affect its relevant native behavior, including combat with other monsters.

Artifact state must track activation **per source**; equipment removal or changing a container cannot silently leave a grant active. However, removing/swapping a source must not clear persistent target statuses, reset saved charges/progress or circumvent D2 episode restrictions.

### 2.2 Global default: independent per-artifact evaluation and execution

For every eligible event, snapshot the identities of the then-active candidate artifact sources. Before each source's individual roll, recheck that it still exists, is active for the property, and meets current eligibility. Sources newly acquired or newly activated during the event wait for a later event. Process the captured, still-eligible sources in a stable deterministic order. For O6, eligibility/rolls are established before lethal primary damage as specified below; committed chains retain their explicit continuation rules. Each source performs its **own roll** using the approved per-property probability. Each successful activation executes its whole valid effect. Different properties also roll independently. Never aggregate duplicate sources into one 20%/25%/33% roll merely because they share a property name.

For **n independent equal-chance sources** with a cancel-on-first-success effect, probability of at least one success is `1 - (1-p)^n`. This is a mathematical result, not a permission to replace sequential source evaluation when later events or state updates matter.

Artifact availability and equipment/activation requirements are the approved **balance levers** for duplicate-source strength. A source may be rare or require a costly slot; no invented global stacking penalty compensates for that design.

**Guaranteed effects** also operate per active artifact when meaningful. If the first succeeds in permanently resolving an event (prevented theft, removed trap, canceled spell, preserved consumed item, stolen last eligible object), subsequent attempts stop because there is no remaining valid event/target, not because a global shared source was substituted.

### 2.3 Explicit exception register

**D2 Reactive Teleportation is the one expressly approved independent-source success limit:** sequential attempts by all eligible sources after failures; stop after first successful teleport; no further successful teleport until hero HP rises **strictly above 25% max**. Failure is not consumption of this global episode. Equipping or removing items does not reset it.

Other **existing intrinsic mechanic constraints** are not duplicate-source exclusions:

- O1A has no more wielded weapon to disarm once it has been removed.
- O3 maintains one 5-turn Anti-Regeneration condition per monster; independent successes refresh it.
- D4 has no extant spell event after the first successful absorption.
- U1 has no trap after the first successful disarm; all eligible sources may try during the same one-turn `#untrap` action.
- U3 each corpse can be preserved **once ever**; each artifact keeps independent proficiency.
- U4 once an item is conserved there is no second consumable destruction to prevent.
- U6 once theft is prevented there is no remaining theft event.
- U7 has at most one **distinct muted ability** per monster; later successes refresh that same mute without shortening it: `remaining = max(remaining, new_2d3)`.
- Manual U2/U3/U5 commands explicitly select an artifact; they do not automatically invoke every possessed artifact at once.
- Target death, loss of attack eligibility, native lifesaving, polymorph state changes and object transfers are revalidated between effects.

The prior one-roll/multiple-source limitations approved in Issues 121-126 were **superseded** by this global rule; later Issues 130-134 are authoritative.

### 2.4 Combat process and lifecycle constraints

- No arbitrary combat recursion. Frenzy extra ticks, Elemental Chain secondary hops and D1 Counterstrike cannot themselves retrigger the 18 offensive/defensive procs as new primary attacks; native applicable weapon effects and passive defenses follow the specific property contract.
- Honor native armor, DR, resistances, attacks, lifesaving, polymorph, kill attribution, XP, drops, conducts, shop billing, intrinsic/equipment updates, teleport legality, and scripted progression unless a property specifically overrides one of these.
- Never duplicate artifacts/items, invoke a second death pipeline for one kill, bypass fatal-damage/lifesaving semantics, mutate global immutable species descriptors, or operate on stale pointers after a transformation, item transfer or death.
- Random target lists and probabilities are defined by approved mechanics; deterministic seeded tests must exercise all paths.
- Source-specific conditions and effect/status/charge counters must survive save/reload and legal level transitions. Increment save epoch deliberately if new serialized state changes the format; previous reported save epoch is 17, but **18 is only a candidate**, not a preapproved code value.

### 2.5 Native time and resource initialization

All durations and recharge intervals use **native elapsed game/world turns**, not individual hero commands, individual monster attacks, or real time. Speed may provide extra actions within a world turn without accelerating timers. Time while the game is closed earns no recharge and consumes no duration.

**Command costs remain native:** An action described as costing one turn consumes one normal time-consuming command action under native movement/speed accounting. It does not force an extra world-clock tick for a fast hero. Distinguish these action costs from the world-turn durations and recharge intervals above; multiple source attempts do not multiply the action cost.

- **O3/U7:** use the native once-per-world-turn monster status lifecycle and off-level elapsed-time catch-up convention. Preserve its final remaining tick convention where applicable instead of inventing a new action-count clock. Account for O3 when reconciling off-level natural regeneration.
- **D3:** use saved game-turn deadlines for source windows and guardian expiry. Reconcile elapsed off-level time at native arrival/load entry points; process overdue guardian expiry before the guardian can act. Lifesaving at expiry follows the explicit D3 exception.
- **U2/U5:** use saved charge state and elapsed game-turn recharge progress. Recharge continues while unequipped and through off-level time; transfer and reload do not reset progress.
- **U3:** extend native effective corpse aging/storage handling; ice slows the aging clock and iceboxes pause it. Preservation changes aging, not the speed of unrelated revival/zombification clocks.

**Initial charges:** Newly created U2 and U5 sources start full at **5** and **10** charges respectively. A full artifact banks no recharge credit. Spending the first charge from full starts a fresh interval; spending another while recharging does not restart that interval. Transfer, dropping, equipping or save/reload restores existing state rather than reinitializing it. Reuse native clock/serialization mechanisms without importing unrelated randomized invocation cooldowns, early-use penalties or Pw costs.

### 2.6 Contact and emergency event snapshots

For each primary contact, preserve the defender form actually struck and its pre-existing mute state for that contact's native passive consequences. A new polymorph or mute cannot retroactively remove those consequences. Preserve native passive-on-killing-blow rules and normal interruption of later strikes. Frenzy adds physical ticks, not additional contacts.

D2/D3 use the **currently active HP pool** after the incoming native damage, form reversion and lifesaving have resolved: monster-form HP/max while still polymorphed, otherwise human HP/max. Record whether that incoming hostile melee event caused positive actual HP damage and left the hero alive at <=25%. Both that original qualification **and** being alive at <=25% when each emergency effect executes are required. Healing above the threshold during Counterstrike cancels pending emergency effects; falling below it solely through counterattack passives creates no new D2/D3 qualification. A change to an active HP pool above 25%, including form reversion, rearms D2. Source activation is still checked per Section 2.2.

---

## 3. Full approved offensive-property contracts

### O1A - Disarming

**Trigger:** Independent **20%** roll on each eligible successful primary melee hit from an active source.

**Effect:** Remove the defender's *currently wielded weapon* using native monster equipment/ownership cleanup. The weapon drops at the monster's tile or nearest valid adjacent tile; it is **not transferred directly to the hero**. No armor or jewelry removal by this property. Maintain object identity and native slot/intrinsic adjustments.

**Eligibility and exclusions:** Cursed/welded wielded weapons cannot be disarmed. Ordinary artifact weapons **can** be disarmed if not quest/progression-protected; mandatory quest/invocation/progression items cannot. Hero's attack eligibility and native peaceful/tame/hostile repercussions still apply. Missing weapon, invalid inventory or a now-dead monster means no effect.

**Multiple sources:** Each independently rolls while a qualifying wielded weapon remains. First actual disarm removes that weapon; later attempts find no currently wielded weapon (unless another has actually become wielded through native state changes). No phantom duplicates.

**Interaction priority:** On a surviving defender, Enhanced Steal resolves **before** Disarming (Issue 113). Both are **after** physical/Frenzy/primary-elemental damage and Executioner, but **before** Polymorphic Strike (Issues 117-118). Killing blows do not perform either operation; native lifesaving survival retains eligibility.

### O1B - Enhanced Steal Object

**Trigger:** Independent **20%** roll on each eligible successful primary melee hit from an active source.

**Effect:** Uniformly randomly select **one eligible monster inventory entry** (not weighted by total unit count). Transfer its **whole stack** when selected. Any equipped item must first be correctly unworn/unwielded and have its active worn effects removed. Ordinary artifacts remain genuine unique objects, never cloned.

**Eligible inventory:** Both equipped and unequipped weapons, armor, jewelry, ordinary inventory objects, and ordinary non-protected artifacts. Cursed **equipped** objects are ineligible; cursed **unequipped** objects are eligible. Quest/progression-protected objects and invalid ownership cases are excluded. Applies against hostile, peaceful and tame monsters, with native alignment/relationship/shop consequences.

**T1 protected nested contents (approved):** Before constructing the uniformly sampled list of eligible inventory entries, recursively inspect each container entry using the same quest/progression protection predicate that excludes direct transfers. If any nested object is protected against transfer, exclude the **whole top-level container entry**; never extract, split, or move protected contents to make the container eligible. Ordinary containers and ordinary artifacts are not excluded merely for being valuable. The test for eligibility is read-only and must handle nested storage safely; a selected eligible container retains its complete ownership, nested object identity, stacking and billing invariants.

**Capacity:** If the entire selected stack can enter hero inventory **without increasing the current encumbrance category**, transfer it. Otherwise put the entire intact stack at the **hero's feet**. Do not split a selected stack to optimize carrying capacity. Handle hazardous touch, wield/unwear, billing and stacking safely.

**Multiple sources:** Each artifact separately rolls and can take another eligible entry, using the updated inventory after previous successful thefts. The same item cannot be transferred twice. No extra thefts from Frenzy ticks or chain hops.

**Interaction priority:** Enhanced Steal -> Disarming -> Polymorphic Strike, only after the defender survives the prior damage/Executioner stages. A killing blow yields native death loot, not a special pre-death theft.

### O2 - Polymorphic Strike

**Trigger:** Independent **20%** roll on eligible successful primary melee hits; transformation occurs only if the defender survives preceding lethal stages and native polymorph eligibility/resistance permits.

**Effect:** Use native monster polymorph machinery with legal random replacement species; no Circe pig-only restriction or donor instant-kill fallback. Candidate **must satisfy both** `mons[new].mlevel <= mons[current].mlevel` **and** `mons[new].difficulty <= mons[current].difficulty` (use the current form at every attempt). Honor genocide/extinction, legitimate replacement rules, equipment, identity, HP, hostility, monster properties and persistence. No XP or loot simply for a transformation.

**T2 finite candidate selection:** Form a bounded legal candidate set from the native polymorph selection domain, applying the current form's level and difficulty ceilings alongside all native generation/transform restrictions. Preserve applicable native candidate weighting within that legal set; do not change it into an arbitrary uniform distribution when the native resolver is weighted. If the candidate set is empty, the successful trigger produces **no transformation**, no instant-kill fallback and no repeated sampling loop. Rebuild/recheck the legal set from the actual current form for every further same-hit artifact activation and use the native transition/equipment/identity machinery.

**Immunity:** Unique monsters, bosses and quest-critical/progression NPCs are immune under the approved O2 rule. Native absolute immunities also remain. This is distinct from U7's more permissive noncritical-unique policy.

**Multiple sources:** Multiple successful artifacts may cause **multiple consecutive transformations within one hit** (Issue 132). Each new form is the input to the next attempt; each independently checks immunity, resistance, both level/difficulty upper bounds and state validity. Stop upon death or ineligibility. Never keep a stale pointer to the pre-transformation form.

**Interactions:** Executioner first; Enhanced Steal/Disarming first; O2 next; then Ability Mute applies to the final extant form. Polymorph **clears existing Ability Mute**, but **does not clear existing Anti-Regeneration**. Any new O3 application after transformations uses the final living form.

### O3 - Anti-Regeneration

**Trigger:** Independent **20%** roll on each eligible successful primary melee hit.

**Effect:** Prevent natural/intrinsic/automatic monster HP regeneration for **5 turns** on the target. All monster categories, including undead and constructs, are eligible if they have a relevant regeneration mechanism. Do **not** block deliberate magical/spell/item healing, lifesaving, revival, reanimation, scripted resurrection or unrelated cooldowns. No global level-wide aura. The original 20-turn or Mortality-Dial-global model is superseded.

**T3 regeneration coverage:** The five-world-turn gate applies to every **automatic** monster HP gain, including passive equipment-granted regeneration, whenever a monster status can suppress it. It does not suppress intentional healing actions performed by spells or items, Pw/energy regeneration or unrelated processes. Centralize the HP-regeneration predicate at the applicable on-level and off-level recovery paths: when catching up elapsed off-level time, only unsuppressed elapsed ticks may award automatic HP, never all elapsed recovery retrospectively. Keep the same remaining timer through species changes, save/reload and level migration. A qualifying landed hit does not gain a new positive-damage prerequisite that O3 did not previously require.

**Multiple sources/repeat hits:** Each active source gets its own 20% roll. Each success refreshes the target's **single 5-turn status**; durations do not add. Existing suppression survives polymorph with the current remaining duration, even if its species changes. A new successful hit after polymorph refreshes to 5 turns. Remove after expiry/death as appropriate.

**Interactions:** Resolve new O3 applications after Polymorphic Strike and Ability Mute on a surviving monster. Keep revival and zombification timing wholly native.

### O4 - Frenzy

**Trigger:** Independent **20%** roll per eligible successful primary melee hit, per active artifact.

**Effect:** Each successful proc causes at least **one bonus physical damage tick** against the original target. After the first bonus tick, each further tick occurs with **45% continuation chance**, with an absolute maximum of **three bonus ticks per artifact activation**. These are extra damage ticks, **not separate to-hit rolls**.

**Damage:** Each tick freshly rolls the triggering strike's ordinary native physical damage, including applicable enchantment, strength, skill and native physical bonuses. Unarmed strikes, kicks and natural melee attacks use their corresponding physical damage calculation. A strike with no physical component cannot trigger Frenzy. Apply native physical mitigation separately to each tick. Do **not** repeat elemental damage, poison, special riders, artifact/Forge property procs, automatic fatal effects, steal/disarm, Executioner, polymorph, on-kill callbacks or special-hit recursion. Defender passives occur once for the primary contact, using the form actually struck and its pre-existing mute state.

**Multiple sources:** Every successful artifact gets its **own** bounded 1-3 bonus tick sequence; no universal three-tick cap across all artifacts. Stop processing a tick sequence if the target no longer exists or is invalid. No additional hit/XP/kills from phantom tick events.

**Interactions:** Complete Frenzy bonus damage before primary-target elemental damage and Executioner evaluation. Frenzy damage can bring a survivor into Executioner's HP window but grants no additional Executioner rolls per bonus tick.

### O5 - Executioner

**Trigger/qualification:** A landed eligible primary melee hit must inflict **positive actual HP damage** and leave a valid living target at or below the threshold **after** primary normal damage, all Frenzy bonus ticks, and primary elemental damage. This is a finisher, not a proc against already dead targets.

**Ordinary target:** Remaining HP **<=25% of max**, then an independent **20% execution roll per active source**.  
**Eligible non-progression unique target:** Remaining HP **<=15% of max**, then an independent **10% roll per active source**.  
**Immune:** Riders, critical/progression uniques, quest-critical bosses and protected quest NPCs. Honor any additional native protected boss rules.

**Effect (follow-up F4, approved Option A):** A successful Executioner roll produces a **fatal effect**, without a second armor/DR or ordinary damage-mitigation calculation. Armor, resistances and DR already influenced the original damage that placed the target in its qualifying HP window. The fatal effect still respects native lifesaving, approved execution immunities, and single kill/XP/drop pipelines. Do not merely set monster HP to zero and delete it or bypass lifesaving. If a target survives through native lifesaving, revalidate current HP/eligibility before processing another artifact's opportunity. No double fatal resolution.

**Interactions:** Executioner resolves **before** Polymorphic Strike and inventory manipulation. If it kills the target, O1 theft/disarm and O2/U7/O3 target statuses are skipped; already triggered eligible O6 chain propagation may continue independently.

### O6 - Elemental Chain Damage

**Trigger:** Independent **30%** roll per active eligible artifact on a successful primary melee hit with a qualifying *intrinsic artifact* elemental attack component: fire, cold, shock or acid. Establish eligible source rolls at the landed-hit boundary **before lethal primary damage**, retaining the impact square and required elemental resolver information. A physical killing blow can therefore launch successful chains. If multiple qualifying intrinsic elements exist, select one uniformly for that artifact's chain. Do not automatically include Forge-added elements on artifacts without a separate later approval.

**Geometry:** The struck monster is **hop 0** at the original impact location. Generate one linear chain with up to **three additional hostile monsters** (hops 1-3). Every next victim must be 8-way adjacent to the preceding hop's location (Chebyshev distance 1); cannot connect through blocked corners, solid walls, closed doors or invalid map geometry. Choose uniformly among currently eligible neighbors at each hop. A monster cannot be hit twice within the **same** chain. Peaceful, tame and friendly actors are never targets or conduits. No branching or recursing; the hero and steed are never victims.

**Damage:** Use only the selected elemental component, never copy whole weapon physical damage. At hop `h`, `damage(h) = floor(base_elemental_damage * 0.75^h)`, with minimum **1** when positive base damage would otherwise round to zero. Full corresponding elemental resistance causes no damage and **terminates that chain**. Secondary hops apply elemental **HP damage only**: no additional statuses, item destruction, theft, disarm, Frenzy, Executioner, polymorph, muting, or other procs. Use native per-victim kill/XP/loot attribution.

**F1 independent base damage (approved Option A):** Every successful O6 source determines its own qualifying intrinsic element and obtains an **independent native elemental damage roll** from that source's actual eligible elemental resolver for the committed chain. Different successful sources may therefore have different base values, even when both select the same element. These independent rolls determine each chain's **secondary-hop** damage; activating a chain does not create an additional application of elemental damage to hop 0. The original monster receives only its legitimately applied normal melee-hit elemental contribution, exactly once. Never replay its original roll as a duplicate hit or apply an extra source roll to the primary. Capture enough resolver inputs at the pre-lethal-hit boundary for a chain to roll/propagate even if primary physical damage kills hop 0. If a proposed artifact grant has no qualifying native intrinsic-elemental source for the eligible strike, it must be resolved in that artifact's later binding design, not synthesized during implementation.

**Multiple sources (Issue 134):** Every successful artifact launches its **own** chain with independent element/path/damage calculation. Separate chains may damage the same secondary monster (one-hit-per-chain only). If an earlier chain kills the primary monster, later triggered chains retain the **original impact square** and still seek surviving hostile neighbors, but never damage the dead primary again. A chain already triggered may finish after its primary dies. No new target source monster is invented.

**Interaction with Executioner:** Applicable primary-target elemental damage is part of the damage assessed *before* Executioner's HP threshold. Preserve exactly-once application of intrinsic elemental damage; implementation must not double-count any elemental component already dealt by the native primary-hit resolver. Every chain base elemental damage roll must be sourced independently from the granting artifact's actual native qualifying elemental resolver, without inventing a new dice formula or reapplying damage to the primary target. Chain continuations retain their separately approved after-death behavior.

---

## 4. Full approved defensive-property contracts

### D1 - Counterstrike

**Trigger:** Following **each eligible attempted adjacent melee attack** by an attacking monster, whether it hits **or misses**, every active Counterstrike source makes an independent **33% roll**. The attack must still be a qualifying hostile monster melee action; no response to spells, projectiles, breath, terrain hazards, self-inflicted damage, or incidental non-melee effects. The player must be alive at execution time, including survival through native lifesaving, and must wield an attack-capable weapon.

**Effect:** On each successful source activation, make **one real native weapon to-hit counterattack** against the *still alive, adjacent* attacker. The counterattack includes native physical/enchantment and permitted ordinary elemental weapon effects, with standard defender passives. It does **not** trigger other offensive artifact properties (including Frenzy, Executioner, chain damage and U7), does not recursively trigger Counterstrike and does not artificially repeat global hit/death bookkeeping.

**F5 source-to-weapon attribution (approved):** A successfully activating Counterstrike property granted by a **wielded weapon** performs exactly one counterattack with **that granting weapon**. When the hero dual-wields two D1-granting weapons, the main-hand and offhand sources each independently roll 33%; each successful source attacks with its respective weapon. One proc does **not** replay a complete dual-wield sequence. Other active D1 artifacts still independently roll; if a **non-weapon artifact** may grant D1, its later artifact-binding design must specify the weapon used for that source's counterattack. No global main-hand-only default and no one-counterattack-per-monster-action cap are permitted.

**Multiple sources:** Every successful source produces **its own counterattack**. If three sources all succeed, up to three counterattacks occur, subject to attacker's continuing survival/adjacency. Multiattack monsters offer new eligible attempts and can cause further retaliations during **the same monster action**. The earlier one-success-per-monster-action limit has been explicitly **removed** by Issue 131.

**Interactions:** Incoming attack resolves, then D1 Counterstrike, then D3 Emergency Summoning, then D2 Reactive Teleportation. A retaliatory kill alone does not cancel the original D3/D2 qualification; each effect still requires current low HP and survival under Section 2.6. Recheck the hero's ability to attack, survival, weapon availability, and attacker adjacency/survival before every riposte. Stop the remaining ripostes if a passive kills or disables the hero, removes the required weapon, or causes hero lifesaving during that riposte sequence. This does not prevent D1 from beginning after lifesaving from the original incoming hit. No counterattack after final unsaved death.

### D2 - Reactive Teleportation

**Trigger:** A deliberate attack by a **currently hostile monster** that is actual **melee**, inflicts **positive HP damage**, leaves the hero alive (native lifesaving counts as survival), and leaves current HP **<=25% of maximum**. Excludes attacks by peaceful/tame/friendly monsters, even if provoked; hero/self damage, environmental effects, traps, falls, lava, poison/DoT outside the relevant direct melee hit, ranged/projectile/gaze/breath and hostile spells.

**HP qualification:** Use the active post-resolution HP pool and require both original incoming-hit qualification and low HP/survival at execution, as in Section 2.6. Reversion to healthy human HP does not trigger an emergency and can rearm D2.

**Effect:** Each active source performs a **guaranteed native same-level teleport attempt** when eligible (no additional 15%/chance roll). Respect native no-teleport terrain/levels, legal placement and Teleport Control or random destination. A failed or prohibited teleport changes neither location nor global armed state and must not inflict duplicate damage. The teleport occurs **after D3**; guardians are created near the pre-teleport hero square.

**Approved explicit exception (Issue 130):** Process eligible artifact sources **sequentially** until one teleport succeeds or all attempts fail. Every artifact gets its own attempt after prior failures. The **first successful teleport** stops processing and globally disarms D2 for that entire low-HP episode. No further success until the hero's HP rises **above 25%**, even after artifact swaps/drops. If none succeeds, a later eligible melee attack may initiate another sequence. Do not erroneously let multiple successes teleport the hero consecutively.

**Teleport completion:** A completed native teleport counts as success **even when the destination is the current square**, because native release effects can still occur. Do not use coordinate change as the success test. Canceling controlled targeting is a failed source attempt and allows a later source to try; an invalid chosen destination retains native random-fallback behavior. Prohibited or genuinely failed attempts do not consume the episode.

**UI/state:** `#inspect` on an identified granting artifact must communicate Armed/Disarmed status and the >25%-HP rearming rule. State is saved; disarmed state cannot be circumvented by equipment changes. A source only participates while its artifact-designed activation condition holds.

### D3 - Emergency Summoning

**Replacement:** Mirror Image/artificial decoys remain withdrawn. Use existing native monsters, not phantom targets or new rendered identities.

**Trigger:** A deliberate hostile-monster melee hit causes positive actual HP damage, leaves the hero alive at <=25% of the active post-resolution HP maximum, and the hero remains alive at <=25% at execution. Use the same exclusions and HP/reversion rules as D2 and Section 2.6. Each ready active source triggers automatically, without a proc roll.

**Placement:** Roll **1d3 existing native, difficulty-appropriate tame creatures** per source. Use `rndmonst_adj` and `NO_MINVENT`; do not generate an initial loot inventory. Place as many of the rolled guardians as legally possible near the hero's pre-D2-teleport location. Partial placement is a completed activation, with no queued or delayed top-up. If **none** can be placed, do not start a window; that source remains ready for the next qualifying incoming hit. Do not retry that same source indefinitely within one event.

**Behavior:** Guardians use normal tame pet AI, combat and spells. D2 does not move them automatically. Normal later following and level transitions remain subject to expiry reconciliation.

**Source window:** Once at least one guardian is placed, start that artifact's **30-world-turn summoning window**. Further qualifying hits do not refresh or resummon during the window. Early deaths do not shorten the window. At the original deadline, the source becomes ready for the next qualifying hit even if the hero has remained below 25% HP. Each source has independent ownership, membership and expiry state; two successfully placed groups can provide 2d3 guardians, subject to legal space.

**Expiry death and loot:** At the deadline, resolve each still-linked living guardian through **normal native monster death and corpse/loot handling**. Do not use inventory-destroying disappearance such as `mongone` for ordinary expiry. Acquired ordinary items go through normal death drops; native species-specific corpse chance, no-corpse flags and special remains still apply. A corpse is not guaranteed for every species. Expiry itself grants **no hero kill credit or XP**. Actual combat deaths retain their ordinary attribution and consequences.

**Lifesaving exception:** If native lifesaving saves the guardian from its expiry death, **clear its temporary-summon/expiry status permanently**, detach it from future expiry cleanup, and let it survive with its existing tame status as an ordinary pet. Do not retry expiry death on later turns. This deliberate exception does not restart or extend its source's window. `NO_MINVENT` remains in force at creation; later acquisition of lifesaving is handled normally. Preserve native death/reversion machinery and verify its source-specific survival outcomes without inventing a second fatal pipeline.

**Persistence:** Save source deadlines, guardian links and detachment after lifesaving. Reconcile off-level expiry before an overdue guardian can act. Dropping, removing, transferring or re-equipping the source does not reset a window or accidentally detach ordinary temporary guardians. Deliberately lifesaved survivors remain detached across save/load and subsequent summon windows.

**T4 lifetime and ownership engineering closure:** Track a source artifact by stable per-instance identity and each placed guardian by stable monster identity plus the exact source/window deadline, using native persistence and migration facilities where possible. Do not rely on inventory list position, current equipment slot or a potentially reused monster pointer/ID alone. Early combat death removes that guardian from membership but does **not** terminate or restart the source's 30-turn window. On expiry, reconcile and unlink each temporary guardian exactly once; process off-level overdue expiration **before** any overdue guardian may act. Native death/loot and the no-expiry-XP rule apply; a successful native lifesaving event permanently detaches that one surviving pet, with no second expiry attempt. Artifact destruction/transfer, guardian polymorph, level migration and save/reload must neither reset the deadline nor leak linked monsters. If native cloning duplicates a temporary guardian, clone metadata must not fabricate a fresh 30-turn window or extend the original deadline; audit the exact native clone behavior before selecting the supported link/expiry hook.

**Ordering and UI:** D1 precedes D3; D3 precedes D2. Attacker death during Counterstrike does not alone cancel D3, but healing above 25% or loss of hero survival does. Identified source `#inspect` shows Ready / Active (linked living guardian count and turns remaining) / Recovering (remaining window when none remain). Ordinary pets saved from expiry are no longer counted as temporary linked guardians.

### D4 - Spell Absorption

**Trigger (follow-up F2, approved Option A):** Evaluate at the **committed hostile monster spell cast** boundary, before hit/miss resolution, resulting hazards, reflection and Magic Resistance. A genuine hostile monster spell **directed at the hero** is eligible even if its eventual beam or attack would miss. An area spell is eligible if its **initial affected footprint includes the hero**. Each active artifact independently rolls **20%** in source order until the cast is absorbed or all rolls fail. Exclude non-spell wands/rays, breath, existing hazards, caster-only buffs/healing, spells solely affecting other creatures, and **ordinary summons that merely create hostile creatures near the hero** without a direct spell effect on the hero. A mixed spell that both directly attacks the hero and benefits its caster is eligible; success cancels **all** components, including the caster benefit. Identify targeting/initial footprint before any irreversible cast effect; never roll after a partial effect and attempt to undo it.

**Effect:** The first successful absorption **atomically cancels the entire eligible spell**, including all of its damage, status, inventory and hazard-creation components. Restore Pw once, capped at the hero's existing maximum:

| Spell metadata | Pw restoration |
| --- | --- |
| Native spell level exists, including level zero | `max(1, native_monster_spell_level) * 1d6`: roll one d6 and multiply |
| No native listed spell level exists | **6d6**: sum six independent d6 rolls; range 6-36, mean 21 |

A listed level zero is not a missing level and restores 1d6. Do not infer spell level from caster level or attack damage dice. A listed level 20 would restore `20 * 1d6`, not 20 independent dice; no new level-20 cap is imposed on future native metadata. If the first artifact fails, the next can roll; after success no spell remains for later sources.

**Spell versus resulting hazard:** An eligible directed spell creating a lingering poison cloud can be fully absorbed before creation, preventing the whole cloud. Existing clouds/environmental damage create no later absorption opportunities. A genuine spell delivered through a beam remains eligible; a non-spell beam does not become eligible merely because it shares a damage routine.

**Concrete baseline examples:** In inspected Step 11 source, Fire Pillar has listed level 12 and would restore `12 * 1d6` (12-72 Pw). Asmodeus's direct `AT_MAGC / AD_COLD` spell bypasses the named spell table and would use **6d6**. Its attack damage dice are not a spell level. Reconfirm metadata at the required Step 21 baseline; do not hardcode monster-name exceptions.

**Priority:** Resolve eligible absorption **before reflection and magic resistance**, so if not absorbed the spell continues through normal/native Step 21 defenses. Do not allow a reflected/absorbed spell to act twice. Check the actual current Step 21 Mirrored Mask redirection/reflection pipeline when choosing a safe hook; earlier v0.1 text proposing absorption *after* reflection is superseded.

**Multiple sources:** Two independent 20% rolls give a **36% chance** of at least one absorption, three give **48.8%**. One spell can cancel and restore Pw **only once**. No global shared 20% roll (Issue 121 superseded).

---

## 5. Full approved utility-property contracts

### U1 - Universal Trap Disarming

**Command selection:** Use existing `#untrap`. With no active U1 source, preserve ordinary native behavior. With at least one active source, artifact-assisted disarming **automatically supersedes** the ordinary disarm procedure for eligible targets. Add no artifact-versus-ordinary method question. Keep existing target selection and native inspection/confirmation flow. Possession alone is insufficient when an artifact's activation condition requires something else.

**Targets:** Include discovered/known, reachable floor hazards, ordinary mechanical and hazardous magical traps, pits, spiked pits, holes and trapdoors, plus **trapped doors and boxes/chests**. Exclude magic portals, vibrating squares, mandatory quest/branch connectors and protected progression/scripted traps. Safely seal eligible holes/trapdoors without removing indispensable access. Do not fall back to a different procedure to bypass protection.

**Discovery:** Do not automatically reveal uninspected traps. Preserve native deliberate door/container inspection and its turn accounting. Once a genuine trap is discovered, its disarm attempt uses U1. Containers retain native trap-knowledge state. Doors retain native discovery behavior, including repeated inspection on later attempts where the native implementation has no persistent knowledge field. No new permanent door-trap memory system is approved. Do not set a general native forced-success flag that would also bypass discovery or U1's probability.

**Self-release:** An active source may attempt to disarm the known trap currently holding the hero, provided the hero is otherwise able to perform the action and the target is the actual eligible holding trap. This includes bear traps, webs and pits/spiked pits. Apply the same source rolls, one-turn cost and safe failure. Success removes/seals the trap and releases the hero through native cleanup, including pit vision, map and blocked levitation/flight updates. This does not make lava entrapment, embedded-floor restraint or a buried punishment ball into an eligible trap, and removing an unrelated trap must not clear those states. Protected targets remain excluded.

**Per-artifact success probability:**

`success_percent = max(20, 100 - 5 * floor((DL - 1) / 5))`

DL1-5 = 100%; DL6-10 = 95%; DL11-15 = 90%; DL76-80 = 25%; DL81-200 = **20%**. Use actual current dungeon depth with the approved 200-level DoD convention and verified branch mapping.

**Action cost and failure:** A valid assisted disarm consumes **one native time-consuming action** whether successful or all rolls fail. No Pw, charges or cooldown. Failed assisted disarms never activate the trap, explode a door/container or add punishment. Unlimited later retries are permitted through the native target/discovery flow. Initial cancellation and invalid/unreachable/protected disarm targets cost no turn. A deliberate native inspection still costs its native turn even when no trap is found or the player declines disarming after inspection; that is not a free invalid-disarm action. A combined native inspect/disarm action does not gain an additional turn per source.

**Multiple sources:** Each eligible active artifact independently rolls during the same assisted disarm action. Stop when one succeeds because the trap no longer exists. If all fail, leave trap and any holding state intact. At 20% each, two sources give 36% and three give 48.8% chance of at least one success. Reuse native successful removal and appropriate bookkeeping without native dangerous failure branches.

**Complexity closure:** Q26 door/container support and Q27 self-release are included. The Step 11 source-level assessment found moderate and low-to-moderate incremental complexity respectively over U1, using existing dispatch, discovery and release helpers. No additional subsystem or saved fields appeared necessary for these extensions. Confirm integration against Step 21; this assessment is not a completed implementation or gameplay-test result.

### U2 - Wall Consumption (native Wand of Digging equivalent)

**Replaces:** The original mana-powered bump-to-excavate concept is **superseded**. U2 is an explicitly invoked **full native Wand of Digging-style effect**, not a single adjacent-wall dig. No hero Pw is charged.

**Command:** `#invoke` the chosen artifact; if multiple invocations exist on the artifact, select its digging ability through the appropriate menu; then choose a direction. Perform native `zap_dig()` behavior: horizontal multi-tile digging; downward digging producing native pits/holes when legal; upward digging and its possible falling-rock consequence. Honor native level hardness, protected walls, shop liability, geometry and all ordinary digging risks.

**Resource model (per artifact):** Maximum **5 charges**, initially full for a newly created source. No recharge credit accumulates while full; spending from full starts a fresh 100-world-turn interval. Sequential regeneration of **one charge every 100 turns** while below full, without resetting the countdown for previously progressing recharge when another charge is spent. Regenerates when not worn/wielded. Charges and partial recharge progress persist across saving/loading, inventory transfer and legal level transitions.

**Cost:** A confirmed invocation consumes **one turn and one artifact charge**, even if the native dig effect changes nothing because the direction/terrain is ineffective or blocked. Canceling direction selection is free. At zero charges there is no effect, no spent turn and no negative-charge or Pw behavior. No cross-artifact charge borrowing; invoking one artifact changes only its charge pool.

**UI:** Identified source `#inspect` shows charge capacity/current charge count and time to next recharge; follow existing rules for presenting invocation powers without leaking unidentified abilities.

### U3 - Progressive Corpse Preservation

**Nature:** A manually applied artifact technique, not a passive aura, character skill or generic immunity to spoiled food. No Pw, charges or cooldown.

**Command and quantity:** Explicitly `#apply` the chosen artifact, then select **one fresh, unpreserved eligible corpse** directly in inventory, on the hero's tile, or on an adjacent reachable floor tile. A corpse in a bag or icebox must first be removed. If selected from a stack, separate **exactly one unit**, preserving total quantity. One successful application costs one game turn and earns one progress point. Cancellation, inaccessible/invalid targets, already-preserved corpses and failed freshness eligibility are free and grant no progress.

**Fresh-only eligibility:** Evaluate native-derived **effective age**, including prior ice/icebox storage, deterministically. The following are the approved application boundaries:

| Current BUC | Fresh enough to preserve | Ineligible at or above |
| --- | --- | --- |
| Cursed | Effective age <20 turns | 20 turns |
| Uncursed | Effective age <40 turns | 40 turns |
| Blessed | Effective age <60 turns | 60 turns |

Reject an existing persistent rotten state (`orotten` in the inspected baseline) and already-established irreversible taint. These freshness boundaries replace the earlier, more permissive not-yet-tainted admission rule. An uncursed corpse at age 35 can qualify; one at age 45 cannot. Do not call a randomized eating trial to decide whether repeated applications succeed. A threshold failure cannot improve by repeated attempts on unchanged state; storage, splitting and reload cannot reset age or known rot/taint. Preservation never rejuvenates a corpse or repairs existing rot/taint. Current native BUC and effective-age handling must be reconciled at the required baseline rather than inventing a flat chronological-age cutoff.

**One-time lock and stacks:** Each corpse can be preserved once ever. Save its preservation marker, captured multiplier and fractional aging state through transfers, storage, splitting and reload. It cannot be reapplied or upgraded by a more proficient artifact. A legitimate one-unit split is allowed; duplicating quantities or clearing markers is not. Prevent merging corpses with incompatible preservation/aging states, and do not let native age averaging launder age or freshness.

**Artifact proficiency:**

| Rank at application | Successful preservations already earned | Aging slowdown | Effective aging rate |
| --- | --- | --- | --- |
| I | 0-19 | **10x** | 1/10 normal |
| II | 20-59 | **20x** | 1/20 normal |
| III | 60+ | **30x** | 1/30 normal |

One point is credited per successfully preserved corpse. The artifact advances at **20** and **60** successes. The corpse receives the multiplier available at application, before crediting that success; the 20th and 60th successes do not retroactively upgrade that corpse. Each artifact ranks independently, and previously preserved corpses retain their captured multiplier.

**What is slowed:** Preservation slows remaining effective aging used by native age-related deterioration, eating freshness and rot-away. It adds no fixed age bonus and does not reset accumulated age. For a future effective-age threshold `T`, current age `A < T`, and multiplier `M`, the remaining ordinary off-ice world turns are `(T - A) * M`; extra time gained is `(T - A) * (M - 1)`, with native integer/fractional accounting preserved.

For an ordinary uncursed corpse preserved at effective age zero, with unchanged BUC and no ice:

| Rank | Earliest age-related sickness risk (effective age 40) | Earliest age-related taint risk (effective age 60) |
| --- | --- | --- |
| I, 10x | 400 world turns | 600 world turns |
| II, 20x | 800 world turns | 1,200 world turns |
| III, 30x | 1,200 world turns | 1,800 world turns |

For age 35 at rank I, the corresponding remaining times are 50 and 250 world turns. These are earliest age-based risk thresholds, not guaranteed edible lifetimes or corpse disappearance deadlines. In the inspected baseline, the often-cited 30-turn deterioration test belongs to non-corpse food. Eligible corpses can separately undergo a native 1-in-7 rotten-food check even when fresh; preservation **does not suppress that check**, poison, acid, petrification or other native hazards. `Fresh` is an application criterion, not a promise that eating is safe.

**Corpse categories:** Eligible decaying categories include ordinary corpses, trolls, zombifiable corpses and acid blobs, subject to the freshness gate. Exclude Rider, lizard and lichen corpses. Preserve species-specific food/hazard semantics. Preservation itself must not delay, disable or alter troll revival, zombification or separate scripted resurrection clocks.

**Ice and iceboxes:** Ordinary ice multiplies the slowdown by another **2x**, giving **20x / 40x / 60x** total while on ice. Melting restores the captured preservation rate. Iceboxes pause aging entirely; a corpse at effective age 35 remains age 35 after 1,000 world turns inside, and resumes aging from that point after removal. Retain the multiplier and fractional age across storage changes. Preserve the native icebox treatment of revival timers; this does not imply every timer resumes unchanged. In the inspected baseline iceboxes stop revival timers and removal treats ice trolls differently from other corpses. Preserve native storage effects without making the new preservation multiplier alter those revival rules.

**T5 lifecycle arithmetic closure:** Maintain each preserved corpse's deterministic native-effective-age state, captured preservation multiplier, fractional elapsed-aging remainder, one-time preservation marker and existing rot/taint state. Reconcile elapsed game turns **once** on each custody, tile/ice, icebox, level or save/load transition: outside icebox, accrue effective aging at 1/(captured multiplier) off ice or 1/(2 * captured multiplier) on ordinary ice; inside icebox, accrue no aging. Do not round away subturn remainders, reset accrued age on splitting, or permit age laundering through merges: deny merges unless complete preservation/age/rot state is compatible. Keep all native species-specific revival/zombification clocks separate and let native icebox-specific treatment govern those clocks. Apply the approved BUC-specific fresh-only thresholds to the actual reconciled effective age; preservation never eliminates independent native rotten-food checks or poisoning/petrification hazards.

**Inspect:** `#inspect` on the corpse visibly identifies it as **Preserved** and displays its captured preservation multiplier. On the identified artifact, show current rank, successful-preservation count/progress and corresponding effect. Preserve normal identification rules for unrelated hidden attributes; the visible preservation marker is not a general identification ability.

### U4 - Consumable Conservation

**Trigger:** Each artifact that is active for a legitimate completed `q` **quaff potion** or `r` **read scroll** action receives an independent preservation opportunity. No applications to dipping, alchemy, mixing, throwing, breaking, potion vapors or unrelated item use; no effect when initial selection is canceled before consumption is committed or native rules consume nothing. If native consumption has already been committed, canceling a later target prompt still allows conservation, even if no useful magical result follows. The native charging-scroll target prompt is a baseline example.

**Probability:** Depend **only** on the standard base price `objects[otyp].oc_cost`, not current shop price, BUC, identified/unknown status or negotiated purchase value:

| Native item base cost | Conservation chance for each active artifact |
| --- | --- |
| 0-100 zorkmids | **30%** |
| 101-200 zorkmids | **20%** |
| 201+ zorkmids | **10%** |

**Eligibility:** **All native potions and scrolls**, including high-power/permanent buff items and scrolls of genocide, charging, gain level, enchant armor and enchant weapon. No special exclusions merely because an item is powerful or expensive. Unknown, blessed, uncursed and cursed items participate subject to normal native completed-use semantics.

**Result:** Upon the first successful artifact roll, the action's full native effect occurs **exactly once**, while **one otherwise consumed unit remains**. This is conservation, **not duplication of the item or of its magical effect**. Preserve object identity, quantities, BUC, identification, dilution, ownership, unpaid merchandise/shopping liability and stack accounting. Never materialize a free additional unit if consumption was never committed. Stop further artifact rolls once the item has been conserved.

**Unpaid merchandise:** Each committed use of unpaid merchandise incurs its normal use charge even when conservation succeeds. The retained unit remains separately unpaid shop merchandise until bought or returned. Returning it does not erase charges for prior uses; using it and then buying it incurs both the use charge and purchase charge. Repeated uses are charged once each, not once per source roll. Preserve bill identity and stack accounting without duplicating physical items.

**T6 atomic consumption closure:** Intercept only at a verifiable native **committed-consumption** boundary for `q`/`r`. Make the native magical effect execute **once**, irrespective of the conservation result; a successful independent source roll keeps precisely one unit that native consumption would otherwise destroy, preserving that unit's object identity and lawful shop/BUC/quantity state (including a one-unit stack). Prefer a transaction/retention path over deleting and recreating the final unique object. Source attempts after the first actual conservation stop; they do not repeat the effect or bill. A later canceled choice prompt may leave the native use committed and is therefore eligible, while canceling before consumption commitment is not. Preserve all native item-type-specific post-use side effects and ordinary unpaid consumption fees **once per committed use**, independently of the separately unpaid retained item's later purchase/return.

**Multiple sources:** Independent rolls in sequence. For two active artifacts, combined probability is **51%/36%/19%** for the low/middle/high cost tiers, respectively; for three sources use `1-(1-p)^3`. This supersedes the original single shared conservation roll (Issue 126).

### U5 - Environmental Transmutation

**Nature and interface:** Manual `#invoke` of the chosen artifact, selecting the Environmental Transmutation ability in an invocation menu if necessary, followed by **map-cursor targeting** of one terrain tile. No player Pw cost. Can target off-axis in any direction. Valid range is **Chebyshev distance <=5** (an 11x11 square centered on hero), requiring visible terrain and **unobstructed line of sight**. Opaque obstacles obstruct; monsters do **not** obstruct.

**Allowed conversions and costs:**

| Target | Outcome | Charges |
| --- | --- | ---: |
| Ordinary pool/moat/bog/puddle water | Ice, remembering exact original water terrain subtype | **1** |
| Ice | Restore the tile's recorded original terrain (native thaw semantics for preexisting ice) | **1** |
| Ordinary lava pool | Solid, traversable floor | **2** |

This includes **Moria bogs** and **Mithardir puddles** where represented by eligible ordinary water-like terrain. It does not include the special Water Plane `WATER` terrain, water/lava walls, ice walls, drawbridges, magic portals, invocation/quest/progression connectors, special protected floor structures or unrelated wall excavation. U2 handles digging separately.

**Duration and occupants:** Successful transmutation is **permanent** until changed by another valid game-world action (including external/native fire melting ice or a deliberate reverse transmutation). No custom automatic melt timer. A target may contain **monsters, objects or traps**; use safe native terrain-transition consequences and placement/visibility updates. Do not invent elemental damage or new combat procs. Never delete protected terrain, stranded branch access, quest objects or occupied contents.

**T7 terrain provenance closure:** For every eligible water subtype converted to ice by U5, save the exact native terrain-subtype provenance in level/tile state so both explicit reverse conversion and an existing native external melt route can restore appropriate native terrain. Do not overwrite preexisting ice's native thaw history; it follows its own native thaw rules. Validate protected progression structures, transition legality, range/LOS, sufficient charges and occupancy **before** committing the tile mutation. Execute the native terrain/vision/light/pathing/trap/occupant updates atomically and clear stale provenance whenever ice is replaced by another legitimate world event. Any absent or invalid provenance must be reconciled with existing native ice semantics, never guessed into a different water subtype, and invalid requests consume no charge/turn.

**Resources (per artifact):** Maximum **10 charges**, initially full for a newly created source, with sequential **+1 charge every 10 world turns** below full. No recharge credit is banked at full; spending from full starts a new interval; spending an additional charge does not restart an already-running recharge countdown. Regenerates while unequipped; state persists with the artifact through transfers/save/restore. Only the explicitly invoked artifact spends its charges.

**Cost and validity:** Successful transmutation costs **one game turn** and the exact charge cost above. Canceled targeting, out-of-range/occluded/invalid/protected target, no actual change, or insufficient charges consumes **no turn and no charges**, and provides an explanatory reason. Do not charge for a mere attempted invalid transition. `#inspect` shows source-specific charge count, cap and recharge progress.

### U6 - Anti-Theft Ward

**Scope:** A **100% guaranteed** protection against all ordinary **monster-initiated theft** while the specific source artifact is active according to **that artifact's own** activation condition.

**Covered:** Gold theft; theft of unequipped items, wielded weapons, worn armor, jewelry and other equipment; seductive theft, forcible disrobing/removal associated with theft, and other ordinary monster theft paths. Ordinary artifacts are protected as normal possessions. After ward prevention, objects remain in the original inventory and slots with no BUC/intrinsic/ownership corruption. Unrelated effects of the monster's attack proceed according to native rules. Monster attack is not retroactively undone or reflected as punishment.

**Not covered:** Actual **quest/progression-critical scripted object transfers** where the scenario explicitly requires them; deliberate hero dropping, selling, quaffing, sacrifice or transfer; theft performed by the hero; environmental/elemental item destruction, corrosion, curse changes, ordinary wetness, shop debt or unrelated terrain effects. Do not inadvertently reproduce Nenya waterproofing or existing 99% extrinsic elemental item protection.

**Activation:** The prior universal 'must be directly carried' requirement (Issue 97) has been **replaced by artifact-specific per-property activation design**. A future U6 artifact may require wearing, wielding, directly carrying or another specifically defined condition. Never assume nested-container protection unless an actual artifact design permits it.

**Multiple sources:** Each active source grants its guaranteed ward effect; the first prevention ends that theft event, so no duplicate prevention is meaningful. **Do not reinterpret this as a shared property activation gate.** Protected progression transfers are classified from actual native scripted behavior, not excluded merely because a valuable ordinary item has an artifact flag.

### U7 - Ability Mute (NOT Monster Mimicry)

**Purpose:** Temporarily disable **one distinct special ability** belonging to a monster, without stealing/copying it or granting the ability to the hero. Do not use global native `mcan` cancellation or rewrite immutable `mons[]` descriptors.

**Trigger:** Independent **25%** chance per eligible successful primary melee hit from each active granting artifact. Artifact design determines *when the source is active and which qualifying hits it grants*, not a universal requirement that U7 itself be on the wielded weapon. Monster Magic Resistance, elemental resistance and status resistance **do not supply an additional save** against muting.

**Duration and concurrent limit:** Initial duration is **2d3 world turns** (minimum 2, maximum 6, mean 4). At most one distinct special ability is muted per monster. Every later successful activation rolls a new 2d3 and refreshes the same ability with `remaining = max(current_remaining, new_2d3)`. An extra success must never shorten the mute; durations do not add. For example, a six-turn mute followed by a two-turn roll stays at six turns. After expiry, a future success may select a different ability. Use native monster-status time/catch-up rules from Section 2.5.

**Ability identity (follow-up F3, approved Option B):** For attack/passive powers group by **delivery method plus special effect**, with spellcasting-channel rules below as explicit exceptions. Repeated equivalent intelligence-draining tentacles are one ability; paralysis gaze and passive-contact paralysis are different candidates. For non-attack innate/special-AI powers, assign a canonical **special-action category plus effect** identity, deduplicating alternate code paths of one capability. An eligible power counts as one candidate, not one per attack slot or AI hook. Map imported attack and AI variants to this contract without weighting duplicates as separate abilities.

**Selection:** From the currently eligible **distinct special abilities**, randomly select **uniformly**, with each eligible innate/special-AI capability also counting once; repeated attack slots or AI dispatch paths sharing one ability count as one candidate. Example: two intelligence-draining tentacles and one paralysis gaze => 50% chance of drain vs 50% gaze after the initial 25% activation. All attack slots and dispatch paths sharing the chosen ability are suppressed during the timer. Do not limit the candidate pool to powers used in the current attack: a discrete eligible capability can be selected even when not presently executing.

**Attack handling:** For an attack combining normal physical damage with a special rider (e.g., physical tentacle + intelligence drain), **preserve physical damage** and suppress the chosen special effect package. For a pure special attack (breath, gaze, spellcasting) the selected special ability cannot execute while muted. Other unmuted abilities, normal movement, ordinary attack damage, innate resistances and unrelated native monster AI remain functional. The targeted special AI action itself is suppressed when it is the muted capability.

**Spellcasting channels:** Treat `AD_SPEL`, `AD_CLRC` and `AD_PSON` as **distinct ability channels**. Muting a channel prevents **all spells through that channel**, including multiple slots sharing it, for the remaining duration. Other different casting channels remain active. Do not randomly select merely one particular spell within the chosen channel.

**F3 special non-attack powers (approved Option B):** Eligible discrete powers also include **innate teleportation, standalone innate/non-spell summoning (including a non-spell path to summon nasties), special transformations, and other independently dispatched supernatural/special monster actions**. When Summon Nasties is cast through `AD_SPEL`/`AD_CLRC`/`AD_PSON`, it is already stopped by muting that whole casting channel; do not offer every spell in that channel as a separate candidate. If the same monster additionally has a genuinely separate innate summoning mechanism, it is a different candidate. Suppress the chosen special action at every native handler through which it can execute, without generically disabling movement, ordinary AI decisions, inventory handling, innate resistances, death-triggered effects or protected scripted progression.

**Passives:** Eligible **passive contact effects** (such as floating-eye paralysis, cockatrice contact petrification and elemental/acid retaliation) can be selected. The triggering melee attack resolves passive consequences using the form actually struck and its pre-existing mute state, even if O2 changes the defender before later phases; a newly applied mute affects **subsequent** interactions, never retroactively cancels the trigger attack's contact effects. Innate resistances, death-triggered actions and critical progression mechanics are not eligible muted abilities.

**Cockatrice clarification:** A living cockatrice's passive contact petrification is an eligible mute; any separately dispatched active stoning attack is a distinct eligible special ability. Muting one does not automatically mute the other. The initial contact that caused the U7 roll retains its native passive consequences. Cockatrice **corpses and other objects** are not live monster abilities and do not become safe from this status.

**Target eligibility:** Ordinary monsters and **non-progression-critical unique monsters/bosses** may be muted. **Riders, quest-critical bosses and protected quest NPCs** are immune. A monster with no eligible special ability receives **no fallback debuff** (no to-hit penalty or bonus physical damage).

**Polymorph:** Any successful species change immediately **clears an existing mute**. If O2 and U7 both trigger on the same hit, O2 transformations resolve first, and U7 then chooses a new eligible ability of the resulting species; a resisted/failed O2 leaves the original form available to U7. U7 cannot be applied to a dead/ineligible monster.

**Multiple sources:** Each active artifact rolls independently; any number can succeed, but the first establishes the randomly selected mute and later successes **refresh that same mute without shortening it**. No second simultaneously muted ability is created. Save expiry and suppressed-ability identity per monster across level changes. Report status without leaking hidden/unidentified abilities.

**T8 registry/dispatch closure:** Build and source-audit an explicit canonical ability catalogue with categories for attack riders, pure attacks, contact passives, three native spellcasting channels, and F3 standalone innate/special-AI powers. Map each eligible distinct ability to **all** live native and imported execution paths, including monster-vs-monster handlers; block only that ability while the per-monster mute is active. Maintain exactly one muted identity and a non-shortening 2d3 refresh. On an actual species change, invalidate the old mute before considering new U7 activations on the new species. Missing code-path mapping is a validation failure, not a reason to revert F3 or use global `mcan` cancellation.

---

## 6. Cross-property ordering and critical interactions

### 6.1 Primary hero melee-hit ordering (approved constraints)

Each property's rolls are independent **per active artifact**, but actual effects run in deterministic phases to preserve outcomes and prevent invalid pointers. The implementation must satisfy the following relative order, with native hit/prevention/mitigation and per-source validation in the appropriate entry points:

1. **Landed-hit snapshot and native primary attack:** validate the actual strike and snapshot candidate sources, original impact square, defender form and pre-existing passive/mute state. At the landed-hit boundary, establish O6 eligibility and independent rolls before lethal damage, retaining the native elemental resolver information. Resolve ordinary physical damage and native defenses. Treat each qualifying main/offhand, unarmed, kick or natural strike as its own event; extra speed actions do not become timer ticks.
2. **Frenzy:** for each triggered source, apply its bounded physical bonus ticks on a still valid living defender.
3. **Primary elemental damage:** resolve the original strike's legitimate native primary elemental effect **once overall**, not once per successful chain. Separately capture/roll each committed O6 artifact's **own qualifying native elemental base** for its future secondary hops; those rolls cause no extra hop-0 hit. Do not damage a primary that already died. Retain successful chains and their original impact information for secondary propagation even when ordinary physical damage or Frenzy killed the primary first.
4. **Executioner:** for each active eligible source, evaluate the post-damage remaining HP threshold and execute through native fatal/lifesaving handling if successful. Revalidate after every source's effect.
5. **Surviving-monster inventory interactions:** Enhanced Steal sources first, then Disarming sources; both skip if monster died. Theft and disarm process current updated inventory, preserving uniqueness.
6. **Polymorphic Strike:** process independent eligible source rolls/attempts consecutively against the current form. Clear an existing U7 mute on species change, **not** an existing O3 Anti-Regeneration timer.
7. **Ability Mute:** process eligible source rolls after polymorphing against the final surviving monster; one eligible canonical special ability (including non-attack innate/special-AI categories) at a time, with non-shortening max(remaining, new 2d3) refresh across sources.
8. **Anti-Regeneration:** process eligible source rolls and apply/refresh 5-turn suppression on the surviving current form.
9. **Elemental chain secondary hops:** execute bounded pending chain continuations with each source's retained origin/path, original elemental selection and independently rolled elemental base. Their damage creates native kills but **no fresh offensive proc events**. Actual local plumbing may schedule hops as soon as their chain's primary elemental component resolves, provided all relative priority/after-death guarantees above are preserved.

**Passive and source safeguards:** At each applicable roll/execution boundary, revalidate captured sources and targets. Newly acquired/activated sources do not join this event. Preserve the contacted form's native passive consequences at the native safe contact boundary; later polymorph/mute cannot rewrite them. Stop later ordinary strikes when native passive interruption requires it. A committed chain follows its explicit after-primary-death continuation contract.

**Ordering commitments:** O4 damage before O5 (Issue 114); O6 primary elemental contribution before O5 (Issue 116); O5 before O2 (Issue 111); O1B before O1A (Issue 113); O1B/O1A before O2 (Issue 118); O2 before U7 (Issue 112); new O3 after O2 while old O3 survives (Issue 115); O1 steal/disarm requires a survivor (Issue 117). Source rolls should be evaluated at an engine-safe point that preserves these semantics. Do not confuse a successful RNG roll with automatic eligibility to mutate an object that has since vanished.

### 6.2 Hostile monster melee response ordering

1. Resolve the original hostile monster melee attack, native damage, hero passives, form reversion and lifesaving. Record positive actual HP damage and whether the surviving hero's active HP pool is <=25% of its maximum.
2. **D1:** process independently rolled counterattacks while hero and attacker remain eligible. A qualifying miss can trigger D1. Stop the riposte sequence on disabling passive consequences or hero lifesaving during that sequence. Killing the attacker does not by itself erase the recorded emergency qualification.
3. **D3:** require the original qualifying damage event and current hero survival/HP <=25%. Each ready active source attempts its 1d3 placements near the pre-teleport location. Start its window only if at least one guardian is placed.
4. **D2:** again require original qualification, current survival/low HP and globally armed state. Try captured eligible sources sequentially until one native teleport completes or all fail. A completed same-square teleport succeeds. Rearm only when the active HP pool rises strictly above 25%, including through a form change.

Counterstrike healing above the threshold cancels pending emergency effects. Damage solely from counterattack passives cannot newly qualify D2/D3. Lifesaving from the original incoming hit can permit D1; lifesaving during a riposte stops further ripostes. D3/D2 never react merely to a missed attack or an excluded ranged/spell/environmental damage source.

### 6.3 Hostile monster spell precedence

1. Identify a genuine hostile monster spell at cast commitment using direct hero targeting or the initial area footprint. Exclude unsupported wand/breath/terrain effects, existing hazards, self-only buffs, and spells indirectly summoning enemies but not directly affecting the hero.
2. At the **committed cast** boundary, classify a hero-directed spell even if its later beam would miss, or an area cast whose **initial footprint** contains the hero; exclude ordinary indirect summoning and caster-only spells. For each captured, still-active D4 source, roll independently **20%**. First success cancels the whole spell, including any resulting cloud creation or caster-beneficial component, and grants Pw once: `max(1, listed_spell_level) * one_d6` when a level exists, otherwise **6d6**. Later sources have no spell to absorb; pre-existing hazards are not new spell events.
3. If none absorbs, continue with Step 21-specific redirection/reflection, ordinary reflection, magic resistance and normal spell effects, in their actual native-specific order. Preserve normal defenses and do not introduce duplicate mitigation events.

### 6.4 Multiple-source behavior examples (authoritative)

- **Two U4 conservation sources:** Each gets an independent cost-based roll. First success prevents the single native consumable destruction. No duplicate effect/item.
- **Two D4 sources:** If first fails, second can absorb; canceled spell can restore Pw only once.
- **Two D3 sources:** Each ready source rolls 1d3 placements independently; only a source placing at least one starts its own 30-turn window. Native expiry death and the lifesaving-detachment exception apply per guardian.
- **Two D1 sources:** Both can counterattack from the same enemy melee attempt if their independent 33% rolls succeed, while the target remains adjacent/alive.
- **Two D2 sources:** Second tries only if first teleport attempt fails; one actual success disarms the global episode.
- **Two O2 sources:** Two independent successful transformations are allowed; new species level/difficulty upper bounds refer to the **current** form on each attempt.
- **Two O6 sources:** Both chains propagate independently after the original target dies; each retains the original impact location, and different chains can strike the same remaining monster.
- **Two U1 sources:** Two rolls within a single valid one-turn `#untrap` command; stop once trap is removed.
- **Two U7 sources:** Both can roll; first success fixes the ability, and a later success uses max(current remaining, new 2d3). Six turns followed by a two-turn roll remains six; no second ability is muted.
- **Two U2/U5 sources:** Each has separate charges and recharge progress, but a manual invocation selects which source performs the action. No automatic double excavation or transmutation.

### 6.5 Known engine interactions that must remain safe

- Monster death/lifesaving during repeated execution rolls, multiple frenzy sequences, repeated polymorphs, chains and equipment theft.
- Monster polymorph rendering existing monster pointers/state invalid; preserve object ownership and avoid duplicate loot.
- Counterstrike counterattacks involving weapon poison/enchantment, contact passive harm, hero lifesaving, attacking monster death and multiple attacks in one action.
- U7 suppression of passive contact effects only *after* the attack that first establishes mute; monster passive combat handlers must not be silently skipped retroactively.
- O3 suppression independent of U7 state, especially when polymorph removes U7 but O3 remains.
- D4 interception of true monster spells without absorbing unrelated beam, breath, magic attack or terrain processes.
- U1 magical traps vs portal/quest connectors; U5 water subtype provenance and ice/melting; U3 decay freshness vs revival timers; U4 transaction-safe native consumption.

---

## 7. Decision and supersession ledger

This ledger explicitly identifies the later-numbered gameplay decisions **95-134**, plus global revisions. Earlier reviewed decisions (before Issue 95) are captured in full in the property contracts in Sections 3-5, including all approved values and target exclusions. Do not confuse initial recommendations in v0.1 with final user decisions.

| Issue | Subject and approved outcome | Final status |
| --- | --- | --- |
| 95 | U6 protects **all ordinary monster theft**, including gold, equipped and unequipped gear, and seduction-linked item removal; exemptions for quest-critical scripted mechanics | **Kept** |
| 96 | U6 protection is **100%**, not probabilistic | **Kept** |
| 97 | U6 originally required direct inventory carrying | **Superseded**: activation is per artifact design |
| 98 | U7 suppresses special **effects**, preserving mixed-attack physical damage; pure special attacks suppressed; duration changed to **2d3 turns** | **Kept** |
| 99 | U7 trigger increased to **25%** | **Kept** |
| 100 | U7 at most one muted ability; further successes refresh its timer | **Kept, clarified by Q1:** max(remaining, new 2d3), never shorter |
| 101 | U7 applies to ordinary/noncritical unique monsters; Riders/quest bosses/protected NPCs immune | **Kept** |
| 102 | Muting a shared special ability suppresses **all matching attack slots** | **Kept** |
| 103 | U7 **ignores monster Magic Resistance** and further resist checks | **Kept** |
| 104 | Monster with no eligible ability gets **no fallback effect** | **Kept** |
| 105 | Species-changing polymorph **clears U7 mute** | **Kept** |
| 106 | U7 mutes an entire selected **spellcasting channel**, not one spell | **Kept** |
| 107 | Eligible **passive contact effects** may be muted, effective from subsequent interactions | **Kept** |
| 108 | Uniform random selection among **distinct eligible abilities**, independent of repeated slots | **Kept** |
| 109 | U7 originally required the property artifact to be the attacking wielded weapon | **Superseded**: artifact-defined activation; melee event still required |
| 110 | **Different** offensive properties roll independently and can occur on one hit | **Kept** |
| 111 | Executioner resolves before Polymorphic Strike | **Kept** |
| 112 | Polymorphic Strike resolves before Ability Mute | **Kept** |
| 113 | Enhanced Steal resolves before Disarming | **Kept** |
| 114 | Frenzy bonus damage resolves before Executioner HP eligibility | **Kept** |
| 115 | Anti-Regeneration survives polymorph; freshly triggered O3 applies after polymorph | **Kept** |
| 116 | O6 primary elemental damage contributes before Executioner HP check; **secondary hops never execute Executioner** (secondary elemental chain damage itself still executes) | **Kept; wording corrected** |
| 117 | Enhanced Steal and Disarming require a survivor; killing blow gives normal native loot | **Kept** |
| 118 | Enhanced Steal and Disarming resolve before Polymorphic Strike | **Kept** |
| 119 | A retaliatory kill does **not** prevent D3/D2 activation on the incoming damaging hit | **Kept** |
| 120 | Counterstrike may activate after native lifesaving, provided hero/attacker remain eligible | **Kept** |
| 121 | One shared D4 absorption roll | **Superseded**: each artifact independently rolls |
| 122 | One shared D3 summoning group/window | **Superseded**: each artifact summons/manages its own 30-turn window |
| 123 | One shared D2 teleport attempt | **Superseded** by Issue 130: independent attempts, one episode success |
| 124 | One shared D1 Counterstrike roll and one Counterstrike per monster action | **Superseded** by Issue 131: each source independently counterattacks; cap removed |
| 125 | One shared O3 Anti-Regeneration roll | **Superseded**: each source independently rolls, one refreshable target status |
| 126 | One shared U4 Consumable Conservation roll | **Superseded**: each source independently rolls until item is conserved |
| 127 | Each U2/U5 artifact maintains **independent charges and recharge progress** | **Kept** |
| 128 | Proposed U4 directly-carried/equipped universal activation selection | **Withdrawn**: no property-wide activation mode |
| 129 | Proposed universal duplicate offensive single-roll option | **Withdrawn**: replaced by global independent-per-artifact rule |
| 130 | D2 duplicate-source exception: every source may attempt after failed prior attempt, stop after **one successful teleport per low-HP episode** | **Kept; sole express success cap** |
| 131 | D1 independent 33% rolls; every success generates a counterattack; old one-per-monster-action cap removed | **Kept** |
| 132 | O2 independent 20% rolls; **multiple consecutive polymorphs** allowed, constrained by each current species | **Kept** |
| 133 | U1 independent source rolls **within one game-turn `#untrap` action**, until one disarms | **Kept** |
| 134 | O6 independent chains; later chains can propagate from original impact square after primary target's death | **Kept** |

### 7.1 Explicit changes to the original 11 October v0.1 proposal

1. O1B original 10% suggestion -> **20%**, uniformly random eligible inventory entry, complete stack, cursed-equipped exclusion, overflow at hero feet; ordering now **steal before disarm** rather than donor proposal's disarm first.
2. O2 original 10% suggestion -> **20%**, strict dual current-level/current-difficulty ceiling, explicit unique/boss protection, consecutive multi-source polymorphs.
3. O3 original 20-turn on-hit (or global donor aura) -> **5-turn per-monster debuff**, 20%, no revival interference; survives polymorph.
4. O4 retains capped 20%/45%-continuation Frenzy as physical damage ticks without a full attack replay; independent per source.
5. O5 original <=20%/<=10% thresholds and 15%/5% suggestion -> **<=25%/<=15% thresholds and 20%/10% roll**, as applicable; explicitly checks after Frenzy and primary elemental damage.
6. O6 original 20% suggestion -> **30%**; eight-way single linear path, maximum 3 additional hostile targets, 75% falloff, full resistance stops; individual chains persist after primary death.
7. D1 original 25% landed-hit riposte -> **33% on an adjacent attempted melee hit or miss**; full restricted native weapon to-hit; unrestricted independent successful source counterattacks.
8. D2 original 15% every damaging melee -> **guaranteed attempt at <=25% HP, one success per low-HP episode**; independent source attempts after failures.
9. D3 original Mirror Image decoys -> **1d3 tame Emergency Summoning guardians** for a per-artifact 30-turn window; triggered at post-damage <=25% HP.
10. D4 original narrow damage-spell-only 1d6 energy suggestion -> 20% of eligible directly hero-affecting monster spells, full cancellation before reflection/MR, independent rolls, known-level `max(1, level) * one_d6` Pw or **6d6** for an unlisted level.
11. U1 original guaranteed known-hazard disarm -> **depth-scaled** success with 20% floor, safe failed attempts, holes and trapdoors included; multiple source attempts in one turn.
12. U2 original bump/wall/5-Pw effect -> **native Wand of Digging via `#invoke`**, 5 charges, +1/100 turns, multi-tile/up/down, no hero Pw.
13. U3 original passive, direct-inventory slow-aging (25/50/75%) -> explicit one-corpse `#apply`, fresh-only admission, **10x/20x/30x** aging slowdown, per-artifact 20/60 progress, floor reach, one-unit stack separation, visible preservation and native ice/icebox handling. The earlier v1.0 2x/3x/4x rates and not-yet-tainted admission have been superseded.
14. U4 original 10% with high-power item exclusions -> **cost-tier 30/20/10%**, **all potions and scrolls including powerful**, `q`/`r` only, independent artifact rolls.
15. U5 original 5/5/15 Pw adjacent terrain conversions -> **charged cursor-range-5** water/ice/lava conversion (1/1/2 charges), source recharge 10 max/+1 per10 turns, permanent result, no hero Pw.
16. U6 original possible broad item protection -> **theft-only 100% Anti-Theft Ward**, protects gold/equipment/seduction theft; artifact-defined activation rather than direct carry across all sources.
17. U7 original 15%, flat 4-turn attack-slot mute -> **25%, 2d3-turn distinct ability mute**; common attack slots and chosen spellcasting channel suppressed; passive contact included; MR ignored; limited noncritical uniques; artifact-defined activation.
18. Original single-roll duplicate default and fixed offense/defense/utility equipment defaults -> **global artifact-designed activation and independent per-artifact effects**, with **only D2** having an explicit multi-source success cap.

### 7.2 Final review decision ledger Q1-Q27

The Q identifiers below belong to this review, separately from historical Issues 95-134. These rows record the **final accepted outcomes**, including later corrections; abandoned intermediate suggestions have no authority over the integrated contracts.

| Review | Final decision | Governing section |
| --- | --- | --- |
| Q1 | U7 refresh never shortens: max(remaining, new 2d3) | U7 |
| Q2 | Establish O6 rolls before lethal primary damage; a physical killing blow can launch committed chains | O6, 6.1 |
| Q3 | Snapshot active candidate sources at the trigger; revalidate before individual rolls; newly acquired/activated sources wait | 2.2 |
| Q4 | D3 accepts partial placement, no delayed top-up; zero placements leave source ready without starting a window | D3 |
| Q5 | Native world-turn clock; native monster-status catch-up/final-tick convention; saved deadlines for guardians/recharge | 2.5 |
| Q6 | New U2/U5 sources start full; no banked recharge at full; retain partial progress while below full | 2.5, U2, U5 |
| Q7 | D2/D3 use active HP pool after damage/reversion/lifesaving; a >25% pool after form change rearms D2 | 2.6, D2/D3 |
| Q8 | Each qualifying direct hero melee strike is its own primary event, including offhand/unarmed/kick/natural melee | 1.4, 6.1 |
| Q9 | Frenzy freshly rolls ordinary physical strike damage with applicable native bonuses; physical-only, no new procs/riders | O4 |
| Q10 | Triggering contact uses the form actually struck and pre-existing mute state for native passives | 2.6, 6.1, U7 |
| Q11 | Revalidate every riposte; disabling passives, lost weapon, death or lifesaving during the sequence stop later ripostes | D1 |
| Q12 | Both original incoming-hit low-HP qualification and current execution-time survival/low HP required for D2/D3 | 2.6, 6.2 |
| Q13 | U7 ability identity is delivery method plus special effect, with casting-channel exceptions | U7 |
| Q14 | Monster/pet usability is assigned per artifact during later artifact design; no blanket actor restriction | 2.1, 8.2 |
| Q15 | Active U1 automatically supersedes ordinary disarming; no active source means native behavior; no new method-choice prompt | U1 |
| Q16 | Fresh-only admission: effective age <20 cursed, <40 uncursed, <60 blessed; reject persistent rot/established taint | U3 |
| Q17 | Preserve exactly one separated corpse; preserve quantity/state; corpse inspect visibly shows preservation/multiplier | U3 |
| Q18 | U4 still applies after native consumption commitment when a later target picker is canceled | U4 |
| Q19 | Conserved unpaid use is charged; retained merchandise remains separately unpaid; use and later purchase both billed | U4 |
| Q20 | Completed same-square teleport counts as D2 success; cursor cancel is failure; retain native invalid-target fallback | D2 |
| Q21 | Listed spell level uses max(1, level) times one d6; unlisted level uses six independent d6; cap at max Pw | D4 |
| Q22 | Absorption fully cancels the whole eligible cast, including cloud creation; existing hazards are not absorbed | D4 |
| Q23 | Preservation ranks are 10x/20x/30x, retaining 20/60 proficiency thresholds and native independent food hazards | U3 |
| Q24 | Guardian saved from expiry by native lifesaving loses temporary expiry status permanently and remains an ordinary tame pet | D3 |
| Q25 | Expiry death grants no hero kill credit/XP; native corpse/loot and actual combat attribution remain | D3 |
| Q26 | Include doors/containers with native discovery; complexity condition closed after source assessment | U1, 8.5 |
| Q27 | Include self-release from the actual eligible known holding trap; complexity condition closed after source assessment | U1, 8.5 |

**Additional D3 reversal:** The review replaced v1.0 disappearance via `mongone` with native expiry death/loot. The suggested repeated expiry-death attempts after lifesaving were rejected in favor of Q24. Likewise, the intermediate 5x/10x/20x preservation suggestion and unknown-level 1d6 fallback were replaced by Q23 and Q21.

### 7.3 Follow-up gameplay approvals F1-F6 (14 October 2026)

These decisions refine the already-approved contracts and do **not** reopen the historical Issue 95-134 or Q1-Q27 rules.

| ID | Approved outcome | Governing sections |
| --- | --- | --- |
| F1 | O6 Option A: each successful granting artifact independently obtains its **own qualifying native elemental damage roll** as the base for its chain; never repeat elemental damage on the primary monster | O6, 6.1 |
| F2 | D4 Option A: cast-time eligibility for genuine hostile spells directed at hero even if they would miss, and area spells initially encompassing hero. Ordinary nearby summoning and caster-only buffs are excluded; mixed directly offensive/buff casts are wholly canceled | D4, 6.3 |
| F3 | U7 Option B: add discrete **non-attack innate/special-AI abilities** (including innate teleportation, summoning and transformation) to the uniformly selected mute candidates. A spellcasting channel still suppresses every spell through that channel; living cockatrice contact stoning remains an eligible passive, not retroactively suppressed on the triggering hit | U7 |
| F4 | O5 Option A: a successful Executioner roll is fatal **without additional armor/DR mitigation**, while preserving native lifesaving and approved immunities | O5 |
| F5 | Rename D1 from its former name to **Counterstrike**. Independently successful main-hand/offhand weapon sources counterattack using their **respective granting weapon** (one swing per source), never a full dual-wield sequence per proc. Non-weapon sources need weapon attribution at artifact binding | D1, 6.2 |
| F6 | Artifact-binding decisions, including monster/pet permissions and all acquisition/distribution, stay **deferred** to a separate approval. The property engine can be verified with controlled fixtures without implying real artifact assignments | 1.3, 8.2, 9.3 |


---

## 8. Technical implementation contract (no unapproved gameplay changes)

### 8.1 Mandatory source-level audit before implementation

- Confirm current local post-Step-21 HEAD, `EDITLEVEL`, active 23-artifact set, original Step 21 behavior, source-specific grants, current `#inspect`, artifact invocation state, ordinary Forge legality and save persistence.
- Pin each donor repository/revision before porting a mechanism. Import only the relevant implementation technique, preserving NetHack and variant attribution/licenses; user-approved rules override donor defaults.
- Identify actual live attack/defense/monster-vs-monster code paths for hero-to-monster hits, native physical damage, elemental components, Counterstrike, death/lifesaving, monster regeneration, monster spellcasting, monster polymorph, passive-contact attacks, trap removal, corpses, item consumption, digging and terrain transitions.
- Audit already-implemented Step 21 Mirrored Mask spell reflection/redirection and the Cloak of the Unheld One active teleport invocation. They must not be broken or silently rewritten by D4/D2.
- Audit monster spell levels and flags from local `src/mcastu.c`/`include/mcastu.h`, including level-zero spells, supported damage/status spell types and spellcaster-specific dispatch. The available inspected phase branch includes spell levels 0-20; actual current HEAD still governs.
- Reconfirm the Step 11 native-behavior findings in Section 10.1 against the actual Step 21 checkout, including freshness arithmetic, expiry lifesaving, spell metadata, trap dispatch and timer catch-up. The source-level feasibility review is not a substitute for that baseline check.
- Verify correct handling of special terrain codes (`WATER`, pool, moat, bog, puddle, `ICE`, lava pool, stairs/portals/quest features) and native frozen-water provenance flags before modifying terrain.

### 8.2 Required architectural separations

Implement this scope using shared typed property/activation/event semantics rather than 18 disconnected hacks. The exact function/module layout is an engineering decision, but it must preserve these separations:

1. **Definition vs grant:** A property contract (effect, chance, duration) is distinct from a granting artifact's per-property activation condition.
2. **Per-source enumeration:** Enumerate all applicable active source artifacts and independently evaluate each; do not collapse duplicates. Assign a stable order for deterministic replay.
3. **Procs vs execution:** Snapshot candidate sources at the trigger, revalidate activation before each roll, then recheck target/effect validity at execution. Establish O6 rolls before lethal primary damage and retain committed chain data. Newly acquired/activated sources do not join the event. A successful roll is not a license to dereference a dead target.
4. **Hit event categories:** Mark primary hero melee hits, Frenzy damage ticks, elemental chain hops and Counterstrike counterattacks distinctly to prevent uncontrolled recursion and false XP. For a D1 weapon grant, preserve the actual granting weapon identity through the counterattack.
5. **Kill/lifesaving integrity:** Use native fatal and restoration pipelines; never fabricate object copies or second deaths.
6. **Monster status identity:** Store U7 delivery-method-plus-effect/channel **or innate special-action-plus-effect** identity and non-shortening remaining duration, plus O3 suppression state on the monster. Use native status maintenance/catch-up; do not mutate global species descriptors or repurpose permanent cancellation.
7. **Artifact resource identity:** U2/U5 full initialization and no banked full-capacity recharge, partial recharge below full, U3 proficiency, and D3 window/group ownership remain per source. Zero-placement summons start no window; expiry-lifesaved pets detach permanently without extending the source window.
8. **Hero emergency identity:** D2 global low-HP disarmed/armed state survives source changes and load; every active source still independently attempts after an unsuccessful earlier one.
9. **Atomic item operations:** O1 theft/disarming and U4 consumption must preserve native equipment removal, stacks, BUC, identity, artifact uniqueness, paid/unpaid/shop state and all before/after hooks exactly once.
10. **Corpse lifecycle:** Implement fresh-only deterministic admission, single-unit separation and 10x/20x/30x effective aging. Separate freshness/rot-away from revival/zombification; preserve fractional remainders, known rot/taint, ice/icebox storage, off-level aging and incompatible-stack protections. Expose preservation on corpse inspect.
11. **Terrain validity:** Centralize legal terrain conversion/trap removal checks, visibility, map/LOS/light/pathing changes, safe occupancy, shops and protected connectors.
12. **Spell interception:** D4 classifies direct/initial-area hostile spells **at cast commitment**, including casts that would miss, and cancels the entire eligible cast atomically before ordinary defense, including hazard creation and any caster-side benefits. Distinguish listed zero from absent level, and multiply one die for listed levels versus sum six dice for absent levels. Existing environmental hazards never become spell events.
13. **Source-specific disclosure:** An identified artifact's `#inspect` shows accurate activation conditions, chance, duration, charge/skill state, rearming, guardians and remaining existing cooldowns. Unidentified items do not leak their property list. Passive properties do not invent invocation cooldowns.
14. **Artifact-specific actor permissions:** Inspect native conventions, but assign monster/pet activation permission only through each later artifact design. No property-wide hero-only or automatic monster-use rule is approved. Record actor-specific event/activation mappings in the binding catalogue; target statuses still govern relevant monster-versus-monster behavior.
15. **Save format:** Detect all newly serialized fields and explicitly update save epoch if incompatible; preserve historical compatibility gates and clearly say when a **new game** is required. Do not guess that EDITLEVEL 18 is automatically correct.
16. **Action accounting:** Initial canceled/invalid U1 disarming is free, but deliberate native door/container inspection keeps its turn cost. U3/U5 invalid/canceled actions are free; valid ineffective U2 invocation spends one turn/charge. All U1 source rolls share one disarm turn; one separated U3 corpse earns one point. U4 respects native consumption commitment, later canceled target selection and per-use unpaid billing; magical effects are never duplicated.
17. **Source-safe clone/transfer cleanup:** Handle monster clones/forms/death, corpses created/destroyed, guardian expiry, inventory transfer, equipment changes, artifact removal from inventory, hero polymorph and level transition without stale links or timer resets.
18. **Repeatable verification:** Seeded native fixtures and save/load roundtrips must show exact probabilities/caps, native geometry/LOS, correct property priority, no infinite loops and no additional artifact or monster content. In particular, verify recursive protected-container rejection; finite O2 candidate selection; off-level O3 suppression; stable D3 guardian links; U3 fractional aging; committed U4 billing; U5 external ice melt; and full F3 U7 native/variant ability dispatch coverage.

**Open source-verification details, not newly approved gameplay choices:** Identify precise attack-class predicates, actual D4 spell interception hook, all U7 active/passive dispatch paths, deterministic fresh-only admission and preservation of known rot/taint without changing native eating hazards, exact ice/rot/revival timer reconciliation, source-specific charging storage, guardian cleanup across level changes, current-HEAD polymorph equipment behavior, and all quest-protected item/terrain classifications. If any cannot satisfy the stated gameplay contract, report the conflict explicitly rather than silently simplifying the mechanic.

### 8.3 Save and state persistence

At minimum validate serialization and recovery of:

- Individual artifact identification and activation conditions as dictated by artifact design.
- U2/U5 charge counts and partial regeneration progress, including full-capacity no-banking state, independent of equip/drop/save/reload.
- U3 rank/progress, corpse-specific one-time lock, captured 10x/20x/30x multiplier, fractional effective age, persistent rot/taint and compatible-stack identity.
- U5 original water terrain subtypes for ice thaw/restoration, where persistent provenance is needed.
- D2 global Armed/Disarmed episode and rearming condition, independent of currently worn sources.
- D3 per-source deadlines, successful placement/window state, guardian membership, native expiry death and permanent detachment of expiry-lifesaved pets. Detached ordinary pets must not be deleted by later group cleanup.
- O3 individual 5-turn suppression; U7 ability identity and remaining duration after max(remaining, new 2d3) refresh, using native off-level status catch-up.
- Spell/attack turn counter semantics on reload, game pause, off-level time and mid-action rollback. No timer can be duplicated/refilled by repeated reload.

### 8.4 Donor fidelity policy

The donors in Section 10 are **implementation leads only**. They do not authorize their original proc rates, special monster forms, global area effects, unrelated roles/quests, item systems, progression rewards or side effects. Use the narrowest native/donor helper compatible with the approved gameplay. Document deviations and license provenance in the implementation report.

### 8.5 Q26 and Q27 development complexity assessment

Both features are retained in scope. The assessment is **incremental over the already-required U1 implementation** and based on read-only inspection of Step 11 `7fa6dd11a`. It does not certify Step 21 integration or provide a build/gameplay PASS.

| Feature | Incremental complexity | Native reuse and implementation boundary |
| --- | --- | --- |
| Q26: doors/containers | Moderate | Reuse `untrap_box`, `disarm_box` and the door branch of `untrap`; preserve discovery and one-action accounting, substitute U1 rolls only at disarming, and avoid native dangerous failure branches |
| Q27: self-release | Low-to-moderate | Narrowly override the trapped-hero refusal for a matching eligible trap; reuse release/removal helpers and pit vision/levitation refresh |

In the inspected `src/trap.c`, `untrap_box` (6165) records container trap knowledge; the door branch (6407) performs native discovery without comparable permanent knowledge storage. A shared forced-success flag would incorrectly bypass discovery and U1 rolls, so it must not stand in for artifact activation.

The trapped-hero refusal is at 6298. `deltrap` (6875) removes the trap but does not clear hero restraint. `delfloortrap` (7012) combines supported trap removal with release; `reset_utrap` (1134) restores blocked flight/levitation state, and `openholdingtrap` (6521) illustrates the pit vision refresh but leaves the trap intact. Validate the real holding trap and its type before using these helpers. No additional subsystem or saved fields appeared necessary for Q26/Q27 when retaining native discovery behavior.

Verify unknown and known boxes/doors; failed inspection and safe failed disarm; independent source rolls with exactly one removal; cancellation and inactive-source regression; self-trapped bear/web/pit success/failure; mounted, conjoined-pit, levitation, protected-target and unrelated-restraint cases. Reconfirm hook locations and behavior at the required baseline.

### 8.6 Follow-up technical resolution register T1-T8 (scope-closed, not implementation-verified)

The following resolutions implement the approved gameplay contracts without adding new proc rolls, character abilities, immunities or universal activation restrictions. **Closed** means no additional user-level decision is needed for the documented behavior. It does not mean the code was examined, changed, compiled or tested against current post-Step-21 HEAD.

| Issue | Normative engineering resolution | Minimum completion evidence |
| --- | --- | --- |
| **T1 O1B nested protected items** | **Explicitly user-approved.** Pre-filter top-level containers recursively when any nested item is quest/progression protected; entire container excluded from uniform selection. No protected object transfer via indirect container move | Nested/deep containers, ordinary containers, mixed eligible inventory uniformity, ownership and quest cases |
| **T2 O2 empty candidate set** | Finite native-weighted valid form pool with both current-form ceilings and other native exclusions; empty pool means safe no-op; rebuild after every consecutive transform | Empty pool, constrained/nonempty pool, weighting and repeated O2 success fixtures |
| **T3 O3 automatic HP restoration** | Apply five-turn suppression to native/intrinsic/passive-equipment automatic HP regen, not deliberate spells/items or non-HP resources; account for only unsuppressed ticks off level | On/off-level native regen, equip changes, polymorph, save/reload, intentional healing unaffected |
| **T4 D3 guardians** | Persistent unique source/guardian link and deadline, exactly-once native expiry processing, off-level catch-up before actions, native death/loot, no expiry XP; native lifesaving makes ordinary detached pet permanently | Early death, transfer/remove source, migration, polymorph, cloning, repeated save/load, overdue expiry, lifesaving |
| **T5 U3 corpse age** | Persist fractional effective-age remainder and one-time marker, reconcile each time transition once, apply 10x/20x/30x and native ice/icebox rules, reject incompatible merges; revival clocks unaffected | Freshness 19/20,39/40,59/60; remainder boundaries; split/merge; 1,000-turn icebox; revival/rot tests |
| **T6 U4 consumption** | A committed native potion/scroll use has one native magical effect; successful source retains one original unit without cloning, preserving quantity/shop debt and independent rolls; no precommit eligibility | Single-unit and stacked items, postcommit canceled target, multiple sources, repeated unpaid uses, subsequent purchase/return |
| **T7 U5 terrain provenance** | Persist U5-produced ice's exact water subtype; preserve preexisting native ice history; native external melt/explicit reversal use original subtype; atomic terrain transitions and stale-flag cleanup | Each water subtype, preexisting ice, external fire/thaw, save/reload, occupants/traps, invalid/protected tiles |
| **T8 U7 capability dispatch** | Explicit unique ability catalogue for normal attacks/passives/spell channels plus F3 innate/special-AI actions; map all handlers and block selected identity only, with 2d3 max-refresh and native catch-up | Summon Nasties spell-channel and standalone cases, innate teleport/transform, passive contact, monster-vs-monster, polymorph, imported handlers |

**Engineering-hard-gate status:** These contracts are specified, but the exact current-HEAD hook locations, compatibility, representation details, native probabilities, serialization shape and full runtime evidence are **still to be verified** under Sections 8.1, 9.1 and 9.2. A code-level inability to preserve one of these contracts is an escalation requiring explicit review, not implicit permission to weaken it.


---

## 9. Mandatory native validation and completion gates

Each property requires a traceable matrix with: active artifact/source configuration, hook/code location, fixture seed, expected positive behavior, expected negative behavior, multi-source operation, cross-property interaction, save/load or level transition, and PASS/FAIL/BLOCKED/N/A results. Static inspection alone is not sufficient for combat, corpse aging, spell cancellation, inventory and terrain mutations.

| ID | Mandatory positive and negative scenarios (non-exhaustive) |
| --- | --- |
| O1A | Armed monster disarm; cursed/welded exclusion; ordinary artifact allowed; quest object excluded; no weapon/no survivor; nearest valid drop; no duplicate/unworn bonuses; peaceful/shop safety; multiple sources |
| O1B | Uniform eligible entry selection; equipped/unequipped and ordinary artifacts; cursed equipped vs unequipped; full stacks; encumbrance-category edge; hero-feet overflow; occupied hero tile; monster tame/peaceful/hostile; billing; recursive nested protected-container exclusion before uniform choice; repeated source thefts and no duplicate IDs |
| O2 | 20% proc; strict dual mlevel/difficulty bound before and after consecutive forms; magic resistance/native immunity; unique/boss protection; genocided/extinct species; equipment/HP/AI; repeated 2+ source transformations; finite no-candidate safe no-op/native weighting; target death/lifesaving; no phantom XP |
| O3 | 20% per source; 5 turns; fast/natural regen and native magical healing separation; undead/construct targets; refreshing while active; polymorph preserves timer; unrelated revival/zombification and cooldowns unchanged; save and expiry; native world-turn cadence independent of speed; off-level catch-up/final tick and suppressed natural-healing reconciliation including passive equipment regeneration and no retroactive off-level healing |
| O4 | 0/1/2/3 bonus ticks with seeded probabilities; hard three-per-source bound; multiple source sequences; each tick physical/enchantment/mitigation; no extra to-hit/element/procs; passive-contact once; early target death; XP exactly once; stress loop; strength/skill/native physical bonuses; weapon/unarmed/kick/natural attacks; no-physical-component exclusion; no poison/special-rider repetition |
| O5 | Threshold boundaries and positive HP prerequisite; normal 25%/20% vs eligible unique 15%/10%; riders/quest exclusions; Frenzy and elemental threshold contribution; DR/armor applied to qualifying damage but **not reapplied to successful fatal Executioner**; native lifesaving; consecutive source rolls; Vorpal/other fatal effect collision; no duplicate kill/drop |
| O6 | 30% per source; chosen intrinsic element among multiple; all four elements; 8-way diagonal, blocked corners, closed door/wall; 3-hop cap, per-hop 0.75 flooring, min1, full resistance stops, no friendly conduit; no re-hit within chain; two chains share secondary target but independently roll native elemental bases; primary elemental damage once total; primary death before subsequent chains; XP/loot, no status/inventory/proc cascades; physical killing blow before elemental stage still permits precommitted chains; source/form/impact snapshots remain valid |
| D1 | Hits **and misses**; 33% independent per source; simultaneous main-hand/offhand granting weapons counterattack separately with their respective weapons, no full dual-wield sequence per proc; several successes in one attack and several attempts in one monster action; adjacency/death/hero weapon conditions; lifesaving; attacker dies yet D3/D2 continue; native weapon elemental/passives; no Counterstrike recursion; explicit main-/offhand source weapon attribution; no full dual-wield replay per proc; stop later counterattacks on paralysis/lost weapon/death/lifesaving during sequence; distinguish incoming-hit lifesaving; preserve contacted-form passives |
| D2 | Qualifying positive HP melee at <=25% and survivor; >25% no activation; hostile vs peaceful/tame; ranged/environment exclusions; sequential per-source attempts after failures; exactly one success per episode; native teleport/no-teleport/control; hero swaps/HP rearm; D3 pre-teleport position; `#inspect`/save; active polymorph/human HP pool; reversion rearm; original and execution low-HP gates; same-square success; canceled control and invalid-destination fallback |
| D3 | 1d3 native adjusted tame monsters, `NO_MINVENT`; hero <=25% after qualifying damage; source-specific 30-turn windows; unique source/guardian IDs; 2 sources yield 2d3 when ready; normal expiry death/corpse/loot with lifesaving exception; killed guardians native corpses/loot; dead-early no reset; no instant D2-follow; level transitions; crowded terrain; `#inspect`/save; zero placements no window; partial placements no top-up; off-level deadline before action; no expiry XP; lifesaving clears expiry permanently and ordinary pet survives later windows |
| D4 | Eligible direct damage and status monster spells, spell level 0; magic/clerical/psionic and species-specific pathways; exclusions wand/breath/nonplayer; 20% independent attempts; cancel whole spell before reflection/MR; exact one 1d6*max(1,level) Pw restore to max; Step21 Mask interaction; save/visibility and no double cast; listed zero versus absent level; absent 6d6 range/mean and independent dice; listed-level multiplier of one die; cast-time eligibility even if a directed beam would miss; initial area includes hero; mixed caster benefit canceled; ordinary nearby summons excluded; fully cancel cloud creation; genuine beam spell eligibility; no existing-cloud absorption |
| U1 | Formula by DL1/5/6/10/11/80/81/200; known/unknown; all mechanical and magical trap codes; holes/trapdoors sealed; portal/vibrating square/special links protected; one turn valid; zero turn invalid; failure safe; multi-artifact rolls until one success; save/reload; active automatic replacement/inactive native behavior; known and unknown doors/containers; native inspection costs; no forced revelation; self-release/matching trap; pit vision and flight restoration; unrelated restraints remain |
| U2 | 5 charges, +1/100 turns sequential; `#invoke` menu/direction; horizontal long dig, down/up native effects; hardness/quest/shop; cancel and zero-charge free; valid ineffective use consumes 1 charge/1 turn; recharge with artifact unequipped/off level; independent charges and save; new source full; no banked credit at full; first spend starts interval; later spend preserves progress; real-time pause gives no recharge |
| U3 | 0/19/20/59/60 preservation thresholds; rank 10x/20x/30x; each artifact independent; corpse in inventory/on tile/adjacent reachable; nested container excluded; one-time persistent marker; fresh-only gate vs old/rotten/tainted; acid blob/troll/zombifiable; Rider/lizard/lichen excluded; no revival/zombie timing changes; ice 20x/40x/60x; icebox pause; eaten/rotted corpse semantics; failed apply free; repeated save/transfer/off-level; freshness C19/20, U39/40, B59/60; native independent rotten-food risk retained; one-unit split/one point; inspect shows preserved multiplier; incompatible merges blocked; exact partial-age preservation with saved fractional remainder and no merge laundering |
| U4 | Base-cost thresholds 100/101/200/201; common and genocide/charging/enchants/gain level; stack 1/N; BUC/unknown/unpaid; `q`/`r` vs throw/dip/alchemy; native effect once; no item duplication; multiple independent source rolls stopping on first conservation; canceled/native-nonconsume operations; save and billing; initial cancel versus committed consumption followed by target cancel; use charges survive item return; retained merchandise separate purchase; native effect once/retained original unit including one-unit stack; no extra bill per source |
| U5 | 10 charges, +1/10 turns, independent storage and regen; pool/moat/bog/puddle->ice->original, lava->floor; cursor 5-range diagonal/off-axis, LOS blocking by walls not monsters; 1/1/2 costs; canceled/invalid no turn/charge; occupant/trap/object and map updates; fire melts; no auto timer; quest/connectors/drawbridges/Water Plane protected; save terrain provenance; new source full; no banked full-capacity progress; off-level catch-up; external ice melting and recorded water subtype restoration; stale provenance cleanup; no recharge reset across transfers |
| U6 | 100% from every individually active source; monster theft of gold/equipped/unequipped/ordinary artifacts, seduction removal; ordinary non-theft attack effects unchanged; progression scripted transfer classification; no broad fire/cold/shock/wetness/curse immunity; item/slot integrity; different artifact activation modes; container/equip transitions |
| U7 | 25%, 2d3 turns, distinct eligible selection uniform not slot-weighted; 2 drain tentacles share one mute; pure vs mixed attacks; all spells of selected casting channel; other channels intact; passive floating eye/cockatrice/acid **after initial hit**; MR ignored; bosses policy; no fallback; timer refresh under multiple source successes; polymorph clears old mute/new can apply; monster-versus-monster and save; 6 then 2 remains6; no additive duration; gaze versus passive same effect separate; original contacted-form passive and prior mute state retained; native world turns and catch-up; non-attack innate teleport/summon/transform; standalone vs spell-channel Summon Nasties; cockatrice passive vs corpse; source-audited AI/variant handlers |

### 9.1 Cross-property regression matrix

- **O4 + O6 + O5:** all Frenzy bonus physical and primary elemental damage contribute before the Executioner threshold; secondary chain hops **do execute their approved elemental HP damage** but never trigger Executioner or recursively proc other new properties.
- **O5 + O1A/O1B:** target killed (including without/with lifesaving) produces correct selection; theft/disarm only if genuinely alive after native death handling.
- **O1B + O1A:** steal weapon -> no disarm; steal another entry -> eligible wielded weapon subsequently disarmed; never double-copy same object.
- **O1B/O1A + O2:** item changes happen before native polymorph equipment reconciliation.
- **O2 + U7:** old mute cleared by every actual species change, new mute can be applied to final form; no stale ability ID.
- **O2 + O3:** old regen suppression retained through form changes, new successful suppression refreshes to 5 turns.
- **O2 + O2:** repeated same-hit transforms use current form ceilings; no upward difficulty drift.
- **O6 + O6:** independent chain paths and damage after primary death; no stale primary pointer.
- **D1 + D3 + D2:** retaliatory kill alone does not cancel emergency qualification; both original and current low-HP/survival checks apply. Counterstrike healing cancels pending effects, and passive damage cannot newly qualify them. Guardians spawn before teleport; D2 episode cap is global.
- **D4 + Step21 reflection/Magicbane:** absorption only on supported original spell and cannot double-cancel or double-restore Pw.
- **U1 and U5 near quest connections:** never delete progression anchors, including trap/terrain type collisions.
- **U3 corpse aging with native resurrection and icebox:** preservation changes effective aging at 10x/20x/30x, not revival/zombification; native icebox revival behavior remains distinct. One-unit stack separation and transfer cannot reset age or markers.
- **Event-source changes:** source removed before its roll is ineligible; an artifact stolen or newly activated mid-hit cannot join later phases of that event; already-committed O6 chains follow their continuation contract.
- **D3 expiry/lifesaving/save:** acquired inventory drops through normal death; successful expiry lifesaving creates an ordinary surviving tame pet, which remains detached after reload and new summon windows.
- **Multiple different artifact grants:** at least two distinct activation conditions for same property (e.g., worn vs directly carried), independent rolls, appropriate removal; no phantom stacking inherited from former Issues 121-126.

### 9.2 Whole-game regression gates

- Preserve **all 23 Step 21 artifacts** and their approved overlapping properties, cooldown `#inspect` and property source bookkeeping.
- Preserve the Step 16-17 Forge and Step 18A/18B recipes, charged objects, wish generation, artifact uniqueness, ownership/equipment legality and all historical tests.
- Preserve Step 19 equipment and unique boss artifacts, Step 20 deep-dungeon monsters/attacks and the 200-level DoD, Castle/Medusa positions, branch entrances, portals and protected progression.
- Run native deterministic unit/integration suites, scenario-driven actual gameplay fixtures, save/reload before and after expiry, long-run randomized combat/trap/terrain/corpse fuzz tests, object identity/ownership invariants, Release x64 console + GUI builds and existing tile/glyph verification. Log any preexisting ancillary tile-check failures distinctly from new regressions.
- Produce a full verification matrix with counts and provenance of PASS/FAIL/BLOCKED/NOT RUN. No sign-off with known new FAIL/BLOCKED cases or silently omitted supported attack variants.

### 9.3 Proposed engineering sequence (not a request to implement immediately)

1. **P0 - Source and compatibility audit:** authoritative local HEAD, Step21 regression baseline, target event hooks, save format, source provenance, reference test matrix.
2. **P1 - Offensive property engine and O1A/O1B/O2-O6:** per-source independent activations, primary/secondary contexts, native fatal and inventory safety, deterministic combat fixtures.
3. **P2 - Defensive D1-D4:** interrupted multi-source counterattacks, active-HP qualification/rearm, partial D3 placement/windows, expiry death/lifesaving detachment, and full spell interception with listed/unlisted energy handling.
4. **P3 - Utility U1-U7:** trap/dig/terrain/consumption atomicity, artifact-specific proficiency/charges, corpse freshness/timers and per-monster selective ability mute.
5. **P4 - Artifact binding (separate user approval):** choose names and sources, required equip/carry/invoke conditions and monster/pet activation permissions **for every property on every artifact**, acquisition and balance through opportunity costs/availability. Engine test harness may temporarily grant properties in controlled fixtures without approving game distribution.
6. **P5 - Integration and final sign-off:** historical source guards, gameplay/save/renderer/Release x64 regressions, full verification matrix, donor attribution and implementation documentation. Do not commit an unapproved artifact assignment.

### 9.4 Remaining **non-gameplay** work and hard implementation gates

The 18 gameplay contracts, numbered conflict decisions and F1-F6 follow-up approvals are locked. T1-T8 have scope-level resolutions. The following **implementation verification requirements** are not permission to change their semantics:

- Verify exact source/struct compatibility and serialization layout at post-Step-21 local HEAD; decide implementation fields and save epoch. When binding O6 to an artifact whose own elemental component is not part of the actual melee hit, explicitly specify an eligible elemental damage source in the artifact design; do not silently invent one.
- Specify an implementation-safe API for independent per-artifact activation, order, per-source state, deterministic rolls and rollback after monster death or polymorph.
- Implement U3's approved fresh-only effective-age boundaries, 10x/20x/30x aging, known rot/taint integrity and stack separation; reconcile native rot-away/eating age without modifying preservation-independent hazards or revival/zombification.
- Reconcile U5 source ice terrain flags, original water subtype persistence and state on externally melted tiles.
- Map U7 eligible attack, passive, spellcasting-channel and **non-attack innate/special-AI** dispatch sites (including atypical imported monster attacks) without broadly canceling monsters.
- Audit D4 **cast-time** spell eligibility (including hero-directed misses, initial-area overlap and mixed offensive/buff spells) and the precise pre-reflection hook so no direct-spell class is missed or absorbed twice; ordinary indirect summoning remains excluded.
- Audit all quest/progression scripted item thefts, monster boss protections, untrappable connectors and terrain protection against the approved exclusions.
- Address race-safe item stack/inventory ownership and native shop/event accounting, D3 guardian lifetime/source-link cleanup and D2 global episode life cycle.
- Artifact bindings, exact activation conditions and monster/pet usability remain a **later user design decision**; no allocation is implied by this consolidated property engine.

**Result:** No additional gameplay exceptions have been identified solely on the basis of duplicate activation. All T1-T8 have explicit technical resolutions and F1-F6 are recorded. Implementation readiness still depends on the authoritative source audit, runtime validation and (for real game distribution) a separately approved artifact-binding catalogue.

---

## 10. Donor-source and native source register (inherited from initial review)

These are concrete source leads, including pinned code where verified. **The 'source' is a reference for a mechanism, not an instruction to copy its whole design or unrelated changes.** Codes correspond to properties above.

- **[S01] Disarm:** NetHackWiki, [Disarm](https://nethackwiki.com/wiki/Disarm); native bullwhip and SLASH'EM/Hack'EM technique. Also inspect [Item theft](https://nethackwiki.com/wiki/Item_theft).
- **[S02] Reaver behavioral donor:** [Reaver](https://nethackwiki.com/wiki/Reaver); [dNetHack Pirate/Reaver](https://nethackwiki.com/wiki/Pirate/dNetHack).
- **[S03] Reaver code donor:** [Hack'EM `src/artifact.c`, Reaver theft branch](https://github.com/elunna/hackem/blob/cebe2f36e1908e82665c01a30a9eef2f108d07d9/src/artifact.c#L3548-L3605). Specifically note first-monster-inventory-item selection and removal of `owornmask`/monster intrinsics.
- **[S04] Polymorphic Strike:** [Hack'EM Circe code, `src/artifact.c`](https://github.com/elunna/hackem/blob/cebe2f36e1908e82665c01a30a9eef2f108d07d9/src/artifact.c#L3238-L3268); [Hack'EM artifact descriptions](https://nethackwiki.com/wiki/Artifact_(Hack%27EM)). Inspect `newcham`/native polymorph eligibility rather than copying pig-only behavior or donor instant-kill fallback.
- **[S05] Anti-Regeneration:** [Mortality Dial](https://nethackwiki.com/wiki/Mortality_Dial); [Hack'EM `mon_regen`, `src/monmove.c`](https://github.com/elunna/hackem/blob/cebe2f36e1908e82665c01a30a9eef2f108d07d9/src/monmove.c#L264-L276); [Hack'EM corpse/revival handling `src/mon.c`](https://github.com/elunna/hackem/blob/cebe2f36e1908e82665c01a30a9eef2f108d07d9/src/mon.c#L3548-L3563).
- **[S06] Frenzy:** [Mouser's Scalpel](https://nethackwiki.com/wiki/Mouser%27s_Scalpel); [Hack'EM on-hit loop](https://github.com/elunna/hackem/blob/cebe2f36e1908e82665c01a30a9eef2f108d07d9/src/artifact.c#L3302-L3326).
- **[S07] Executioner:** [historical Mirrored Mask low-HP invocation](https://nethackwiki.com/wiki/The_Mirrored_Mask); [My-NetHack Vorpal/Tsurugi, available Phase 1 branch](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/artifact.c#L1629-L1710). `SPFX_BEHEAD` is not an Executioner HP-threshold mechanic.
- **[S08] Elemental chain:** [NetHack 5.0 Chain Lightning behavior](https://nethackwiki.com/wiki/Spellbook_of_chain_lightning); [My-NetHack Phase 1 `src/spell.c`, propagation and cast](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/spell.c#L1020-L1215). The native spell can propagate across many tiles and drain Pw, so a bounded artifact implementation must differ deliberately.
- **[S09] Counterstrike:** [dNetHack artifacts - Sansara Mirror](https://nethackwiki.com/wiki/dNetHack_artifacts); [dNAO `src/xhity.c` actual 50% counterattack](https://github.com/Chris-plus-alphanumericgibberish/dNAO/blob/d512426550448e944f381469e498423b656f2e74/src/xhity.c#L20062-L20125); [artifact registration](https://github.com/Chris-plus-alphanumericgibberish/dNAO/blob/d512426550448e944f381469e498423b656f2e74/include/artilist.h#L1785-L1797).
- **[S10] Reactive teleport:** [Teleportation](https://nethackwiki.com/wiki/Teleportation); [My-NetHack Phase 1 `src/teleport.c`](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/teleport.c). Compare current local Step 21 Cloak of the Unheld One invocation, which is deliberate rather than reactive.
- **[S11] Mirror Image:** [Displacement](https://nethackwiki.com/wiki/Displacement); no independently targetable decoy implementation verified in survey. Treat an actual multi-image system as new.
- **[S12] Spell Absorption:** [Magicbane](https://nethackwiki.com/wiki/Magicbane); [My-NetHack current local Step 21 implementation log](https://github.com/yarivk12/My-Nethack) describes Mirrored Mask source-specific spell redirection; use the **local** `src/mcastu.c`/`src/zap.c` before modifying spell order. Magicbane energy absorption on cancellation is not general incoming spell absorption.
- **[S13] Untrapping:** [Master Key of Thievery](https://nethackwiki.com/wiki/The_Master_Key_of_Thievery); [Untrap mechanics](https://nethackwiki.com/wiki/Untrap); [My-NetHack Phase 1 `src/trap.c` `dountrap`](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/trap.c#L5627-L5830).
- **[S14] Magical hazard/portal caveats:** [Magic trap](https://nethackwiki.com/wiki/Magic_trap), [polymorph trap](https://nethackwiki.com/wiki/Polymorph_trap), [level teleport trap](https://nethackwiki.com/wiki/Level_teleport_trap), [Magic portal](https://nethackwiki.com/wiki/Magic_portal). In particular, portals must not be removed as though they were ordinary hazards.
- **[S15] Wall Consumption:** [Digging](https://nethackwiki.com/wiki/Digging); [Spell of dig](https://nethackwiki.com/wiki/Spellbook_of_dig); [My-NetHack Phase 1 `src/dig.c`](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/dig.c).
- **[S16] Corpse Preservation:** [Ice box](https://nethackwiki.com/wiki/Ice_box), [Corpse aging](https://nethackwiki.com/wiki/Corpse); [My-NetHack Phase 1 `src/pickup.c` `Icebox` age/timer logic](https://github.com/yarivk12/My-Nethack/blob/7813481f704e323d00700934ec54082af67d3ebe/src/pickup.c#L2682-L2697), and removal/revival resumption around lines 2819-2835.
- **[S17] Item conservation analogy:** [Scroll of cloning](https://nethackwiki.com/wiki/Scroll_of_cloning); [SpliceHack rewrite `src/read.c` cloning handler](https://github.com/NullCGT/SpliceHack/blob/Spl-R-1.2.0/src/read.c#L2433-L2493). Native scroll and potion consumption is in `src/read.c` and `src/potion.c`; exact source-side conservation is **new**.
- **[S18] Terrain:** [Cold terrain freezing](https://nethackwiki.com/wiki/Cold), [Digging](https://nethackwiki.com/wiki/Digging); local native `src/zap.c`, `src/dig.c`, `src/trap.c`, `src/timeout.c` govern terrain and melting.
- **[S19] Inventory protection:** [Fire resistance](https://nethackwiki.com/wiki/Fire_resistance), [Cold resistance](https://nethackwiki.com/wiki/Cold_resistance), [Shock resistance](https://nethackwiki.com/wiki/Shock_resistance), [EvilHack theft/grease behavior](https://nethackwiki.com/wiki/Item_theft). Step 21 Nenya's waterproofing is locked in the local October 10 implementation log; do not overwrite it.
- **[S20] Ability muting:** [Cancellation](https://nethackwiki.com/wiki/Cancellation); local `src/zap.c` cancellation and `src/uhitm.c`, `src/mhitu.c`, `src/mhitm.c` combat dispatch, and monster `mcan` conventions. **Temporary attack-slot suppression is not donor cancellation.**
- **[S21] Forge and previous artifacts (local authority):** final Step 21 artifact scope from October 10; post-Step 21 implementation/verification log (reported commit `e61a290de`); Phase 1 Step 17 Forge/Affix Design FINAL. These are internal project authorities, not web donor links.


### Source-register caveat

These citations preserve the original dated donor/native research leads and pinned code references. They are not a new live verification of the current local Step 21 HEAD, which must be checked before implementation. In particular, v0.1 source mechanics such as donor 50% Counterstrike, pig-only Circe transformation, global Mortality Dial, native Chain Lightning branches, basic displacement and Magicbane cancellation are **not** imported as gameplay contracts. The authoritative replacements are Sections 2-7.

### 10.1 Local native findings used in the final review

These references were inspected in **Step 11 `7fa6dd11a`**, branch `phase0/dod-length`, at `H:/app/mynethack/YarivNetHack`. They establish the reviewed native mechanisms, not current Step 21 integration. Revalidate corresponding code and metadata before implementation. The review performed source inspection only; no new-property builds or gameplay tests were run.

| Topic | Baseline reference | Finding and scope relevance |
| --- | --- | --- |
| World turns versus actions | [allmain.c:212](H:/app/mynethack/YarivNetHack/src/allmain.c:212) | Movement processing separates extra speed actions from world-turn increments |
| Monster status maintenance | [mon.c:1241](H:/app/mynethack/YarivNetHack/src/mon.c:1241), [dog.c:681](H:/app/mynethack/YarivNetHack/src/dog.c:681) | Once-per-turn status updates and off-level elapsed-time catch-up, including pending final tick for relevant statuses |
| Artifact and object timing | [artifact.c:2291](H:/app/mynethack/YarivNetHack/src/artifact.c:2291), [timeout.c:2276](H:/app/mynethack/YarivNetHack/src/timeout.c:2276), [do.c:1888](H:/app/mynethack/YarivNetHack/src/do.c:1888) | Saved game-turn deadlines; overdue timers processed on level arrival |
| Active polymorph HP | [mhitu.c:2229](H:/app/mynethack/YarivNetHack/src/mhitu.c:2229) | Monster-form HP is separate; rehumanization can change the active HP pool |
| Separate strikes and passives | [uhitm.c:841](H:/app/mynethack/YarivNetHack/src/uhitm.c:841), [uhitm.c:5990](H:/app/mynethack/YarivNetHack/src/uhitm.c:5990), [uhitm.c:6464](H:/app/mynethack/YarivNetHack/src/uhitm.c:6464) | Separate weapon/natural subattacks, passive interruption, and some killing-blow passive consequences |
| Corpse age and food hazards | [eat.c:1904](H:/app/mynethack/YarivNetHack/src/eat.c:1904), [eat.c:1956](H:/app/mynethack/YarivNetHack/src/eat.c:1956), [eat.c:1966](H:/app/mynethack/YarivNetHack/src/eat.c:1966) | Random age-based sickness/taint and an independent rotten-food check; the approved gate uses earliest sickness boundaries deterministically |
| Non-corpse 30-turn check | [eat.c:3001](H:/app/mynethack/YarivNetHack/src/eat.c:3001), [eat.c:3044](H:/app/mynethack/YarivNetHack/src/eat.c:3044) | Corpse path is separate; the 30-turn rule is not a universal corpse freshness stage |
| Icebox aging and revival | [pickup.c:2655](H:/app/mynethack/YarivNetHack/src/pickup.c:2655), [pickup.c:2792](H:/app/mynethack/YarivNetHack/src/pickup.c:2792) | Stored elapsed age is preserved; revival handling has native species distinctions |
| Corpse splitting and merging | [eat.c:361](H:/app/mynethack/YarivNetHack/src/eat.c:361), [mkobj.c:469](H:/app/mynethack/YarivNetHack/src/mkobj.c:469), [invent.c:820](H:/app/mynethack/YarivNetHack/src/invent.c:820) | Unit splitting exists; native merges can average age and discard incoming timers, requiring preservation compatibility guards |
| Committed consumption | [read.c:1981](H:/app/mynethack/YarivNetHack/src/read.c:1981), [shk.c:1240](H:/app/mynethack/YarivNetHack/src/shk.c:1240) | Charging scroll may be consumed before target selection; native consumed merchandise retains debt; conserved-item billing follows explicit Q19 |
| Teleport completion | [teleport.c:487](H:/app/mynethack/YarivNetHack/src/teleport.c:487), [teleport.c:893](H:/app/mynethack/YarivNetHack/src/teleport.c:893) | Same-square teleport can release restraint; cursor cancellation and invalid-destination fallback are distinct |
| Listed/unlisted spell examples | [monsters.h:3554](H:/app/mynethack/YarivNetHack/include/monsters.h:3554), [mcastu.c:715](H:/app/mynethack/YarivNetHack/src/mcastu.c:715), [mcastu.h:32](H:/app/mynethack/YarivNetHack/include/mcastu.h:32) | Asmodeus direct cold lacks named spell level; Fire Pillar has level 12 |
| Whole spell versus hazard | [mcastu.c:1492](H:/app/mynethack/YarivNetHack/src/mcastu.c:1492), [mcastu.c:1576](H:/app/mynethack/YarivNetHack/src/mcastu.c:1576) | Genuine spell beam delivery and directed cloud creation require cast-time classification |
| Guardian expiry cleanup | [mon.c:3305](H:/app/mynethack/YarivNetHack/src/mon.c:3305), [mon.c:3538](H:/app/mynethack/YarivNetHack/src/mon.c:3538), [mon.c:3552](H:/app/mynethack/YarivNetHack/src/mon.c:3552) | Native death can lifesave; normal corpse/loot differs from inventory-destroying disappearance; use the approved D3 death contract |
| Q26/Q27 hooks | [trap.c:6138](H:/app/mynethack/YarivNetHack/src/trap.c:6138), [trap.c:6298](H:/app/mynethack/YarivNetHack/src/trap.c:6298), [trap.c:6407](H:/app/mynethack/YarivNetHack/src/trap.c:6407), [trap.c:7012](H:/app/mynethack/YarivNetHack/src/trap.c:7012) | Existing discovery/disarm paths and release/removal helpers support the bounded complexity assessment in Section 8.5 |

---

## 11. Final approval and handoff state

**Gameplay review complete:** This v2.1 scope integrates all 18 property contracts within 17 families, historical Issues 95-134, Q1-Q27, and follow-up approvals F1-F6 with technical closure T1-T8. It includes fresh-only 10x/20x/30x preservation, ordinary guardian expiry death with the permanent lifesaving-survivor exception, full spell absorption with 6d6 for unlisted levels, and automatic U1 disarming including Q26/Q27. The earlier v1.0 file is superseded for these contracts.

**Q26/Q27 and T1-T8 scope-closed:** Both U1 features remain included after a source-level complexity assessment. The technical resolution register supplies implementable contracts for the eight reviewed technical issues, not a claim of Step 21 integration or runtime PASS.

**Still reserved for artifact design:** Named artifact assignments, exact activation conditions for each property, monster/pet usability, appearance, rarity, availability, acquisition, distribution and balance through artifact opportunity cost. Forge compatibility or changes to the 23 Step 21 artifacts require separate approval.

**Implementation prerequisite:** Locate and audit the actual post-Step-21 checkout, its approved artifact set, regression baseline and save format. Step 11 source observations and this document do not establish that the new property engine is implemented or that Step 21 integration passes. No game source was modified or tested by this consolidation.


**Next work:** Use this integrated scope to prepare the implementation plan against the verified post-Step-21 source, or conduct the separate artifact-binding design pass. Confirm each T1-T8 resolution in native fixtures and the historical regression suites. Do not infer approval to implement, assign artifacts or change existing artifacts merely from this document; the current deliverable is an updated scope and verification contract.
