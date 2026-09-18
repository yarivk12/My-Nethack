# Step 13 — Unified Equipment Enhancement Architecture and Implementation

## 1. Executive design decision

Step 13 should implement one generic, target-neutral enhancement layer for eligible weapons, weapon-tools, launchers, ammunition, thrown missiles, and armor. The layer should be stored inline in `struct obj`, with fixed-width fields, and should remain conceptually separate from the imported `obranch_*` metadata.

The recommended persistent representation is:

```c
uint32 o_enh_props;          /* active generic property bits */
uint32 o_enh_known;          /* property bits whose presence is known */
uint8  o_enh_quality;        /* OQ_STANDARD, OQ_FINE, OQ_EXCEPTIONAL */
uint8  o_enh_flags;          /* OEF_QUALITY_KNOWN, reserved bits zero */
```

These fields belong beside the existing per-object state in `include/obj.h`, immediately after the current `obranch_*` fields and before the remaining object payload. They must be zero for a newly created ordinary object and must be cleared whenever an object changes into an ineligible class or an artifact. The fixed-width aliases are already provided by `include/integer.h`; no native `long` should be introduced for the new masks.

The engine should expose a small API from a new `include/enhance.h` / `src/enhance.c` pair. Callers should ask the engine for eligibility, hit/damage bonuses, post-hit effects, worn properties, display state, price adjustment, and artifact sanitization. Acquisition systems in Steps 14–18 should set state through that API rather than writing combat or worn-effect code of their own.

Artifacts remain an explicit boundary: generic enhancement state is not added to an artifact and is stripped if an ordinary object is converted into one. Artifact abilities continue to come from `artilist.h` and `src/artifact.c`.

The current historical save/bones codec may be retained for the new epoch because Phase 1 expressly does not promise compatibility with old files. The Step 13 implementation must increment `EDITLEVEL` from 6 to 7, verify the resulting `sizeof(struct obj)` through the existing critical-size header, and provide round-trip tests for saves and bones produced by the new executable.

**Decision: the repository is architecturally ready for Step 13 implementation once this contract is accepted.**

## 2. Audited repository baseline

The audit was performed against the current working tree, not the older Phase 1 research snapshot.

| Item | Verified value |
|---|---|
| Branch | `phase1/equipment-enhancement` |
| Current `HEAD` | `f27b7f20fd90da8dcb7aa0e90ee85f1af1123444` |
| Transition commit | `Phase 1: establish equipment enhancement baseline` |
| Phase 0 tag | `phase0-dod-expansion-complete` |
| Phase 0 tag commit | `216bf60903cbde8b3ef1de33aeb7368a09ba6719` |
| Existing untracked state | `.codegraph/` (pre-existing; preserve and do not commit) |
| Current save epoch | `EDITLEVEL 6` in `include/patchlevel.h` |
| Step 13 gameplay implementation at audit start | None |

`doc/phase1.md:1-117` is the governing Phase 1 contract. It identifies Step 13 as the next milestone, limits the initial scope to equipment and generic metadata, requires fixed-width persistent representations where relevant, forbids artifact fusion, and explicitly rejects migration/backward compatibility for old save and bones formats.

Audit notation: statements tied to current source locations or repository state are **Verified current behavior**; statements derived from `doc/phase1.md` are **Phase 1 requirement**; recommendations introduced by this document are **Proposed Step 13 design**; Section 21 contains **Deferred decisions**, and Section 22 contains **Genuine unresolved blockers**. The audit does not present proposed behavior as an existing implementation.

The current worktree was not reset, cleaned, stashed, committed, tagged, or pushed during this audit.

## 3. Current object model and `obranch_*` interaction

`include/obj.h:35-198` defines the single runtime object model. The important existing state is:

| Area | Current fields / behavior | Step 13 consequence |
|---|---|---|
| Identity and links | `nobj`, `v`, `cobj`, `o_id` (`include/obj.h:35-47`) | Enhancement state follows the object instance and is copied by normal object-copy paths. |
| Type and stack | `otyp`, `oclass`, `quan`, `owt` (`include/obj.h:43-52`) | Eligibility is derived from the current object table, not stored as a second class. |
| Native quality | `spe` (`include/obj.h:49-72`) | Generic quality must not overload `spe`; existing enchantment/charge semantics remain intact. |
| Artifact identity | `oartifact` (`include/obj.h:49-72`) | Artifact state is mutually exclusive with generic enhancement state. |
| Knowledge | `known`, `dknown`, `bknown`, `rknown`, and related flags (`include/obj.h:90-158`) | Generic property/quality knowledge needs its own bits; `known` cannot represent partial property knowledge safely. |
| Existing donor metadata | `obranch_props`, `obranch_material`, `obranch_size` (`include/obj.h:160-183`) | Keep this dimension separate and preserve it independently. |
| Persistence payload | `oeaten`, `age`, `owornmask`, Lua and migration fields (`include/obj.h:186-198`) | New fields must be placed in the persistent object body and included in the existing object codec. |

The current `obranch_*` model is already a real gameplay system, not unused storage:

- `include/objclass.h:194-196` makes `obranch_material` override the base material, and `src/mkobj.c:1973-1997` uses it for weight together with branch size.
- `src/invent.c:4432-4556` compares branch properties, material, and size before merging stacks.
- `src/objnam.c:954-989` exposes branch size, material, and selected branch property names in object descriptions.
- `src/weapon.c:346-505` consumes branch size/material in damage calculation; `src/weapon.c:940-980` consumes branch size for handedness.
- `src/weapon.c:2152-2257` applies branch coatings after a confirmed hit and may consume coating bits.
- `src/shknam.c:574-632`, `src/sp_lev.c:2241-2314`, and monster-generation code assign branch metadata during generation.

The branch property constants are donor-specific (`OBP_ANARCHIC` through `OBP_DEEP`, with `OBP_COATINGS`) at `include/obj.h:171-183`. Generic Step 13 bits must not reuse those values or be placed in `obranch_props`. A shared helper may later combine display or validation code, but the two masks must remain distinct in storage and semantics.

## 4. Object lifecycle map

### Creation

- `src/mkobj.c:1210-1329` (`mksobj`) allocates a new object, copies `cg.zeroobj`, assigns identity/type/quantity, calls `unknow_object`, performs type-specific initialization, and may create a unique artifact.
- `src/mkobj.c:870-1119` (`mksobj_init`) assigns native random weapon/armor `spe`, BUC, poison, and artifact state.
- `src/mkobj.c:200-250` and the broader `mkobj` path create ordinary random objects.
- `src/makemon.c:148-235`, `:396-705`, and later equipment tables create monster weapons, ammunition, tools, and armor through `mksobj`, `mongets`, and `mpickobj`.
- `src/shknam.c:574-632` can replace shop stock and add existing branch metadata.
- `src/sp_lev.c:2241-2314` constructs Lua-defined level objects and copies branch metadata from descriptors.

Step 13 must not add random enhancement generation to these paths; that belongs to Step 14. It must ensure every constructor starts with zero generic state and that a later generator can call one validated setter.

### Ownership and movement

The same object instance moves through inventory, monster inventory, floor, container, migrating, buried, and bill-object chains. Relevant owners are:

- hero inventory: `src/invent.c:1150-1155` (`addinv`) and its `addinv_core0` path;
- monster inventory: `src/mkobj.c:2747-2769` (`add_to_minv`);
- container contents: `src/mkobj.c:2772-2799` (`add_to_container`);
- migrating and buried lists: `src/mkobj.c:2802-2832`;
- floor placement and extraction: `src/mkobj.c:2410+` and `src/mkobj.c:2650+`;
- shop bill objects: `src/shk.c:3337-3405` and `src/shk.c:3480-3625`.

These paths do not reconstruct enhancement state; they move the object or merge it. The implementation must therefore make the inline fields safe under ordinary pointer movement and test every owner.

### Copy and split

`src/mkobj.c:459-505` (`splitobj`) performs `*otmp = *obj`, assigns a new identity, adjusts quantity/weight, clears worn/timer/Lua-specific state, copies `oextra`, and preserves the remaining object payload. The proposed inline enhancement fields will therefore split correctly automatically, provided no setter is added that treats a stack as individually heterogeneous.

### Merge and deletion

`src/invent.c:4432-4556` (`mergable`) is the single compatibility predicate used by hero, monster, floor, and container merges. `src/invent.c:814-948` (`merged`) retains the destination object, updates quantity/weight, and frees the other object. Generic fields must be added to `mergable`; they must never be combined by arithmetic or union.

`obj_extract_self`, `obfree`, `delobj`, and `dealloc_obj` remove/free the complete object. Generic state requires no destructor, but tests must prove that deletion of an enhanced object does not leave stale worn or shop references.

### Type-changing paths

`src/zap.c:1753-2005` (`poly_obj`) replaces the object with a fresh `mksobj` result and therefore starts generic state at zero. Other transformations mutate an existing object in place, including paths in `src/potion.c:2514-2606`, `src/uhitm.c:1242-1300`, and trap/object transformation code. The implementation must call `enhancement_normalize(obj)` after every in-place type/class change: preserve state only if the resulting object remains eligible and the change is explicitly defined as identity-preserving; otherwise clear it. The safe Step 13 default is to clear generic state on any type-changing conversion.

## 5. Weapon and armor integration paths

### Eligibility boundary

The authoritative current predicates are in `include/obj.h:228-283` and `:294-313`.

- `WEAPON_CLASS` covers ordinary weapons, launchers, ammunition, and thrown weapons.
- `is_weptool(o)` includes `TOOL_CLASS` objects with a non-`P_NONE` weapon skill; these are weapon-like and are in Step 13 scope when used as weapons.
- `is_launcher(o)` identifies `WEAPON_CLASS` launchers.
- `is_ammo(o)` includes weapon-class ammunition and qualifying `GEM_CLASS` ammunition.
- `is_missile(o)` includes qualifying weapon-class and tool-class thrown missiles.
- `ARMOR_CLASS` is partitioned into suit, shield, helmet, gloves, boots, cloak, and shirt by `is_suit`, `is_shield`, `is_helmet`, `is_gloves`, `is_boots`, `is_cloak`, and `is_shirt`.

Step 13 includes all `ARMOR_CLASS` slots and weapon-like objects covered by the predicates above. It excludes rings, amulets, ordinary tools such as towels and blindfolds, potions, gems that are not ammunition, and unrelated object classes. This explicit rule avoids assuming that `oclass == WEAPON_CLASS` is the whole weapon universe.

### Hero melee and ranged combat

- Melee damage starts in `src/uhitm.c:989-1110` (`hmon_hitmon_weapon_melee`) and uses `src/weapon.c:346-505` (`dmgval`).
- Hero weapon dispatch between melee and ranged is `src/uhitm.c:1126-1148` (`hmon_hitmon_weapon`); ranged handling is `hmon_hitmon_weapon_ranged`.
- Hero throw/projectile resolution is `src/dothrow.c:2073-2155` (`thitmonst`), which uses `omon_adj`, `hitval`, and `dmgval`; impact damage is continued around `src/dothrow.c:1349-1400`.
- `src/weapon.c:189-227` (`hitval`) is the shared object-to-hit calculation and is the correct common surface for `CUMBERSOME` and the single ranged `TRUEFLIGHT` bonus, with launcher context supplied where the shot has both launcher and ammunition.

Generic quality and property damage must be applied once after base physical damage and before the existing post-hit donor coating hook. They must not be added to `dmgval` if they can produce side effects there: the current code explicitly keeps `mith_weapon_effects` out of `dmgval` because `dmgval` is called speculatively (`src/weapon.c:2149-2153`).

### Monster melee and projectiles

- Monster melee uses `src/mhitm.c:395-424`, `:1138-1162`; the weapon is `mwep` or `MON_WEP` and damage is finalized before `mith_weapon_effects`.
- Monster attacks against the hero use `src/mhitu.c:1318-1335`; the weapon is `mhm.weapon` and the same post-physical-damage hook is available.
- Monster projectile hit calculation and damage use `src/mthrowu.c:264-318` (`monshoot`), `:367-515` (`ohitmon`), and `:72-159` (`thitu` for the hero target).

The generic engine needs one side-effect-free `enhancement_hit_bonus(obj, launcher, target, mode)` and one post-hit `enhancement_weapon_effects(obj, launcher, target, damage, mode)`. Every relevant hero and monster attack path listed above must call the latter exactly once. The existing `mith_weapon_effects` call remains separate and must continue to consume only `obranch_props` coatings.

### Worn armor

Hero slots are `uarm`, `uarmc`, `uarmh`, `uarms`, `uarmg`, `uarmf`, and `uarmu` (`include/decl.h:94-96`). `src/do_wear.c:2500-2545` (`find_ac`) sums native `ARM_BONUS` and artifact bonuses for each slot.

`src/worn.c:250-381` (`setworn`) and `src/worn.c:383-420` (`setnotworn`) are the central add/remove surfaces for worn properties. They update `u.uprops[p].extrinsic`, blocked properties, artifact intrinsics, and inventory display. Generic armor properties must be applied and removed there through a helper that maps the generic bit to an existing property index.

Monsters choose and wear armor through `src/worn.c:1007-1261` (`m_dowear`/`m_dowear_type`), use the object’s `owornmask`, and receive native resistances through `src/worn.c:814-959` (`update_mon_extrinsics`). Monster armor class is computed in `src/worn.c:963-985` (`find_mac`). Repository-evidence-driven exception approved for implementation: `OEP_WARNING`, `OEP_SEARCHING`, and `OEP_STEALTH` are hero-only worn/extrinsic effects. Native `update_mon_extrinsics` explicitly treats Stealth as inert and has no Warning/Searching storage or gameplay semantics. Monsters may carry and wear these items without losing enhancement state, but these three properties remain inert on monsters. Step 13 adds no monster storage, AI, or perception semantics. Monster armor quality and weapon combat enhancements remain active.

## 6. Step 13 metadata design

### Storage

Add the following definitions to a new enhancement header and the four fixed-width fields to `struct obj`:

```c
enum enhancement_quality {
    OQ_STANDARD = 0,
    OQ_FINE = 1,
    OQ_EXCEPTIONAL = 2
};

#define OEP_FIRE        0x00000001U
#define OEP_COLD        0x00000002U
#define OEP_SHOCK       0x00000004U
#define OEP_TRUEFLIGHT  0x00000008U
#define OEP_WARNING     0x00000010U
#define OEP_SEARCHING   0x00000020U
#define OEP_STEALTH     0x00000040U
#define OEP_CUMBERSOME  0x00000080U

#define OEF_QUALITY_KNOWN 0x01U
```

`OEP_*` values are generic Step 13 identifiers and are deliberately not `OBP_*` values. Bits 8–31 remain available for Steps 15–18. `o_enh_known` is a per-property knowledge mask: a set bit means the player knows whether that property is present. `OEF_QUALITY_KNOWN` is independent because quality is not a property bit.

### Invariants

1. `o_enh_props` may contain only defined bits and only bits allowed by the current object eligibility matrix.
2. `o_enh_quality` is one of the three enum values; invalid values are normalized to `OQ_STANDARD` and logged as an impossible state in debug builds.
3. `o_enh_known` may contain only defined property bits.
4. Artifact objects have all four generic fields cleared.
5. Ineligible object classes have all four generic fields cleared.
6. Generic state does not change `spe`, `oartifact`, `obranch_*`, material, size, erosion, quantity, or object identity.
7. A mutation of an unpaid object must recalculate its shop price before the mutation becomes visible to the player.

### API contract

The implementation should provide at least:

```c
enum enhance_use {
    ENHANCE_MELEE = 0,
    ENHANCE_THROWN,
    ENHANCE_LAUNCHER,
    ENHANCE_AMMO,
    ENHANCE_ARMOR
};

boolean enhancement_eligible(const struct obj *obj);
boolean enhancement_property_allowed(const struct obj *obj, uint32 prop);
void enhancement_normalize(struct obj *obj);
boolean enhancement_set(struct obj *obj, uint32 props,
                        enum enhancement_quality quality, boolean known);
uint32 enhancement_visible_props(const struct obj *obj, boolean force_id);
int enhancement_quality_bonus(const struct obj *obj, enum enhance_use use);
int enhancement_hit_bonus(const struct obj *obj, const struct obj *launcher,
                          const struct monst *target, enum enhance_use use);
int enhancement_damage_bonus(const struct obj *obj, const struct obj *launcher,
                             const struct monst *target, enum enhance_use use);
int enhancement_weapon_effects(struct obj *obj, const struct obj *launcher,
                               struct monst *target, int damage,
                               enum enhance_use use);
void enhancement_worn_on(struct obj *obj, struct monst *wearer);
void enhancement_worn_off(struct obj *obj, struct monst *wearer);
void enhancement_strip_for_artifact(struct obj *obj);
long enhancement_price_adjustment(const struct obj *obj);
```

The enum names above are the proposed stable use-mode vocabulary; implementation may add an internal delivery-context structure, but it must preserve the separation between validation, pure combat queries, side-effecting post-hit effects, worn-property application, persistence state, and pricing.

### Footprint

The logical payload is 10 bytes: two 4-byte masks and two 1-byte fields. Normal C alignment may add padding, so the exact `sizeof(struct obj)` increase must be measured by the authoritative build rather than guessed. `src/version.c:546-664` already records `sizeof(struct obj)` as a critical byte. The Step 13 compile/test contract must record the before/after sizes and verify that the new size is accepted only by the new save epoch.

## 7. Initial property catalogue

The initial catalogue is intentionally small and exercises the required integration surfaces.

| Bit | Name | Eligible recipients | Positive/negative behavior | Identification |
|---|---|---|---|---|
| `OEP_FIRE` | Fire | weapon-like melee objects, launchers, ammunition, thrown missiles | Adds `1d4` fire damage once on a confirmed physical hit; fire resistance suppresses the extra damage. | Reveal when the player observes the property’s hit message or fully identifies the item. |
| `OEP_COLD` | Frost | same weapon/delivery classes as Fire | Adds `1d4` cold damage once on a confirmed physical hit; cold resistance suppresses the extra damage. | Same as Fire. |
| `OEP_SHOCK` | Shock | same weapon/delivery classes as Fire | Adds `1d4` shock damage once on a confirmed physical hit; shock resistance suppresses the extra damage. | Same as Fire. |
| `OEP_TRUEFLIGHT` | Trueflight | launchers, ammunition, thrown missiles | Adds `+2` to-hit for a ranged delivery. If launcher and ammunition both have it, the shot receives only one `+2`. | Reveal when the player’s ranged attack consumes the bonus or via identification. |
| `OEP_WARNING` | Warning | all armor slots | While worn, maps to the existing `WARNING` property in `u.uprops`; no new warning subsystem. | Reveal on observed worn/status effect or identification. |
| `OEP_SEARCHING` | Searching | all armor slots | While worn, maps to `SEARCHING`; existing automatic-search rules consume the result. | Reveal on observed worn/status effect or identification. |
| `OEP_STEALTH` | Stealth | all armor slots | While worn, maps to `STEALTH`; existing stealth state consumes the result. | Reveal on observed worn/status effect or identification. |
| `OEP_CUMBERSOME` | Cumbersome | weapon-like melee objects, launchers, ammunition, thrown missiles | Applies `-2` to-hit through the shared `hitval`/ranged-adjustment surface. It does not alter damage or breakage. | Reveal when the penalty is observed or via identification. |

The Fire/Frost/Shock values are deliberately low and use the target’s existing resistance functions. They do not grant the wielder resistance, create permanent monster status, bypass artifact defenses, or consume generic bits. The existing donor coating system remains responsible for `OBP_*` status effects and consumption.

Quality behavior is separate from the property catalogue:

- `OQ_STANDARD`: no generic bonus;
- `OQ_FINE`: `+1` to-hit and `+1` physical damage for a melee or thrown weapon, `+1` projectile damage for ammunition, `+1` ranged hit for a launcher, and `+1` armor class for armor;
- `OQ_EXCEPTIONAL`: the same bonuses at `+2`.

For a matched launcher/ammunition shot, launcher quality supplies the ranged hit bonus and ammunition quality supplies projectile damage. A thrown object uses its own quality for both. Armor quality is added by `find_ac`/`find_mac` through the enhancement helper. Quality never changes native `spe`, erosion, weight, or material.

## 8. Quality model

Quality is a three-state ordinal, not another enchantment counter. It is intentionally orthogonal to native `spe`:

| Quality | Stored value | Display | Weapon use | Armor use |
|---|---:|---|---|---|
| Standard | 0 | no quality adjective | no generic bonus | no generic bonus |
| Fine | 1 | `fine` when known | +1 in the applicable hit/damage channel | +1 AC |
| Exceptional | 2 | `exceptional` when known | +2 in the applicable hit/damage channel | +2 AC |

Quality may be present on a stack only when every item in the stack has the same quality. It is never averaged or downgraded by merging. Splitting copies it exactly. The Step 13 engine should expose `enhancement_quality_bonus` so later forging and gemstone code cannot duplicate the arithmetic.

Quality is not generated randomly in Step 13. Step 14 owns depth-scaled acquisition and must use this enum and the recipient matrix. Step 15 may upgrade quality through a transaction that also recalculates price and inventory display.

## 9. Recipient eligibility matrix

| Object predicate | Quality | Fire/Frost/Shock | Trueflight | Cumbersome | Warning/Search/Stealth | Notes |
|---|---:|---:|---:|---:|---:|---|
| Ordinary `WEAPON_CLASS` melee weapon | Yes | Yes | No | Yes | No | Includes swords, axes, polearms, blunt weapons, and native weapon artifacts only before artifact conversion. |
| `is_weptool(obj)` `TOOL_CLASS` | Yes | Yes | No unless separately classified as a ranged delivery | Yes | No | Covers wieldable weapon-tools; ordinary tools remain out of scope. |
| `is_launcher(obj)` | Yes | Yes as shot-source | Yes | Yes for direct use / launcher shot penalty | No | Launcher properties are applied to the shot once, with ammo union rules. |
| `is_ammo(obj)` | Yes | Yes | Yes | Yes | No | Includes qualifying `GEM_CLASS` ammunition. |
| `is_missile(obj)` thrown weapon/tool | Yes | Yes | Yes | Yes | No | Applies to the thrown delivery path. |
| `ARMOR_CLASS` suit | Yes | No | No | No | Yes | All armor slots use the existing worn property indices. |
| `ARMOR_CLASS` shield | Yes | No | No | No | Yes | Shield remains a normal armor slot; no special stacking rule. |
| `ARMOR_CLASS` helmet | Yes | No | No | No | Yes | Existing helmet restrictions remain unchanged. |
| `ARMOR_CLASS` gloves | Yes | No | No | No | Yes | Existing bow-glove rules remain; quality is an additional small AC bonus. |
| `ARMOR_CLASS` boots | Yes | No | No | No | Yes | Existing movement/fit rules remain. |
| `ARMOR_CLASS` cloak | Yes | No | No | No | Yes | Existing cloak blocking remains. |
| `ARMOR_CLASS` shirt | Yes | No | No | No | Yes | Existing shirt/suit layering remains. |
| Ring, amulet, ordinary tool, potion, scroll, food, gem not used as ammo, ball, chain, or other class | No | No | No | No | No | Normalize any accidental state to zero. |
| Any `oartifact != 0` object | No | No | No | No | No | Artifact powers stay in the artifact subsystem. |

The matrix is based on current predicates, not object-name lists. New weapon-tools added later become eligible automatically only if they satisfy the explicit predicate and the property policy; they do not gain armor properties merely because their class changes.

## 10. Naming and identification

Current naming is layered: `src/objnam.c:581-989` builds the basic name and branch metadata, while `doname_base` at `src/objnam.c:1259-1729` adds quantity, BUC, erosion, native `spe`, worn state, and pricing. Step 13 should extend those layers without changing `oartifact` naming.

Recommended normal descriptions when the relevant state is known are:

- `fine +1 fire sword`;
- `exceptional +0 trueflight arrow`;
- `warning exceptional +2 cloak`;
- `cumbersome -1 dagger`.

The exact adjective order should be fixed by the implementation test, but it must be deterministic and must preserve the existing `obranch_*` prefixes and native `spe` placement. Unknown property bits and unknown quality must contribute no adjective. Known absence is also silent; the player should not see “non-fire.”

`xname_flags` should be responsible for property adjectives, and `doname_base` should be responsible for quality adjectives if that produces the cleanest native grammar. `simpleonames`, `ysimple_name`, and all temporary “bare object” copies must clear or suppress the new fields exactly as they already suppress native identification details (`src/objnam.c:1080-1122`).

Full identification must set all allowed property bits in `o_enh_known` and set `OEF_QUALITY_KNOWN`. Observation helpers should update only the item’s knowledge, never the active state. Monster-owned hidden items must not reveal generic state merely because the monster used them; visible player-owned effects may reveal the observed bit according to the table in Section 7.

`readobjnam` (`src/objnam.c:5000+`) continues to parse ordinary object names, native `spe`, BUC, and artifact names. Step 13 does not add wish syntax for generic properties or quality. That avoids creating a second acquisition/parser contract before forging and gemstone affixing exist.

## 11. Merge/split/stack semantics

`mergable` must reject a merge unless all of the following match exactly:

- `o_enh_props`;
- `o_enh_known`;
- `o_enh_quality`;
- `o_enh_flags` (including quality knowledge);
- current native merge fields already checked by `src/invent.c:4432-4556`.

This is a strict equality rule. It prevents an unknown ordinary item from merging with a known enhanced item, prevents active-property loss, and prevents a partial-identification leak through stack descriptions. `merged` continues to retain the destination object and only adds quantity/weight; it must not OR masks, choose the higher quality, or union knowledge.

`splitobj` already copies the complete `struct obj` at `src/mkobj.c:469-481`, so all enhancement fields copy exactly to both stacks. The new tests must assert that splitting an enhanced stack preserves active bits, knowledge bits, quality, branch metadata, native `spe`, shop state, and names independently.

Stacks are permitted for enhanced ammunition and other types whose object table has `oc_merge`. A stack with distinct generic state occupies distinct inventory/floor stacks. `nomerge` remains an explicit caller override. `unsplitobj` uses the same strict predicate and therefore cannot silently recombine divergent state.

## 12. Artifact policy

Generic enhancement and artifact state are mutually exclusive.

### Ordinary object converted by naming

`src/do_name.c:371-425` (`oname`) stores the requested name and calls `artifact_exists`. If the name creates an artifact, the implementation must clear generic fields at the central artifact transition before the item is displayed or billed. A custom non-artifact name does not clear or alter generic state.

### Random/programmatic artifact conversion

`src/artifact.c:173-310` (`mk_artifact`) converts an existing object, resets erosion, names it, sets `oartifact`, and applies artifact origin. It must call the same strip helper. Direct creation paths that call `mksobj` for an artifact must start with zero generic state. `src/artifact.c:370-422` (`artifact_exists`) also has explicit material side effects for named artifacts and is the correct central guard for both creation and un-creation.

### Fountain and other named conversions

`src/fountain.c:392-442` converts a long sword to Excalibur through `oname`; the central transition must strip generic state before the artifact becomes active. The same rule covers naming, wishing, gifts, level-defined artifacts, bones origin, random artifacts, and any later programmatic path that calls `oname` or `mk_artifact`.

### Artifact gameplay

Artifact attack/defense/intrinsic logic remains in `src/artifact.c`, including `attacks`, `defends`, `set_artifact_intrinsic`, and `artifact_arm_bonus`. `setworn` must not apply both artifact powers and generic enhancement powers to an artifact. No artifact fusion, artifact property transfer, or generic-property inheritance is part of Step 13.

## 13. Shops/economy

Current shop prices are computed from `src/shk.c:4349-4388` (`getprice`) and wrapped by `get_cost` at `src/shk.c:2905-3010`. Current positive weapon/armor `spe` adds `10 * spe` to the base price; artifacts use `arti_cost` (`src/artifact.c:2550-2560`). Billing records the computed per-unit price in `bill_x` (`src/shk.c:3337-3393`), and mergeability already checks `same_price` for unpaid stacks (`src/invent.c:4515-4517`).

Step 13 should include generic state in `getprice` for non-artifact eligible objects, independent of player knowledge, so the shopkeeper’s price is stable even when the player cannot yet identify the property. Use this exact initial adjustment:

```text
price_percent = 100
                 + 10 * quality_value
                 + 20 * count(Fire, Frost, Shock, Trueflight,
                              Warning, Searching, Stealth)
                 - 10 * count(Cumbersome)
adjusted = max(1, base_price * price_percent / 100)
```

Apply the modifier after the existing native `spe`/artifact selection and before existing charisma, role, knowledge, and angry-shopkeeper adjustments. Do not modify artifact cost. For `TOOL_CLASS` weapon-tools, the generic price policy follows the new enhancement eligibility predicate even though the current `getprice` switch only groups armor and weapons.

The following rules are required:

- generated shop stock is priced after its complete generic state is assigned;
- an unpaid object cannot be mutated without calling `alter_cost(obj, 0L)` or an equivalent bill update;
- splitting a billed stack preserves the same per-unit generic price and continues to use `nextoid` for the existing object-ID surcharge;
- merging requires both generic state equality and `same_price` as today;
- dropping, throwing, selling, stealing, and placing an enhanced object in a container use the existing bill transitions unchanged;
- generic state does not affect weight or material pricing; `obranch_material` remains independent.

## 14. Persistence/save/bones design

The current save architecture is a native historical struct codec:

- `src/sfstruct.c:27-40` writes/reads historical `struct obj` with `sizeof *d_obj`;
- `src/save.c:752-786` (`saveobj`) writes `sizeof(struct obj)` followed by the optional `oextra` payload;
- `src/restore.c:182-229` (`restobj`) reads the object and reconstructs `oextra`;
- `src/save.c:788-829` and `src/restore.c:231-304` recurse through object chains and container contents;
- `src/files.c:828-985`, `:1148-1244`, and the level-file paths create/open bones and save files with `structlevel = TRUE`.

Adding the fixed-width fields to `struct obj` automatically includes them in ordinary saves, level files, monster inventories, containers, buried objects, migrating objects, bill objects, and bones. No parallel `oextra` codec or custom object payload is necessary for Step 13.

`src/version.c:541-664` includes `sizeof(struct obj)` in the critical-size list, writes the struct/data-model indicator, and checks it on restore at `src/version.c:676-834`. Because Phase 1 does not require old save/bones compatibility, Step 13 must:

1. increment `EDITLEVEL` from 6 to 7 in `include/patchlevel.h`;
2. keep `SAVEFILE_REVISION_LEVEL` unchanged unless implementation discovers a separate revision need;
3. reject all pre-Step-13 saves and bones through the existing incarnation check;
4. verify the new object size and all fixed-width field sizes in the critical-size header;
5. round-trip enhanced objects through normal save/restore and bones restore;
6. test both hero and monster inventories, nested containers, floor, buried, migrating, and bill-object chains.

This is fixed-width for the new logical fields but remains the project’s current native historical file format. Cross-data-model file interchange is not promised. A future fully portable field-level object codec is outside Step 13 and must not be smuggled into the enhancement implementation.

## 15. Step 13 vs Step 14 boundary

### Step 13 owns

- the object fields, masks, enum, invariants, and validation API;
- the initial property catalogue and exact effects;
- quality arithmetic and display/knowledge semantics;
- all attack and worn-property integration surfaces;
- merge/split/stack rules;
- artifact exclusion and conversion sanitization;
- shop price integration;
- save/bones epoch handling and test fixtures;
- source/runtime verification of the engine with hand-constructed fixtures.

### Step 14 owns

- depth-scaled random enhanced equipment generation;
- probability curves for DoD depths 1–200;
- placement/loot policies and eligible generation contexts;
- whether a generated property is common, rare, or restricted by depth;
- preventing generator output from overwhelming native artifacts and native `spe`.

Step 13 must not add random property rolls to `mksobj`, `mksobj_init`, monster creation, shop stock, or level generation. Step 14 should be able to add generation by calling the Step 13 setter without changing combat, naming, persistence, or billing code.

## 16. Compatibility with Steps 15–18

| Later step | Required compatibility |
|---|---|
| Step 15 — Forging | Must call the common setter/transaction API, validate recipient class, update knowledge and price, and refuse artifacts unless a later explicit artifact policy is approved. |
| Step 16 — Gemstone affixing | Must allocate future `OEP_*` bits or a separate future payload only after checking the 32-bit headroom; it must not reuse `obranch_material` or overload quality. |
| Step 17 — Advanced properties | May add combinations and target-specific effects behind the same pure-query/post-hit/worn-effect interfaces. Combination state must remain deterministic and merge-safe. |
| Step 18 — Endgame/Mythic-Lite | May add a new provenance or progression field only through a separately versioned design; it must not make artifacts inherit generic state accidentally. |

The four-field layout intentionally leaves property-bit headroom and reserved flags. It does not reserve unimplemented mechanics, gemstone IDs, donor provenance, or artifact fusion markers. Those are deferred decisions, not hidden meanings.

## 17. Implementation file/function map

The following is the smallest coherent implementation surface.

| File | Planned responsibility |
|---|---|
| `include/obj.h` | Add the four fixed-width fields near `obranch_*`; keep native object layout comments accurate. |
| `include/enhance.h` | Define quality enum, `OEP_*`/`OEF_*` masks, use-mode enum, and public engine prototypes. |
| `src/enhance.c` | Implement eligibility, validation, setter/normalizer, quality math, pure hit/damage queries, post-hit elemental effects, worn-property mapping, artifact strip, and price adjustment. |
| `include/extern.h` | Export the public enhancement functions if the project’s header convention requires it. |
| `src/mkobj.c` | Keep constructor zeroing explicit; ensure any generic constructor helper is invoked before weight/merge. |
| `src/invent.c` | Add exact generic-field comparisons to `mergable`; preserve strict stack semantics. |
| `src/objnam.c` | Display known generic properties and quality; suppress them in simple/bare-name temporary objects; keep artifact naming intact. |
| `src/weapon.c` | Add generic hit bonus to `hitval`, generic base damage/quality query where pure, and expose the shared post-hit handoff without changing `mith_weapon_effects`. |
| `src/uhitm.c` | Pass launcher/ammunition context through hero melee/ranged handling and call the generic post-hit hook once. |
| `src/dothrow.c` | Apply ranged/thrown hit context and generic effects in `thitmonst`; preserve shop return/breakage flow. |
| `src/mthrowu.c` | Apply generic effects in `ohitmon`/`thitu` and pass the matched launcher for monster shots. |
| `src/mhitm.c` / `src/mhitu.c` | Apply generic post-physical-damage effects once for monster melee attacks. |
| `src/worn.c` | Apply/remove generic armor properties in `setworn`, `setnotworn`, `update_mon_extrinsics`; add generic armor bonus to `find_mac`. |
| `src/do_wear.c` | Add generic armor quality to hero `find_ac` and ensure wear/takeoff status updates expose the correct knowledge. |
| `src/shk.c` | Add generic non-artifact price adjustment to `getprice`; update unpaid objects through existing billing functions. |
| `src/artifact.c` / `src/do_name.c` | Centralize strip-on-artifact conversion across `artifact_exists`, `mk_artifact`, and `oname`. |
| `src/zap.c` and other in-place transformation owners | Call `enhancement_normalize` after direct type/class mutation. |
| `include/patchlevel.h` | Bump `EDITLEVEL` to 7 for the changed persistent `struct obj`. |
| `test/test_step13_source.py` | Assert field types, disjoint masks, all lifecycle hook names, artifact guards, save epoch, and unchanged donor `obranch_*` ownership. |
| `test/test_step13_runtime.c` | Exercise pure engine functions, property matrix, quality, hit/damage, worn mapping, naming fixtures, price math, and merge/split behavior. |
| `test/run_step13.py` | Build/run the diagnostic fixture plus the authoritative Release x64 save/bones flow, following the existing `test/run_step11.py` and `test/run_step11_save.py` conventions. |

`src/save.c` and `src/restore.c` should not receive an ad hoc second object codec. Their existing `Sfo_obj`/`Sfi_obj` calls are the serialization surface; the Step 13 tests must prove that the new fields travel through it.

## 18. Risk register

| Risk | Consequence | Mitigation / acceptance condition |
|---|---|---|
| Historical raw `struct obj` grows | Old files read the wrong offsets or are rejected | Bump `EDITLEVEL`, verify critical size, test rejection of epoch 6 and round-trip epoch 7. |
| Fixed-width fields still sit inside a native struct | Cross-platform byte interchange is not guaranteed | Document native historical format explicitly; rely on data-model/version gate; do not claim portable files. |
| Generic state is copied into an artifact | Native and generic powers stack unexpectedly | Strip in `artifact_exists`, `mk_artifact`, and `oname` transition; add direct/naming/fountain tests. |
| Generic property is applied inside `dmgval` | Speculative calls produce damage/status side effects | Keep pure queries separate from post-hit effects; assert one side-effecting call per attack path. |
| Hero and monster attack paths diverge | Property works only for player melee or only for projectiles | Matrix test hero melee, hero throw, launcher+ammo, monster melee, monster projectile, and monster-to-monster. |
| Launcher and ammo both contribute twice | Ranged damage/to-hit inflation | Union property bits once; assign launcher quality to hit and ammo quality to damage; Trueflight caps at +2. |
| Unknown state merges with known state | Identification leak or lost metadata | Compare active state, knowledge state, quality, and flags exactly in `mergable`. |
| A stack split creates mismatched bill price | Shop accounting corruption | Preserve fields and use existing `nextoid`/`same_price`; test unpaid enhanced ammunition. |
| Naming output collides with branch names | Ambiguous or malformed descriptions | Add naming contract tests with each branch property/material/size plus every generic adjective and unknown state. |
| Generic armor bypasses native extrinsics | Hero effects survive removal, or monster behavior changes | Route hero effects through `setworn`/`setnotworn`; verify native monster wear/remove remains inert and preserves metadata. Test quality AC separately. |
| `TOOL_CLASS` weapon-tools are omitted | Pick-axes/unicorn horns receive inconsistent behavior | Use `is_weptool`, `is_launcher`, `is_ammo`, and `is_missile` predicates rather than class-only checks. |
| Direct in-place type changes retain state | Ineligible objects carry stale enhancements | Normalize after every direct conversion and add transformation fixtures. |
| Price is based on hidden state inconsistently | Shopkeeper price changes on identification or stack merge | Price active generic state independent of `o_enh_known`; update billed objects on mutation. |
| Future steps exhaust the mask or overload quality | Later work requires another incompatible layout | Reserve bits/flags, keep acquisition behind the common API, and defer provenance/gemstone IDs explicitly. |

## 19. Verification contract

The following must pass for Step 13 implementation completion. The audit itself does not claim these implementation tests have run.

### Source and compile contract

1. `test/test_step13_source.py` confirms the four fields use `uint32`/`uint8`, `OEP_*` is disjoint from `OBP_*`, artifact stripping exists on all three central paths, `mergable` compares all state, and `EDITLEVEL == 7`.
2. A compile-time fixture asserts the quality enum range, mask type widths, no undefined property bits, and the exact current object-size delta reported by the authoritative build.
3. `python test/test_step10b5_source.py` and the existing Step 9/10/11/12 source contracts continue to pass, proving that `obranch_*`, artifact, save, and room ownership were not silently changed.
4. `MSBuild.exe sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64` completes with no new errors; unchanged warnings remain separately identified.

### Pure engine/runtime contract

5. Enumerate every recipient row in Section 9 and assert allowed and rejected setters.
6. Assert `enhancement_normalize` clears invalid bits, invalid quality, artifact state, and ineligible classes while leaving native `spe`, branch metadata, quantity, and names intact.
7. For each of Fire/Frost/Shock, exercise resistant and non-resistant hero, monster, and monster-to-monster targets; assert exactly one extra damage roll and no mutation during a speculative `dmgval`/selection call.
8. Exercise Trueflight and Cumbersome through hero melee, hero thrown, launcher+ammo, monster melee, monster throwing, and monster shooting. Assert Trueflight is one +2, not +4, when both source objects have it.
9. Exercise quality on melee, thrown, launcher+ammo, and every armor slot; assert the correct hit/damage/AC channel and no native `spe` mutation.
10. Wear/remove each armor property in every hero armor slot; verify existing `WARNING`, `SEARCHING`, and `STEALTH` extrinsics are added and removed. Wear/remove the same items on monsters and verify the properties remain inert and all enhancement state remains intact; do not add monster property storage or gameplay semantics. Verify monster armor quality separately.

### Object lifecycle contract

11. Create eligible and ineligible objects through `mksobj`, `mkobj`, monster equipment, shop stock, and Lua-level fixtures; assert zero/default state and valid later assignment.
12. Split every enhanced stack and assert both resulting objects retain identical generic state and independent IDs/quantities.
13. Attempt merges for every equal and unequal combination of active mask, knowledge mask, quality, and quality-known flag; assert only exact state merges.
14. Move enhanced objects through hero inventory, monster inventory, floor, nested containers, migrating, buried, and bill-object lists; assert state is unchanged.
15. Transform eligible objects in `poly_obj` and in-place type-change paths; assert the documented clear/normalization rule.

### Naming, artifact, and economy contract

16. Check known/unknown descriptions with all property combinations, both non-standard qualities, branch metadata, native `spe`, BUC, erosion, worn state, and custom names.
17. Name an enhanced eligible object as an ordinary custom name and as each artifact-conversion route; assert only the artifact route clears generic state.
18. Exercise `mk_artifact`, `oname`, Excalibur fountain conversion, random generation, programmatic artifact creation, and un-creation; assert no artifact retains generic state and no ordinary object loses state on failed conversion.
19. Assert exact shop price formula for quality, each positive property, Cumbersome, native `spe`, unknown state, and artifacts. Test unpaid mutation, split, merge rejection, sale, theft, and return-to-shop.

### Persistence contract

20. Save and restore an enhanced hero inventory, nested container, monster inventory, floor object, buried object, migrating object, bill object, and every combination of property/knowledge/quality state.
21. Create and restore a bones file containing enhanced hero and monster equipment; verify ghostly ID remapping and age adjustment do not alter enhancement state.
22. Verify an epoch-6 fixture is rejected by `check_version` and a new epoch-7 save/bones file restores correctly.
23. Run the authoritative production save/recover flow using the existing `test/run_step11_save.py` pattern, then run focused diagnostic assertions in an isolated output directory.

## 20. Explicit design decisions

1. Generic enhancement state is inline in `struct obj`, not in `oextra`.
2. New masks and fields use fixed-width aliases from `include/integer.h`.
3. Generic masks are distinct from `obranch_props`; existing donor material/size/coating behavior remains unchanged.
4. `WEAPON_CLASS`, `is_weptool`, `is_launcher`, `is_ammo`, `is_missile`, and `ARMOR_CLASS` predicates define eligibility.
5. The initial property set is Fire, Frost, Shock, Trueflight, Warning, Searching, Stealth, and Cumbersome.
6. Quality has exactly Standard, Fine, and Exceptional states in Step 13.
7. Quality is not native `spe`, is not averaged, and is not stored in `oextra`.
8. Elemental properties are post-hit effects and must not be added inside speculative damage calculation.
9. Trueflight is capped at one +2 ranged-hit contribution per delivery.
10. Warning, Searching and Stealth map to existing hero property storage and wear/remove flows. They remain inert on monsters without clearing item state; monster armor quality remains active.
11. Stack merges require exact generic state equality, including knowledge state.
12. Artifact conversion strips all generic state; artifact systems remain authoritative for artifact power.
13. Shop prices include active generic state but do not depend on whether the player knows it.
14. The current historical object codec remains in use for the new epoch; no parallel object codec is introduced.
15. `EDITLEVEL` increments from 6 to 7; no old save/bones migration is implemented.
16. Step 13 supplies the engine and fixtures; Step 14 supplies depth-scaled generation.

## 21. Deferred decisions

- Step 14’s depth bands, probability distributions, guaranteed versus incidental acquisition, and monster/shop generation frequency.
- Forging inputs, costs, failure modes, and whether forging can upgrade quality or only add properties.
- Gemstone identity, slot capacity, affix combinations, and any new persistent gemstone payload.
- Advanced property combinations and conditional target rules for Step 17.
- Endgame progression, provenance, mythic state, and any artifact interaction beyond the Step 13 exclusion.
- Whether a future release should replace the native historical struct codec with a fully field-level portable object codec.
- Player-facing commands and wish syntax for directly selecting generic properties.

None of these is required to implement and verify the Step 13 engine defined in this document.

## 22. Genuine unresolved blockers

None; design contract is complete.

## 23. Implementation-readiness verdict

The current repository provides the required object lifecycle, combat, worn-property, artifact, shop, and save/bones integration surfaces. The existing `obranch_*` system is sufficiently understood and can remain separate. The proposed fields, property catalogue, quality behavior, recipient matrix, stack rules, artifact policy, price policy, persistence epoch, implementation map, and verification contract are concrete enough for production work.

Implementation now accompanies this design in the working tree. The implementation record below supersedes the original audit-only readiness statement.


## 24. Implementation and validation record

The implementation starts from `phase1/equipment-enhancement` at
`f27b7f20fd90da8dcb7aa0e90ee85f1af1123444`. Initially only `.codegraph/` and
this document were untracked. Both are preserved. No commit, tag, push,
reset, stash, or clean was performed.

### Implemented contract and exception

`include/enhance.h` and `src/enhance.c` provide eligibility, assignment,
normalization, identification, pure hit/damage/quality queries, confirmed-hit
elemental effects, hero worn effects, bounded name prefixes, pricing and
artifact/type-conversion clearing. Acquisition remains absent.

The four inline fields use the approved widths. Their physical order is
quality, flags, active mask, knowledge mask immediately after `obranch_*`:
this uses existing alignment padding without moving unrelated fields.
Authoritative MSVC x64 measurement is **104 → 112 bytes** for `struct obj`.
The two masks are four bytes each; quality and flags are one byte each.
`EDITLEVEL` is **7**. The historical native object codec is retained.

The user-approved repository-evidence-driven exception in Section 5 is the
only gameplay departure from the original target-neutral proposal:
**Warning, Searching and Stealth are hero-only worn extrinsics.** Native
`update_mon_extrinsics` has no matching Warning/Searching storage and treats
Stealth as inert. No monster structure, AI, perception or property semantics
were added. Monsters retain all four enhancement fields through wear/remove,
carry, combat and persistence. Their armor quality and weapon enhancements
remain active. Section 19 item 10 explicitly verifies this exception.

Known-state observation is restricted to the hero's attacks or effective
hero worn-property transitions; a monster using the shared projectile global
does not identify its enhancements. Blocked Stealth does not reveal itself.
Artifacts are sanitized through `artifact_exists`, `mk_artifact`, and `oname`.
Unsuccessful artifact naming and ordinary custom names retain generic state.
Direct identity conversions use `enhancement_change_type`; `poly_obj` creates
a fresh object with default state. Existing branch material, size and coating
rules remain separate.

### Reproducible focused verification

- `python test/test_step13_source.py`: field/mask/epoch, exact merge,
  pure-query, integration, artifact exclusion and no-acquisition guards.
- `python test/run_step13.py`: isolated MSVC build and native assertions.
  Covers every object-table recipient and quality/property eligibility;
  normalization; native damage RNG; actual hero melee/throw/shot and monster
  melee/projectile hits against hero and monster targets; each element's
  resistance and one-d4 behavior; duplicate-source union; seven armor slots;
  inert monster wear/remove and preserved state; blocked hero Stealth;
  split/merge, partial/full/minimal names, worst-case name bounds;
  native polymorph, artifact conversions, valuation with native enchantment,
  multi-shop billing increases/decreases, split, theft debt and returns.
- Native `saveobjchn`/`restobjchn` round-trip **61,440** legal weapon/armor
  active-mask × all knowledge-mask × quality × flag combinations, plus nested
  containers. Native `savelev`/`getlev` and accepted `mklev`/`getbones` preserve
  enhanced floor, hero-remains, buried and monster inventory objects.
- Epoch rejection uses a controlled native version header changed from 7 to
  6, passed to the real `check_version`. This is a version-gate fixture, not
  a claim to have restored or migrated an independently archived epoch-6 game.
- `C:/Python311/python.exe test/run_step13_save.py _qa/step13-diagnostic/bin
  _qa/step13-game-save-final2`: actual tty save/restore and production
  `recover.exe` checkpoint recovery preserve all seven ownership chains,
  including nested containers and bill objects.
- `C:/Python311/python.exe test/run_step11_save.py binary/Release/x64
  _qa/step13-production-save3 4`: production executable save/restore,
  leave/revisit, custom room identity and real checkpoint recovery pass.
  The shared tty harness uses uppercase yes/no responses because the active
  Windows keyboard layout remaps lowercase input; native prompts are unchanged.

Diagnostic hooks are conditional on `STEP13_TEST` and compiled into isolated
outputs only. Production exposes no test command or acquisition mechanism.
The build registers `enhance.c` in console/GUI Visual Studio projects, native
Windows nmake and Unix source/object lists.

### Existing regressions and authoritative package

Step 9A/9B pinned donor tables and packaged resources, Step 9C mirage source,
all Step 10 source gates and Step 11 resource/package gates pass. The existing
Step 11/12 focused native suite also passes selector, all eight room features
(including Library), recurrence, coexistence, level/bones codecs, clean failure
and partial failure checks; no probability corpus was run. Historical
whole-file tests now freeze the verified cumulative Step 12 checkpoint where
later accepted work had invalidated their old baseline. Epoch assertions
advance only from 6 to 7. `step13_historical_changes.json` records exact,
frozen enhancement hunks for historical source projection; it is not generated
by tests. All unlisted source bytes remain protected. Step 13's new behavior
has its own source and linked runtime tests. The older defense extraction
ends at the new enhancement boundary while retaining its native physical
mitigation assertions.

Native donor regression results:

| Gate | Result |
|---|---|
| Weapon dice, size, material, phase and native RNG | 377,568 comparisons pass |
| Existing coatings and hero/monster projectile dispatch | 800 cases pass |
| Physical defense and projectile dispatch | 20,200 cases pass |
| Deep-one equipment | 180 armor/weapon combinations and six weapon choices pass |
| Fey equipment | 28,672 donor loadouts and RNG traces pass |
| Sized handedness | 4,032 comparisons pass |
| Clothing sizes | 864,864 comparisons pass |
| Shop services | 1,689,120 price comparisons and payment/credit checks pass |

The authoritative gate is `MSBuild.exe sys/windows/vs/NetHack.sln
/p:Configuration=Release /p:Platform=x64 /m`. Package validation checks the
ZIP's console executable, GUI executable and DLB byte-for-byte against the
Release output and all 186 Lua resources against source. Logs and fixtures
are isolated under `_qa/step13-*`; the package is
`vspackage/nethack-500-win-x64.zip`. Existing compiler warnings are the unused
`trop` parameter, potentially uninitialized `mkmap.c` coordinates and GUI's
unused `fmt` parameter; no new enhancement warnings are accepted.

No Step 14 generation/distribution logic or probability corpus is introduced.
The working tree is left uncommitted for manual gameplay and review.

### Changed-file inventory

- `doc/step13.md`
- `include/enhance.h`
- `include/hack.h`
- `include/obj.h`
- `include/patchlevel.h`
- `include/you.h`
- `src/allmain.c`
- `src/artifact.c`
- `src/do.c`
- `src/do_name.c`
- `src/do_wear.c`
- `src/dothrow.c`
- `src/enhance.c`
- `src/invent.c`
- `src/mhitm.c`
- `src/mhitu.c`
- `src/mthrowu.c`
- `src/objnam.c`
- `src/polyself.c`
- `src/read.c`
- `src/restore.c`
- `src/save.c`
- `src/shk.c`
- `src/trap.c`
- `src/uhitm.c`
- `src/weapon.c`
- `src/wield.c`
- `src/worn.c`
- `src/zap.c`
- `sys/unix/Makefile.src`
- `sys/windows/Makefile.nmake`
- `sys/windows/vs/NetHack/NetHack.vcxproj`
- `sys/windows/vs/NetHackW/NetHackW.vcxproj`
- `sys/windows/windmain.c`
- `test/run_step13.py`
- `test/run_step13_save.py`
- `test/run_step8a_runtime.py`
- `test/run_step9c_defense.py`
- `test/step13_historical_changes.json`
- `test/step13_source_projection.py`
- `test/step9c_source_projection.py`
- `test/test_step10b2_2_source.py`
- `test/test_step10b3_1_source.py`
- `test/test_step10b3_4_source.py`
- `test/test_step10b4_source.py`
- `test/test_step10b5_source.py`
- `test/test_step10b_source.py`
- `test/test_step10qa2_source.py`
- `test/test_step11_resources.py`
- `test/test_step13_runtime.c`
- `test/test_step13_source.py`
- `test/test_step7_source.py`
- `test/test_step8a_source.py`
- `test/test_step9a_source.py`
- `test/test_step9b_source.py`
