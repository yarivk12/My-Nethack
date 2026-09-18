# Step 13 — Unified Equipment Enhancement Engine (finalized by Step 14)

This document describes the current shared engine. Step 14 supersedes the
original binary elemental catalog and removes the former Cumbersome property.
The original Step 13 design and evidence remain available in Git history.
[Step 14](step14.md) specifies the finalized catalog and natural generation.

## Persistent representation and compatibility

The engine retains four inline object fields: `uint32 o_enh_props`,
`uint32 o_enh_known`, `uint8 o_enh_quality`, and `uint8 o_enh_flags`.
Windows x64 `sizeof(struct obj)` remains 112 bytes (104 before Step 13).
The historical native object serializer remains authoritative for every chain;
there is no second enhancement serializer, migration, or property-slot model.

Bits 0–6 keep their property identities: Fire I, Cold I, Shock I, Trueflight,
Warning, Searching, Stealth. Bit 7 (`0x80`) is retired and never reassigned.
Bits 8–24 extend the catalog; actual and known masks share exactly that mapping.
`OEP_ALL` is `0x01ffff7f`. The maximum of two properties is enforced separately
by assignment, normalization, and natural selection. Knowledge can also describe
absence; visibility is always the intersection of known and actual properties.
`OEF_QUALITY_KNOWN` remains bit 0 of the flags byte.

**EDITLEVEL is 8 (Step 13 used 7).** Narrowed eligibility, removed state,
changed combinations and elemental-source stacking change persisted semantics.
Old save/bones data is rejected by the native version gate, not silently
reinterpreted. No old-save migration is provided or implied.

## Shared mechanics

Only non-artifact `WEAPON_CLASS` and `ARMOR_CLASS` objects qualify. All tools,
including weapon-like tools, all gems/stones, and every other class are excluded.
Ordinary magical equipment remains eligible. Standard/Fine/Exceptional qualities
give 0/1/2 respectively: melee/direct throw hit and physical damage; launcher
ranged hit; fired-ammunition physical damage; worn armor AC for hero and monster.
Quality has the existing 0/10/20 percent valuation adjustment.

The catalog in `src/enhance.c` owns identity, tier, native property, elemental
dice, naming and ordering. Elemental tiers are 1d4, 3d4, 5d6. Primordial rolls
three separate 5d6 components. Different tiers of the same element add. Launcher
and ammunition sources roll separately, even when their bits match. Trueflight
adds at most +2 per ranged delivery and never identifies merely from a hit.

`enhancement_weapon_effects` supplies additive elemental damage at the existing
confirmed-hit hooks: hero `hmon_hitmon`, monster `hitmu` and `mdamagem`, and
`thitu_enhanced`/`ohitmon_enhanced` projectile impacts. It follows physical
mitigation and the existing Anarchic/Concordant effects; it never enters
`dmgval`. Native elemental resistances suppress only matching components.
Elemental contact is allowed against shades even when physical damage is zero.

All thirteen armor properties contribute native hero extrinsic slot bits.
Monster resistances flow through `update_mon_extrinsics`; Speed through
`mon_adjust_speed`; regeneration through `mon_regen`; displacement through the
repository's `mith_displaced`; magic resistance through `resists_magm`; reflection
through `mon_reflects`. Searching, Warning, Stealth and Slow Digestion remain
hero-only. Existing source redundancy, blocking, intrinsic interactions and caps
remain native. Removing enhanced resistance also uses native monster-knowledge
invalidation. Native secondary armor powers are filtered, including blue-dragon
Speed, white-dragon Slow Digestion, alchemy-smock resistance and chromatic armor.

## Knowledge, naming and pricing

Unknown equipment has its full mechanics. Full identification reveals quality
and properties. Visible unresisted elemental components identify their supplying
properties, including Primordial when any component manifests. Trueflight needs
normal identification. The established Step 13 Searching/Warning/Stealth equip
observations remain. Other supported observations use native speed, healing,
displacement, resistance and reflection events and require an attributable source;
redundant sources do not identify each other. No new identification-only messages
are introduced. Magic Resistance and Slow Digestion require normal identification.

Quality precedes the property prefix. One known property uses its prefix. Two
use higher-tier prefix/lower-tier suffix, with catalog order resolving equal tiers.
Partial knowledge, minimal names and bounded formatting remain supported.
T1/T2/T3/T4 surcharges are 50/100/200/400 percent of base value, added together
before applying the independent quality factor. Actual state determines shop
value regardless of player knowledge. Native enchantment, billing and credit
calculations remain in place.

## Lifecycle

Splits copy all fields. Merge compatibility requires exact equality of actual
properties, known properties, quality and flags. Base-type transformations clear
all four fields; same-type, BUC, enchantment, repair and movement operations
preserve them. Artifact conversion sanitizes generic state. There is no reroll
on transformations, inventory transfer, movement, save/restore, bones, migration,
recovery or billing. Natural acquisition is opt-in as specified in Step 14.

## Validation

`python test/run_step13.py --out _qa/step14-diagnostic` extends the original
linked native suite with `test_step14_{combat,generation,names,persistence}.c`.
It covers all object-table recipients, all armor slots, nine combat pipelines,
stacking/resistances/physical separation, identification, naming, pricing,
split/merge, artifact/type changes, billing and native level/bones codecs.
Persistence includes 5,345,280 finalized legal active-mask × class-relevant
knowledge-mask × quality × flags states, plus the retained low-byte knowledge
matrix and nested-container fixtures. Full-game save/recovery retains seven
ownership chains. Step 14 records commands, statistical counts and build evidence.
