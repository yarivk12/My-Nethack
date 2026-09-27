# Phase 1 Step 17: Forge and Affix System

## Final Development-Ready Design Specification

## 1. Purpose

Step 17 introduces player-controlled Forge manipulation of the ordinary affix system developed during Phase 1.

The feature set is:

1. **Add Random Affix**
2. **Reroll Affix**
3. **Extract Affix**
4. **Imprint Affix**
5. **Salvage Item**

Step 17 also introduces two crafting-resource concepts:

- **Generic Essence**, a normal physical inventory resource.
- **Affix Essence**, an exact-affix resource stored in a persistent game-wide Forge ledger.

Step 17 must not become a second enhancement system. It is a transactional layer over the canonical enhancement catalogue and legality engine already established by Steps 13 through 16.

The following existing systems remain authoritative:

- affix definitions
- affix tiers
- item eligibility
- affix-family restrictions
- mutual exclusions
- item-specific restrictions
- ordinary-affix capacity
- artifact restrictions
- ammunition restrictions
- tool eligibility
- socket eligibility
- property effects
- rolled-value generation
- identification behavior where already defined
- enhancement lifecycle behavior

Forge code must query and mutate those systems through shared enhancement APIs rather than duplicating their rule tables.

---

# 2. Implementation Baseline

Step 17 must be implemented against the **actual clean, completed, and validated post-Step16C repository HEAD**.

Before implementation begins:

1. Record the current branch.
2. Record the exact commit SHA.
3. Confirm Step 16C is complete.
4. Confirm the working tree is clean.
5. Run the relevant Step 13 through Step 16 validation suite.
6. Audit the actual enhancement, object, socket, save, inventory, naming, Forge, generation, and transformation implementation before deciding physical storage layout.

The historical pre-Step17 repository snapshot previously inspected is research context only. It must not be treated as the implementation baseline if Step 16C has subsequently changed the repository.

Any conceptual function or structure names used in this specification are interface descriptions only. The implementation must bind them to the actual post-Step16C code.

---

# 3. Core Architectural Rule

## 3.1 Canonical affix slots

Ordinary affixes must have persistent slot identity.

A logical affix slot contains:

```text
slot identity
slot state
fixed tier, once created
current ordinary-affix identity, if occupied
persistent reroll history
```

The exact physical structure is intentionally not dictated by this design. It must be selected after auditing the post-Step16C object layout.

The slot representation becomes the **canonical logical representation** of ordinary affixes.

Any existing structures such as:

```text
o_enh_props
o_enh_values
o_enh_known
```

may remain where useful, but they must be derived from or centrally synchronized with the canonical slots.

There must never be two independently mutable representations of ordinary affix state.

All ordinary-affix mutation must pass through centralized enhancement helpers.

---

# 4. Affix Slot State Model

There are exactly three logical states.

| State | Tier | Affix | Reroll history | Consumes capacity |
|---|---:|---|---:|---|
| **Unused capacity** | 0 | None | 0 | No |
| **Occupied Affix Slot** | T1-T4 | Present | 0-10 | Yes |
| **Open Affix Slot** | T1-T4 | None | 0-10 | Yes |

## 4.1 Unused capacity

Unused capacity means that no slot exists yet.

It has:

```text
tier = 0
affix = none
rerolls = 0
```

Unused capacity:

- does not consume a created slot;
- has no salvage value;
- has no tier;
- has no identity visible to the player.

---

## 4.2 Occupied Affix Slot

An Occupied Affix Slot contains an ordinary affix.

It has:

```text
fixed tier = T1-T4
current affix = canonical property ID
reroll history = 0-10
```

Its tier never changes.

---

## 4.3 Open Affix Slot

**Open Affix Slot** is the canonical player-facing term.

An Open Affix Slot is created when an occupied ordinary affix is extracted.

It retains:

```text
slot identity
fixed tier
reroll history
```

It does **not** retain:

```text
the identity of the previously extracted affix
the previous rolled numeric magnitude
the previous property-specific runtime state
```

An Open Affix Slot:

- consumes affix capacity;
- prevents item stacking;
- has salvage value;
- grants no gameplay property;
- does not count as an active affix;
- cannot change tier;
- persists through save/restore;
- is always fully identified;
- may remain open indefinitely.

The player always knows an Open slot's:

```text
slot position
fixed tier
reroll history
```

Open-slot state does not require separate knowledge or identification metadata. This is intentional because Open slots are created through the player's deliberate Extract operation on an already identified ordinary-affix state.

If no legal property currently exists for an Open Affix Slot, the slot remains valid. Add Random and Imprint simply remain unavailable for that slot until the item's state changes sufficiently to make a property legal.

There is no "last extracted affix" field.

Therefore:

- Add Random may roll the same property that was previously extracted.
- Imprint may restore the same property that was extracted.
- No previous-property history is required.

---

# 5. Slot Identity and Ordering

Slot identity is stable.

Slots must never be compacted or reordered because one becomes Open.

Example:

```text
Slot 1: Vampiric II
Slot 2: Caustic III
```

After extracting Slot 1:

```text
Slot 1: Open, Tier II
Slot 2: Caustic III
```

Slot 2 must not move into Slot 1.

New slots always use the first unused canonical slot position.

Natural multi-affix generation must assign slots deterministically at the time each affix is acquired.

Slots must never later be reconstructed by alphabetically sorting properties or by sorting property IDs.

UI sorting must not change underlying slot identity.

---

# 6. Affix Capacity

Capacity is based on **created slots**, not active properties.

```text
created_slots =
    occupied slots
  + open slots
```

```text
active_affixes =
    occupied slots only
```

An Open Affix Slot therefore:

- consumes capacity;
- does not provide an affix effect.

Step 17 must use the canonical Phase 1 affix-capacity function. The implementation must not hard-code `2` even if the current practical maximum is two.

---

# 7. Immutable Slot Tier

Once a slot is created, its tier is permanent.

No Step 17 operation may:

- increase the tier;
- decrease the tier;
- reroll the tier;
- convert an Open slot to unused capacity;
- transfer the slot's tier to another slot.

The allowed state transitions are:

```text
Unused
   |
   | Add Random
   v
Occupied

Occupied
   |
   | Reroll
   v
Occupied, same tier

Occupied
   |
   | Extract
   v
Open, same tier

Open
   |
   | Add Random / Imprint
   v
Occupied, same tier
```

---

# 8. Phase 1 Global Enhancement Invariants

Step 17 introduces invariants that must be audited across **all Phase 1 code**, not only new Forge code.

## 8.1 Enhanced items cannot stack

An item containing an ordinary affix cannot stack.

---

## 8.2 Open Affix Slots prevent stacking

Any item containing **any created affix slot**, occupied or Open, cannot stack.

This remains true even when every created slot is Open.

---

## 8.3 Socketed items cannot stack

Any item containing socketed gems cannot stack.

---

## 8.4 Ammunition exclusion

Ammunition is completely excluded from:

- ordinary enhancements;
- affix slots;
- sockets;
- Add Random;
- Reroll;
- Extract;
- Imprint;
- Salvage.

This must be enforced consistently across the complete Phase 1 implementation.

---

## 8.5 Generated stacks

Objects generated or dropped as stacks must not receive:

- ordinary enhancements;
- created affix slots;
- socketed gems.

Example:

```text
6 daggers
```

must remain an ordinary stack rather than generating a shared enhancement or socket state.

---

## 8.6 Forge target stack rule

**Salvage Item is the only Forge target operation that accepts a stack.**

All other Forge targets must satisfy:

```text
quan == 1
```

This includes Step 17 affix manipulation and Forge targets used by existing Forge systems.

This restriction applies to the **target item**, not crafting materials.

Valid stacked materials include:

- Generic Essence
- gemstones where existing socket/recipe behavior permits
- other existing Forge recipe materials where already supported

---

## 8.7 Invariant failure handling

If the game somehow encounters:

```text
quan > 1
```

together with:

```text
ordinary affix slots
or
socketed gems
```

that is a broken Phase 1 invariant.

Forge must reject the object rather than attempting to interpret duplicated enhancement state.

Tests and debug builds should expose the invariant violation.

---

# 9. Artifact and Protected Item Rules

Artifacts are completely excluded from Forge crafting participation for Step 17.

An artifact cannot be used as a Forge target, crafting source, ingredient, donor, or output-modification target for:

- Forge an item
- Socket gemstone
- Add Random Affix
- Reroll Affix
- Extract Affix
- Imprint Affix
- Salvage Item

The existing Forge activation method remains unchanged. A war hammer that is itself an artifact may still be used solely to activate/open the Forge interface if the existing activation logic recognizes it as a war hammer. Activation does not make the hammer a crafting target, source, ingredient, or donor.

This artifact exclusion must be audited across the complete Phase 1 Forge implementation, including functionality created before Step 17.

Other unique, quest, or protected objects continue to follow the canonical Phase 1 restrictions already applicable to them.

Step 17 must not invent an alternative protection model.

---

# 10. Socket Separation

Ordinary affixes and socket-derived properties remain separate systems.

A property provided by a gemstone:

- is not an ordinary affix slot;
- cannot be Rerolled;
- cannot be Extracted;
- cannot create Affix Essence;
- cannot be Imprinted as a Forge affix;
- does not create reroll history.

Socketed gems continue to use the existing socket system.

Salvage destroys socketed gems together with the source item, but the gems contribute no Generic Essence.

---

# 11. Generic Essence

## 11.1 Definition

**Essence** is the generic Forge crafting material.

It is a normal physical inventory object.

Player-facing name:

```text
Essence
```

---

## 11.2 Properties

Generic Essence is:

- stackable;
- weight 1 per unit;
- always identified;
- permanently neutral/uncursed;
- not affected economically by B/U/C;
- not eligible for ordinary affixes;
- not eligible for sockets;
- not Salvageable.

Beatitude must not fragment otherwise identical Essence stacks.

If the object framework requires beatitude fields, Essence is normalized to the neutral/uncursed state on creation and remains in that state. Normal blessing, cursing, unblessing, or similar gameplay effects must not leave Generic Essence blessed or cursed. Any generic object-state path that touches Essence must preserve or immediately restore the neutral/uncursed invariant.

Forge behavior never applies a blessing or curse bonus or penalty to Essence.

---

## 11.3 Forge payment

Only **directly carried Essence in the main inventory** counts toward Forge payment.

Essence contained inside:

- a bag;
- a box;
- another carried container;
- any nested container

does not count until removed into the main inventory.

---

## 11.4 Object class

The implementation should choose the least disruptive existing object/material class after auditing the post-Step16C code.

Do not classify Essence as a gemstone merely because the theme is crafting.

Its implementation class should be selected for:

- clean inventory behavior;
- stacking;
- wishing;
- object generation;
- save behavior;
- minimal interference with unrelated mechanics.

---

## 11.5 Shops

Shops do not participate in the Essence economy.

Generic Essence:

- is not stocked by shops;
- is not purchased by shopkeepers;
- has no player-facing resale economy.

---

# 12. Natural Generic Essence Generation

Generic Essence may appear as rare dungeon loot.

For eligible random object-generation events:

```text
1% chance to generate Essence
quantity: 1-2
```

Rules:

- no dungeon-depth scaling;
- available from the beginning of the game;
- no guaranteed Essence placement;
- no dedicated Essence rooms or piles;
- Salvage remains the intended primary supply.

The 1% proc is a **replacement**, not an additive bonus object. For an eligible general random-loot generation event, a successful Essence proc replaces the ordinary random object that event would otherwise have produced and generates 1-2 Essence instead. It must not create an extra object in addition to the normal result.

The proc applies only to the canonical **general random-loot generation path**. It must not intercept:

- explicit or specific-object creation;
- class-constrained creation whose caller requested a particular object class;
- scripted or guaranteed placements;
- quest/special-level fixed rewards;
- Forge recipe outputs;
- wishes;
- shop-stock generation.

If the canonical general random-loot helper is also used for otherwise ordinary container loot, that use remains eligible unless the post-Step16C architecture has a more specific canonical distinction. The implementation should bind this rule at the narrowest shared general-random-loot point that preserves these exclusions.

The implementation must integrate this through the canonical object-generation architecture rather than creating unrelated special placement logic.

---

# 13. Wishing for Generic Essence

Generic Essence is wishable.

Requested quantity behaves as follows:

| Requested amount | Result |
|---:|---|
| unspecified | 1 Essence |
| 1-5 | requested amount, 100% |
| 6 | requested amount 80%; otherwise 1 |
| 7 | requested amount 60%; otherwise 1 |
| 8 | requested amount 40%; otherwise 1 |
| 9 | requested amount 20%; otherwise 1 |
| 10+ | requested amount 0%; receive 1 |

A failed high-quantity wish always produces exactly:

```text
1 Essence
```

It never falls back to 2-5.

---

# 14. Affix Essence

## 14.1 Definition

Affix Essence represents an **exact ordinary affix identity**.

It is **not a physical object**.

It is stored in one persistent, game-wide Forge ledger.

Every Forge accesses the same ledger.

---

## 14.2 Identity

Affix Essence identity is the canonical existing property/affix ID.

Tier is derived from the canonical enhancement catalogue.

Do not create:

- a second affix-ID namespace;
- one object type per affix;
- redundant Forge-specific affix definitions.

---

## 14.3 Ledger

Conceptually:

```text
Affix Essence ledger:
    canonical affix ID -> count
```

Only positive counts need to be displayed.

There is no intended gameplay capacity limit.

Implementation storage should simply use a practical datatype appropriate to the finite enhancement catalogue.

No artificial gameplay cap is required.

---

## 14.4 Acquisition

Affix Essence is created only by:

```text
Extract Affix
```

One successful extraction produces:

```text
+1 exact Affix Essence
```

for the extracted ordinary property.

---

## 14.5 Consumption

Imprint consumes:

```text
1 matching exact Affix Essence
```

from the ledger.

If the count reaches zero, the entry disappears from the normal ledger view.

---

## 14.6 What Affix Essence preserves

Affix Essence preserves:

```text
exact property identity
tier implied by that property
```

It does not preserve:

```text
rolled numeric value
source item
source item type
quality
reroll history
socket state
property-specific runtime state
provenance
```

---

## 14.7 Availability

Affix Essence is:

- not carried;
- not dropped;
- not stored in containers;
- not naturally generated;
- not sold;
- not purchased;
- not wishable;
- not an inventory object.

It persists with the active game's Forge ledger through save/restore/recovery.

---

# 15. Affix Essence Naming

The ledger uses one stable canonical name per property.

Format:

```text
<canonical primary/prefix affix name> <Roman tier> Essence
```

Examples:

```text
Corrosive I Essence
Caustic II Essence
Discerning III Essence
Sanctified III Essence
Aegis-Bound III Essence
```

The canonical **primary/prefix name** is used even if the affix can normally render as a suffix on an item.

For example, an item may display a suffix form while the ledger continues to use:

```text
Corrosive I Essence
```

The tier is always shown explicitly in Roman numerals.

---

# 16. View Stored Affix Essence

Affix Crafting contains:

```text
View Stored Affix Essence
```

This is an informational operation.

It consumes:

```text
0 turns
```

Entries are displayed:

1. grouped by T1, T2, T3, T4;
2. alphabetically by canonical affix name inside each tier.

Each entry should show:

```text
canonical name
tier
stored count
short property description
```

Only positive counts need to appear.

Descriptions should reuse the canonical property/inspection description source wherever practical.

Forge must not maintain an independent description catalogue that can drift from actual behavior.

---

# 17. Existing Forge Activation and Menu

The existing Forge activation method remains unchanged:

```text
apply a war hammer while standing on a Forge
```

This activation check remains an invocation mechanic, not a crafting-target check. Therefore an artifact war hammer may activate the Forge if the existing war-hammer activation logic permits it; artifact exclusion applies to Forge crafting targets, sources, ingredients, donors, and modified outputs.

Step 17 expands the existing Forge menu rather than replacing it.

## Top-level Forge menu

```text
1. Forge an item
2. Socket gemstone
3. Affix Crafting
4. Salvage item
5. Leave Forge
```

`Forge an item` and `Socket gemstone` preserve the functionality already implemented in earlier Phase 1 steps.

---

## Affix Crafting submenu

```text
1. Add Random Affix
2. Reroll Affix
3. Extract Affix
4. Imprint Affix
5. View Stored Affix Essence
6. Back
```

There is **no** separate `Create Affix Slot` operation.

Slot creation happens only through Add Random Affix against unused capacity.

---

# 18. Common Target Requirements

Unless explicitly overridden by Salvage, Step 17 target items must be:

- directly carried in the main inventory;
- not inside a container;
- not equipped;
- not worn;
- not wielded;
- not actively in use;
- `quan == 1`;
- eligible under canonical Phase 1 restrictions;
- non-artifact.

For:

- Add Random Affix
- Reroll Affix
- Extract Affix
- Imprint Affix

the item's ordinary-affix state must also be fully identified.

Salvage has different identification and stack rules described later.

---

# 19. Canonical Legality Engine

All candidate generation and validation must query the canonical enhancement legality implementation.

Forge legality is specifically **ordinary-affix acquisition/manipulation legality**, not generic property compatibility. A property may enter Add Random, Reroll, Extract, or Imprint only if the canonical Steps 13-16 enhancement system considers that property part of the ordinary-affix manipulation path for the exact target and operation.

This includes:

- ordinary-affix acquisition/manipulation eligibility;
- weapon-only restrictions;
- armor-only restrictions;
- tool-specific restrictions;
- container-specific restrictions;
- Step16C tool rules;
- mutually exclusive properties;
- mutually exclusive property families;
- ordinary-affix capacity;
- artifacts;
- ammunition;
- item-type restrictions;
- canonical non-manipulable/acquisition-mode metadata where present;
- any future canonical restrictions added before implementation.

Socket-only properties, base properties, artifact powers, and any other non-ordinary acquisition modes must never enter ordinary Forge candidate pools merely because they are mechanically compatible with the item.

Step 17 must not create its own duplicate eligibility tables.

If exact duplicate properties are blocked by existing enhancement rules, Forge naturally blocks them.

Step 17 does **not** add an independent "no duplicate affix" rule.

---

# 20. Natural Generation Restrictions Versus Forge

Natural-generation depth restrictions and natural rarity weights are **not Forge restrictions**.

For Forge Add and Reroll:

- dungeon depth does not restrict the candidate pool;
- natural affix-generation weights are ignored;
- every currently legal **ordinary-Forge-eligible** property within the selected tier participates uniformly.

Forge ignores only natural-generation depth gating and natural-generation weighting. It does **not** bypass ordinary-affix acquisition-mode restrictions, non-manipulable metadata, item compatibility, conflicts, or any other canonical legality rule.

The canonical legality engine remains authoritative.

---

# 21. Add Random Affix

## 21.1 Purpose

Add Random Affix either:

1. creates a new occupied slot from unused capacity; or
2. fills an existing Open Affix Slot.

It never overwrites an occupied slot.

---

## 21.2 Flow

The player selects:

```text
target item
```

Then chooses a destination:

```text
specific Open Affix Slot
or
Create New Affix Slot
```

---

## 21.3 Filling an Open Affix Slot

An Open slot already has a fixed tier.

The player does not choose a tier.

Candidate pool:

```text
all canonical legal ordinary-Forge-eligible affixes
for that exact item
at that slot's fixed tier
given all currently occupied other slots
```

The property previously extracted from that slot is **not remembered** and receives no special exclusion.

Therefore the same property may be randomly generated again.

---

## 21.4 Creating a new slot

If unused capacity remains, the player may choose a tier.

The Forge only offers tiers for which at least one legal candidate exists.

Once selected:

```text
new slot tier = selected tier
reroll history = 0
```

The slot uses the first unused canonical slot position.

---

## 21.5 Matching-tier Open Slot rule

If an Open Affix Slot already exists at tier `T`, the player cannot create another new tier `T` slot while leaving the existing `T` slot Open.

The matching Open slot must be filled first.

Example:

```text
Slot 1: Open Tier II
Unused capacity: 1
```

The player may:

```text
refill Slot 1 at T2
or
create a new T1/T3/T4 slot, if legal
```

The player may not:

```text
create another new T2 slot
```

If multiple Open slots at the same tier exist, the player may choose which one to refill.

---

## 21.6 Candidate selection

Candidate selection is uniform.

Natural affix rarity weights are ignored.

If legal pool size is:

```text
0 -> operation unavailable
1 -> deterministic result by exhaustion
2+ -> uniformly choose one from full legal pool
```

A one-entry pool is valid and uses the normal cost.

There is no discount.

---

## 21.7 Commitment

Before commitment the player sees:

```text
target
destination
tier
cost
warning that the exact property is random
```

After confirmation:

1. consume gold and Generic Essence;
2. generate one legal affix uniformly;
3. roll any variable magnitude using canonical rules;
4. occupy the selected slot;
5. update canonical enhancement state;
6. identify the newly created affix;
7. consume one turn.

There is no reject-result option.

---

# 22. Add Random Affix Costs

## 22.1 New slot

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 300 | 1 |
| T2 | 500 | 3 |
| T3 | 1,000 | 6 |
| T4 | 2,500 | 12 |

A new slot begins with:

```text
reroll history = 0
```

---

## 22.2 Refill Open slot

Base refill cost:

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 1 |
| T2 | 200 | 3 |
| T3 | 400 | 5 |
| T4 | 800 | 8 |

This cost is multiplied by the selected slot's current reroll-history multipliers using the same Gold and Essence multiplier tables defined for Reroll.

Final refill cost is:

```text
gold = base refill gold × current-history gold multiplier
Essence = ceil(base refill Essence × current-history Essence multiplier)
```

Any fractional Generic Essence cost is always rounded **up** to the next whole Essence.

Refilling does **not** increment reroll history.

---

# 23. Reroll Affix

## 23.1 Target

Reroll operates only on an:

```text
Occupied Affix Slot
```

It cannot target:

- Open Affix Slots;
- unused capacity;
- socket-derived properties;
- base item properties.

---

## 23.2 Tier

The selected slot's tier remains unchanged.

Reroll never changes tier.

---

## 23.3 Building the legal pool

For legality checking:

1. logically remove the selected slot's current property;
2. retain all other occupied slots;
3. build all canonical legal ordinary-Forge-eligible properties at the same tier;
4. explicitly exclude the currently selected property from the final reroll pool.

This allows candidate legality to be evaluated as a replacement rather than as an additional simultaneous affix.

---

## 23.4 Zero legal alternatives

If no legal alternative remains after excluding the current property:

```text
Reroll is unavailable.
```

The Forge performs:

```text
no payment
no RNG
no mutation
no history increment
no turn
```

This covers the case where the existing affix is the only legal property at that tier.

---

## 23.5 One legal alternative

If exactly one legal replacement exists, Reroll is allowed.

After payment the player sees:

```text
1. <one legal replacement>
2. Keep Existing
```

---

## 23.6 Two or more legal alternatives

If at least two legal alternatives exist, sample:

```text
2 distinct properties
```

uniformly without replacement.

The candidates cannot duplicate one another.

Natural-generation weights are ignored.

---

## 23.7 Candidate presentation

After payment, display:

```text
candidate name
short canonical property description
```

for each candidate.

Also display:

```text
Keep Existing
current affix description
```

Random numeric magnitudes are not generated or displayed at candidate-reveal time.

If the player chooses a replacement requiring a variable numeric magnitude, that magnitude is rolled only after the candidate is selected.

This prevents value-fishing between candidate options.

---

## 23.8 Keep Existing

`Keep Existing` is always available after a committed reroll.

Choosing it:

- retains the current property;
- retains the current rolled value;
- does not refund resources;
- retains the already-incremented reroll history;
- consumes the committed turn.

---

## 23.9 ESC after candidate reveal

After candidates have been revealed:

```text
ESC = Keep Existing
```

ESC does not:

- refund gold;
- refund Essence;
- decrement history;
- rewind RNG;
- cancel the turn.

Before payment, ESC remains a normal free cancellation.

---

# 24. Reroll Costs

Base cost:

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 2 |
| T2 | 200 | 4 |
| T3 | 400 | 7 |
| T4 | 800 | 10 |

Both resources scale using the slot's **current reroll count before this reroll is committed**.

### Gold multiplier

| Current history | Multiplier |
|---:|---:|
| 0 | 1.0x |
| 1 | 1.5x |
| 2 | 2.0x |
| 3 | 2.5x |
| 4 | 3.0x |
| 5 | 3.5x |
| 6 | 4.0x |
| 7 | 4.5x |
| 8 | 5.0x |
| 9 | 5.5x |
| 10 | 6.0x |

### Essence multiplier

| Current history | Multiplier |
|---:|---:|
| 0 | 1.00x |
| 1 | 1.25x |
| 2 | 1.50x |
| 3 | 1.75x |
| 4 | 2.00x |
| 5 | 2.25x |
| 6 | 2.50x |
| 7 | 2.75x |
| 8 | 3.00x |
| 9 | 3.25x |
| 10 | 3.50x |

Fractional Essence cost is always rounded **up** to the next whole Essence. The same multiplier tables and Essence ceiling rule apply when Add Random refills an Open Affix Slot.

---

# 25. Reroll History

Reroll history belongs to the **specific slot**, not the item and not the affix property.

Persistent range:

```text
0-10
```

The counter saturates at:

```text
10
```

It never wraps.

History `10` is a **cost/history cap, not an operation limit**. A slot at history 10 may still be Rerolled, Extracted, refilled through Add Random after extraction, or Imprinted when otherwise legal. Any further history-incrementing operation leaves the stored value at 10, and any history-scaled cost continues to use the history-10 multiplier.

---

## 25.1 History increment events

History increments by one after:

### Committed Reroll

Even when the player:

- chooses Keep Existing;
- presses ESC after candidate reveal.

### Successful Extract

Extraction increments the selected slot's history by one.

---

## 25.2 Operations that do not increment history

The following do not increment it:

- creating a new slot through Add Random;
- filling an Open slot through Add Random;
- Imprint;
- Salvage.

---

## 25.3 History and refill pricing

The history multiplier affects:

- Reroll;
- Add Random when filling an Open slot.

It does not affect:

- Add Random when creating a new slot;
- Extract;
- Imprint.

---

# 26. Extract Affix

## 26.1 Target

Extract operates only on an:

```text
Occupied ordinary Affix Slot
```

It cannot extract:

- sockets;
- gemstone effects;
- quality;
- enchantment;
- material;
- artifact powers;
- base object properties;
- other non-ordinary enhancement state.

All ordinary Forge-compatible affixes are extractable unless canonical enhancement/acquisition metadata already marks a property as non-manipulable or outside the ordinary-affix manipulation path.

Step 17 does not create a new extraction blacklist.

---

## 26.2 Cost

Extraction cost is flat by tier.

It is not affected by reroll history.

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 2 |
| T2 | 200 | 4 |
| T3 | 400 | 7 |
| T4 | 800 | 10 |

---

## 26.3 Result

Successful extraction:

1. consumes the cost;
2. removes the current ordinary affix;
3. clears all property-specific runtime state associated with that property;
4. keeps the same physical slot;
5. keeps the same tier;
6. increments slot reroll history by one, saturating at 10;
7. converts the slot into an Open Affix Slot;
8. adds exactly one matching Affix Essence to the global ledger;
9. consumes one turn.

Example:

```text
Before:
Slot 1: Caustic II
Rerolls: 4

After:
Slot 1: Open, Tier II
Rerolls: 5

Forge ledger:
Caustic II Essence +1
```

---

## 26.4 Rolled value

The extracted Affix Essence does not preserve the source affix's numeric roll.

Example:

```text
Strength III +2
```

extracts:

```text
Strength III Essence
```

not:

```text
Strength III +2 Essence
```

---

# 27. Imprint Affix

## 27.1 Destination

Imprint operates **only on an Open Affix Slot**.

It cannot:

- create a new slot;
- use unused capacity directly;
- overwrite an occupied slot.

Therefore a completely fresh unenhanced item cannot be directly imprinted.

A slot must first exist, normally because:

- the item naturally had an affix;
- Add Random created an affix;
- that affix was then Extracted.

---

## 27.2 Flow

Player selects:

1. target item;
2. specific Open Affix Slot;
3. one legal stored Affix Essence.

The Forge then lists only ledger entries that:

- match the Open slot's fixed tier;
- have positive stored count;
- are currently legal for that exact target.

---

## 27.3 Legality

Imprint does not bypass canonical rules. The selected stored property must remain part of the canonical ordinary-affix manipulation path for the target; a property cannot become Imprint-eligible merely because it is generically compatible or socket-compatible.

Filtering must respect:

- item type;
- tier;
- weapon rules;
- armor rules;
- tool rules;
- container rules;
- Step16C eligibility;
- affix families;
- mutual exclusions;
- artifact exclusion;
- ammunition exclusion;
- all other canonical Phase 1 rules.

---

## 27.4 Same affix may be restored

The exact affix that was extracted from a slot may be imprinted back into that same slot.

Because Affix Essence does not preserve the old rolled magnitude, this may produce a fresh value.

Example:

```text
Strength III +2
    ↓ Extract
Strength III Essence
    ↓ Imprint into same Open T3 slot
Strength III +fresh canonical roll
```

This behavior is intentional.

---

## 27.5 Cost

Imprint cost is flat by tier.

It does not scale with reroll history.

| Tier | Gold | Essence | Affix Essence |
|---|---:|---:|---:|
| T1 | 100 | 2 | 1 exact |
| T2 | 200 | 4 | 1 exact |
| T3 | 400 | 7 | 1 exact |
| T4 | 800 | 10 | 1 exact |

---

## 27.6 Result

Successful Imprint:

1. consumes gold;
2. consumes Generic Essence;
3. consumes one exact stored Affix Essence;
4. applies that exact property to the selected Open slot;
5. retains the slot's fixed tier;
6. retains its existing reroll history;
7. generates a fresh variable numeric magnitude using canonical rules;
8. initializes property-specific runtime state as a newly acquired property;
9. identifies the imprinted affix;
10. consumes one turn.

Imprint does **not** increment reroll history.

---

# 28. Property-Specific Runtime State

Runtime state belongs to the active property, not the slot.

Example:

```text
Stoning cooldown
```

If Stoning leaves a slot because of:

- Reroll;
- Extract;
- another legitimate property replacement,

its Stoning-specific runtime state is cleared.

If Stoning is later added or imprinted again, it starts with fresh canonical state.

For Stoning that means:

```text
Ready
```

rather than inheriting the previous cooldown.

The same principle applies to any future affix-specific runtime state.

---

# 29. Salvage Item

## 29.1 Position in Forge

Salvage is a top-level Forge operation:

```text
Forge
 -> Salvage item
```

It is not inside Affix Crafting.

---

## 29.2 Eligibility

Salvage is limited to the canonical Phase 1 **equipment domain**:

- weapons;
- armor;
- tools that are eligible under the canonical Phase 1 tool/equipment rules.

An otherwise eligible equipment item does **not** need to have Quality, an ordinary affix, an Open slot, or a socket in order to be Salvageable. A plain Standard item in the equipment domain is valid and uses the normal zero-Essence gold fallback.

Salvage explicitly excludes:

- ammunition;
- artifacts;
- Generic Essence;
- gemstones;
- currency/gold objects;
- food;
- potions;
- scrolls;
- spellbooks;
- other consumables;
- objects outside the canonical Phase 1 equipment domain.

Other unique, quest, or protected objects continue to follow their canonical Phase 1 restrictions.

Containers are Salvageable only when they are themselves eligible tools/equipment under the canonical rules and are empty.

---

## 29.3 Stack behavior

Salvage is the only Forge target operation that accepts:

```text
quan > 1
```

If a stack is selected, Salvage destroys the **entire selected stack**.

There is no Forge quantity prompt.

If the player wants to Salvage only part of a stack, the player must split the stack before using the Forge.

---

## 29.4 Container rule

A non-empty container cannot be Salvaged.

The player must empty it first.

---

## 29.5 Socketed items

A socketed ordinary item may be Salvaged.

On success:

```text
item destroyed
all socketed gems destroyed
```

Socketed gems contribute:

```text
0 Essence
```

If their presence is known, the confirmation must warn the player that they will be destroyed.

Salvage must not reveal hidden information merely through the warning.

---

# 30. Salvage Yield

Salvage yield is deterministic except for the zero-Essence gold fallback.

## 30.1 Quality contribution

| Quality | Essence |
|---|---:|
| Standard | 0 |
| Fine | +1 |
| Exceptional | +2 |

---

## 30.2 Created Affix Slot contribution

Every **created slot** contributes according to tier, regardless of whether the slot is occupied or Open.

| Slot tier | Essence |
|---|---:|
| T1 | +1 |
| T2 | +2 |
| T3 | +3 |
| T4 | +5 |

Therefore:

```text
Occupied T3 slot = +3 Essence
Open T3 slot     = +3 Essence
```

Reroll history contributes:

```text
0
```

Socketed gems contribute:

```text
0
```

---

## 30.3 Formula

For one physical item:

```text
Essence yield =
    quality contribution
  + sum(contribution of every created affix slot)
```

Maximum under the current two-slot model:

```text
Exceptional quality   +2
T4 slot               +5
T4 slot               +5
                      ---
                      12 Essence
```

The implementation must nevertheless use canonical slot capacity rather than hard-coding two.

---

# 31. Zero-Essence Salvage Fallback

If calculated Essence yield is greater than zero:

```text
award exactly that much Essence
award no gold fallback
```

If calculated Essence yield is exactly zero:

```text
award 2d10 gold per physical item
award no Essence
```

Example:

```text
1 Standard plain item:
2d10 gold
```

```text
10 Standard plain items:
2d10 gold rolled for each of the 10 physical items
```

Equivalent aggregate implementations are acceptable as long as they preserve the same distribution.

---

# 32. Stack Salvage Calculation

Salvage value is computed per physical item and summed over the stack.

Examples:

```text
10 Fine items:
10 Essence
```

```text
10 Exceptional items:
20 Essence
```

```text
10 Standard plain items:
10 × 2d10 gold
```

Due to global Phase 1 invariants, a legitimate stack cannot contain:

- created affix slots;
- socketed gems.

If such a stack is encountered, reject it as invalid state.

---

# 33. Salvage Identification Policy

Salvage may be used on an unidentified item.

However, it must never become a reversible identification oracle. The rule applies to **all state that affects Salvage return or destruction warnings**, not only ordinary-affix identity.

Open Affix Slots are always fully known under the canonical slot model, so their existence, tier, and reroll history are never hidden state.

## 33.1 All Salvage-relevant state known

Before commitment, show the complete deterministic breakdown only when every piece of state that can affect Salvage return is already known to the player.

At minimum, this includes:

- Quality when Quality contributes to the yield;
- created ordinary-affix slot existence and tier;
- any future canonical state that contributes to Salvage value.

Example:

```text
Salvage Exceptional longsword

Quality:
  Exceptional            +2 Essence

Affix Slots:
  Slot 1: Tier III        +3 Essence
  Slot 2: Open Tier II    +2 Essence

Total:
  7 Essence

This will permanently destroy the item.
```

Also warn about socketed gems only when their presence is already known.

---

## 33.2 Unknown Salvage-relevant state

If any state that affects Salvage return or a destruction warning is unknown, the pre-commit UI must not expose that hidden fact indirectly.

Before commitment, do not reveal information such as:

- whether an unidentified ordinary affix exists;
- the tier of an unidentified occupied slot;
- an unidentified Quality level;
- the presence of an unidentified socketed gem;
- whether the final reward will be Essence or gold;
- the exact reward amount or breakdown.

Use a non-specific warning such as:

```text
Some properties relevant to Salvage are unidentified.
The exact Salvage return cannot be determined before destruction.

Salvaging will permanently destroy this item.
```

After commitment and destruction, calculate the real result from the actual state and report what was recovered.

Example:

```text
hidden T3 slot on Standard item
-> after destruction: 3 Essence
```

```text
truly plain Standard item
-> after destruction: 2d10 gold
```

The information is allowed to become visible after destruction because it can no longer be exploited on the source item.

---

# 34. Forge Identification Requirements

The following operations require the target's ordinary-affix state to be fully identified:

- Add Random Affix
- Reroll Affix
- Extract Affix
- Imprint Affix

This avoids hidden occupied affixes influencing legal candidate pools in ways that leak secret information through Forge menus.

Open Affix Slots themselves are always fully identified and require no separate identification flag.

Forge-generated properties are immediately identified.

---

# 35. Turn Consumption

The following successful committed operations each consume:

```text
1 turn
```

- Add Random Affix
- Reroll Affix
- Extract Affix
- Imprint Affix
- Salvage Item

For Reroll, a committed transaction consumes the turn even if:

- Keep Existing is chosen;
- ESC is pressed after candidate reveal.

The following consume:

```text
0 turns
```

- opening menus;
- browsing Affix Essence;
- inspecting costs;
- pre-commit cancellation;
- selecting an invalid target;
- operation unavailable due to no legal candidates;
- insufficient resources;
- any rejected preflight.

Existing `Forge an item` and `Socket gemstone` retain their existing established turn semantics unless their own implementation requires adjustment for Phase 1 consistency.

---

# 36. Transaction Model

All Step 17 Forge operations must be transactional.

There is a clear commit boundary.

## Before commitment

The Forge may:

- inspect target state;
- query canonical legality;
- calculate candidate-pool size;
- calculate costs;
- check resources;
- display consequences.

It must not:

- consume gold;
- consume Essence;
- alter inventory;
- increment reroll history;
- create Affix Essence;
- mutate slots;
- advance RNG for the actual result;
- consume a turn.

---

## At commitment

All gameplay validation must already have succeeded.

Then the operation may:

1. consume resources;
2. perform required random selection;
3. mutate slot/item state;
4. produce outputs;
5. update derived enhancement state;
6. consume the turn.

Operations must be structured so ordinary gameplay cannot fail halfway through and leave resources partially consumed.

---

# 37. RNG Commitment Rules

## Add Random

Exact property is generated only after payment/commitment.

Result is mandatory.

---

## Reroll

Candidate identities are generated only after payment/commitment.

The player then sees the candidates.

Variable numeric magnitudes are rolled only after selecting a replacement candidate.

---

## Extract

Deterministic after commitment.

---

## Imprint

Exact property is already known.

Fresh variable numeric magnitude is rolled only after commitment.

---

## Salvage

Essence return is deterministic.

The `2d10` gold fallback is rolled only after committed destruction.

Cancelled preview screens must not consume result RNG.

---

# 38. Forge Gold and Shop Interaction

Forge structures cannot exist in shops.

Step 17 therefore does not introduce a new special unpaid-item or shop-billing Forge subsystem.

Existing game ownership rules remain authoritative.

Step 17 does not define a second gold-payment model. All Step 17 gold costs must use the existing canonical Forge gold-payment semantics and helper(s) found in the post-Step16C implementation. Whatever inventory scope, deduction behavior, messaging, and ownership handling the existing Forge uses for gold remains authoritative unless required to fix a pre-existing Phase 1 invariant.

The direct-main-inventory restriction introduced by Step 17 applies specifically to **Generic Essence**, not to gold.

Generic Essence itself has no normal shop buy/sell economy.

---

# 39. Existing Forge Recipes

Existing Forge recipes remain available under:

```text
Forge an item
```

When an existing Forge recipe destroys source items and creates a new output item:

- the result is a **new item**;
- it receives fresh canonical slot identity;
- inherited ordinary affixes follow the existing recipe inheritance rules;
- each inherited affix receives a fresh output slot;
- output slot reroll history begins at `0`;
- Open Affix Slots are not inherited;
- donor reroll history is not inherited.

The recipe must not clone persistent donor-slot history onto the newly created object.

---

# 40. All Affix Acquisition Paths Must Use Slots

Step 17 makes canonical slots a Phase 1-wide requirement.

The implementation audit must ensure slots are populated for every ordinary-affix acquisition path, including:

- natural enhanced-item generation;
- Add Random Affix;
- Reroll replacement;
- Imprint;
- existing Forge recipe inheritance;
- any other Phase 1 function that grants an ordinary affix.

No legacy path may directly set an ordinary-property bit while bypassing canonical slot creation/synchronization.

---

# 41. Transformations and Enhancement Lifecycle

Existing enhancement lifecycle behavior remains authoritative.

If an existing transformation clears ordinary enhancement state, it must also clear:

- occupied slots;
- Open slots;
- slot tiers;
- reroll history;
- property-specific runtime state.

If a legitimate transformation preserves ordinary enhancements, its corresponding slot state must remain synchronized and legal.

No special Step 17 polymorph exception is required.

Current audited behavior for actual object-type-changing polymorph creates fresh enhancement state rather than carrying the old enhancement onto the new object type. Step 17 should preserve the canonical lifecycle behavior found in the actual post-Step16C implementation.

---

# 42. `#inspect` Display

Inspection of a sufficiently identified enhanced item must expose slot state clearly.

Example:

```text
Affix Slots:

Slot 1:
  Vampiric II
  Rerolls: 1

Slot 2:
  Open Affix Slot
  Tier: III
  Rerolls: 4

Unused Affix Capacity:
  0
```

If unused capacity remains:

```text
Unused Affix Capacity:
  1
```

At reroll history 10, inspection should indicate that the slot has reached its maximum history/cost level, for example:

```text
Rerolls: 10 (maximum)
```

Inspection should make the distinction between:

- Occupied slot;
- Open slot;
- unused capacity

unambiguous.

Open-slot position, tier, and reroll history are always known and may always be shown. Occupied-affix details continue to follow the canonical identification rules.

---

# 43. Generic Essence Inspection

Generic Essence may display:

```text
Essence

A generic Forge crafting material.
Obtained primarily through Salvage.
Used for Forge affix manipulation.
```

No redundant eligibility rules are needed.

---

# 44. Affix Essence Ledger Display Example

Example:

```text
Tier I
  Corrosive I Essence x2
    <canonical short description>

Tier II
  Caustic II Essence x1
    <canonical short description>

Tier III
  Discerning III Essence x3
    <canonical short description>
  Sanctified III Essence x1
    <canonical short description>
```

Only positive entries are shown.

---

# 45. Save Format and Compatibility

Step 17 introduces persistent state.

The implementation must bump:

```text
EDITLEVEL
```

exactly once after the final Step 17 persistent layout is established.

Backward compatibility with development saves is not required.

Older saves, recovery files, and incompatible bones data must be rejected cleanly according to the project's normal edit-level compatibility behavior.

Do not add a migration shim solely for old development saves.

---

# 46. Persistent State

The following must survive save/restore where applicable:

## Per item

- slot existence;
- slot position;
- slot tier;
- Occupied versus Open state;
- current affix identity;
- reroll history;
- current affix rolled values;
- property-specific runtime state while that property remains active;
- socket state.

No separate Open-slot knowledge field is required or persisted because Open-slot existence, position, tier, and reroll history are always known.

## Global game state

- Affix Essence ledger counts by canonical property identity.

## Normal objects

- Generic Essence quantities through normal object serialization.

---

# 47. No Affix Provenance

Do not store whether an affix is:

- naturally generated;
- Added;
- Rerolled;
- Imprinted;
- inherited from a Forge recipe.

Current state determines behavior.

Provenance is unnecessary for:

- legality;
- Salvage;
- extraction;
- imprinting;
- reroll costs.

---

# 48. Economy Summary

## Add Random, new slot

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 300 | 1 |
| T2 | 500 | 3 |
| T3 | 1,000 | 6 |
| T4 | 2,500 | 12 |

## Add Random, Open-slot refill base

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 1 |
| T2 | 200 | 3 |
| T3 | 400 | 5 |
| T4 | 800 | 8 |

Scaled by slot history using the shared Gold/Essence multiplier tables; fractional Essence rounds up.

## Reroll base

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 2 |
| T2 | 200 | 4 |
| T3 | 400 | 7 |
| T4 | 800 | 10 |

Scaled by slot history.

## Extract

| Tier | Gold | Essence |
|---|---:|---:|
| T1 | 100 | 2 |
| T2 | 200 | 4 |
| T3 | 400 | 7 |
| T4 | 800 | 10 |

Flat cost.

## Imprint

| Tier | Gold | Essence | Exact Affix Essence |
|---|---:|---:|---:|
| T1 | 100 | 2 | 1 |
| T2 | 200 | 4 | 1 |
| T3 | 400 | 7 | 1 |
| T4 | 800 | 10 | 1 |

Flat cost.

---

# 49. Reroll-History Summary

| Action | History effect |
|---|---:|
| New slot via Add Random | Starts at 0 |
| Reroll committed | +1 |
| Reroll then Keep Existing | +1 |
| Reroll then ESC after reveal | +1 |
| Extract | +1 |
| Add Random into Open slot | No change |
| Imprint into Open slot | No change |
| Salvage | Not relevant, item destroyed |
| New item from Forge recipe | Fresh inherited slots start at 0 |

Maximum stored history:

```text
10
```

This is a saturation/cost cap only. Further otherwise-legal operations remain available and continue using the history-10 cost level where history scaling applies.

---

# 50. Operation State Matrix

| Operation | Unused capacity | Occupied slot | Open Affix Slot |
|---|---|---|---|
| **Add Random Affix** | Yes | No | Yes |
| **Reroll Affix** | No | Yes | No |
| **Extract Affix** | No | Yes | No |
| **Imprint Affix** | No | No | Yes |

Salvage operates on the item rather than on an individual slot.

---

# 51. Important Edge Cases

## Add Random with one legal property

Allowed.

The outcome is deterministic because only one legal candidate exists.

Normal cost applies.

---

## Add Random with zero legal properties

Unavailable.

No resources, RNG, mutation, or turn.

---

## Reroll with one replacement

Allowed.

Show:

```text
replacement
Keep Existing
```

---

## Reroll with zero replacements

Unavailable.

This explicitly includes the case where the current property is the only legal property.

---

## Open slot with no legal properties

The slot remains Open and persistent.

It is not deleted or retiered.

---

## Same property extracted and randomly re-added

Allowed.

The Open slot does not remember its previous property.

---

## Same property extracted and imprinted back

Allowed.

A fresh numeric magnitude is generated where applicable.

---

## Multiple same-tier Open slots

The player chooses a specific destination slot.

Its own history determines refill cost.

---

## Duplicate properties

Forge relies solely on canonical existing legality rules.

No Step 17-only duplicate rule is added.

---

## History already at 10

Reroll and Extract remain available when otherwise legal.

The stored history remains 10. Reroll and Open-slot refill costs continue to use the history-10 multiplier.

---

## Open-slot knowledge

Every Open Affix Slot is always known. Its slot position, fixed tier, and reroll history may be displayed without creating an identification oracle.

---

# 52. Required Phase 1 Audit

Implementation of Step 17 is incomplete until the entire Phase 1 enhancement code has been audited for the following.

### Stack behavior

- occupied slots prevent stacking;
- Open slots prevent stacking;
- socketed objects prevent stacking;
- target Forge operations reject stacks except Salvage.

### Generation

- generated/dropped stacks do not receive affixes;
- generated/dropped stacks do not receive sockets;
- natural affixes populate canonical slots;
- Generic Essence 1% generation replaces an eligible general-random-loot result rather than adding an extra object;
- explicit/specific/class-constrained creation, fixed rewards, Forge outputs, wishes, and shop stock are not intercepted by the Essence proc.

### Ammunition

- no ordinary enhancements;
- no slots;
- no sockets;
- no Forge affix manipulation.

### Artifacts

- excluded as Forge crafting targets, sources, ingredients, donors, and modified outputs;
- artifact war-hammer activation preserves the existing invocation behavior and is not treated as crafting participation.

### Existing Forge

- target stack restrictions;
- artifact crafting-participation exclusion;
- artifact war-hammer activation remains an invocation-only exception when supported by existing activation logic;
- Step 17 gold payment reuses canonical existing Forge payment semantics;
- inherited-affix slot construction;
- no inherited reroll history.

### Sockets

- remain separate from Forge ordinary affixes.

### Transformations

- clear or preserve slots consistently with canonical enhancement lifecycle.

### Object lifecycle, save, copy, and recovery

Audit every object lifecycle path that can create, copy, clone, move, serialize, deserialize, or destroy enhanced objects, including where applicable:

- normal object initialization;
- object duplication/copy helpers;
- Forge recipe output construction;
- transformation replacement;
- level migration;
- save/restore;
- recovery;
- bones;
- shop/billing copies or shadow objects;
- destruction/free paths.

Verify:

- new slot fields/state are initialized deterministically;
- slot state is copied only when the canonical lifecycle says the enhancement itself is preserved;
- no pointer/sidecar aliasing or stale state survives object destruction if sidecar storage is used;
- slot state survives where appropriate;
- global ledger survives;
- stale incompatible save formats are rejected.

---

# 53. Validation Plan

Step 17 completion requires functional tests, structural invariant tests, persistence tests, and integration tests.

## 53.1 Slot tests

Test:

```text
new T1/T2/T3/T4 slot creation
Open slot refill
occupied -> Open extraction
Open -> occupied Add
Open -> occupied Imprint
multiple slots
multiple Open slots
unused + occupied + Open simultaneously
```

Verify:

- stable slot position;
- immutable tier;
- correct history;
- capacity counting.

---

## 53.2 Add Random tests

For every eligible object category and tier:

- candidate is canonical ordinary-Forge-eligible;
- socket-only and other non-ordinary acquisition modes never enter the pool;
- natural depth restrictions do not incorrectly apply;
- natural generation weights do not affect uniform Forge selection;
- pool 0 blocks;
- pool 1 succeeds;
- pool 2+ selects uniformly from legal membership;
- occupied slots cannot be overwritten;
- matching-tier Open-slot rule enforced;
- new slot history starts 0;
- Open refill retains history;
- refill does not increment history;
- refill uses the shared history multiplier tables;
- fractional refill Essence costs round up;
- exact property is not shown before commitment.

---

## 53.3 Reroll tests

Verify:

- tier unchanged;
- current property excluded;
- all other slots considered;
- socket-only and other non-ordinary acquisition modes never enter the pool;
- zero alternatives blocks;
- one alternative displays one candidate plus Keep;
- two or more produces two distinct candidates;
- candidate selection uniform without replacement;
- natural generation rarity ignored;
- cost based on pre-increment history;
- history increments after commitment;
- history 10 remains legal for further rerolls and stays saturated at 10;
- history-10 rerolls continue to use the maximum multiplier;
- Keep Existing retains property but not resources/history;
- ESC after reveal equals Keep Existing;
- candidate numeric rolls occur only after selection.

---

## 53.4 Extract tests

For every ordinary property tier:

- correct cost;
- property removed;
- runtime state cleared;
- same slot retained;
- same tier retained;
- history +1;
- counter saturates at 10;
- Extract remains legal at history 10 when otherwise valid and history remains 10;
- exact ledger count +1;
- no numeric roll stored in ledger;
- no physical Affix Essence object created.

---

## 53.5 Imprint tests

Verify:

- only Open slots accepted;
- unused capacity rejected;
- occupied slots rejected;
- only matching-tier ledger properties shown;
- illegal exact properties filtered out;
- same extracted property may be restored;
- exact ledger count decremented;
- history retained;
- history not incremented;
- fresh numeric magnitude created;
- flat cost unaffected by history.

---

## 53.6 Salvage tests

Test all quality values:

```text
Standard
Fine
Exceptional
```

Test created slots:

```text
T1
T2
T3
T4
Occupied
Open
mixed
```

Verify exact deterministic Essence totals.

Test:

```text
Standard plain -> 2d10 gold
Fine plain -> 1 Essence
Exceptional plain -> 2 Essence
Exceptional + 2x T4 -> 12 Essence
```

Verify:

- weapons, armor, and eligible tools are accepted;
- plain Standard equipment remains eligible;
- ammunition, artifacts, Essence, gemstones, currency, consumables, and non-equipment objects are rejected;
- no gold when Essence yield >0;
- reroll history contributes 0;
- socket gems contribute 0;
- gems destroyed;
- non-empty containers rejected;
- artifacts rejected;
- complete stack destroyed;
- stack rewards multiplied per physical item;
- invalid enhanced stack rejected.

---

## 53.7 Generic Essence tests

Verify:

- physical object;
- weight 1;
- stacks;
- remains permanently uncursed/neutral;
- blessing/cursing attempts do not leave a changed beatitude;
- beatitude cannot fragment Essence stacks;
- no affix;
- no socket;
- cannot Salvage;
- only direct-inventory quantity counts;
- container-held Essence ignored;
- shops do not stock/buy it;
- natural generation 1% and quantity 1-2;
- successful natural proc replaces the eligible normal random-loot result rather than adding a second object;
- explicit/specific/class-constrained creation, scripted/guaranteed rewards, Forge outputs, wishes, and shop stock are not intercepted;
- no depth scaling;
- wish quantity table exactly implemented.

---

## 53.8 Affix Essence ledger tests

Verify:

- exact canonical property IDs;
- count increments on Extract;
- count decrements on Imprint;
- zero entries hidden;
- global across all Forges;
- save/restore;
- recovery;
- tier derived correctly;
- canonical primary/prefix name;
- Roman tier displayed;
- grouping T1 through T4;
- alphabetical order;
- descriptions use canonical source.

---

## 53.9 Identification tests

Verify:

- Add target must have fully identified ordinary-affix state;
- Reroll target must be identified;
- Extract target must be identified;
- Imprint target must be identified;
- Salvage accepts an otherwise eligible unidentified target;
- Open-slot existence/tier/history are always known;
- Salvage preview reveals exact return only when all Salvage-relevant state is already known;
- unknown Quality, occupied-affix state, socket presence, or other Salvage-relevant state is not leaked through preview or warnings;
- actual reward and destroyed hidden state may be reported only after committed destruction.

---

## 53.10 Turn and cancellation tests

For each Step 17 operation:

### Pre-commit cancel

Verify:

```text
0 turns
no resource change
no object mutation
no ledger mutation
no history mutation
```

### Successful commit

Verify:

```text
1 turn
```

For every operation with a gold cost, verify gold availability and deduction use the same canonical Forge payment semantics as the existing pre-Step17 Forge implementation.

### Reroll Keep/ESC after reveal

Verify:

```text
1 turn
resources consumed
history increment retained
original affix retained
```

---

## 53.11 Stack-invariant tests

Across all Phase 1 code:

- affixed item refuses merge;
- Open-slot item refuses merge;
- socketed item refuses merge;
- generated stack receives no enhancement;
- generated stack receives no gem;
- ammunition cannot become enhanced/socketed;
- all Forge targets except Salvage require `quan == 1`.

---

## 53.12 Runtime-state tests

Especially test Stoning:

1. acquire Stoning;
2. trigger cooldown;
3. Extract or Reroll it away;
4. verify cooldown state cleared;
5. reacquire Stoning;
6. verify new property begins Ready.

Repeat equivalent tests for any other stateful property.

---

## 53.13 Existing Forge recipe tests

Verify:

- existing recipe behavior preserved;
- artifacts rejected as crafting participants;
- target stack restrictions correct;
- inherited ordinary affixes receive canonical output slots;
- output reroll histories are 0;
- Open slots are never inherited;
- if an artifact war hammer is recognized by the existing activation mechanic, it can still open the Forge menu without becoming a crafting participant.

---

## 53.14 Persistence tests

Round-trip through save/restore:

```text
occupied slot, history 0
occupied slot, history 10
Open slot, history 0
Open slot, history 10
item containing occupied + Open slots
item with unused capacity
socketed item
Generic Essence stack
non-empty Affix Essence ledger
```

Verify exact logical equality after restore.

Also validate:

- Open slots require no separate persisted knowledge state and restore as fully known;
- normal object initialization cannot inherit stale slot state;
- object copy/clone paths preserve or clear slots according to the canonical lifecycle;
- level migration;
- recovery;
- relevant bones handling for enhanced physical objects;
- shop/billing copy paths where applicable;
- safe cleanup for any sidecar slot storage if that architecture is chosen;
- clean rejection of old edit-level formats.

---

## 53.15 RNG and probability validation

Probability validation must avoid flaky tests.

Use deterministic seeded tests wherever the canonical RNG architecture permits. For distribution requirements that cannot be proven by direct pool inspection, use sufficiently large bounded statistical tests with explicit acceptance criteria rather than one-off random expectations.

At minimum validate:

- Add Random selects only from the complete legal ordinary-Forge pool and is uniform over that pool;
- Reroll samples distinct alternatives uniformly without replacement;
- natural Essence generation implements the 1% replacement rule and 1-2 quantity distribution;
- cancelled/pre-commit paths do not consume result RNG;
- deterministic replay with the same seed produces the same Forge result sequence where the existing test harness supports deterministic replay.

---

# 54. Implementation Architecture Requirements

Forge code should depend on reusable enhancement APIs conceptually equivalent to:

```text
get canonical item affix capacity
enumerate legal ordinary-Forge properties for item+tier
test exact ordinary-Forge property legality
create canonical slot
occupy canonical slot
open canonical slot
clear property-specific runtime state
roll canonical property magnitude
rebuild/synchronize derived enhancement representation
query identification state
initialize/copy/clear canonical slot state through object lifecycle
```

If these already exist, reuse them.

If they do not, refactor existing Steps 13-16 logic into reusable helpers.

Do **not** solve the problem by copying rules into Forge code.

---

# 55. Completion Gates

Step 17 is complete only when all of the following are true.

## Architecture

- Forge uses canonical Steps 13-16 ordinary-affix acquisition/manipulation legality.
- Slots are canonical logical ordinary-affix state.
- No competing independent property representation exists.
- Sockets remain separate.

## Slot model

- slot identity stable;
- tier immutable;
- history persistent and saturating at 10 without blocking further legal operations;
- Open and unused states distinct;
- Open-slot existence, tier, and history always known;
- Open slots consume capacity;
- Open slots prevent stacking.

## Forge operations

- Add Random correct;
- Reroll correct;
- Extract correct;
- Imprint correct;
- Salvage correct.

## Economy

- all costs match this specification;
- history scaling exact;
- Salvage yield exact;
- Generic Essence acquisition exact, including 1% replacement semantics;
- Generic Essence remains permanently uncursed/neutral;
- Affix Essence ledger exact;
- Step 17 gold payment preserves canonical existing Forge semantics.

## Global Phase 1 invariants

- enhanced objects do not stack;
- Open-slot objects do not stack;
- socketed objects do not stack;
- generated stacks are never enhanced/socketed;
- ammunition excluded;
- Salvage eligibility limited to the canonical equipment domain;
- artifacts excluded as Forge crafting targets/sources/materials/donors, while existing war-hammer activation semantics remain unchanged.

## Identification

- affix manipulation requires known ordinary-affix state;
- Open slots are always known;
- Salvage does not leak any unknown Salvage-relevant state, including Quality or socket presence, before commitment.

## Transactionality

- no pre-commit side effects;
- committed Reroll cannot be refunded after candidate reveal;
- successful Step 17 actions consume one turn.

## Persistence

- all new state persists correctly;
- `EDITLEVEL` bumped once;
- no development-save migration shim;
- incompatible old data rejected cleanly.

## Regression

- Steps 13 through 16 behavior remains intact;
- existing Forge recipes remain intact;
- sockets remain intact;
- natural enhancement generation remains intact;
- save/restore/recovery remain intact;
- object initialization/copy/clone/migration/bones/billing lifecycle paths remain correct for canonical slot state.

---

# 56. Final Authoritative Rules Summary

The final Step 17 model is:

```text
Ordinary affixes live in persistent slots.

A created slot has:
    permanent identity
    permanent tier
    persistent reroll history
    optional current affix

Extraction:
    Occupied -> Open
    same tier
    history +1
    +1 exact Affix Essence in global ledger

Add Random:
    unused capacity -> new Occupied slot
    OR
    Open -> Occupied
    random legal same-tier property
    uniform selection
    no natural depth/rarity weighting

Reroll:
    Occupied -> Occupied
    same tier
    current property excluded
    up to 2 alternatives
    Keep Existing always available after payment
    history +1 even when keeping
    history saturates at 10 but does not block further legal rerolls

Imprint:
    Open -> Occupied
    same tier
    exact stored property
    cannot create slots
    history unchanged

Salvage:
    equipment-domain targets only: weapons, armor, eligible tools
    destroys item or entire selected stack
    deterministic Essence from quality + all created slot tiers
    Open slots have full tier salvage value
    0-Essence items give 2d10 gold per physical item

Generic Essence:
    physical, stackable, weight 1
    permanently uncursed/neutral
    primary source Salvage
    rare natural loot via 1% replacement of eligible general random loot
    wishable
    direct inventory only for payment

Affix Essence:
    non-physical
    exact property identity
    global persistent Forge ledger
    Extraction creates it
    Imprint consumes it

Artifacts:
    no Forge crafting participation as target/source/material/donor
    artifact war hammer may still activate Forge under existing activation semantics

Ammunition:
    no enhancements or sockets

Stacks:
    only Salvage accepts a stack target
    affixed/Open/socketed items cannot stack

Save compatibility:
    one final EDITLEVEL bump
    no backward-compatibility shim
```

This specification is the authoritative Phase 1 Step 17 design. Superseded rules from earlier drafts must not be reintroduced during implementation.