# Step 21 — Artifact Expansion: Final Implementation Scope

**Project:** My-NetHack (NetHack 5.0)  
**Baseline:** Completed Step 20  
**Status:** FINAL — All gameplay decisions locked; all 28 review findings dispositioned  
**Scope revision:** 2026-10-10; final decisions #3 A, #7 A, #8 B and 25 technical resolutions incorporated  
**Target:** 23 approved artifacts, including artifacts already implemented  
**Implementation stage:** Ready for development-prompt preparation

---

## 1. Objective

Expand My-NetHack's artifact catalogue to include the 23 approved artifact designs, emphasizing:

- Carried utility and passive effects.
- Exploration, detection, and mobility.
- Defensive alternatives to conventional artifact weapons.
- Pet management and summoning.
- Magical resource regeneration.
- Special item interactions and invocation abilities.

The expansion must integrate with the existing NetHack 5.0 architecture and the project's 200-level Dungeons of Doom.

The goal is to implement the approved gameplay mechanics, not to import unrelated donor systems, character roles, quests, branches, or dependencies.

The scope includes the existing Silver Key, which must be preserved rather than duplicated.

**Completion requires functional, validated gameplay for all 23 approved artifacts, not merely artifact-table registrations.**

## 2. Scope authority and implementation principles

### 2.1 Source precedence

When implementations or source references disagree, apply the following precedence:

1. Explicit final decisions in this document.
2. Previously approved artifact behavior explicitly incorporated here.
3. Existing My-NetHack implementations, where compatible.
4. Authoritative donor source code for technical details not overridden.
5. Native NetHack 5.0 mechanics for equivalent gameplay behavior.

This document is the self-contained, authoritative Step 21 gameplay specification. The three October 10 decisions in Sections 3.3, 3.20, and 3.21 supersede any conflicting donor behavior. No open gameplay decisions remain in this scope.

Donor implementations are technical references, not authorization to import unapproved powers.

### 2.2 Approved-power boundary

Only the abilities explicitly defined in this scope are approved.

Do not import additional donor abilities merely because they appear in an artifact's original definition.

In particular:

- Do not add the Tie-Dye Shirt's donor-only Charisma/Wisdom bonuses or special enchantment-cap exception.
- Do not add Nenya's additional donor combat or spellcasting bonuses.
- Do not add Vilya's additional donor combat or attribute-dependent mechanics.
- Do not add the Lyre's unrelated donor invocation abilities.
- Do not import generic mask polymorph functionality.
- Do not add unrelated artifact skill-cap bonuses, sanity, insight, or quest systems.
- Do not restore any abilities explicitly removed during the overlap review.

**Exception:** The Holographic Void Lily retains its full original SpliceHack gameplay capability, with no nerfs.

Existing base-item functionality remains available where expressly retained.

### 2.3 Mandatory source audit

Before implementation:

- Inventory the current artifact registry and all relevant base objects.
- Confirm the existing Silver Key implementation from Step 10.
- Determine whether any other approved artifacts already exist.
- Identify existing generic property, invocation, lighting, pet, equipment, and item-generation infrastructure.
- Audit artifact IDs, object IDs, enumeration limits, array capacities, serialization formats, and naming collisions.
- Audit existing artifact generation, sacrifice gifts, wishing, and uniqueness mechanisms.
- Identify reusable gameplay handlers.
- Record the authoritative donor implementation for each artifact, with repository, immutable revision, definitions, and call sites; distinguish verified donor source from approved local overrides.
- Establish which existing tests exercise the affected subsystems.
- Inventory cooldown-bearing items, native earthquake parameters, source-owned defensive properties, artifact stacking, ordinary base-object commands, and all Forge input/consumption paths.
- Establish per-artifact acquisition eligibility, aligned or role-restricted paths, and executable reachability evidence.

Do not create duplicate artifact records, base objects, or independent implementations of existing mechanics.

Preserve all completed Step 10, Step 19, and Step 20 functionality.

The target is **23 final artifact designs**, not necessarily 23 newly appended records.

### 2.4 Activation conditions

Activation conditions are fundamental parts of the approved design.

**Carried:** The artifact must be in the hero's direct, active inventory. Wearing or wielding a directly owned item also qualifies; keeping an artifact inside a carried container does not. Wearing or wielding is not required for an artifact's carried powers.

**Worn:** The artifact must occupy the applicable equipment slot.

**Wielded:** Requires the relevant wielding state.

**Mounted:** Requires the specified relationship between the hero, the saddle, and the currently mounted steed. An equipped saddle belongs to the steed's equipment/inventory and is not treated as an ordinary direct-carried property source. Its explicitly authorized mounted effects and invocation-selection exception are handled separately.

**Applied, read, or invoked:** Requires deliberate use of the corresponding native command.

Carried properties must not accidentally depend on an equipment slot.

Carried effects stop when the artifact is dropped, destroyed, transferred to another owner, or moved into storage that is not an active carried-property source.

Moving an artifact between carried and worn states must preserve any independently valid carried effects.

Permanent illumination has a separate world-location lifecycle and is not limited to the carried-property rule.

### 2.5 Donor compatibility

Where a donor feature depends on a subsystem that My-NetHack lacks, implement the approved gameplay outcome using existing local mechanisms.

Do not automatically import donor roles, races, quests, skill trees, monster templates, sanity, insight, or alternate progression rules.

For technical details not explicitly fixed here, follow authoritative donor behavior when compatible with the approved local mechanics.

Any required adaptation must be documented.

If an approved capability cannot be implemented using the current architecture, develop the smallest necessary compatible extension. Do not silently omit the capability.

Unresolved approved functionality is a completion blocker.

---

## 3. Complete artifact catalogue

### 3.1 The Silver Key

**Base:** Silver universal key  
**Status:** Already implemented in Step 10  
**Role:** Teleportation, polymorph control, and mobility

**Carried abilities**

- Teleport Control.
- Polymorph Control.
- Existing Energy Regeneration.

**Invocation**

- Preserve the existing branch-safe portal/mobility effect.

**Implementation requirements**

- Reuse the existing artifact registration and behavior.
- Preserve its R'lyeh acquisition rules and persistence.
- Preserve current portal and level-transition restrictions.
- Do not introduce an alternative implementation.
- Prevent bypasses of the Castle, Gehennom, quests, mandatory Invocation items, and ascension progression.

**Locked decision:** Retain both Teleport Control and Polymorph Control.

### 3.2 The Brass Lantern of Zork

**Base:** Brass lantern  
**Role:** High-value exploration and detection

**Abilities**

- Permanent illumination.
- Reveal secret doors in its illuminated area.
- Reveal concealed traps in its illuminated area.
- Reveal invisible monsters in its illuminated area.

**Light radius: 4 — LOCKED**

**Implementation requirements**

- The Lantern is permanently lit and consumes no fuel.
- Detection is automatic; repetitive searching commands are unnecessary.
- Use the native light-source system.
- Detection must respect actual illuminated squares, opaque terrain, and applicable visibility restrictions.
- Secret-door and trap discovery should use native discovery state.
- Reveal otherwise observable invisible monsters inside the Lantern's actually illuminated and observable squares, overriding only invisibility at those locations. This is not global See Invisible and does not grant vision through walls.
- Reevaluate revelation whenever the Lantern, monster, lighting, blindness, or observation state changes.
- Blindness and other visibility limitations remain effective unless explicitly overridden by the approved detection behavior.
- Permanent illumination continues when the item is placed on the floor.
- Preserve native container occlusion, world-location, and light-source cleanup behavior.
- Do not give other permanent-light artifacts the Lantern's enhanced detection abilities.

**Locked decision:** All three detection abilities remain, with radius 4.

### 3.3 The Book of Infinite Spells

**Base:** Artifact spellbook; reuse an appropriate local base or introduce the smallest compatible spellbook type  
**Role:** Renewable spell learning and flexible spellcasting

**Capabilities**

- Stores one current spell.
- The initial spell is randomly selected.
- Reading allows the stored spell to be learned using normal spell-learning rules.
- Reading does not exhaust or permanently deplete the artifact.
- Invocation offers two actions:
  1. Turn the page to obtain another randomly selected spell.
  2. Cast the current stored spell directly.
- Direct book-casting does not consume ordinary Pw and does not require the hero to have previously learned or memorized the current spell.
- Both invocation actions use the same artifact invocation cooldown.
- **Locked decision #3 A:** Only a deliberate Turn Page invocation changes the stored spell. Reading and direct casting never automatically reroll the page, including the donor's optional 5% after-use change.

**Implementation requirements**

- Use one authoritative stored-spell field.
- Restrict the spell pool to spells actually supported by My-NetHack.
- Do not import unavailable donor spells merely to match the original donor pool.
- Preserve normal relevant spell targeting, resistances, and effect resolution.
- Do not grant permanent knowledge of every spell simultaneously.
- Preserve normal learning behavior independently of book-casting; reading grants ordinary spell knowledge, while direct book-casting is available without that prior knowledge.
- Apply relevant native targeting, defenses, and casting restrictions other than the expressly waived ordinary Pw cost; do not accidentally impose the memorization prerequisite on direct book-casting.
- Maintain the current stored spell through save/reload.
- Use the existing invocation and spellcasting infrastructure.
- Preserve applicable native spellcasting restrictions except for the explicitly approved zero-Pw book-casting cost.
- Do not introduce unrelated donor spellbook systems.

**Final behavior — LOCKED:** One random initial supported spell. Deliberate Turn Page selects a random supported spell; no player-selected spell menu, and no automatic page changes after reading or casting. Both invocation actions share one cooldown.

### 3.4 The Lodestone of Gravity

**Base:** Loadstone or compatible heavy-stone relic  
**Role:** Complete defense against eligible physical missiles

**Carried ability**

- Deflect 100% of eligible incoming physical projectiles.

Examples of eligible attacks include:

- Arrows.
- Crossbow bolts.
- Darts.
- Thrown daggers and other thrown weapons.
- Boulders, when they are processed as ordinary incoming physical missiles by the local projectile-hit pipeline.
- Other projectiles classified as eligible physical missiles by the adopted local implementation.

**Explicit exclusions**

- Spell bolts.
- Wand rays.
- Elemental breath.
- Explosions.
- Area attacks.
- Other attacks not classified as eligible physical projectiles.

**Implementation requirements**

- Deflection is guaranteed for every eligible incoming physical projectile while the artifact is actively carried.
- Successfully deflected attacks cause no projectile-hit damage or hit-dependent contact effect to the hero, including poison carried by an eligible missile.
- Evaluate deflection before hit-dependent contact effects; preserve native misses, object disposition, breakage, and cleanup.
- Integrate with native missile-hit resolution.
- Handle monster-fired and thrown projectiles consistently.
- Preserve normal projectile disposition and cleanup.
- Do not create duplicate projectiles or invalid floor objects.
- Do not inadvertently impose the cursed loadstone's undesirable movement restrictions.
- Use the native projectile classification wherever possible. Explicitly test monster-fired, monster-thrown, trap-fired, and other existing physical-missile pathways that reach the hit-resolution hook.
- Do not extend protection to magical rays, explosions, elemental breath, general area effects, or non-missile splash effects merely because they originated from a throwable object.
- Do not implement generalized immunity to ranged attacks.

**Locked decision:** 100% eligible physical-projectile deflection.

### 3.5 Hellrider's Saddle

**Base:** Saddle  
**Role:** Mounted protection and invisibility

**While the hero is mounted using this saddle**

- Reflection for the rider.
- Reflection for the steed.
- Magic Resistance for the rider.
- Magic Resistance for the steed.

**Invocation**

- Toggle the hero's invisibility using donor-style activation.
- `#invoke` can select the Saddle while it is directly carried by the hero **or** equipped on the hero's currently ridden steed. Normal inventory selection alone is insufficient for the equipped case; extend the existing selector rather than introducing a separate command.

**Implementation requirements**

- Associate mounted defensive properties with the actual saddle/mount relationship.
- Remove mounted-only benefits when the relationship ends.
- Handle mounting, dismounting, steed death, polymorph, saddle removal, and switching steeds.
- Track the invoked invisibility state and its contributing source independently of the saddle's mounted defensive properties; dismounting must immediately remove the rider/steed defenses without automatically corrupting the separate invocation state.
- Reconcile that invocation state with the artifact's actual ownership, accessibility, removal, transfer, loss, and destruction. Never leave an orphaned invisibility grant or inadvertently suppress other valid sources.
- Preserve other valid invisibility sources when the invocation is toggled or the saddle state changes.
- Handle mounted defenses through the separate hero-targeted and steed-targeted combat paths; neither target may be protected only indirectly by the other's property flags.
- Do not permanently grant mounted properties to the hero or steed.
- Reuse native mounting and equipment-state infrastructure.

### 3.6 The Mirrored Mask

**Base:** Mask using the compatible face/eyewear equipment mechanism  
**Role:** Worn anti-magic protection and spell counterattack

**When worn**

- Reflection.
- Half Spell Damage.
- 50% chance to redirect an eligible hostile targeted spell back at its caster.

**Implementation requirements**

- Ordinary Reflection and targeted-spell redirection are separate mechanics.
- Apply the 50% check only to eligible targeted hostile spells. Perform that special check at the appropriate targeted-spell interception point before generic Reflection could suppress the Mask's independent counterattack.
- Once redirected, resolve the attack once against its caster with applicable defenses; do not then also resolve the original hit or enter reflection recursively. If not redirected, continue normal defense resolution.
- Test this ordering with another independent Reflection source, including the carried Holographic Void Lily.
- Do not redirect unrelated environmental effects or general area attacks.
- Apply appropriate normal defenses to the redirected target.
- Avoid recursive redirection and duplicate damage-resolution passes.
- Remove all worn-only benefits immediately when the Mask is removed.
- Preserve native helmet compatibility and face/eyewear conflicts.
- Do not import ordinary donor-mask polymorph functionality.

**Locked decision:** No nerf despite overlap with the Holographic Void Lily.

### 3.7 Tie-Dye Shirt of Shambhala

**Base:** T-shirt  
**Role:** Carrying capacity, status recovery, and enlightenment

**When worn**

Modifies the hero's normal carrying capacity according to beatitude:

| Beatitude | Carrying-capacity multiplier |
|---|---:|
| Blessed | 150% |
| Uncursed | 125% |
| Cursed | 75% |

**Read/use capability**

- Cure the donor-supported, locally representable ailments using the shirt's reading behavior and local status-cure routines. Do not infer new cures or restore lost ability scores.
- Preserve donor/native restrictions caused by covering armor, including any actually supported transparent-covering exception.
- Reading's prerequisites, ailment eligibility and outcomes must be captured in an explicit donor-verified condition matrix, including blindness and inability to read.

**Invocation**

- Enlightenment.

**Implementation requirements**

- Recalculate carrying capacity when the artifact is equipped, removed, blessed, cursed, or otherwise changes state.
- Apply the fixed 150% / 125% / 75% multiplier to the established native carrying-capacity value once, using native-compatible integer rounding and without compounding on repeated recalculation.
- Preserve native encumbrance thresholds and inventory-weight calculations.
- Implement the approved curing behavior using supported local conditions.
- Do not restore ability scores through the obsolete unicorn-horn behavior.
- Do not import donor-only Charisma/Wisdom bonuses or the special +7 enchantment-cap exception.
- Preserve normal wearable-shirt compatibility.
- Reuse the native enlightenment implementation.

### 3.8 Beastmaster's Duster

**Base:** Jacket/duster  
**Role:** Carried pet-management utility

**Locked activation rule:** Approved pet utility functions while the Duster is carried. Wearing is not required.

**Carried functionality**

- Pet-location assistance.
- Tameness management.
- Animal-care supply retrieval.

**Invocation**

Provide the donor-compatible supported choices:

- Retrieve a selected animal-care item.
- Alternatively locate pets and improve their tameness.

Supported supply options include:

- Magic whistle.
- Leash.
- Saddle.
- Tripe ration.
- Apple.
- Banana.

**Implementation requirements**

- All approved pet-management actions work while the Duster is carried.
- Do not require the jacket/duster to occupy an armor slot.
- Adapt donor menu and action handling so all supported choices work while directly carried; remove the donor-only requirement to wear the Duster.
- Pet location examines eligible living tame pets on the hero's current level; it does not search other levels. Eligible pets may be located regardless of distance.
- The donor tameness action increases tameness by **+1**, capped at **20**, for eligible pets that the hero can actually see. Merely detected, nonvisible pets are not automatically increased. Preserve pet identity and existing tame state.
- Supply retrieval creates one ordinary instance of the selected supported item with donor-compatible beatitude handling, subject to normal inventory/floor-placement behavior.
- Ensure generated supplies are valid local objects.
- Do not bypass artifact uniqueness or special-item generation restrictions.
- Use normal artifact cooldown behavior.
- Do not import a donor beast-mastery skill system.
- Preserve tameness, pet identity, and normal monster state.

### 3.9 Cloak of the Unheld One

**Base:** Oilskin cloak  
**Role:** Layered carried/worn defense and teleportation

**While carried**

- Sleep Resistance.

**Additional abilities while worn**

- Free Action.
- Magic Resistance.
- Magic Cancellation, using donor MC3 as the reference.
- Normal oilskin cloak protection.

**Invocation — LOCKED**

- Teleport the hero whether the Cloak is carried or worn.
- Wearing the Cloak is not required for invocation.
- Invocation does not teleport the Cloak itself.
- Preserve normal native teleport restrictions.

**Implementation requirements**

- Keep carried and worn property grants separate.
- Do not import donor artifact-self-teleport behavior.
- Do not permit donor-only interlevel travel that bypasses native teleport or progression restrictions.
- Preserve the existing oilskin/water-protection mechanics.
- Preserve ordinary cloak-slot interactions.
- Use native teleport resolution and standard invocation cooldown handling.

### 3.10 The Iron Spoon of Liberation

**Base:** Spoon or compatible weapon-tool  
**Role:** Utility, luck, and temporary wall phasing

**Carried abilities**

- Automatic Searching.
- Free Action.
- Luckstone-equivalent behavior using native Luckstone source/beatitude semantics, not an invented flat luck bonus.

**Tool functionality**

- Digging.
- Engraving.

**Invocation**

- Temporary phasing through walls for a donor-derived duration of **`50 + rnd(100)` turns (51–150 turns)**, using native time progression.

**Implementation requirements**

- Use existing digging and engraving handling.
- Preserve the appropriate local action requirements and costs.
- Use a single authoritative temporary-phasing state.
- Use the donor phasing duration fixed above; where compatible with the existing cooldown system, do not begin the subsequent invocation downtime until the active phasing period has elapsed.
- Respect existing conditions when phasing is already active and do not stack duplicate phasing states or cooldowns.
- Prevent entry into protected progression areas through unintended terrain bypasses.
- Handle phasing expiration safely, including cases where the hero is standing inside terrain that is normally impassable.
- Do not introduce unrelated donor combat bonuses.
- If a spoon base type is missing, add only the minimum necessary object implementation.

### 3.11 The Candle of Eternal Flame

**Base:** Magic candle  
**Role:** Permanent light, warning, resistance, and elemental summoning

**Carried abilities**

- Warning.
- Cold Resistance.

**Illumination**

- Permanently lit.
- Never burns out.
- Does not consume fuel.
- **Light radius: 3 — LOCKED.**

**Invocation**

- Summon one tame Fire Elemental.

**Explicit exclusion**

- No Teleport Control.

**Implementation requirements**

- Reuse the existing Magic Candle infrastructure where appropriate.
- Preserve ordinary Magic Candle behavior for non-artifact candles.
- Maintain permanent illumination through time passage, floor placement, inventory movement, level transitions, and save/reload.
- Use native light-source cleanup and location handling.
- Create a valid local tame Fire Elemental.
- Handle unavailable summon placement through existing monster-creation conventions.

### 3.12 The Ring of P'hul

**Base:** Ring, with exact compatible object type determined by audit  
**Role:** Sickness resistance and worn anti-magic protection

**While carried**

- Sickness Resistance.

**Additional abilities when worn**

- Free Action.
- Magic Resistance.

**Implementation requirements**

- Keep carried and worn activation separate.
- Carrying the Ring must not grant its worn-only powers.
- Avoid accidentally inheriting additional powers from an inappropriate base ring.
- Preserve ordinary ring-slot behavior.
- Use native sickness-resistance handling rather than importing a donor-specific disease system.
- Correctly handle removal, destruction, and independent defensive property sources.

### 3.13 Nenya

**Base:** Ring  
**Role:** Exploration, water protection, and magical energy regeneration

**When worn**

- Monster Detection.
- Waterproofing.
- Water Walking.
- Energy/Pw Regeneration.

**Implementation requirements**

- Use native monster-detection and visibility systems.
- Preserve correct interactions with pools, moats, and supported water terrain.
- Implement waterproofing through the existing item-water-damage pipeline.
- Protect precisely the inventory/worn-item scope already covered by the existing local item-water-damage pipeline; avoid independently extending that protection to nested container contents or unrelated effects.
- Keep Waterproofing, Water Walking, and Magical Breathing as distinct mechanics.
- Use native resource-regeneration infrastructure.
- Do not introduce an additional regeneration stacking multiplier.
- Do not import unrelated donor combat, attribute, or spellcasting bonuses.

**Locked decision:** Retain Energy Regeneration despite overlap with Glitterstone.

### 3.14 Vilya

**Base:** Ring  
**Role:** Survival, awareness, and HP regeneration

**When worn**

- Shock Resistance.
- Warning.
- Stealth.
- Magical Breathing.
- HP Regeneration.

**Implementation requirements**

- Use native HP regeneration.
- Integrate Magical Breathing with existing drowning and suffocation checks.
- Treat Warning and Stealth as normal property sources.
- Remove all worn-only effects upon ring removal.
- Do not import donor-only attack-study bonuses or additional attribute-dependent mechanics.
- Preserve normal ring-slot behavior and property-source accounting.

### 3.15 Ehecailacocozcatl

**Base:** Amulet of Magical Breathing  
**Role:** Breathing, reflection, and elemental summoning

**When worn**

- Magical Breathing.
- Reflection.

**Invocation**

- Summon one tame Air Elemental using an existing supported local monster type.

**Explicit exclusion**

- No Energy Regeneration.

**Implementation requirements**

- Preserve normal amulet-slot and breathing mechanics.
- Use the existing Air Elemental rather than introducing a separate wind-elemental monster.
- Use normal monster-creation, placement, and tameness handling.
- Do not reintroduce Energy Regeneration from the donor implementation.

### 3.16 The Talisman of Buer

**Base:** Amulet of Life Saving  
**Role:** Life saving, regeneration, sustain ability, and healing

**When worn**

- Native Life Saving.
- HP Regeneration.
- Sustain Ability.

**Invocation**

- Powerful healing.
- Cure supported ailments.

Donor reference: restore approximately half the hero's maximum HP and cure supported conditions such as sickness, blindness, and sliming.

**Critical lifecycle requirement**

**The Talisman is consumed when Life Saving triggers.**

**Implementation requirements**

- Use the native amulet-of-life-saving path.
- Consume the artifact instance after successful Life Saving.
- Remove all granted effects immediately.
- Never allow the consumed instance to trigger a second Life Saving.
- Preserve artifact uniqueness history after consumption.
- Do not automatically make another copy available through generation.
- Keep healing invocation independent of Life Saving until the item is consumed.
- Handle polymorph, simultaneous protective sources, and other relevant death-resolution states.
- Use the donor healing formula where compatible and document the exact implementation.

### 3.17 The Holographic Void Lily

**Base:** Credit card  
**Original donor:** SpliceHack  
**Role:** High-value carried defensive utility, regeneration, and summoning

**While carried**

- Reflection.
- Half Spell Damage.
- Fast Energy Regeneration, preserving its original SpliceHack behavior.

**Normal tool functionality**

- Preserve credit-card unlocking behavior.

**Invocation**

- Preserve the original SpliceHack monster-summoning ability.
- The donor generates up to 10 monsters around the hero.
- Preserve the original monster-selection behavior, initial disposition, placement restrictions, cooldown, and other relevant invocation limitations.

**Non-negotiable approval**

**Full original donor gameplay capability. No nerfs.**

**Implementation requirements**

- Do not weaken the carried Reflection or Half Spell Damage.
- Do not replace fast donor Energy Regeneration with a weaker generic implementation.
- Do not automatically convert the summoned monsters into tame elementals.
- Do not replace summoning with NerfHack's later card-drop/light invocation.
- Do not import the Cartomancer role or quest solely for acquisition.
- Preserve normal credit-card use.
- Respect artifact uniqueness and normal inventory behavior.
- Use the authoritative original SpliceHack source to verify all invocation and regeneration details, recording its exact immutable revision and handlers. Do not silently substitute the later NerfHack card-drop/light invocation or other derivative implementations.

The original donor's gameplay powers are preserved even though its role-specific acquisition is replaced by the approved My-NetHack acquisition policy.

### 3.18 The Mask of Tlaloc

**Base:** Mask  
**Role:** Carried elemental defense and worn water protection

**While carried**

- Cold Resistance.
- Shock Resistance.
- Magic Resistance.

**Additional ability while worn**

- Protection against water damage.

**Explicit exclusion**

- No Half Spell Damage.

**Implementation requirements**

- Carried elemental resistance and Magic Resistance remain active without wearing the Mask.
- Additional water protection requires the Mask to be worn.
- Use the same existing item-water-damage protection infrastructure as Nenya for the applicable inventory scope; do not grant Water Walking, Magical Breathing, or newly invented nested-container protection.
- Use the compatible face/eyewear equipment mechanism.
- Do not import generic donor-mask polymorph functionality.
- Do not restore the removed Half Spell Damage flag.

### 3.19 The Hand-Mirror of Cthylla

**Base:** Mirror  
**Role:** Detection, teleport control, magic resistance, and offensive reflection

**While carried**

- Automatic Searching.
- Teleport Control.
- Magic Resistance.

**Active ability**

- A special reflected attack that inflicts **15d15 damage** on a valid target that sees its reflection.
- When ready, the special attack may be activated through **either `#apply` or `#invoke`**, preserving the approved donor-style dual-command behavior.
- The special attack has one shared cooldown regardless of which command activates it.

**Self-targeting — LOCKED**

- The hero is a valid target when the required reflection and visibility conditions are satisfied.
- The hero can suffer the full reflected attack damage.
- No artificial protection against self-damage is introduced.

**Cooldown behavior**

- While the special attack is on cooldown, `#apply` performs ordinary mirror functionality.
- `#apply` and `#invoke` share one authoritative special-attack cooldown and may never reset or bypass each other's timer.

**Explicit exclusion**

- No Polymorph Control.

**Implementation requirements**

- Use native mirror targeting and visibility rules.
- Do not damage targets through walls or targets unable to see their reflection.
- Apply normal applicable defenses to the reflected target; verify and document the reflected attack's native damage classification before choosing any mitigation path. Do not invent an unconditional defense bypass.
- Preserve ordinary mirror behavior during cooldown.
- Display remaining cooldown through `#inspect`.
- Do not import donor-specific quest teleport bypasses.
- Do not import unrelated donor Energy Regeneration or other unspecified powers.

### 3.20 The Earth Crystal

**Base:** Crystal ball  
**Role:** Physical damage protection and terrain disruption

**While carried**

- Half Physical Damage.

**Invocation — LOCKED DECISION #7 A**

- Cause **one ordinary native earthquake** centered according to the existing NetHack 5.0 earthquake implementation, using the same level-dependent strength as a successful native Drum of Earthquake effect: **`(u.ulevel - 1) / 3 + 1`** with native integer arithmetic.
- Preserve applicable ordinary earthquake terrain, pit/fall, monster-waking, and other native side effects; do not require or consume a Drum of Earthquake charge.
- **No second donor earthquake and no donor-style mass reduction of monster HP.**

**Implementation requirements**

- Integrate Half Physical Damage with the standard physical-damage pipeline.
- Do not reduce damage classified as magical or nonphysical.
- Preserve established interactions with existing defensive affixes and other mitigation sources.
- Reuse or minimally expose the established native earthquake resolver at the specified native strength; do not recreate an incompatible parallel earthquake algorithm.
- Respect non-diggable terrain and protected structures, including progression-critical terrain restrictions.
- Do not import dNetHack's additional 75%-approximate mass monster-HP reduction, its second earthquake pass, or other donor-only effects.
- Prevent unwanted progression bypasses.
- Do not import unrelated elemental-crystal systems, collections, skill bonuses, or resurrection abilities.

### 3.21 The Glitterstone

**Base:** Amber gemstone  
**Role:** Energy regeneration, permanent light, and charging

**While carried**

- Energy Regeneration.
- Permanent illumination.

**Light radius: 2 — LOCKED**

**Invocation — LOCKED DECISION #8 B**

- Charge a supported magical item using **beatitude-dependent native strength**, with no race or role restriction: **blessed = blessed-strength charging; uncursed = ordinary charging; cursed = cursed charging**.
- Route these through the native recharge mechanism (equivalent recharge modes **`+1 / 0 / -1`** respectively), preserving native eligibility, overload, charge-count, and explosion behavior.

**Implementation requirements**

- Use native magical-item charging semantics.
- Preserve normal target eligibility, charge limits, recharge rules, and associated risks.
- Do not duplicate charges or bypass recharge restrictions.
- Do not retain the donor's gnome-only qualification for blessed-strength charging; beatitude alone determines the recharge mode.
- Use the normal invocation cooldown.
- Use the existing regeneration framework.
- Preserve permanent light during floor placement, inventory movement, and save/reload.
- Do not inherit the Brass Lantern's detection powers.

**Locked decision:** Retain Energy Regeneration despite overlap with Nenya.

### 3.22 The Storm Whistle

**Base:** Magic whistle  
**Role:** Fire resistance, teleport control, pet recall, and elemental summoning

**While carried**

- Teleport Control.
- Fire Resistance.

**Normal application**

- All ordinary Magic Whistle functionality.
- Teleport eligible pets to the hero using the native Magic Whistle implementation.

**Invocation**

- Summon one tame Water Elemental.

**Explicit exclusion**

- No Warning.

**Implementation requirements**

- Ordinary Magic Whistle use and summoning invocation coexist.
- Applying the Whistle must not accidentally trigger elemental summoning.
- Ordinary pet recall must not consume the summoning invocation cooldown.
- Use native pet teleportation restrictions and placement rules.
- Preserve existing Fire Resistance and Teleport Control.
- Do not restore Warning.

### 3.23 The Lyre of Orpheus

**Base:** Magic harp  
**Role:** Remote pet-following across dungeon levels

**While carried**

Eligible tame pets anywhere on the hero's current dungeon level can accompany the hero during a qualifying level transition.

Ordinary adjacency or proximity is not required.

**Supported transitions — ALL SIX REQUIRED**

1. Stairs.
2. Ladders.
3. Holes.
4. Trap doors.
5. Level teleportation.
6. Portals.

**Required behavior**

1. At transition initiation, identify candidate tame pets on the hero's current level.
2. Apply the existing native follow-eligibility rules, removing only the normal distance/proximity restriction.
3. Commit pet migration only once the hero's transition is actually committed; a canceled, failed, or denied level transition must not detach or duplicate pets.
4. Transfer eligible pets exactly once using existing monster migration and level-transition systems.
5. Place accompanying pets using normal safe-arrival behavior, including occupied or restricted destination handling.
6. Preserve pet identity, tameness, inventory, HP, statuses, and other persistent state.

**Restrictions**

- No teleporting pets during ordinary same-level movement.
- No retrieving pets from other dungeon levels.
- No resurrection of dead pets.
- No bypassing inability-to-follow restrictions unrelated to distance.
- No transferring hostile or merely peaceful monsters as tame pets.
- No global modification of ordinary pet-following behavior when the Lyre is absent.
- Do not bypass transition-specific inability-to-follow restrictions unrelated to distance or accidentally omit any of the six approved transition entry paths.

**Base-item compatibility**

Preserve supported normal Magic Harp behavior.

**Explicit exclusion**

- No Magic Resistance.

**Locked decision:** Complete anywhere-on-the-current-level pet-following capability for all six approved transition types.

---

## 4. Global item and artifact rules

### 4.1 Universal cooldown display

**This is a global item-system enhancement, not an artifact-only feature.**

Every identified item with an invocation or use cooldown must report its actual cooldown state through `#inspect`.

Examples:

`Cooldown: 37 turns remaining`

`Cooldown: Ready`

Where an item has multiple independent cooldowns, identify them separately.

**Requirements**

- Works for all eligible identified items, including existing non-artifact items.
- Reads authoritative gameplay state. Before implementing displays, inventory every existing and newly introduced invocation/use cooldown, its authoritative timer, and its genuine item-identification precondition.
- Updates correctly as game turns advance.
- Displays `Ready` when zero turns remain.
- Never displays negative remaining turns.
- Does not reveal unidentified special abilities; an identified base item is not necessarily a fully identified artifact or revealed special power.
- Does not invent cooldowns for items without cooldown mechanics.
- Does not confuse cooldowns with charges, fuel, or ordinary item age.
- Handles multiple cooldowns independently.
- Preserves cooldowns through save/reload.
- Does not change cooldown duration or activation rules.

Where an artifact has separate normal-use and invocation behavior, display only cooldowns that genuinely exist.

Do not implement a parallel artifact-specific cooldown display.

### 4.2 Property ownership and source tracking

Use native property/extrinsic infrastructure or a compatible centralized extension.

Properties must reflect their currently valid sources. Direct carried ownership excludes objects inside bags and other containers; a mounted Saddle uses an explicit hero/steed association rather than a false carried-property flag. World-located light sources and valid invoked toggles have their own source lifecycles.

Examples:

- Removing the Mirrored Mask removes its worn-only properties.
- Dropping the Hand-Mirror removes its carried Magic Resistance and Teleport Control.
- Removing the Mask of Tlaloc from the face does not remove its carried properties while it remains in active inventory.
- Removing Ehecailacocozcatl removes its worn Reflection.
- Losing the Lyre disables its remote pet-following functionality.
- Dismounting removes Hellrider's Saddle's mounted defenses.

Do not permanently modify intrinsics to simulate temporary artifact effects.

Recompute or update properties safely after inventory, equipment, mounting, polymorph, and destruction events. Route hero and steed defenses through their respective combat/equipment property contexts. Separate invocation source cleanup from binary mounted-property removal.

### 4.3 Multiple sources of the same property

Binary defensive properties do not multiply merely because several artifacts provide the same effect.

Rules:

- Multiple Reflection sources do not reflect twice.
- Two Half Spell Damage sources do not automatically quarter spell damage.
- Multiple Magic Resistance sources do not create extra resistance tiers.
- Multiple standard regeneration sources do not introduce a new additive multiplier.
- Existing stronger or specialized regeneration behavior must not be unintentionally weakened by source removal or source ordering.

Removing one source must preserve a property still granted by another source.

Preserve the existing native semantics for nonbinary or specialized sources. Validate independent-source removal in both orders, including Mirrored Mask plus Void Lily Reflection/Half Spell Damage; Nenya, Glitterstone, Silver Key and Void Lily energy regeneration; and Earth Crystal physical halving combined with Step 16 defensive affixes. Preserve the local damage-mitigation order.

### 4.4 Artifact invocation handling

Use the established artifact invocation infrastructure wherever practical.

Each invocation must:

- Validate the correct artifact.
- Verify that the artifact is accessible and eligible for use, including the explicit mounted Hellrider's Saddle selector exception.
- Validate targets and action-specific restrictions; dual entry paths to a shared active ability must use the same real cooldown.
- Honor the authoritative cooldown.
- Apply native turn and resource costs.
- Handle user cancellation consistently.
- Preserve applicable resistances and terrain restrictions.
- Use native spell, summon, healing, or charging handlers where possible.
- Avoid partial effects on failure.
- Preserve persistent activation state through save/reload.

Do not introduce independent invocation timers for operations sharing one native cooldown.

Do not assume that an unsuccessful action should consume or avoid a cooldown without checking the established invocation semantics.

### 4.5 Elemental summoning

The following summons are fixed:

| Artifact | Summoned monster |
|---|---|
| Candle of Eternal Flame | One tame Fire Elemental |
| Storm Whistle | One tame Water Elemental |
| Ehecailacocozcatl | One tame Air Elemental |

Use existing local monster types, tameness, creation, and placement mechanics.

Failed creation must not leave partially initialized monsters or duplicate summons.

Use native invocation failure and cooldown conventions.

**The Holographic Void Lily is separate.** Preserve its original SpliceHack monster-summoning behavior rather than forcing it through the three-elemental template.

### 4.6 Permanent light sources

Approved illumination values:

| Artifact | Radius | Additional light behavior |
|---|---:|---|
| Brass Lantern of Zork | 4 | Reveals secret doors, traps, invisible monsters |
| Candle of Eternal Flame | 3 | Permanent light without fuel consumption |
| Glitterstone | 2 | Permanent light without fuel consumption |

All radii use normal engine distance and illumination calculations.

Permanent light survives:

- Ordinary time passage.
- Inventory movement.
- Floor placement.
- Level transitions.
- Save/reload.
- Item-location changes supported by the native light system.
- Transfer into/out of containers, transfer to monster inventories, unloading/reloading a dungeon level, and ordinary object removal or destruction.

Native container occlusion and world-location rules still apply. Never leave duplicate/orphaned light sources, emit light from an occluded container contrary to native rules, or leak the Lantern's detection through unexplored, unobservable terrain.

Carried-only passive properties remain governed by active-inventory rules even where illumination persists on the floor.

### 4.7 Mask equipment compatibility

Both the Mirrored Mask and Mask of Tlaloc use the compatible face/eyewear equipment mechanism.

Required behavior:

- Masks occupy the face/eyes equipment slot.
- Masks conflict with eyewear such as blindfolds and lenses.
- Masks can coexist with helmets.
- Ordinary donor-mask polymorph behavior is not imported.
- Existing equipment and property handling must remain intact.

Audit current support before adding objects or equipment hooks.

Prefer a minimal implementation within the existing equipment model instead of introducing a new major equipment slot.

### 4.8 Forge and ordinary enhancement restrictions

Preserve the existing project rules that exclude artifacts from ordinary Forge enhancement systems.

Artifacts cannot receive:

- Ordinary Quality enhancements.
- Ordinary affixes.
- Ordinary sockets or socket gems.
- Ordinary equipment-salvage processing.

Forge recipes must neither duplicate **nor consume artifacts as ordinary ingredients**. Reject artifacts at both target selection and every material/input-consumption path, including transformation recipes and salvage.

Distinguish ordinary enchantment from Forge-specific enhancements.

Native enchantment remains available where permitted by the existing object and artifact rules.

Do not import unapproved donor-only enchantment exceptions.

### 4.9 Generation and acquisition — FINAL DECISION

**Use My-NetHack's existing artifact generation and acquisition mechanisms.**

No independent acquisition system is created for these artifacts.

**Requirements**

- The Silver Key preserves its established R'lyeh acquisition.
- All 22 additional artifact designs must be obtainable through applicable existing My-NetHack mechanisms.
- Reuse existing artifact generation, artifact gifts, wishing, and other supported acquisition pathways according to their current rules.
- Remove dependencies on donor-exclusive roles or quests where necessary to make the approved artifacts eligible for the local framework.
- Preserve native uniqueness, alignment, artifact-generation history, gift restrictions, and wish restrictions where applicable.
- Do not assume an artifact is wishable merely because it exists.
- Do not invent individual guaranteed locations, independent spawn probabilities, or depth thresholds.
- Do not create role-specific quests or classes for artifact distribution.
- Avoid creation through unsupported polymorph, Forge, object-conversion, or container-generation paths.
- Ensure no approved artifact is registered but permanently unreachable in normal gameplay. For Step 21, reachable means obtainable by **at least one legitimate character configuration** through an actually executable, permitted native pathway; it does not imply that all roles/alignments can obtain every artifact.
- Do not bypass normal rarity, alignment, wishing, artifact history, or class rules solely to make every artifact available to every hero.

**Mandatory acquisition audit**

For every artifact, document:

1. Whether it already exists.
2. Its artifact-generation eligibility.
3. Its supported acquisition pathways.
4. Relevant alignment or wish restrictions.
5. Uniqueness behavior.
6. Evidence that the artifact is reachable through the selected native mechanism, including a practical deterministic acquisition fixture or test proving eligibility without relying on rare random rolls.
7. Whether the underlying base object can be found or used without introducing unrelated ordinary generation-table changes.

Where necessary, register a new artifact as eligible in an existing generic acquisition mechanism without creating a new probability system.

If current acquisition architecture cannot support a particular approved artifact, report that as an implementation blocker instead of inventing unapproved gameplay.

### 4.10 Base item compatibility

Reuse compatible existing base objects wherever practical.

Where a required object is absent:

- Perform an object-table audit first.
- Add the smallest necessary base object.
- Preserve native item class, appearance, material, weight, equipment slot, and serialization conventions.
- Avoid inherited properties not included in the approved artifact.
- Avoid importing entire donor object families.
- Integrate with inventory, shops, identification, naming, and persistence.
- Record inherited ordinary properties and active commands to prevent unapproved artifact powers.
- Do not modify ordinary item-generation weights merely to make an artifact base type available; audit generic object paths separately.
- Ensure unique artifact objects cannot be merged into ordinary stacks, split into duplicates, or recreated through supported polymorph, container, wish, Forge, conversion, and ordinary object-generation paths.

For ring artifacts, verify that the selected base ring does not silently add unapproved ordinary ring properties.

For artifact spellbooks, tools, and gemstones, verify native object-class behavior before registration. Audit all supported `#read`, `#apply`, `#invoke`, charging, conversion, depletion, and consumption paths. The Book is never used up by reading, permanent-light artifacts cannot accidentally burn out, and ordinary functionality survives where expressly retained. Talisman Life Saving remains an explicit approved consumption exception.

### 4.11 Identification and inspection

Integrate the 23 artifacts with:

- Artifact identification.
- Artifact naming.
- Inventory representation.
- `#inspect`.
- Equipment and carried-property descriptions.
- Invocation availability.
- Existing encyclopedia entries where applicable.
- Discovery history.
- Save/reload persistence.

Identified artifacts must display accurate approved properties and activation requirements.

Removed abilities must not remain in any UI description, artifact definition, or property flag.

Unidentified artifacts must not leak hidden functionality through cooldown inspection or other informational commands. `#inspect` must use existing full-artifact-identification and power-disclosure semantics rather than assuming the ordinary base type is sufficient.

### 4.12 Persistence and compatibility

Preserve:

- Artifact identity and uniqueness.
- Base-object state.
- Current stored spell for the Book of Infinite Spells.
- Invocation timers.
- Active invocation state.
- Light-source state.
- Temporary phasing and other active effects.
- Property sources.
- Pet identities and migration state.
- Normal item ownership and inventory state.
- Distinct direct-carried versus contained ownership, equipped-Saddle invocation state, source-aware invisibility cleanup, and nonstackable artifact identity.
- Completed versus canceled pet migrations, including exactly-once transfers.

Update the project's EDITLEVEL/version gate when required by structural changes.

Validate save/reload using the new executable.

Historical save compatibility is not required for this development-stage project.

Bones-file compatibility is not a required Step 21 acceptance condition.

### 4.13 Progression and terrain safety

Artifacts must respect existing progression-critical movement and terrain restrictions unless explicitly approved otherwise.

Apply this consistently to:

- The Silver Key.
- Cloak teleportation.
- Iron Spoon wall phasing.
- Earth Crystal earthquakes.
- Other artifact-controlled movement and level transitions.

Do not bypass protected quest, Castle, Gehennom, Invocation, or ascension mechanics.

Avoid adding independent checks where an existing centralized restriction already handles the case. Build concrete positive/negative fixtures for protected quest transitions, Castle/Gehennom boundaries, mandatory Invocation prerequisites, protected terrain, and ascension-critical locations. Verify permitted movement and earthquake effects remain usable.

### 4.14 Base-item command compatibility

Preserve normal base-item behavior alongside artifact abilities.

Specific examples:

- Storm Whistle: `#apply` for pet recall; `#invoke` for elemental summoning.
- Lyre of Orpheus: retain ordinary Magic Harp functionality while carried remote-following remains passive.
- Hand-Mirror: when ready, `#apply` and `#invoke` both access the 15d15 special attack and share one cooldown; during cooldown `#apply` retains ordinary mirror behavior.
- Book of Infinite Spells: `#read` preserves learning without depleting the Book or changing its page; `#invoke` deliberately changes the page or casts the stored spell without ordinary Pw.
- Glitterstone: `#invoke` uses blessed/uncursed/cursed native recharge strength with no donor race restriction.
- Hellrider's Saddle: `#invoke` must be selectable when directly carried or equipped on the currently ridden steed.
- Iron Spoon: supported digging and engraving operations remain available.
- Void Lily: preserve ordinary credit-card unlocking functionality.

For each artifact, document which commands are supported and their cooldown, failure, cancellation, and turn-cost behavior.

---

## 5. Explicitly locked gameplay decisions

These requirements supersede differing donor behavior.

| Artifact / feature | Final locked requirement |
|---|---|
| Silver Key | Keep Teleport Control, Polymorph Control, existing Energy Regeneration, and established portal functionality |
| Brass Lantern of Zork | Radius 4; preserve all approved detection capabilities |
| Book of Infinite Spells | **#3 A:** Random initial spell; deliberate random page change only; **no automatic page change after reading/casting**; shared invocation cooldown; direct casting does not require memorization or ordinary Pw |
| Lodestone of Gravity | 100% deflection of eligible incoming physical projectiles while carried |
| Hellrider's Saddle | Mounted Reflection and MR for rider and steed; hero invisibility toggle accessible while directly carried or on the currently ridden steed, with independent source tracking |
| Mirrored Mask | Preserve Reflection, Half Spell Damage, and 50% targeted-spell redirection |
| Beastmaster's Duster | All approved pet-management functionality works while carried; current-level pet detection; +1 visible-pet tameness (max 20); one supported supply item per successful retrieval |
| Cloak of the Unheld One | Invocation teleports the hero whether worn or carried; no cloak-self-teleport |
| Iron Spoon of Liberation | Carried Searching, Free Action, native Luckstone-equivalent behavior; digging and engraving; 51–150-turn donor-based phasing |
| Candle of Eternal Flame | Radius 3; remove Teleport Control |
| Nenya | Preserve approved Energy Regeneration |
| Ehecailacocozcatl | Remove Energy Regeneration |
| Talisman of Buer | Consume artifact when Life Saving triggers |
| Holographic Void Lily | Full original SpliceHack gameplay capability; no nerfs |
| Mask of Tlaloc | Remove Half Spell Damage; retain Cold Resistance, Shock Resistance, MR, and worn water protection |
| Hand-Mirror of Cthylla | Remove Polymorph Control; preserve reflected 15d15 attack, including valid self-targeting; `#apply`/`#invoke` share one attack cooldown and ordinary apply works during cooldown |
| Earth Crystal | **#7 A:** Carried Half Physical Damage; one standard native, level-scaled earthquake (native Drum of Earthquake strength); no second quake or mass monster-HP reduction |
| Glitterstone | **#8 B:** Radius 2; preserve Energy Regeneration; blessed/uncursed/cursed native charging strength, with no race restriction |
| Storm Whistle | Remove Warning; retain Teleport Control, Fire Resistance, ordinary Magic Whistle use, and Water Elemental summon |
| Lyre of Orpheus | Remove Magic Resistance; preserve complete current-level remote pet following across six transition types |
| Universal cooldown inspection | Show actual remaining cooldown for every eligible identified item |
| Artifact acquisition | Use existing My-NetHack acquisition mechanisms; no new independent probability system |

**These are final implementation requirements, not invitations for further balance adjustments.**

---

## 6. Donor-source verification requirements

Some exact low-level mechanics remain source-derived rather than independently specified numerical changes.

These must be verified against authoritative donor code before implementation.

| Area | Required donor verification |
|---|---|
| Mirrored Mask | Targeted-spell classification, 50% redirection, and resolution order |
| Hand-Mirror | 15d15 damage, valid reflection, self-targeting, shared active-command behavior |
| Book of Infinite Spells | Supported spell pool, reading and casting handlers; approved #3 A **overrides** any donor random page changes after reading/casting |
| Tie-Dye Shirt | Capacity multipliers, status-curing behavior, and covering-armor checks |
| Talisman of Buer | Healing formula and supported condition removal |
| Cloak of the Unheld One | MC3 and relevant worn-property mechanics; approved local teleport behavior supersedes donor unworn behavior |
| Beastmaster's Duster | Supply menu/beatitude, same-level pet detection, visible-pet +1 tameness capped at 20; approved direct-carried activation overrides donor worn requirement |
| Holographic Void Lily | Original SpliceHack summoning, regeneration strength, and all relevant invocation limitations |
| Hellrider's Saddle | Invisibility toggle and source lifetime, selector access to the equipped saddle, and separate rider/steed property ownership |
| Lodestone of Gravity | Eligible physical-projectile classification and native handling; approved 100% deflection supersedes any differing donor probability |
| Lyre of Orpheus | Follow-eligibility and monster-migration integration |
| Permanent light artifacts | Exact native light radius, location/occlusion behavior, and Lantern-specific invisibility visibility handling |
| Iron Spoon of Liberation | Native Luckstone BUC rules and donor `50 + rnd(100)` phasing duration; invocation downtime and safe expiry |
| Earth Crystal | Donor `QUAKE` differs from approved #7 A: use one current My-NetHack native Drum of Earthquake-strength call; exclude donor extra earthquake and monster HP reduction |
| Glitterstone | Donor blessed charging depended on race; approved #8 B uses native `+1 / 0 / -1` by item BUC with no race/role check |
| Item base commands and acquisition | Verify native execution hooks, command prerequisites, artifact-specific disappearance/merging risks, and each real acquisition path |

**Donor verification rules**

- Prefer authoritative source code over secondary documentation.
- Record the donor repository and exact immutable commit/tag used. Record the local My-NetHack base commit and any later changes as well.
- Identify relevant definitions and gameplay handlers, including actual item read/apply/invoke/cooldown and monster-migration paths.
- Document differences between donor behavior and approved local behavior.
- Do not import excluded powers.
- Do not weaken the Void Lily.
- Do not silently substitute a different variant's implementation.
- Where native equivalents are used, verify that the approved gameplay result remains intact.

The final documentation must distinguish source-verified behavior from local adaptations. As one directly inspected technical reference (not an authorization to import other abilities), the dNetHack/dNAO implementation was examined at `Chris-plus-alphanumericgibberish/dNAO`, commit `d512426550448e944f381469e498423b656f2e74`, including `src/artifact.c`, `src/spell.c`, `src/detect.c`, and `include/artilist.h`. The current My-NetHack `src/music.c` implementation of a normal Drum of Earthquake was inspected on `yarivk12/My-Nethack` main at `3529330468d3b638a5397d51840e92a184766b17`. The full 23-artifact donor provenance matrix and complete local audit remain implementation deliverables; do not mark them source-verified merely because these examples were checked.

---

## 7. Implementation workstreams

### Workstream A — Audit, artifact registry, and base objects

- Inventory existing artifacts and supporting objects.
- Preserve the Silver Key.
- Assign noncolliding artifact and object IDs.
- Confirm registry and array capacity.
- Add required base objects minimally.
- Integrate masks with compatible face equipment.
- Build an immutable-revision artifact-to-donor mapping, explicitly recording every approved donor deviation.
- Establish executable acquisition eligibility and uniqueness, with one legitimate obtainable path for each artifact.
- Audit artifact object stacking, base-item consumption/transformation, and existing Forge recipe ingredients.
- Audit persistence and EDITLEVEL requirements.

### Workstream B — Passive property infrastructure

Implement or adapt:

- Carried resistance properties.
- Worn defensive properties.
- Mounted rider/steed defenses, sourced and resolved for each actor separately.
- Valid invoked invisibility and its independent source cleanup.
- Reflection.
- Half Spell Damage.
- Half Physical Damage.
- Energy and HP regeneration.
- Searching.
- Warning.
- Teleport Control.
- Polymorph Control.
- Free Action.
- Water protection.
- Capacity modification.
- Luckstone behavior.

Prefer centralized, source-aware handling.

### Workstream C — Active abilities and item interactions

Implement or adapt:

- Guaranteed eligible physical-projectile deflection.
- Special permanent illumination and detection.
- Deliberate-only spellbook page changes, normal reading, and no-Pw book casting without memorization.
- Beatitude-controlled native magical-item charging independent of role and race.
- Targeted spell redirection.
- Hand-Mirror reflected damage and self-targeting.
- Healing and condition removal.
- Temporary wall phasing.
- Single native-strength Earth Crystal earthquakes without donor mass HP reduction.
- Elemental summoning.
- Void Lily donor summoning.
- Pet supplies and tameness.
- Hero invisibility toggling.
- Cloak teleportation.

Reuse native action and invocation infrastructure.

### Workstream D — Pet transitions

Implement the Lyre's remote-following extension through existing monster-migration infrastructure.

Cover all six approved transition types with committed-transition gating and exactly-once migration.

Preserve standard follow eligibility, except for distance.

Do not modify normal pet-following behavior when the Lyre is absent.

### Workstream E — Universal cooldown inspection

Implement global `#inspect` cooldown reporting using actual gameplay state.

Support existing and new cooldown-capable items using a documented field/identification inventory.

Avoid artifact-only timer-display duplication.

### Workstream F — Generation, identification, and persistence

- Connect all 22 additional artifacts to supported native acquisition mechanisms.
- Preserve Silver Key acquisition.
- Verify uniqueness and generation accounting.
- Integrate inventory and identification.
- Preserve activation state through save/reload.
- Verify artifact compatibility with the existing Forge restrictions, including rejection as inputs/ingredients.
- Verify no artifact merging, splitting, accidental base-command consumption, or object-conversion duplication.
- Ensure artifact destruction and explicitly approved consumption are handled correctly.

### Workstream G — Documentation and regression integration

- Document all implemented abilities.
- Record source provenance.
- Extend native test coverage.
- Add targeted source/mutation guards where appropriate.
- Update the project roadmap.
- Produce the implementation verification matrix and closeout report, including the command/cooldown inventory, acquisition proof, property-source combinations, and gameplay decision overrides.

---

## 8. Mandatory validation plan

### 8.1 Artifact-by-artifact validation

Each of the 23 artifacts must have a validation matrix covering all applicable categories.

1. Artifact registration and identity.
2. Base-object compatibility.
3. Creation and uniqueness.
4. Acquisition reachability.
5. Carried properties.
6. Worn or wielded properties.
7. Mounted behavior, where applicable.
8. Normal item application or reading.
9. Invocation and cooldown.
10. Save/reload persistence.
11. Property removal when conditions cease.
12. Interaction with an independent source of the same property.
13. Invalid actions, failure, and cancellation handling.
14. Identification and `#inspect` behavior.
15. Regression against ordinary base-item behavior.

During development use **PASS, FAIL, BLOCKED, NOT RUN, or N/A** for each test.

N/A is permitted only for genuinely inapplicable mechanics and must be justified; NOT RUN means an applicable test has not executed, never that it is inapplicable.

Final acceptance requires **zero FAIL, BLOCKED, or NOT RUN** across all applicable mandatory tests. A successful build, source inspection, or an N/A without justification cannot substitute for a native runtime check.

### 8.2 Critical specialized tests

**Silver Key**

- Existing portal and teleport restrictions remain functional.
- Existing acquisition and persistence remain intact.
- No duplicate registration.
- No Invocation or ascension bypass.

**Brass Lantern, Candle, and Glitterstone**

- Exact illumination radii 4/3/2.
- No fuel depletion.
- Correct floor and inventory illumination.
- Correct save/reload behavior.
- Correct light-source cleanup.
- No unintended detection powers on Candle or Glitterstone.

**Brass Lantern detection**

- Detect secret doors.
- Detect traps.
- Reveal applicable invisible monsters.
- No detection beyond eligible illuminated regions.
- Correct discovery persistence and visibility restrictions.
- Invisible monsters reveal only in illuminated observable squares; moving Lantern/monster, blindness, and walls correctly change revelation; no global See Invisible leak.

**Book of Infinite Spells**

- Random initial spell.
- Random page changes.
- Valid supported spell pool.
- Correct spell-learning behavior.
- No ordinary book depletion.
- Correct no-Pw book-casting behavior even when the hero has never learned the current spell.
- **No automatic page changes** after reading or direct casting, including large deterministic samples; only deliberate Turn Page changes stored spell.
- Shared invocation cooldown.
- Stored-spell persistence.
- No unsupported spell or state corruption.

**Lodestone of Gravity**

- 100% deflection of eligible physical missiles.
- Correct handling of arrows, bolts, darts, thrown weapons and ordinary physical-missile boulders, including actual trap/monster firing paths where applicable.
- No secondary hit/contact status (such as projectile poison) after a successful deflection.
- No deflection of spells, rays, breath, explosions, or independent splash/area effects.
- No item duplication.
- No projectile cleanup failures.
- No unintended loadstone movement restrictions.

**Hellrider's Saddle**

- Rider and steed gain the correct defenses.
- Dismounting removes mounted-only properties.
- Saddle removal and steed death are handled.
- Steed changes and polymorph are handled.
- Invisibility can be invoked while the Saddle is directly carried or equipped on the currently ridden steed.
- Invisibility toggle behaves independently from mounted rider/steed defenses, including on mount changes, dismount, transfer, loss and save/reload.
- Verify steed-targeted attacks are protected separately from hero-targeted attacks.
- Other invisibility sources remain valid.

**Mirrored Mask**

- Worn Reflection.
- Worn Half Spell Damage.
- Correct targeted-spell eligibility.
- Deterministic verification of the 50% redirection decision.
- Correct caster defenses.
- No reflection loops, duplicate passes or suppression of the special 50% redirection by another generic Reflection source.
- Simultaneous Mirrored Mask plus Holographic Void Lily works without double-reflection.
- All worn benefits disappear when removed.

**Tie-Dye Shirt**

- Correct 150% / 125% / 75% carrying capacity.
- Correct recalculation on BUC and equipment changes.
- Native encumbrance behavior.
- Correct donor-verified ailment coverage, including explicitly excluded attribute restoration.
- Covering-armor restrictions and any verified transparent-covering exception, plus blind/otherwise-unreadable states.
- BUC multiplier applied exactly once using stable integer rounding.
- Enlightenment invocation.
- No unapproved attribute-restoration behavior.

**Beastmaster's Duster**

- Full pet functionality without wearing.
- Supply retrieval.
- Pet location.
- Current-level pet location, distant pets and no other-level retrieval.
- Visible eligible tame pet receives +1 tameness capped at 20; nonvisible pet does not gain tameness merely by being detected.
- Exactly one selected ordinary supply item and compatible beatitude behavior.
- Valid generated item types.
- Correct invocation cooldown.
- No duplication of restricted items.

**Cloak of the Unheld One**

- Carried Sleep Resistance.
- Worn Free Action, MR, and MC3.
- Normal oilskin behavior.
- Hero teleportation when worn.
- Hero teleportation when carried but unworn.
- No artifact-self-teleport.
- Native teleport restrictions.
- Correct property removal.

**Iron Spoon**

- Searching, Free Action, and luck while carried.
- Digging and engraving.
- Temporary wall phasing.
- Explicit 51–150-turn phasing-duration boundary coverage and cooldown behavior, including independent sources of phasing.
- Native Luckstone-equivalent BUC and independent source interactions.
- Safe resolution when phasing ends.
- No bypass of protected terrain or progression.

**Ring of P'hul, Nenya, Vilya, Ehecailacocozcatl**

- Correct carried versus worn property grants.
- Ring and amulet slot behavior.
- Water and breathing interactions, including scope of water protection, containers/nested contents, worn gear, and absence of unapproved Water Walking/Magical Breathing effects.
- Correct HP/Pw regeneration.
- Independent source preservation.
- Excluded powers remain absent.

**Talisman of Buer**

- Correct healing and supported status curing.
- Native Life Saving.
- Artifact consumed on Life Saving.
- Granted powers disappear afterward.
- No repeated Life Saving from the same instance.
- Artifact uniqueness history preserved.
- Polymorph and other relevant death-path interactions.

**Holographic Void Lily**

- Carried Reflection.
- Carried Half Spell Damage.
- Original fast Energy Regeneration.
- Credit-card functionality.
- Original SpliceHack monster-summoning behavior.
- Maximum summon count and donor restrictions.
- Correct summoned-monster disposition.
- Normal cooldown and failure behavior.
- No NerfHack card-drop replacement.
- No nerf to approved powers.

**Mask of Tlaloc**

- Correct carried resistances and MR.
- Worn-only water protection.
- Compatible face/eyewear slot.
- No Half Spell Damage.
- No unapproved mask polymorph behavior.

**Hand-Mirror of Cthylla**

- Correct target visibility.
- Correct 15d15 damage.
- Hero self-targeting.
- Shared special-attack cooldown across `#apply` and `#invoke`.
- Ordinary `#apply` mirror fallback during cooldown.
- Native applicable defenses and verified damage classification.
- No through-wall reflection damage.
- No Polymorph Control.
- No quest teleport bypass.

**Earth Crystal**

- Half Physical Damage.
- Correct native mitigation ordering.
- Exactly one ordinary native earthquake with the native Drum of Earthquake strength formula `(u.ulevel - 1) / 3 + 1`; no second quake.
- No donor-style mass reduction of monster HP; preserve native ordinary quake effects and monster wake-up behavior.
- Protected terrain preserved.
- No magical damage reduction.
- No unintended progression bypass.

**Glitterstone**

- Permanent light radius 2 and carried Energy Regeneration.
- Blessed/uncursed/cursed native recharge strengths map to `+1/0/-1` and honor target, recharge-count and explosion semantics.
- Same charging outcomes regardless of hero role or race; no donor gnome-only bonus.
- Cancellation and invocation cooldown follow native conventions.

**Storm Whistle**

- Ordinary Magic Whistle pet recall on apply.
- Tame Water Elemental summon on invoke.
- Separate normal-use and invocation behavior.
- Correct Fire Resistance and Teleport Control.
- No Warning.

**Lyre of Orpheus**

Test:

- Adjacent eligible pets.
- Distant eligible pets.
- Multiple distant pets.
- Stairs.
- Ladders.
- Holes.
- Trap doors.
- Level teleportation.
- Portals.
- Pets unable to follow.
- Pets on different levels.
- Dead pets.
- Hostile and peaceful non-tame monsters.
- Blocked or restricted arrival positions.
- Preservation of pet identity, state, and inventory.
- Save/reload after transitions.
- No duplicated pets or lost pets when a destination is blocked, transition is canceled, or transition fails.
- Atomic, exactly-once pet migration on all six transition paths.
- Ordinary following without the Lyre.
- No Magic Resistance.

### 8.3 Universal cooldown tests

- Existing artifact invocation cooldowns.
- New artifact invocation cooldowns.
- Existing non-artifact item cooldowns.
- Multiple independent cooldowns.
- Correct timer expiration.
- Zero shown as `Ready`.
- No negative values.
- Correct unidentified-item behavior, including known base objects whose artifact powers are not identified.
- Authoritative source-field inventory covering every existing and new cooldown-bearing item.
- Save/reload persistence.
- No artificial cooldown for items without timers.
- No change to the underlying gameplay cooldown.

### 8.4 Property-source regression tests

Test multiple independent sources of:

- Reflection.
- Magic Resistance.
- Half Spell Damage.
- Half Physical Damage where supported.
- HP Regeneration.
- Energy Regeneration.
- Teleport Control.
- Searching.
- Free Action.
- Relevant resistances.

Test source removal in both orders.

Verify that removing one artifact does not remove a property still granted by equipment, an intrinsic, or another artifact. Include explicitly: Mirrored Mask plus Void Lily, Silver Key plus Nenya plus Glitterstone plus Void Lily, and Earth Crystal plus existing defensive DR/affix sources. Validate native damage pipeline order.

Preserve existing nonstacking and mitigation semantics.

### 8.5 Global regressions

Confirm no regressions to:

- Existing artifact powers.
- Existing Magic Whistles.
- Existing Magic Candles.
- Existing artifact invocations.
- Ordinary mask/eyewear and helmet interactions.
- Ordinary rings and amulets.
- Native Light Sources.
- Ordinary pet following.
- Level transitions.
- Physical and magical damage handling.
- Reflection and Magic Resistance.
- Forge, affixes, sockets, and salvage.
- Item naming and identification.
- Item generation and artifact uniqueness.
- Existing spellcasting.
- Existing inventory operations, artifact merge/split rejection, and preservation of unique artifact identity.
- Base-item use/depletion/consumption and carried-versus-contained property sources.
- Forge recipe input rejection, not merely target restriction.
- Eligible artifact acquisition paths and role/alignment restrictions.
- Step 10 Neutral Quest progression.
- Step 19 equipment and monster content.
- Step 20 monsters and related integration.
- The 200-level dungeon structure.

### 8.6 Native validation gates

Use the project's established build and test infrastructure.

Required:

- Focused native fixtures for every new mechanic.
- Deterministic tests of probabilistic behavior.
- Source and mutation guards where appropriate.
- Relevant historical regression suites.
- Runtime save/reload tests.
- Relevant acquisition and uniqueness tests, including at least one deterministic permitted acquisition route for each added artifact.
- Explicit protected-location movement/earthquake positive and negative fixtures.
- Appropriate gameplay smoke tests.
- Release x64 build.

Do not substitute source inspection for native runtime validation where the mechanic can be tested.

Failure of an existing historical gate requires investigation and repair without silently removing prior assertions.

---

## 9. Out of scope

Step 21 does not authorize:

- New player roles or races.
- New donor-specific class quests.
- New dungeon branches.
- New mandatory boss progression.
- New summoning monster families where existing monsters suffice.
- Large spellcasting-system rewrites.
- Changes to existing Forge progression.
- New ordinary Quality, affix, or socket mechanics.
- General artifact-weapon rebalancing.
- Artifact crafting or artifact multiplication.
- New independent artifact-acquisition probability systems.
- Guaranteed artifact placements not approved here.
- Historical save migration.
- Changes to the 200-level Dungeons of Doom.
- Donor-specific sanity, insight, or skill-tree mechanics.
- Unapproved secondary donor powers.
- Automatic Book of Infinite Spells page changes caused by reading or casting.
- Donor-style additional Earth Crystal earthquakes or mass monster-HP reduction.
- Race- or role-restricted blessed Glitterstone charging.
- New mask-based polymorph mechanics.
- Unapproved changes to existing artifact enchantment rules.
- Further removal of powers solely because artifacts share a property.
- Any weakening of the Holographic Void Lily's original approved gameplay capability.

---

## 10. Required deliverables

The completed implementation must include:

1. A complete supported artifact registry representing the 23 approved designs exactly once.
2. Every approved passive and active gameplay feature.
3. All final locked gameplay decisions in Section 5 implemented exactly as specified, including #3 A, #7 A, and #8 B.
4. Universal cooldown inspection for all eligible identified items.
5. Minimal supporting base-object definitions where required.
6. Artifact identification, naming, inventory, and persistence integration.
7. Existing-framework acquisition eligibility for all newly added artifacts.
8. Automated native validation and regression tests.
9. A donor-source provenance matrix for all 23 artifacts, including immutable revisions, handler references, and approved local overrides.
10. An artifact verification matrix mapping abilities to implementation locations and test results, including every review-resolution acceptance fixture.
11. An acquisition eligibility/reachability matrix and deterministic acquisition evidence for every additional artifact.
12. A centralized cooldown-field/identification inventory and a base-item command/consumption matrix.
13. A detailed implementation document describing actual gameplay behavior.
14. Updated project roadmap and Step 21 development closeout summary.
15. Successful Release x64 build and applicable historical regression suites.

The closeout report must distinguish:

- Implemented behavior.
- Runtime-tested behavior.
- Source-verified donor behavior.
- Approved local adaptations.
- Tests marked N/A and their justification; applicable NOT RUN tests must be reported until executed.
- Any unresolved blockers.

Do not claim complete donor parity merely because matching flags or artifact definitions exist.

---

## 11. Acceptance criteria

Step 21 is complete only when all of the following conditions are satisfied:

- [ ] All 23 approved artifact designs are represented exactly once.
- [ ] The existing Silver Key remains compatible with Step 10.
- [ ] All 22 additional artifacts have valid, documented acquisition pathways.
- [ ] All explicitly approved powers function correctly.
- [ ] Book pages change only through deliberate Turn Page invocation; direct Book-casting works without memorization or ordinary Pw.
- [ ] Earth Crystal invokes one standard native level-scaled earthquake without extra donor mass damage.
- [ ] Glitterstone uses BUC-dependent native charging strength with no race/role restriction.
- [ ] All abilities respect their carried, worn, mounted, or active-use conditions.
- [ ] No removed donor ability has been accidentally restored.
- [ ] Lodestone deflects 100% of eligible physical missiles.
- [ ] Cloak invocation teleports the hero whether carried or worn.
- [ ] Hand-Mirror self-targeting works with the approved damage.
- [ ] Void Lily retains its full original SpliceHack gameplay capability.
- [ ] Lantern, Candle, and Glitterstone have exact light radii of 4, 3, and 2.
- [ ] Mirrored Mask retains Reflection, Half Spell Damage, and eligible spell redirection.
- [ ] Beastmaster's Duster operates without being worn.
- [ ] Talisman of Buer is consumed upon successful Life Saving.
- [ ] Storm Whistle preserves ordinary Magic Whistle pet recall.
- [ ] Lyre of Orpheus works for all six approved transition types with exactly-once migration only on committed transitions.
- [ ] Saddle invisibility can be invoked when equipped on the currently ridden steed; hero and steed defenses are resolved separately.
- [ ] Hand-Mirror dual-command behavior and shared cooldown work without suppressing ordinary apply while cooling down.
- [ ] Passive property sources are correctly tracked and removed.
- [ ] Multiple independent property sources behave correctly.
- [ ] Global cooldown inspection works for all eligible identified items.
- [ ] Artifact uniqueness is maintained.
- [ ] No Forge, enhancement, socket, item-generation, merge/split, artifact-consumption, or ordinary base-object regression remains.
- [ ] Every added artifact has a valid deterministic proof of reachability in the existing acquisition framework.
- [ ] Persistence and save/reload tests pass.
- [ ] All mandatory native runtime tests pass with zero FAIL, BLOCKED, or NOT RUN for applicable tests.
- [ ] Relevant historical regression suites pass.
- [ ] Release x64 build succeeds.
- [ ] Donor provenance, verification matrix, documentation, and roadmap are complete.
- [ ] No approved gameplay capability remains unsupported or untested without an explicit justified N/A classification.

**A successful build alone is not sufficient. Full completion requires correct gameplay behavior, native verification, and documented evidence.**

---

## 12. Implementation completion policy

The approved gameplay design is frozen for Step 21.

During development, implementation decisions may be made without additional gameplay review when they:

- Preserve the exact approved behavior.
- Use existing native systems.
- Avoid introducing new gameplay mechanics.
- Maintain compatibility with existing project functionality.
- Preserve donor behavior where this scope explicitly requires it.
- Do not contradict any locked decision.

Source verification may determine how a mechanic is implemented, but must not silently change what the mechanic does.

If implementation reveals a genuine conflict between two approved requirements, or a required behavior cannot be reproduced without a meaningful gameplay change, treat the issue as a blocker rather than inventing an alternative.

The development closeout must reflect actual verified results.

**Final scope status: APPROVED AND LOCKED — Ready for Step 21 development-prompt creation; gameplay decisions #3 A, #7 A, and #8 B integrated.**

---

## 13. Step 21 review-resolution traceability (closed)

All 28 findings from the post-approval review are dispositioned. This section is a **traceability index**; the operative requirements are integrated into Sections 2–12 above. It does not authorize any additional gameplay beyond those sections.

| Review item | Disposition | Primary specification location |
|---|---|---|
| #1 Saddle invocation accessibility | Technical resolution incorporated | §§3.5, 4.4, 4.14, 8.2 |
| #2 Saddle invisibility lifecycle | Source-aware technical resolution incorporated | §§3.5, 4.2, 4.12, 8.2 |
| #3 Book page-change behavior | **Gameplay A locked:** deliberate page change only | §§3.3, 5, 8.2 |
| #4 Hand-Mirror active commands | Dual `#apply` / `#invoke` and shared cooldown incorporated | §§3.19, 4.14, 8.2 |
| #5 Lodestone missile and contact effects | Physical missile classification and pre-hit deflection incorporated | §§3.4, 8.2 |
| #6 Mirrored Mask resolution ordering | One special interception and no reflection loops incorporated | §§3.6, 4.3, 8.2 |
| #7 Earth Crystal quake strength | **Gameplay A locked:** one native quake, no mass HP reduction | §§3.20, 5, 8.2 |
| #8 Glitterstone charging strength | **Gameplay B locked:** beatitude based, race independent | §§3.21, 5, 8.2 |
| #9 Iron Spoon phasing and Luckstone | Donor duration and native luck semantics incorporated | §§3.10, 5, 8.2 |
| #10 Water protection scope | Native inventory water-damage scope incorporated | §§3.13, 3.18, 8.2 |
| #11 Tie-Dye Shirt curing and capacity | Donor/source condition matrix and stable capacity arithmetic incorporated | §§3.7, 6, 8.2 |
| #12 Beastmaster's Duster effects | Current-level detection, +1 visible tame, supply retrieval incorporated | §§3.8, 6, 8.2 |
| #13 Lantern visibility override | Local illuminated-square invisible-monster revelation incorporated | §§3.2, 4.6, 8.2 |
| #14 Acquisition reachability | Existing-framework eligibility and deterministic proof incorporated | §§4.9, 7, 8.6 |
| #15 Donor revision pins | Immutable provenance and original SpliceHack priority incorporated | §§3.17, 6, 10 |
| #16 Lyre migration atomicity | Committed-transition and exactly-once migration incorporated | §§3.23, 7, 8.2 |
| #17 Saddle actor-specific defenses | Hero/steed combat paths independently covered | §§3.5, 4.2, 8.2 |
| #18 Active-carried definition | Direct inventory versus container and mounted exceptions clarified | §§2.4, 4.2 |
| #19 Artifact stacking | Unique instance/merge/split safeguards incorporated | §§4.10, 7, 8.5 |
| #20 Base-object selection | Audit inherited powers and avoid incidental ordinary-generation changes | §§2.3, 4.10 |
| #21 Light-source lifecycle | Occlusion, monster inventory, off-level cleanup and persistence covered | §§3.2, 4.6, 8.2 |
| #22 Universal cooldown inventory | Authoritative timer/identification audit integrated | §§4.1, 4.11, 7, 8.3 |
| #23 Mitigation/regeneration interactions | Independent-source combination and ordering tests incorporated | §§4.3, 8.4 |
| #24 Progression restrictions | Protected transition and terrain test matrix integrated | §§4.13, 8.6 |
| #25 Forge input handling | Artifact-target and ordinary recipe-input rejection included | §§4.8, 7, 8.5 |
| #26 Base-item consumption | Ordinary command paths and approved Talisman exception audited | §§4.10, 4.14, 8.5 |
| #27 Ambiguous locked-decision count | Corrected to all Section 5 locked decisions | §10 |
| #28 Test not-run status | NOT RUN added; required zero at final sign-off | §§8.1, 11 |

**Final scope status:** APPROVED AND LOCKED — no pending gameplay choices or unresolved scope-review decisions. Implementation, repository-wide source audits, and validation remain to be carried out against this specification. Any later discovered genuine incompatibility is an implementation blocker under Section 12, not permission to weaken a locked behavior.

