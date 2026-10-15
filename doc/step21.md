My-NetHack modification notice, added 2026-10-15.
Local project integration/modifications; original authorship is retained.
Recorded project change dates (Git author dates): 2026-10-10.
See doc/license-compliance.md and doc/my-nethack-modifications.json.
Existing copyright, attribution, license and warranty notices still apply.

# Step 21: Artifact Expansion

Authority: [final implementation scope](Step21_Artifact_Expansion_FINAL_Implementation_Scope.md).
Baseline: `7813481f7`, completed Step 20. All 23 approved designs are registered
once: the existing Silver Key and 22 additions. Gameplay and native validation
are implemented. Final acceptance status is recorded below, separately from
implementation or build success.

## Behavior, commands and source traceability

Carried means directly in inventory, not inside a container. Equipment powers
require the actual slot; the ridden Saddle is the explicit exception. Ordinary
commands retain native prerequisites and action costs. Timed invocation uses
the existing artifact age/cooldown conventions, including retry backoff.
Cancellation refunds the deadline where the native handler does so.

| Artifact | Base and approved behavior | Native commands / consumption | Main implementation |
|---|---|---|---|
| Silver Key | Universal key; carried energy regeneration, teleport control, polymorph control | Ordinary key; invoke existing restricted R'lyeh portal, no cooldown | `artifact.c`, `lock.c` |
| Brass Lantern of Zork | Brass lantern; permanent radius 4; observable local secret doors, traps and invisible monsters | Apply retains permanent light; no fuel depletion or invocation | `light.c`, `vision.c`, `apply.c` |
| Book of Infinite Spells | Blank spellbook with saved supported spell | Read learns without depletion/page change; invoke deliberate Turn Page or no-Pw, unmemorized Cast; shared cooldown; no marker rewrite/eating | `spell.c`, `read.c`, `write.c`, `artifact.c` |
| Lodestone of Gravity | Heavy flint; carried 100% physical missile deflection before poison/contact | Native inventory/throw; no invoke or cursed-loadstone movement restriction | `mthrowu.c`, `artifact.c` |
| Hellrider's Saddle | Saddle; actual rider and steed each receive Reflection and MR | Native saddle/ride; invoke carried or ridden saddle (`-`) toggles hero invisibility; off starts cooldown | `steed.c`, `worn.c`, `artifact.c` |
| Mirrored Mask | Face mask; worn Reflection, Half Spell Damage and 50% eligible hostile targeted-spell redirection | Equip/remove; one special decision per attack, including bouncing rays; no invocation | `mcastu.c`, `zap.c`, `artifact.c` |
| Tie-Dye Shirt of Shambhala | T-shirt; worn BUC capacity 150% / 125% / 75% | Read donor cures/cursed effects; opaque covering blocks, crystal plate does not; blind allowed; invoke enlightenment | `hack.c`, `read.c`, `apply.c` |
| Beastmaster's Duster | Jacket; active powers do not require wearing | Invoke current-level pet detection/tameness or one selected ordinary supply, same BUC | `artifact.c` |
| Cloak of the Unheld One | Oilskin cloak; carried sleep resistance, worn Free Action/MR/MC3 | Native oilskin/equipment; invoke native hero teleport carried or worn | `artifact.c`, `worn.c` |
| Iron Spoon of Liberation | Pickaxe; carried searching, Free Action and native luckstone BUC semantics | Native digging and dust writing; invoke 51-150 turns phasing, then downtime; active phasing does not stack | `dig.c`, `engrave.c`, `timeout.c`, `teleport.c` |
| Candle of Eternal Flame | Magic candle; radius 3, carried warning/cold resistance | Apply retains permanent light without fuel/Candelabrum consumption; invoke one tame Fire Elemental | `light.c`, `apply.c`, `artifact.c` |
| Ring of P'hul | Free-action ring; carried sickness resistance, worn Free Action/MR | Native ring equipment; no invocation | `artifact.c`, `objnam.c` |
| Nenya | Water-walking ring; worn detection, waterproofing, Water Walking, energy regeneration | Native ring equipment; no Magical Breathing/Half Spell Damage | `artifact.c`, `trap.c`, `objects.h` |
| Vilya | Regeneration ring; worn shock resistance, warning, stealth, breathing, HP regeneration | Native ring equipment; no invocation | `artifact.c`, `allmain.c` |
| Ehecailacocozcatl | Breathing amulet; worn breathing and Reflection | Invoke one tame Air Elemental; no Energy Regeneration | `artifact.c` |
| Talisman of Buer | Life-saving amulet; worn regeneration, Fixed Abilities, Life Saving | Invoke half missing HP healing and native sickness/slime/blindness cures; successful Life Saving consumes artifact | `artifact.c`, `end.c` |
| Holographic Void Lily | Credit card; carried Reflection, Half Spell Damage, original fast energy regeneration | Native unlocking; invoke original SpliceHack `create_critters(rnd(10), NULL, TRUE)`, no card replacement | `artifact.c`, `allmain.c`, `lock.c` |
| Mask of Tlaloc | Face mask; carried cold/shock/MR, worn-only waterproofing | Native equipment; no Half Spell Damage or polymorph | `artifact.c`, `trap.c` |
| Hand-Mirror of Cthylla | Mirror; carried searching, teleport control, MR | Ready apply/invoke 15d15 reflection including self, shared cooldown; ordinary apply fallback while cooling | `apply.c`, `artifact.c` |
| Earth Crystal | Crystal ball; carried Half Physical Damage | Native charged scrying, canceled artifact preserved; invoke one level-scaled native quake, no mass HP reduction | `detect.c`, `music.c`, `artifact.c` |
| Glitterstone | Amber; radius 2, carried energy regeneration | Invoke native recharge BUC +1/0/-1 for every role/race; target recharge counts/explosions retained | `artifact.c`, `read.c`, `light.c` |
| Storm Whistle | Magic whistle; carried fire resistance/teleport control | Apply native pet recall; invoke one tame Water Elemental; no Warning | `apply.c`, `artifact.c` |
| Lyre of Orpheus | Magic harp; carried eligible remote pets follow committed transitions | Native charged harp apply; passive following has no cooldown or MR | `dog.c`, `do.c`, `music.c` |

All registrations are in `include/artilist.h`; identified descriptions are in
the native inspection path. No unapproved donor role, quest, enchantment
exception, item family or acquisition probability system was imported.

## Architecture and local adaptations

The only new ordinary base is `RIN_WATER_WALKING`. Existing mask, jacket,
pickaxe and flint bases are reused. Heavy flint avoids loadstone curse rules.
The Spoon retains native pickaxe dust writing, not a new permanent engraving
power. No unrelated ordinary generation weights were changed.

`artifact_extra_property` is a pure grant predicate. Native pickup grants
before inventory linking; removal excludes the departing source. Extra and
legacy property masks preserve independent equipment/intrinsics. Worn
waterproofing protects direct inventory and nested contents through normal
water-damage recursion, not floor objects.

Book page and Saddle toggle use saved `obj.oartifact_state`. Native object
serialization preserves identity, invocation age and that state.
`EDITLEVEL` is **17**, deliberately invalidating older development saves.
Historical save migration and bones compatibility are outside this scope.
Current catalogue: 531 monsters, 569 objects, 71 artifacts, sentinel 72,
13071 glyphs. Silver Key/Necronomicon IDs remain 46/45.

Permanent lights use native object sources, radii 4/3/2, no fuel timer and no
reuse of invocation age. Containers occlude light; destruction releases it.
Lantern revelation requires source LOS and current hero observability and
overrides only invisibility. Null camera sources and saved-identifier light
records are not dereferenced as objects.

Monster-targeted spells share the native resolver. Reflected death touch is
fatal damage, bypassing ordinary DR while retaining Life Saving. Aimed rays
carry explicit hero-target context and check the Mask once before generic
Reflection. Caster defenses remain native; area effects are excluded.
Monster punishment uses native slowing because monsters have no hero
ball-and-chain state. Mirror gaze is nonphysical, routed through native
mitigation rather than direct HP subtraction.

Phasing expiry uses nearest legal native teleport placement, retaining region,
trap and terrain restrictions. If no legal exit exists, timeout extends one
turn and retries. Independent phasing and amorphous closed-door legality
survive. Earth Crystal calls one ordinary native earthquake at
`(u.ulevel - 1) / 3 + 1`. Shared quake handling now honors nondiggable dead trees
without confusing door flags with wall flags.

Lyre changes only the proximity condition in `keepdogs`. Native sleep and
eligibility restrictions remain. Existing transition code commits before
migration; no separate pet relocation or duplication system was introduced.

All additions are singletons. Converting an owned stack retains the ordinary
remainder. Artifact merge/split, ordinary eating, Book rewriting and Forge
target/ingredient paths reject these artifacts. Buer Life Saving is the
approved consumptive exception. Earth Crystal still uses scrying charges and
mishaps but cannot implode just because cancellation depleted its base.

## Cooldown and identification inventory

| Mechanism | Authoritative state | Inspection |
|---|---|---|
| Native and new timed artifact invocation | `obj.age`, absolute `svm.moves` deadline | Known/seen object plus discovered artifact |
| Mirror apply and invoke | Same `obj.age` | One shared deadline |
| Book Turn Page and Cast | Same `obj.age`; page separate | Reading changes neither page nor invoke deadline |
| Saddle invisibility | `oartifact_state`; `obj.age` on deactivation | Accessibility-aware toggle, independent mounted defense |
| Spoon phasing | Native `HPasses_walls`; `obj.age` includes active period | 51-150 active turns precede downtime |
| Stoning enhancement | `o_stoning_remaining` | Identified affix, existing active-level countdown |
| Purifying enhancement | `o_purification_remaining` | Identified affix, existing active-hero countdown |
| Silver Key / Necronomicon | No invocation timer | No fabricated cooldown |
| Lantern, Lodestone, Mirrored Mask, P'hul, Nenya, Vilya, Tlaloc, Lyre | No timed invocation | No cooldown line |
| Fuel, corpse age, eggs and lifecycle timers | Existing native fields/timers | Not use cooldowns |

Inspection is read-only. Positive deadlines show remaining turns; zero/expired
deadlines show `Ready`, never negative. Known bases do not reveal undiscovered
artifact powers. Independent enhancement cooldowns remain separate.

## Donor provenance

Directly inspected immutable snapshots:

| Key | Revision | References |
|---|---|---|
| D | [dNAO d512426](https://github.com/Chris-plus-alphanumericgibberish/dNAO/tree/d512426550448e944f381469e498423b656f2e74) | `include/artilist.h`; `src/artifact.c`, `apply.c`, `read.c`, `spell.c`, `detect.c`, `dog.c`, `attrib.c`, `mcastu.c`, `mthrowu.c` |
| S | [original SpliceHack b3a1427](https://github.com/NullCGT/SpliceHack/tree/b3a1427a43fd6ecc6088129e72ac594a1d3e531f) | Artifact definition, SUMMONING, native fast energy regeneration |
| N | [notdnethack 7a18812](https://github.com/demogorgon22/notdnethack/tree/7a18812f35d77109923fe6022576073c16ebcc67) | Buer definition and HEALING |
| L | SLASH'EM `fbd743a6081f4a447b9fc2dc545f20c3bc603330` | Pinned Step 19 artifact definitions |
| H | Hack'EM `cebe2f36e1908e82665c01a30a9eef2f108d07d9` | Pinned Step 19 Ring of P'hul definition |
| B | Local completed Step 20 `7813481f7` | Existing Silver Key and native light, movement, spell, damage and item handlers |

| Artifact | Source reference | Approved adaptation / parity boundary |
|---|---|---|
| Silver Key | B artifact and portal handlers | Preserve existing Step 10 design/acquisition |
| Zork Lantern | Locked local design; B light/vision/detection | No exact named donor established; do not claim donor parity |
| Book | D definition, INFINITESPELLS, learning/casting | Deliberate page changes only; native supported pool and restrictions except Pw/memorization |
| Lodestone | Locked local design; B missile handling | No exact named donor established; approved heavy-flint/pre-contact design |
| Saddle | D definition/property/invocation handlers | Actor-specific native defenses and carried/ridden toggle |
| Mirrored Mask | D definition and `mcastu.c` Mask branch | Native caster defense, one decision, no donor combat subsystem import |
| Shirt | D `read.c:arti_mandala`, `apply.c:use_unicorn_horn` | Six native conditions below, no attribute restoration |
| Duster | D PETMASTER | Carried instead of worn requirement; exactly one ordinary supply |
| Cloak | D definition | Native hero teleport and listed carried/worn grants only |
| Spoon | D definition, PHASING | Pickaxe/dust-writing adaptation; no combat/stealth bonus; native luck |
| Candle | L definition | Fixed radius 3 and one native tame Fire Elemental |
| P'hul | H definition | Native free-action base, approved sickness/MR only |
| Nenya | D definition | Minimal water-walking base, omit unapproved breathing/Half Spell Damage |
| Vilya | D definition | Native regeneration base; only approved worn grants |
| Ehecailacocozcatl | D definition | Remove Energy Regeneration and unapproved extras |
| Buer | N definition, HEALING | Half missing HP, polymorph-aware; native consumptive Life Saving |
| Void Lily | S definition, SUMMONING, regeneration | Original source priority; no card-drop replacement or power reduction |
| Tlaloc | D definition | Omit Half Spell Damage/polymorph; worn waterproofing |
| Hand-Mirror | D `apply.c` mirror/self/gaze | 15d15, shared commands/deadline, native damage mitigation |
| Earth Crystal | D definition; B native earthquake | Locked override: one quake, no mass HP effect |
| Glitterstone | D definition/CHARGE_OBJ; B recharge | Locked override: native BUC strength for all races/roles |
| Storm Whistle | L definition; B magic whistle | Preserve recall, one tame elemental, remove Warning |
| Lyre | D definition/`dog.c` | Native six-path following; remove MR; preserve harp |

The ordinary earthquake was also inspected at project revision
`3529330468d3b638a5397d51840e92a184766b17`, as specified by the scope.
Local donor snapshots are under ignored `_qa/step21/donors` and
`_qa/step19-donors`; revision pins above are the provenance, not a claim of
complete donor parity.

### Shirt condition reconciliation

| Condition | Donor result / local mapping |
|---|---|
| Sickness | Native `make_sick`, all local sickness types |
| Blindness | Timed component above face cream; preserve independent blindness |
| Hallucination, vomiting, confusion, stun | Native timed condition setters |
| Deafness, slime, petrification, attributes | Explicitly not cured |
| Blessed/uncursed | Donor random subset `rn2(d(2,4/2))` |
| Cursed | Exactly sickness/blindness/confusion/stun/hallucination, donor duration semantics |
| Covered/blind | Opaque body armor blocks, crystal plate is transparent, blindness alone allowed |

Buer uses donor `(missing_HP + 1) / 2`, not half maximum HP. For example,
80/100 becomes 90/100; polymorph HP is selected when applicable.

## Acquisition matrix

All additions have `NON_PM` role/race, no `SPFX_NOGEN`, and existing native
generation/gift/conversion eligibility. No guaranteed placement or new rarity
system exists. Native history and ordinary wish restrictions remain.
Wizard wishes below prove naming, not unrestricted player wishing.
`SPFX_RESTR` retains native naming restrictions; Lily also retains intelligence.

| Artifact | Existing? | Base | Alignment | Deterministic native evidence |
|---|---|---|---|---|
| Silver Key | Yes | Universal key | Neutral | Preserved R'lyeh path and historical Step 10 identity/portal gate |
| Lantern | No | Brass lantern | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Book | No | Blank spellbook | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Lodestone | No | Flint | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Saddle | No | Saddle | Chaotic | PASS conversion, canonical wizard wish, uniqueness |
| Mirrored Mask | No | Mask | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Shirt | No | T-shirt | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Duster | No | Jacket | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Cloak | No | Oilskin cloak | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Spoon | No | Pickaxe | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Candle | No | Magic candle | Lawful | PASS conversion, canonical wizard wish, uniqueness |
| P'hul | No | Free-action ring | Chaotic | PASS conversion, canonical wizard wish, uniqueness |
| Nenya | No | Water-walking ring | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Vilya | No | Regeneration ring | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Ehecailacocozcatl | No | Breathing amulet | Lawful | PASS conversion, canonical wizard wish, uniqueness |
| Buer | No | Life-saving amulet | Lawful | PASS conversion, canonical wizard wish, uniqueness |
| Void Lily | No | Credit card | Chaotic | PASS conversion, canonical wizard wish, uniqueness |
| Tlaloc | No | Mask | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Hand-Mirror | No | Mirror | Chaotic | PASS conversion, canonical wizard wish, uniqueness |
| Earth Crystal | No | Crystal ball | Chaotic | PASS conversion, canonical wizard wish, uniqueness |
| Glitterstone | No | Amber | Neutral | PASS conversion, canonical wizard wish, uniqueness |
| Storm Whistle | No | Magic whistle | Lawful | PASS conversion, canonical wizard wish, uniqueness |
| Lyre | No | Magic harp | Neutral | PASS conversion, canonical wizard wish, uniqueness |

The native acquisition fixture excludes competing identities using existing
history, then executes `mk_artifact` on the real base. A second conversion
cannot reproduce the unique identity, even after destruction. This does not
change production rarity or allow a forbidden route. Reused bases remain
ordinary objects; Nenya is the minimal base exception. P'hul canonical naming
now resolves the exact artifact before generic `ring of ...` parsing.

## Verification

Main native tests: `test/test_step21_inspect.c`, invoked by the existing Step 15
runtime suite. Forge input checks use actual private gates in
`test_step15b.c`. Test-only wrappers call real regeneration/monster-spell
handlers. Ray counters only observe native decisions and are excluded from
production.

### Common artifact categories

Scope categories 1, 2, 3, 4, 10 and 14 apply to every row of the acquisition
and behavior tables. Registration, native object construction, all-23 identity,
all-22 deterministic creation/uniqueness, exact equipment slots, production
inspection and full save/reload pass. Silver Key uses its historical native
gate plus the all-23 production save/inspection and combined-source tests.
This shared evidence avoids duplicating the same fixture 23 times.

### Focused categories and review-resolution evidence

The following lists runtime checks, not source inspection substitutes.
The behavior table maps every artifact to its implementation and commands.
The numbered categories are those in scope section 8.1.

| Artifact / category coverage | Executed native evidence |
|---|---|
| Silver Key (5,9,11,12,13) | Historical portal domain restrictions and IDs; combined nonstacking Pw sources, both removal orders |
| Lantern (5,8,11,13,15) | Fixed light radius, no fuel, native apply/ordinary control, secret doors/traps, local invisible monster visibility, walls, blindness, container/camera/removal controls |
| Book (8,9,13,15) | 100 deliberate pages in supported pool, 100 unlearned no-Pw casts, 100 studies past normal depletion, no automatic flips, stun rejection, rewrite/eating rejection |
| Lodestone (5,11,13,15) | 100 poisoned darts, arrow/bolt/dart/dagger/boulder direct-hit and monster-throw paths, venom negative; hand-catching disabled to isolate deflection |
| Saddle (5,7,8,9,11,12,13,15) | Real mount, mounted `-` invoke, hero `castmu` and steed `zhitm` MR targets, polymorph, dismount, saddle removal, steed death, independent invisibility |
| Mirrored Mask (6,8,11,12,13,15) | 400 direct native spells; seeded rays exercise both decisions, repeated contacts roll once, caster Reflection/fire resistance/death/Life Saving; incidental/wand/breath negatives; Lily coexistence |
| Shirt (6,8,9,11,13,15) | Stable BUC capacity and rounding, covering/blind conditions, six cures and excluded attributes/conditions, 100 cursed readings covering five outcomes |
| Duster (9,13,15) | Six supply types across all BUC, exactly one ordinary item, current-level remote detection, visible +1 capped20, nonvisible unchanged |
| Cloak (5,6,9,11,12,13) | Carry versus exact slot, MR resolver/MC3, actual hero teleport carried and worn, native no-teleport rejection |
| Spoon (5,8,9,11,12,13,15) | 1000 invocations reach 51 and150; active rejection, post-active cooldown, luck BUC; actual ordinary/artifact digging and dust writing, protected rock; expiry region/hazard/no-exit retry |
| Candle (5,8,9,11,12,13,15) | Radius3/no fuel, native apply and ordinary control, one initialized tame Fire Elemental |
| P'hul (5,6,8,11,12,13) | Carry/slot source rules and removal; exact canonical native wish |
| Nenya (6,8,11,12,13) | Slot, direct/nested/floor native water damage, no unapproved breathing, combined nonstacking native Pw regeneration |
| Vilya (6,8,11,12,13) | Exact worn grants and removal, independent intrinsic source preservation |
| Ehecailacocozcatl (6,8,9,11,12,13) | Worn breathing/actual Reflection resolver; one initialized tame Air Elemental; no energy regeneration |
| Buer (6,8,9,11,12,13,15) | Exact healing/cures, polymorph healing and rehumanization, actual `done(DIED)` consumes artifact, source/history checks |
| Lily (5,8,9,11,12,13,15) | Native credit-card unlock during cooldown, summon disposition/cooldown, 100-turn fast native Pw regeneration, mitigation and both Mask source orders |
| Tlaloc (5,6,8,11,12,13) | Actual face equip/remove, worn native water damage, both MR source orders, no Half Spell Damage |
| Hand-Mirror (5,8,9,11,12,13,15) | Apply/invoke shared age, ordinary fallback, self and monster damage, sleep/blind/invisible/wall/cancel negatives |
| Earth Crystal (5,8,9,11,12,13,15) | Native physical+DR ordering and magical negative; 40 quakes preserve nondiggable tree/high altar/stairs/portal/vibrating square; ordinary tree positive; native charged/canceled scrying with ordinary control |
| Glitterstone (5,9,11,12,13,15) | Radius2/native light, BUC charging, same seeded outcomes for role/race combinations, native target recharge explosion and cancellation |
| Storm Whistle (5,8,9,11,12,13,15) | Native apply/ordinary whistle, one tame Water Elemental, fire/teleport properties and exclusions |
| Lyre (5,8,11,13,15) | Actual stairs/ladder/hole/trapdoor/level teleport/paired portal; six pets exactly once, identity/state/inventory and reload; occupied arrival, sleep/hostile/peaceful exclusions; canceled/failed/restricted transition controls, ordinary harp |

For category 7, all artifacts except Saddle are N/A because they have no
mounted power. Categories 5/6 are N/A where the behavior table grants no
carried/worn power. Category 9 is N/A for artifacts with no invocation.
Category 12 is N/A for non-property mechanics such as Book pages, light radius,
Duster supplies and Lyre migration; native state/persistence still applies.
Normal equipment operation is included under category8 where a base has no
separate apply/read ability. No general N/A exempts an applicable failure case.

All 22 additions also pass direct-carried versus contained source tests,
independent intrinsic preservation and native Forge input/target rejection.
Each has an ordinary base-object control, including native ordinary slot
properties. Real `Ring_off` and `Amulet_off` tests verify immediate escape
from water after Water Walking or breathing is lost. Native oilskin slip
checks compare the Cloak with its ordinary base. Vilya restores HP during
encumbered movement and stops after removal. Lodestone also deflects real
arrow, dart and rolling-boulder traps without duplicating the boulder.
Storm Whistle recalls the same distant pet through native apply. Lily's
native creation request reaches both bounds, 1 and 10, with `neverask` true;
native monster groups can add companions beyond that requested count.
An occupied-level creation failure still spends its cooldown.
Native object reload preserves their IDs, single quantities, ages, Book page,
Saddle toggle and exactly one light source per permanent light. Production
full save/relaunch covers all23, including permanent identification.

Explicit combined-source tests include Mask+Lily both removal orders,
Key+Nenya+Glitterstone+Lily nonstacking native regeneration both orders, and
Earth Crystal+DR exact physical versus magical damage. Cooldown tests cover
all added timed/untimed contracts and unknown/known/expired/read-only states,
plus existing artifact, fuel exclusions and enhancement counters.

### Complete artifact matrix

Column numbers are the 15 categories in scope section 8.1, in order:
identity, base, uniqueness, acquisition, carried, worn, mounted, normal use,
invoke/cooldown, persistence, removal, independent sources, failures,
identification/inspect, ordinary-base regression. PASS means executed native
evidence in the common and focused coverage above, not registration alone.

| Artifact | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Silver Key | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Lantern | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | N/A | PASS | PASS | N/A | PASS | PASS | PASS |
| Book | PASS | PASS | PASS | PASS | N/A | N/A | N/A | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS |
| Lodestone | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | N/A | PASS | PASS | N/A | PASS | PASS | PASS |
| Saddle | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Mirrored Mask | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS |
| Shirt | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | N/A | PASS | PASS | PASS |
| Duster | PASS | PASS | PASS | PASS | N/A | N/A | N/A | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS |
| Cloak | PASS | PASS | PASS | PASS | PASS | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Spoon | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Candle | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| P'hul | PASS | PASS | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS |
| Nenya | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS |
| Vilya | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS |
| Ehecailacocozcatl | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Buer | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Lily | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Tlaloc | PASS | PASS | PASS | PASS | PASS | PASS | N/A | PASS | N/A | PASS | PASS | PASS | PASS | PASS | PASS |
| Hand-Mirror | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Earth Crystal | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Glitterstone | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Storm Whistle | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | PASS | PASS | PASS | PASS | PASS | PASS | PASS |
| Lyre | PASS | PASS | PASS | PASS | PASS | N/A | N/A | PASS | N/A | PASS | PASS | N/A | PASS | PASS | PASS |

N/A in 5 or 6 means no approved passive power in that condition, as specified
in the behavior table; negative native tests still verify absent grants.
Only Saddle has mounted powers (7). No-invocation artifacts have N/A in 9.
Book and Duster have no ongoing conditional property to remove (11).
Lantern light/revelation, Book pages, Lodestone deflection, Shirt capacity,
Duster actions and Lyre migration have no independent source of the same artifact-specific property
(12). Their own lifecycle and ordinary-base controls still pass.

Lyre progression negatives include real pre-Invocation Sanctum teleport
clamping, post-Invocation permission, declined Valley entry, quest/endgame
restrictions, Castle fall-through restrictions and failed/canceled travel.
Dead tame pets are excluded alongside sleeping, hostile and peaceful pets.

### Reproduction and run evidence

From repository root, with Python and Visual Studio Build Tools:

```text
python test/run_step21_native.py --label final
python test/run_step21_source.py
python test/test_step20_tiles.py --step21
python test/run_step9c_elder_mm.py _qa/step21/elder-mm
python test/run_step10b3_3.py _qa/step21/step10b3_3-current --step21
python test/run_step21_save.py binary/Release/x64 _qa/step21/full-save
```

The native runner copies packaged resources into its isolated runtime.
The production driver uses real WinPTY gameplay and the existing Step8A
driver's pywinpty/pyte dependencies. Optional `--state-only` skips the initial
23-item UI tour but retains all-23 identity and full saved-state checks.

Historical Step10 catalogue assertions remain in the default branch;
`--step21` explicitly selects current counts without removing the original
ID, operation or portal tests. The elder spell harness extracts the shared
native resolver while preserving its existing behavior assertions.

Evidence under ignored `_qa/step21`:

- `final-runtime.log`: PASS latest broad native suite, including
  prior Step15-20 fixtures and the focused assertions above.
- `full-save-08.log`: PASS production all23 identified UI contracts, Book
  page/cooldown, Saddle toggle, lights and full save/relaunch.
- `full-save-09.log`: PASS production all23 identities and full saved state.
- `full-save-package.log`: PASS final packaged production all23 identities,
  Book page/cooldown, Saddle toggle, lights and full native save/reload.
- `elder-mm.log`: PASS historical native elder/monster spells.
- `step10b3_3-current.log`: PASS current-catalogue Silver Key/Necronomicon.
- `release-package-build.log`: PASS final authoritative Release x64 console,
  GUI, recovery tool, tile map and bitmap utility builds.
- `resources.log`: PASS seven current-catalogue resource tests, including
  the new water-walking ring's reused pixels, shifted indices, bounds and
  embedded GUI bitmap. The default historical counts remain available.
- `historical-source.log`: PASS exact Step 21 deltas for 39 files and 494
  rejected mutations, followed by unchanged Step 20 validation fixes,
  Step 20/19/18B/17 and Step 13 through 16C historical source/mutation gates.
  The frozen manifest is anchored to `7813481f7`; only exact reviewed hunks
  are reversed for older source contracts. Current native tests run against
  the actual Step 21 implementation, never against projected source.

Preserved RED/GREEN cases include canonical P'hul naming, expiry on the
hero's own rock square, nondiggable dead-tree quakes, reflected fatal death
versus DR and canceled Earth Crystal consumption. Earlier failures are
retained as evidence. Harness-only issues (catalogue snapshots, native hand
catching, dust text representation, ray constants and animation delay) were
investigated, not silently ignored.

## Closeout status

Step 21 is complete as of 2026-10-10. All 23 approved designs are present once,
all 22 additions have a validated permitted acquisition route, and all
applicable mandatory native checks pass. The matrix has zero FAIL, BLOCKED
or NOT RUN entries. Independent final review found no remaining concrete
gameplay blocker or missing mandatory gameplay fixture.

Implemented behavior is recorded in the first table; executed native evidence
and current resource/build gates are recorded above. The pinned donor table
distinguishes verified donor mechanics from approved local adaptations and
does not claim full variant parity. There are no unresolved implementation
blockers. Historical saves and bones compatibility remain outside the scope.

Final production outputs are `binary/Release/x64/NetHack.exe`,
`NetHackW.exe` and `recover.exe`, with regenerated packaged resources.
The console package passed real production gameplay and full save/relaunch;
the GUI build and embedded tile resources passed their build/resource gates.
The prior Step 20 baseline is `7813481f7`; incremental Step 21 gameplay
commits are `51aa5450d`, `1442e472c`, `046a22463` and `aa55fd807`, followed
by the validation fixes, resource update and this closeout record.
