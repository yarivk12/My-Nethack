# Step9C: Mithardir — final production parent placement

Implementation baseline: `phase0/dod-length` at
`5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`. Production code selects
Mithardir's parent once from the shared persistent DL30–199 scheduler.
Historical user manual validation used temporary DoD110; the detailed
fixed-parent findings below are historical. Step9D remains canceled. The final
compiled closeout matrix passes on x64 and Win32; [step9.md](step9.md) records
the authoritative status.

Donor: `Chris-plus-alphanumericgibberish/dnethack`, requested `master`, resolved
to immutable commit **17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0**. Use this same
revision for both Step9C and Step9D. The clone is outside the project at
`../step9-donor-dnethack`; source reads use `git show` at that pin or its exact
archive export under `../_qa/step9-audit/dnethack-pinned/dnethack-3.4.3`.

The selected repository's latest commit advertises a move; that does not change
the requested donor authority. Do not follow another repository or mix in a
newer revision. Audit the actual pinned source, including its differences from
the prompt's approximate topology, before production edits.

## Pinned topology and resource inventory

`dnethack-3.4.3/dat/dngnch2.def` defines ten Chaos-variant-2 levels.
`dat/chaos2.des` supplies the parent approach and seven branch special maps.
There are no file variants. The three remaining levels use C room generation.
The word "cat" in a donor filename does not by itself identify the generated
Catacombs region.

| Parent/local branch level | Donor resource | Connections and content |
| --- | --- | --- |
| DoD110 | `chalv2` | Cavern/maze approach; ordinary DoD up/down stairs; portal in central water-surrounded structure to Elshava. Fourteen acid blobs, two independent 50% wraithworms, fourteen random objects. |
| 1, Elshava | `ossa1` | Return branch portal at map (15,8); hidden western portal to `mith1`; eight shops; lit moat/shallow water; shortsighted, hardfloor. |
| 2, Wastes 1 | `mith1` | Western portal to Elshava; eastern portal to Wastes 2; stone islands in white dust; three guaranteed ceramic tiles plus independent 75/50/25% tiles. |
| 3, Wastes 2 | `mith2` | Western portal to Wastes 1; eastern portal to Wastes 3; same procedural and population rules. |
| 4, Wastes 3 / Last Spire exterior | `mith3` | Random outer portal back to Wastes 2; exact central soil/wall spire map; down stair (9,9) to `cat1`; six guaranteed tiles and two each at 75/50/25%. |
| 5, Last Spire 1 | `cat1` | Up stair (6,6), central pools, crystal ooze, 31 blessed historic/faceless statues; secret side chamber portal (13,6) to `cat2`. |
| 6, Last Spire 2 | `cat2` | Return portal (1,6) to `cat1`; First Wraithworm at (7,6); Second Key of Chaos at a random coordinate; portal on eastern half to `cat3`. |
| 7, Last Spire 3 | `cat3` | West hidden chamber portal returns directly to `mith3`, **not** to `cat2`; east hidden chamber down stair to generated Catacombs; two equipment piles; one randomly selected unmade slab and probabilistic tiles at shuffled donor coordinates. |
| 8–9, Catacombs 1–2 | C generator | Dense, dark connected rooms, locked doors, pools, niches with mummies/tiles, normal up/down stairs. |
| 10, Catacombs terminus | C generator | Same generator plus central slab chamber and meandering river; no down stair; Aspect of The Silence and final slab/key logic. |

Donor `lastspire_level` is the `mith3` landmark (branch level 4). Thus its
`In_mithardir_catacombs()` predicate includes special levels 5–7 even though
those fixed maps are conventionally described as Last Spire. Runtime monster
pools must follow the predicate; generated-room code only runs at levels 8–10.
The route from level 7 back to level 4 is deliberately a shortcut. Preserve
that directed connection and resolve arrival portals by destination where
NetHack 5.0's usual single-portal assumption is insufficient.

## Dependency audit

Classification numbers: **1** local equivalent; **2** narrow extension;
**3** absent, import; **4** donor-global, exclude. The audit below is the
implementation contract established before production changes. The notes and
validation checkpoints below record its implementation and deliberate deviations.

| Donor element | Donor file(s) | Local equivalent | Required action | Compatibility adaptation |
| --- | --- | --- | --- | --- |
| Ten-level topology, eight `.des` resources including approach | `dat/dngnch2.def`, `dat/chaos2.des` | Persistent scheduler, Lua dungeon/special-level API | 2/3: reserve parent110 before Step6–8 allocation; convert all eight resources; generated floors8–10 | Standalone optional Mithardir, no Chaos-variant selector or global quest dependency. Parent approach retains normal DoD stairs. |
| Cavern initialization / white-dust rock formations | `src/mkmap.c`, `src/mkmaze.c:fixup_special`, `util/lev_main.c` | `mkmap`, mines-style Lua initialization | 1/2: compare cellular passes, iteration counts, region placement and wall cleanup with pinned donor | Use existing generator only where equivalent; import narrowly differing generation steps. No static replacement. |
| Catacomb dense rooms, niches, pools, slab chamber, river | `src/mklev.c:makerooms/makeniche/make_niches/makelevel`, `src/mkroom.c:mkslabroom/mkpoolroom/mkriver/liquify` | Native room/corridor/door generator | 2/3: 3–6-square rooms, at least nine where geometry allows; 6–8 niches; 0–6 pool-room attempts; donor per-room monsters/boxes/tiles; final dark walls and locked doors | Use native room records and stairs; port donor river and slab placement. Preserve accessibility and terminal level without a down stair. |
| Water, `w` shallow water, `s` sand, `e` soil | `include/rm.h`, `util/lev_main.c`, `src/hack.c`, `src/trap.c`, `src/zap.c`, `src/dig.c` | Native pools/moat; Step8 bog differs | 1/3: import distinct shallow water, white dust and soil, not bog substitutes | Shallow water is walkable, wets feet, splits gremlins/rusts iron forms, freezes/thaws/evaporates; dust burial/unearthing and engraving erosion remain branch-local. |
| Dust storms | `src/dungeon.c:dust_storm`, `src/region.c:inside_dust_cloud/expire_dust_cloud/create_dust_cloud` | Saved regions and cloud callbacks | 2/3: per-dust-cell 1/6000 spawn; diamond regions, 3d3 duration, wandering growth/shrink; blindness, damage, spores sickness, salt damage; sentinel healing | Keep native region persistence; derive center from symmetric rectangle bounds rather than adding donor rx/ry fields. Do not equate dust with native poison gas. |
| Dust pits / buried treasure | `src/dungeon.c:dust_storm`, `src/engrave.c` | Native burial, engraving and pit handling | 2: donor erosion and 1/200 pit filling, 1/6000 unearth then 1/2000 bury | Scope to new dust terrain; preserve stored buried objects and native trap handling. |
| Hidden portals, proximity reveal, paid guidance | `dat/chaos2.des`, `src/allmain.c`, `src/shk.c:shk_guide` | Magic portal traps and trap detection | 2: desert portals revealed at Chebyshev distance <3; Elshava guide costs500, detects traps | Do not invent automatic portal discovery on Elshava or remove directed Spire shortcut. Save `tseen` normally. |
| Native monster/class entries | `src/questpgr.c:chaos2_montype`, `dat/chaos2.des` | Acid blobs, dust/fog vortices, piranhas/eels, werewolves, native blob/pudding/wraith/zombie/golem classes | 1: retain native definitions and use branch pool probabilities | Do not wholesale-import donor classes or rebalance existing monsters. Narrow new explicit entries listed below. |
| Alabaster elf / elf-elder | `src/monst.c`, `src/makemon.c`, `src/mon.c`, `src/mcastu.c`, `src/mondata.c` | Native elves, equipment, spellcasting | 3: donor stats/gear/groups, deafness, elder support spells, growth pair, 1/20 death putrefaction | Preserve new species and native roles/races; Alabaster elves/elders and sentinels oppose pudding/blob/umber classes. Do not import donor-wide civilian/undead factions. |
| Alabaster mummy and six syllables | `src/monst.c`, `src/makemon.c`, `src/xhity.c`, `src/worn.c`, `src/mcastu.c`, `src/monmove.c`, `src/mon.c` | Native mummy/combat/save hooks | 3: uniform syllable at birth, cursed stone elf mask, physical/spell second attack, Uur speed/AC, Hoon healing, Krau spell amplification, Naen success/cooldown, Vaul displacement/DR; drop tile once on death/putrefaction | Store syllable in unused bits of existing monster `mspare1`, preserving Sheol freeze and Dragon Caves revival bits. Death removes syllable before retained traits; donor corpse is elf-elder. |
| Sentinel of Mithardir | `src/monst.c`, `src/makemon.c:golemhp`, `src/mon.c`, `src/region.c`, `src/mondata.c`, `src/polyself.c` | Native golem attacks/statues | 3: fixed60HP, electric magic/passive, dust healing, stone conversion exception, Alabaster-elf statue on death | Keep donor nonrandom/no-corpse entry and narrow resistance/physical vulnerability behavior. |
| Wraithworm / First Wraithworm | `src/monst.c`, `src/xhity.c`, `src/mon.c:mcalcdistress`, `src/mthrowu.c`, `include/mondata.h` | Native drain/poison/paralyzing gaze and hurtle | 3: both drain plus poisonous bite and displacement; unique stationary wall-walking First has range5 bite and wind displacement within5 | Extend attack targeting locally; no generic ranged-melee framework. Preserve First uniqueness and no-take behavior; key is floor treasure, not monster inventory. |
| Crystal ooze / living mirage | `src/monst.c`, `src/xhity.c`, `src/makemon.c` | Native touch paralysis/wrap/corrosion and mimic fields | 3: actual species; mirage imitates shallow water, drains moisture and heals by damage; dry/nonliving immunity, watery double damage | Narrow dehydration attack and physical resistances; no global donor damage engine. |
| Aspect of The Silence | `src/monst.c`, `src/makemon.c`, `src/allmain.c`, `src/spell.c`, `src/mon.c`, `include/mondata.h` | Native invisibility, passwall, drain, disease, disintegration, lighting | 3: invisible nonunique NOGEN/NOPOLY boss; blood drain, dehydration, armor-eroding disintegration, disease and passive cold; displacement/DR; radius3 darkness, level energy drain3, distance-based spell penalty | Donor does **not** mark Aspect G_UNIQ. Keep random terminal appearances and initial two-Words trigger; first unmade slab and artifact existence enforce reward uniqueness. No global sanity/insight framework. |
| Coure/Noviere/Bralani Eladrin and alternate forms | `src/monst.c`, `src/makemon.c`, `src/were.c`, `include/mondata.h` | Native shapechanging, equipment and light | 3: import three humanoids, mote of light, water dolphin and singing sand; donor wounded transformation and regeneration | Explicit branch generation and six-species form mapping; omit unrelated Eladrin roster/global celestial faction mechanics. |
| Elshava merchants | `src/shknam.c`, `src/sp_lev.c:fill_room`, `src/monst.c`, `src/were.c` | Native shopkeeper extensions/billing | 2/3: eight independently uniform sea garden/fishery/sand-walker/spa shops; yurian/deep one/selkie/oceanid identities, seal alternate form | Native `eshk` plus species; protect Step5 shop probabilities and 10% mimic cap. These shop types have zero ordinary generation probability. |
| Shop stock | `src/shknam.c:shtypes/garden_armors/garden_weapons/sand_armors/sand_weapons/fancy_clothes/mkshobj_at` | Native stock and equivalent equipment | 2/3: exact class weights and custom arrays; shell armor/weapons, silver acid-coated weapons, named foods, breathing amulets, ceramic tiles, living gear, fancy clothing | Import absent named equipment; use native equivalent for renamed gloves, leather helm, cloak and other equivalent types. Do not replace living gear with generic armor. |
| Elshava services | `src/shknam.c:init_shk_services`, `src/shk.c:shk_other_services/shk_identify/shk_uncurse/shk_appraisal/shk_weapon_works/shk_guide/shk_offer_price/shk_smooth_charge` | Native menus, money/credit, identification, enchantment | 3: persistent donor service choices; basic/premier ID, uncurse, sand-walker weapon works/coatings and guide (appraisal is disabled in the pinned donor menu) | Isolate service access to imported shops. Preserve prices, failure/cap rules and lack of services on unrelated native shops. No unrelated armor/charging shops need importing. |
| Living armor / barnacle armor | `src/objects.c`, `src/allmain.c`, `src/do_wear.c:dosymbiotic` | Worn suit slot, monster melee helpers | 3: real items, enchantment-scaled 3d(3+spe) tentacles, 75% eligible-neighbor attempts, up to5 hits/turn, hunger cost, avoid petrifiers | Native armor model plus narrowly scoped flat DR; no wholesale donor slot/location-DR system. |
| Living mask / stone masks | `src/objects.c`, `src/apply.c`, `src/do_wear.c`, `include/youprop.h` | Native blindfold/lenses slot | 3: nonblinding worn masks; living mask magical breathing, stone face masks with donor name/erosion | Native wear/remove/property source bookkeeping; no unrelated role-mask powers. |
| Six ceramic syllables | `src/objects.c`, `src/read.c:read_tile`, `src/weapon.c:aeshbon`, `src/zap.c:kraubon`, `src/allmain.c`, `src/timeout.c`, `src/do_wear.c`, `src/spell.c` | Native object discovery, timers, combat/regeneration | 3: randomized glyph appearances, weighted167/166/167/167/167/166; must identify pronunciation; consume one; temporary10/15/40 turns uncursed/blessed/cursed; noncursed permanent counter | Dedicated readable tools and explicit tile picker instead of changing global RANDOM_CLASS probabilities. Keep name/weight3/cost300 and mineral identity; no unrelated bone glyphs. |
| Three slabs / Words | `src/objects.c`, `src/objnam.c`, `src/mkobj.c`, `src/read.c:study_word/learn_word`, `src/spell.c:wordeffects`, `src/attrib.c`, `include/youprop.h`, `src/monmove.c` | Native occupation/menu, attributes, flight, movement effects | 3: unique nonwishable slabs selected without replacement;99-turn study; First/Dividing/Nurturing Words, +1 all attributes each; flight / AC-3 / pet healing; light / part-water / overgrow powers with rnz100 cooldown (Priest80%) | Scope to three Words. No Red Word, seals, skills or role abilities. Native turn accounting replaces donor partial-action scheduler; exact choice documented with implementation. |
| Second and Third Keys of Chaos | `include/artilist.h`, `dat/chaos2.des`, `src/makemon.c` | Native skeleton-key artifact model | 3: NOGEN/RESTR chaotic keys, cost1500, no combat/carry/invocation powers | Work as ordinary keys; never gate Gehennom/invocation/ascension. Second fixed on Spire2; Third appears relocated on first Aspect creation. Exclude other alignment keys. |
| Faceless historic statues | `dat/chaos2.des`, `src/sp_lev.c:create_object`, `include/obj.h` | Native statues/historic flag/naming | 2: retain31 positions, blessing, faces/historic presentation | Decorative donor-global deities/roles/Eladrin precursor variants are not independent live branch encounters. Preserve inscriptions/identities without importing their unrelated quest systems; document precise statue-body mapping before gate. |
| Size/material/anarchic and weapon coatings | `src/objnam.c`, `src/weapon.c`, `src/xhity.c`, `include/obj.h` | Native materials/poison plus object data | 2/3: minimum properties needed by specified gear/shops, merge/save/name/erosion/combat support | Narrow stored metadata only; do not import the full donor object-property framework or alter native objects with no imported metadata. |
| Save, bones, recovery | `include/you.h`, `include/monst.h`, `include/obj.h`, native save infrastructure | EDITLEVEL4 epoch, generated structure serializers, monster spare, region arg | 2: audit new hero syllable counters/timers, Word bits/cooldowns and unique slab-generation bits; object metadata; retain existing trap/region/artifact state | Existing hero spare is occupied by Sheol freeze and cannot represent twelve independent counters/timers. Add explicit narrowly scoped fields using native serializers, validate both architectures; no migration/pre-Step9 compatibility. |
| Display and packaging | Donor maps/tables/colors, native tile manifests and DLB lists | TTY descriptions, Windows tiles, three build manifests | 2: every new monster/object/terrain/cloud gets correct table slot, documented reused artwork and both DLB packages | Test indices/bitmap capacity and package bytes; no generated files committed. |
| Chaos quest, alignment keys as endgame gates, sanity, insight, seals, global DR/skills/roles | Donor-wide architecture | Local NetHack5.0 remains authoritative | 4: exclude unrelated frameworks and global progression | Reproduce selected branch effects with explicit predicates, native status/resistance/save APIs and documented narrow adaptations. |

### Important exact mechanics for implementation tests

- `chaos2_montype()` has two apparent donor copy/paste cases: a wraithworm
  genocide check returns a **sentinel**. Record whether preserved or corrected;
  do not silently claim a different probability distribution.
- Wastes1/2 each place six Alabaster elves, two independent25% Bralani,
  two independent25% mirages and two independent50% wraithworms. Wastes3 places
  six elves and **four** independent50% wraithworms (two before and two after
  the central map).
- Last Spire3 uses ten shuffled coordinates: one slab, three guaranteed tiles,
  three66% tiles and three33% tiles. Two equipment piles each include robe,
  elven boots, toga, gloves, metal anarchic quarterstaff and cracked stone
  human mask.
- The terminus creates one randomly unmade slab. First Aspect creation scatters
  the Third Key of Chaos. An Aspect death creates one randomly unmade slab.
  Slab creation flags survive leaving/restoring levels, reading, death and
  later Aspect appearances.
- Aesh temporary +10 physical damage; permanent expected+1/3 per use. Krau
  temporary spell damage multiplier1.5; permanent expected+1/3. Hoon/Naen
  temporary+10 HP/energy each turn, permanent+1 per90 turns per tile. Uur
  temporary speed+6/AC-10 and permanent AC-1 per use. Vaul temporary
  displacement/half damage and permanent donor flat DR contribution.
- First Word lights/sears visible-area hostile monsters with donor extra dice
  for undead/demon/draining/disintegrating/shadow-like bodies. Dividing Word
  parts pools into pits/dry ground, can bisect or throw enemies sideways and
  escape engulfers. Nurturing Word grows vegetation, damages the specified
  unnatural bodies, can turn killed enemies' squares into trees and restores
  fruit-tree harvesting. Do not import the excluded Red Word.

## Implementation and validation status

The branch is connected at DoD110. All ten logical floors, the eight Lua
resources, generated Catacombs, and the return route are implemented. Both
Release architectures have built successfully; focused donor comparisons and
wizard traversal/save checks are recorded below. The complete Step9C automated
gate has passed, including the refreshed ten-floor round trip on both
architectures. Step9D was subsequently canceled by the user.

The dated/ordered checkpoints below are an implementation history. Statements
that a feature was pending at an earlier checkpoint are superseded by its later
test result; they do not override this current status. Earlier failed runs
remain documented and are not counted as successful validations.

Retain EDITLEVEL4 as the combined Step9 epoch. Preserve the local vanilla
endgame, role Quest, Elbereth, alignment and skill systems. Keep branch-local
content and rewards; do not import donor-global requirements for Gehennom or
ascension. The implementation phase withheld publication; the separate
checkpoint authorization above now permits README/commit/push only.

### Implementation notes added during dependency expansion

- Donor shallow-water `w` maps to local Lua **Q**, because NetHack5.0 reserves
  `w` for the wall-selection wildcard. Geometry comparisons normalize only
  this character; native wildcard semantics remain intact.
- Deep-one `AD_SOUL` strengthens surviving kin on death (donor `mon.c`), and
  the Aspect's cold passive can heal/grow/split it (`xhity.c`). These hooks
  are required alongside the definitions, not optional flavor.
- The selected shop lists require six additional weapon types and nineteen
  armor/clothing types; thirty-six object entries including masks, six
  syllables and three slabs have been added with zero ordinary generation
  probability. Donor DR, moon-axe phase dice and stiletto charisma behavior
  require scoped runtime hooks. All twenty-two required monster entries
  (including three Eladrin alternate forms, the selkie's seal, and both
  deep-one growth forms) have been
  added; difficulty values are calculated with the current native `mstrength()`
  implementation in `src/mondata.c` (also used by `#wizmondiff`). The focused
  difficulty test extracts that implementation and checks all imported entries.
  All twenty-two now precede `SPECIAL_PM` and sit outside optional compilation
  blocks. The initial mechanical insertion placed seal under `CHARON` and
  some species among quest duplicates; compiled foundation tests now guard
  against both errors.
- Saved hero fields: `mith_syllables[6]`, `mith_timers[6]`,
  `mith_word_timeout[3]`, `mith_words`, `mith_slabs`. Saved object fields:
  `obranch_props`, `obranch_material`, `obranch_size`; zero leaves native
  behavior unchanged. These are within EDITLEVEL4, with no save migration.

### Foundation implementation checkpoint (not the Step9C gate)

- Terrain display/status/debug tables now cover every terrain and synthetic
  status index. This also corrects missing Sheol ice-wall status entries and
  incomplete older debug names. Shallow water has donor foot wetting, tiny-body
  inventory wetting, gremlin splitting, iron-form rust (mud boots exemption),
  boiling/fire evaporation, freezing and distinct thawing. Frozen shallow water
  does not bury/unearth floor objects or settle boulders as a deep pool does.
- Dust uses appended native region callback indices, saved damage and a center
  derived from the symmetric diamond bounds. It is a visible nonopaque overlay,
  preserving opaque native gas even where clouds overlap. Growth naturally stays
  within donor damage/radius1–6. Damage, spores, salt, sentinel healing, pit
  filling, engraving erosion, burial/unearthing are connected; runtime validation
  is still pending. The native pit-removal API also clears a trapped hero's stale
  pit state before the donor three-turn immobilization.
- All new art reuses existing local tiles. The generated tile-reference table
  exceeded its old arbitrary2500-entry limit; it now derives capacity from
  `MAX_GLYPH` and validates indices before access. This is build tooling only.
- The six syllables have an isolated description shuffle and exact weighted
  selection. Slab selection tracks all three creation bits without replacement;
  an exhausted random slab request produces no object. Reading, study, permanent
  and timed counters, Aesh damage, Krau spell damage, Hoon/Naen regeneration,
  Uur speed/AC, Naen spell success, Vaul displacement, Word attribute bonuses,
  First Word flight and Dividing Word AC are connected. Active Word powers and
  the remaining audited effects are **not yet complete**. Study stores a transient
  object ID rather than an unsafe pointer; after process restart, interrupted
  study begins anew. Regeneration preserves the donor90-turn distributed integer
  schedule without overflowing32-bit arithmetic at late-game turn counts.
- Living and ordinary masks use the native facewear slot; the living mask's
  breathing property uses ordinary worn-property bookkeeping. Their other
  audited metadata, descriptions and runtime tests remain to be completed.

External evidence under `../_qa/step9-audit`:

- `build-mithardir-tiles-x64.log`: x64 Release build **PASS** for the terrain,
  dust, type and tile foundation. Later mechanics require a refreshed build.
- `mithardir-base-tiles.log`: every added artwork slot maps to reused art;
  **2546 tiles** fit the generated **640×1386** bitmap.
- `mithardir-foundation-x64-2.log` and
  `mithardir-foundation-Win32-1.log`: compiled actual production tables and
  selectors **PASS**; complete terrain indices, all20 enabled NOGEN species
  outside quest range, exact1000-roll syllable weights, all8 slab states,
  Aesh rounding and90-turn regeneration through turn1,000,000,000.

No complete Mithardir traversal, boss, services, package, topology or Step9C
gate success is claimed at this checkpoint. At that checkpoint the branch was unwired; current connection work is recorded below.

### Equipment and monster foundation checkpoint

- Item material/size/coating metadata is parsed by Lua, retained in native
  whole-structure save records, included in stacking checks and descriptions,
  and used for weight/material interactions. The instance-only material
  overrides preserve ordinary object-class defaults. Moon axes retain their
  creation moon phase. Anarchic and acid/sleep/blinding/paralysis/filth effects
  run on actual hits, outside speculative weapon scoring. Full equipment
  runtime validation and the remaining donor damage-dice audit are pending.
- Alabaster mummies store one of six syllables in existing `mspare1` bits7–9,
  disjoint from Sheol freeze and Dragon Caves revival. They receive a cursed
  stone elf mask. Their Uur speed/AC, Hoon regeneration, syllable-dependent
  second attack, Krau spell dice, and Naen casting/cooldown behavior are wired.
  A dropped syllable is cleared before corpse traits are retained. Their corpse
  is an Alabaster elf-elder; sentinel death leaves an Alabaster-elf statue and
  sentinel creation fixes HP at60. Complete mummy/boss validation is pending.
- Desiccation preserves dry/nonliving immunity, double damage to watery bodies,
  and healing limited to the victim's remaining HP. Wraithworm life drain and
  poison use separate donor handlers, preserving native `AD_DRLI`. Their
  five-square bite, paralyzing gaze and First Wraithworm wind are connected;
  wind runs once per world turn rather than once per monster movement action.
- Living mirages use native mimic state to resemble shallow water. The Aspect
  is invisible, scatters its first Third Key of Chaos, drains energy and reduces
  spell success while present, and drops an uncreated slab on actual death after
  life-saving checks. Donor nonliving/displacement traits are narrowly scoped.
  Its generation rules and full encounter validation are still pending; subsequent combat/darkness work is recorded below.
- `mithardir-terrain-x64-1.log`: real wizard shallow-water freezing, save/reload,
  distinct thawing, fire evaporation and saved dust/soil/grass **PASS**.
- `mithardir-foundation-x64-3.log`: actual weight calculations for materials,
  sizes and all five moon-axe phases **PASS**, alongside earlier foundation tests.
- `build-mithardir-metadata-fixed-x64.log` and
  `build-mithardir-attacks-fixed-x64.log`: x64 Release **PASS** at their respective
  checkpoints. Subsequent mummy/gaze edits require a refreshed build.
- `mithardir-attacks-x64-3.log`: actual desiccation and drain handlers **PASS**,
  including watery/dry targets, healing caps, both worms, independent poison,
  lethal/life-saving paths and vampiric bypass of armor magic cancellation.
- Wizard reward test corrections: Lua-created items must first be seen;
  Uur speed changes hero-action/world-turn counts; console input must settle
  between actions; native breathing feedback says “survive without air.” These
  were fixture assumptions, not reasons to alter gameplay. The corrected combined
  reward/mask/restore run passed; evidence is recorded below.


### Words, Aspect and defensive mechanics checkpoint

- `mithardir-rewards-x64-7.log`: **PASS** actual unidentified-read refusal,
  identification, uncursed/cursed/blessed durations measured in world turns,
  permanent effects, and mask breathing/nonblindness across save/reload.
- `mithardir-words-x64-3.log`: **PASS** all three actual 99-turn studies and
  consumption, saved cooldown refusal, First Word changing actual lighting,
  Dividing Word drying shallow water and turning deep water into pits,
  Nurturing Word growing room/soil grass, and restored terrain. Tests send
  only a menu-selection letter: a following Enter is a separate game command.
- The three active Words use one native action. The donor's partial-action
  allowance is excluded with its global action scheduler. Cooldowns remain
  `rnz(100)`, reduced to80% for Clerics. Canceled directions do not consume a
  cooldown. Nurturing does not put a life-saved victim inside a newly grown
  tree; this corrects the donor ordering while preserving real-death trees.
- Living/barnacle armor now makes the donor autonomous tentacle attacks at
  neighboring hostiles (up to five attempts, hunger cost). Alabaster death
  putrefaction runs after amulet life saving, excludes stoning, drops/clears
  a mummy's tile before transformation, and restores HP. A genocided target
  form rejected by native `newcham` leaves the ordinary death path intact;
  it does not revive the original body into a prohibited transformation.
  Full live encounter tests for these mechanics remain pending.
- Aspect darkness uses native saved/moving monster light sources with transient
  outer radius3 and inner radius2 dark bits. The regular-sight predicate matches
  the donor, including overlap with ordinary light. Donor low-light3's third
  layer and its global racial-vision framework are excluded; native infrared,
  night vision and x-ray rules remain in place. Movement/restore runtime tests
  for the dark source itself are still required.
- The Aspect's disintegration touch selects armor once per damage point,
  honors donor80% artifact resistance and the additional10%/90% erosion roll,
  reduces enchantment through negative values, then destroys exhausted armor.
  Unprotected targets disintegrate. Native corpse-free death/life saving is
  used; this melee attack does not erase the remaining inventory. Imported
  chromatic scales retain their disintegration protection. Cold retaliation
  increases maximum HP only when healing would exceed it, with native splitting.
- `mithardir-attacks-x64-5.log`: **PASS** prior drain/desiccation cases plus
  armor erosion, immunity, lethal and life-saving disintegration and cold
  healing. The first expanded compile exposed a donor-only death enum; it was
  replaced with native `DIED` and corpse-free `ugrave_arise`, then rerun.
- Imported armor uses its audited donor DR/material ratio, erosion and half
  enchantment. Shields give no DR. Five equally likely body slots preserve
  Vaul's permanent per-slot distribution; mummy Vaul gives10 DR, yurian has2
  natural DR, mummy4, and Aspect3 natural plus3 uncanceled aura DR. Natural
  and armor contributions use donor square-root combination and diminishing
  returns. Negative armor DR is clamped before combination rather than
  amplifying damage. Native armor does not gain donor DR.
- Physical resistance uses native named damage masks because the bit values
  differ from the donor: ooze/mummy resist blunt/piercing and are vulnerable
  to slashing; sentinel reverses blunt/slash; the three Eladrin energy forms
  and Aspect resist all weapon types; mirage reduces weapon strikes to1.
  These rules and DR are applied to resolved physical combat, before imported
  weapon coatings. Native combined hit damage remains the local representation;
  the donor's global split-damage combat engine is not imported. Remaining
  projectile and nonphysical contact-path coverage must be audited before gate.
- **Vaul timed protection remains in progress.** The pinned donor does not
  add Vaul to `Half_physical_damage` or `Half_phys` in its melee engine. It
  separately halves selected spell, explosion, beam and environmental paths.
  Do not implement an unconditional global `losehp` multiplier and claim it
  matches that donor revision.
- `mithardir-defense-x64-1.log`: **PASS** production armor/material/erosion/
  enchantment, all five body locations, permanent Vaul aggregation, natural and
  canceled aura DR, vulnerability/resistance, and all64 lighting truth-table
  combinations. Unaffected native bodies and armor preserve damage and consume
  no extra RNG. This is focused evidence, not full encounter validation.
- `build-mithardir-words-x64.log`, `build-mithardir-living-x64.log`,
  `build-mithardir-darkness-x64.log`, `build-mithardir-aspect-x64.log`:
  **PASS** at their respective checkpoints. Statue/DR work requires refresh.

### Last Spire statue conversion

The map has **31 statues**, each blessed with donor `+9`; it does not have
nine extra statues. In the donor,9 is `STATUE_HISTORIC|STATUE_FACELESS`.
NetHack5.0 uses different `spe` bits, so the Lua map now requests `historic`
explicitly and stores faceless presentation in the scoped object-property
bit `OBP_FACELESS` (0x40). This does not overlap weapon coatings. It survives
native object persistence and produces a faceless statue description.

Each position independently selects one of the donor's30 weighted identities.
Existing Elvenking/Elvenqueen, elf-lord/elf-lady, doppelganger, coure, noviere,
bralani and titan bodies are retained. Excluded donor-global figures have
inscribed donor identities and these local animation bodies:

| Donor identity | Local statue body |
| --- | --- |
| nobleman / noblewoman (two each) | human, explicit male/female |
| dark young | umber hulk |
| firre, shiere, ghaele, tulani, dracae eladrin | Alabaster elf-elder |
| Masked Queen / Queen of Stars | Elvenqueen |
| god | titan; one of the three exact donor divine names |
| dread seraph | Angel |

The donor's global Eladrin precursor/Tulani variant selectors are excluded;
those identities remain inscriptions. Masked Queen and god retain faces.
Animating a substituted statue yields its documented local body, not an
unrelated donor quest creature. No live species are imported solely to animate
these historic decorations. `test_step9c_statues.lua` exhaustively passes all
30 selections and three divine names,31 distinct positions, and face/history
flags. `mithardir-statues-x64-1.log` now also passes actual native Lua loading, all30 body/name resolutions, faceless display and save/reload of the31 statues.


### Branch connection and generator checkpoint

Mithardir is now connected by a branch portal at DoD110, with the exact
`chalv2` approach, seven fixed branch resources, and C-generated levels8–10.
All eight Lua resources were added to the three build/package manifests.
The shop regions currently load native shops; required Elshava stock/services
remain unfinished. This is **not** a passed 9C gate or a manual-ready milestone.

- Generated Catacombs use donor3–6 room sizes, a nine-room minimum while packing
  space remains,6–8 niche attempts,0–6 pool-room attempts, per-room mummy/pool
  monster selection, boxes and tiles. The terminus begins with the7×7 slab
  room, four central doorways and a randomly unmade slab. Native room records,
  stair generation and corridor joining are retained; ordinary filling is
  bypassed. Native `THEMEROOM` marks pool/slab rooms to exclude ordinary stock
  without adding unused donor room categories. Catacomb secrets become locked
  doors, corridors become floor, walls are rebuilt, and the donor river then
  erodes and lights its path. Complete packing/accessibility tests are pending.
- Native room-table bounds retain one sentinel entry; a5000-attempt guard
  prevents an unfillable native rectangle from causing endless room retries.
  This leaves ordinary dungeon room generation untouched.
- River orientation/width/drift follow pinned `mkriver`, including its unusual
  vertical drift comparison against `ROWNO`. Water population/item/gold
  denominators are clamped to at least1 at extended local depths. This is
  necessary because `85-depth`, `100-depth`, and `140-depth` can otherwise
  reach zero or become negative. It does not change logical depth semantics.
- The native random-monster entry point now uses the donor Mithardir pool
  only within this branch. Both donor wraithworm-check/sentinel-return cases
  are preserved. Eladrin genocide fallback is limited to the three imported
  humanoid Eladrin instead of importing unrelated donor class members.
  The first Aspect is forced at the terminus after learning any two Words;
  later Aspects can appear through the donor pool. The Aspect is intentionally
  not `G_UNIQ`; key/slab creation remains bounded by persistent artifact/slab
  state. First Wraithworm remains unique.
- Portal arrival first matches both source dungeon and level. The directed
  `cat3 -> mith3` shortcut uses the donor first-portal fallback when there is
  no reciprocal portal. Wastes portals become visible within Chebyshev
  distance2; this does not reveal Elshava's hidden portal automatically.
- `mithardir-generators-x64-1.log`: **PASS**512 immutable-donor/production river
  path/boundary/RNG traces and40960 monster-pool selections across all ten
  levels. Adapters isolate native class expansion and terrain side effects;
  this does not replace whole-floor wizard/accessibility validation.
- `build-mithardir-defenses-x64.log` and
  `build-mithardir-defenses-Win32.log`: **PASS** complete Release solutions
  before connection. `mithardir-defense-Win32-1.log` and
  `mithardir-attacks-Win32-1.log`: **PASS** corresponding32-bit focused suites.
- First connected build failed on the donor-only `IS_ROCK` macro. Replacing
  it with native `IS_OBSTRUCTED` fixed the build;
  `build-mithardir-connected-fixed-x64.log` is **PASS**.
- First connected traversal reached the portal but failed the panic-log check:
  Elshava's `des.region` uses integer lighting, unlike `des.level_init`'s
  boolean lighting. `lit=true` caused Lua loading to fail and fall back to a
  maze. The eight shop regions were corrected to `lit=1`; the affected build
  and full traversal are being rerun. No successful traversal is claimed yet.


The next packaged traversal (`mithardir-runtime-x64-2`) loaded Elshava but
failed its panic-log check: the hidden portal could not be placed on shallow
water. The pinned donor's `bad_location()` explicitly admits PUDDLE, SAND,
SOIL and GRASS; the port had not extended the native region-placement check.
Those four imported walkable types now have the same eligibility, while
occupied squares and excluded regions still reject placement. The foundation
test exercises every terrain type, occupied/excluded squares, and native maze
corridors. A rebuilt packaged traversal is required before this finding is
closed. The earlier `lit=1` correction resolved the Lua load error.

All twenty-two current Mithardir monster difficulty fields now match NetHack 5.0's
actual `mstrength()` implementation in `src/mondata.c`, verified by
`mithardir-difficulty-x64-1.log`. This replaces provisional hand-entered values;
future imported attack changes must rerun this calculation.


### Elshava merchant checkpoint (gate still in progress)

The eight map regions now independently choose the pinned donor's four shop
types uniformly. Native shop IDs 26–29 have zero ordinary generation probability;
Step 5 probabilities and the 10% stock mimic cap remain unchanged. The native
shop extension/billing is retained for yurian, deep-one, selkie and oceanid
merchants. Imported names and all five weighted equipment arrays come directly
from pinned `shknam.c`. Renamed native equivalents are leather cloak, leather
gloves and elven leather helm. Shell equipment and poison/acid, silver acid-coated
weapons, golden amulets, fancy clothing, named food and exact tile percentages
are implemented. `mithardir-shops-x64-3.log` passes the actual production stock
functions, donor-array comparisons and native-shop guards. An earlier focused
failure found missing coatings on weapon-list pick-axes (native TOOL_CLASS);
classification now follows the selected donor stock category.

Elshava loaded and saved/restored without diagnostics in
`mithardir-runtime-x64-3`. Subsequent traversal-driver corrections wait out
native portal dizziness and track actual arrival portals through horizontal
map reflection; they do not alter production placement. The fifth traversal
has reached all three Wastes levels; the complete route remains under test.

The service implementation is now present but not yet validated. Chat with an
Elshava merchant in their shop offers donor identification, uncursing,
sand-walker weapon work/coatings, and a 500-zorkmid trap-detection guide.
Service choices reuse monster `mspare1` bits 10–16, preserving lower freeze,
revival and mummy bits and requiring no additional save fields. The donor
initialization code yields 1/2 both identification levels, 1/8 premier only and
3/8 basic only; its old comment disagrees with the actual code. Uncursing is
1/3. Sand-walker proof/enchant choices are independently 1/4 and coatings 3/4.
The donor stores appraisal availability but comments out the actual menu;
that disabled operation is not exposed here. Unrelated global service, role,
seal and conduct frameworks remain excluded. Services use native menus and
credit/cash transfer. Ceramic tiles keep the donor TILE_CLASS service price
while represented locally as tools. Native identification flags and object
properties persist through existing object serialization. No service PASS is
claimed until the new build, transaction and save/reload checks complete.


Service helper validation now passes (`mithardir-services-x64-2.log`):
1,689,120 comparisons against the actual pinned donor pricing function,
service selection branches, preservation of lower saved bits, decline and
insufficient-funds paths, mixed credit/cash and credit-only payment. Native
`money2mon()` rejects a zero transfer, so credit-only purchases explicitly
skip that call. Full menu/effect/save checks remain pending. The current
traversal reached Last Spire 1; its isolated driver is being extended to
search and open the map's actual secret portal door before proceeding.


The shop fixture exposed a production conversion omission: `des.region`
defaults to `filled=0`. All eight Elshava regions now explicitly use
`filled=1`, and `test_step9c_shops.lua` executes the real map resource to
check eight independent choices, integer lighting and filling. A fixture
also incorrectly assumed an object's `known` flag implied its type name was
identified; the live test now checks failed pronunciation before paying and
successful pronunciation afterward. Native chat while standing on shop stock
quotes its price before prompting for a direction. The donor also offers
services through payment; that path is now supported for the four imported
merchants (and after settling their bill), preserving native shops.

`mithardir-wastes-x64-5.log` passes 1,024 actual donor/native cellular map
comparisons (Wastes and Elshava), matching RNG traces, plus 256 exact donor
wall-pruning/lighting comparisons. The imported post-map cleanup relocates
entities trapped in overwritten rock and removes dangling Wastes walls, then
uses native wallification. Callback fixtures validate relocation and guard
behavior; packaged reruns are still required after this change.

The sixth traversal reached all ten levels, saved/reloaded the terminal floor,
and returned through Catacombs to Last Spire 3. Its return-door step needed
to retry a normal resisted opening; no production failure or diagnostic was
present. Complete return-to-DoD validation remains pending.


`mithardir-shop-runtime-x64-4.log` now passes an actual sand-walker
transaction: the tile cannot be pronounced initially, paid identification
allows its consumption, gold decreases, and the merchant's available services
and completed transaction survive save/reload. Both x64 and Win32 focused
stock, price/payment, foundation, difficulty and river/pool comparisons pass;
Win32 also passes the cellular/cleanup comparisons in
`mithardir-wastes-Win32-1.log`. The latest x64 cleanup/shop build passes.
The full topology/traversal and remaining service/boss/monster gates are still
in progress, so this is not a Step 9C completion claim.

Additional transitive dependency audit: `AD_SOUL` in pinned `mon.c:4752`
requires deep one → deeper one → deepest one (`mondata.c` growth pairs).
Deaths grant surviving kin 2/4/8 HP below 300 HP, implemented by a preliminary
1/3/7 plus `grow_up(mon,mon)`; at 300+ the donor adds 1 then that same final
HP, totaling 2 before the donor's level-limit correction. At level 45 that
correction removes one maximum HP when the result exceeds 360. Native
`grow_up()` differs (random HP), so this special call now has a scoped one-HP
path. The two growth forms, corpse handling, death pulse and growth pairs are
implemented. Native kill/potion growth is unchanged. Outside
R'lyeh, deep/deeper ones use uniform jacket/leather/rusted chain and
`d(4,2)/3` rusted weapon choices. The R'lyeh enhanced gear belongs to the
later 9D audit; Anachrononaut firearms remain excluded. The deepest-one
outside-R'lyeh weapon table includes intentional dual-wield fallthroughs.
Do not substitute the R'lyeh gear for Mithardir merely because both appear
in the same donor switch.

The two growth forms reuse the deep-one/mind-flayer artwork (both sexes),
remain `G_NOGEN`, and use native-calculated difficulty 20 and 36. No save
fields were added for their death effect. The donor permits growth above
400 HP; the special soul path preserves that, bounded by `LARGEST_INT`.
It retains the native current-HP <= maximum-HP invariant, unlike the donor's
commented-out clamp. This is a deliberate compatibility correction.
`mithardir-souls-x64-4.log` passes all three death pulses, both transformations,
300-HP taper and level limits, overflow bounds, and 14,688 ordinary kill/potion
growth comparisons against the committed Step 8 function.

The Wastes cleanup now leaves unused column zero dark. The donor lights that
non-playable column; in NetHack 5.0 this can make vision request `newsym(0,0)`
when the hero moves to the top map row. Playable geometry and lighting remain
donor-identical. `mithardir-wastes-boundary-x64.log` checks 1,024 cellular maps,
256 cleanup maps, object/monster relocation and the dark boundary. Both Release
builds pass (`build-mithardir-boundary-deep-x64.log` and `...-Win32.log`).
Full traversal and the remaining encounter/service gates remain in progress.

### Subsequent traversal and equipment checkpoint

`mithardir-runtime-x64-8.log` passes the complete ten-level route, saves/restores
on Elshava, Wastes 3, Last Spire 3, Catacombs 1 and the terminal floor, the
directed Spire shortcut, and return to DoD110 with no panic-log entries.
`mithardir-shop-runtime-Win32-1.log` passes paid tile identification,
pronunciation, merchant/service persistence and save/reload. The new explicit
top-row check reaches both map edges without a vision error. Its first run
incorrectly tried to teleport back onto the arrival portal, which native
teleport rejects; the runner now remembers arrival portals independently of
the hero's position. The full Win32 route is being rerun.

The deep/deeper/deepest-one non-R'lyeh gear tables now pass focused tests on
both architectures (180 armor/rusted-weapon combinations and all six deepest
weapon selections). Alabaster elves/elders, the three humanoid Eladrin,
selkies and oceanids now receive their selected donor loadouts. Coure gear
uses its tiny size, including arrow stacks; elf weapons preserve metal and
oversized variations. Native leather gloves, elven leather helm and leather
cloak represent the donor's equivalent names. Independent extraction of the
pinned donor equipment blocks passes 28,672 loadouts per architecture,
including RNG traces, creation order, quantity, enchantment, material and size
(`mithardir-fey-equipment-x64-3.log`, `...-Win32-1.log`).

Alternate-form behavior is now being connected using a separate Mithardir
helper in `were.c`, preserving native lycanthropy classification. Humanoid
Eladrin change below half HP with the donor's 1/2 chance; Noviere needs deep
water. Elemental forms can revert above half HP (1/30, always at full HP).
Selkie/seal use the donor's moon/night chances. Transformations heal a quarter
of missing HP and update light sources. Elemental forms retain unequipped
inventory and re-equip it when humanoid. Protection from shape changers blocks
the donor's outward transformations without forcing elemental forms back.
These new form/armor hooks still require their focused and live validation.

The subsequent focused form checks pass on both architectures: 413,696
decisions match the pinned donor, including RNG consumption, HP thresholds,
deep-water restriction, moon/night chances and shapechanger protection.
Actual shift-function fixtures verify healing, wakeup, retained inventory,
re-equipping, light updates and preservation of `mspare1` service/syllable bits
(`mithardir-forms-x64-2.log`, `...-Win32-1.log`). The live armor check is pending.

`mithardir-runtime-Win32-2.log` now passes the full ten-floor round trip and
all five save/reload checkpoints, including the deliberate map-edge vision
regression. Both architectures have complete traversal passes. These runs
precede the latest fey equipment/AI additions and will be refreshed at the gate.
The tile audit passes all 2,554 mappings and bitmap capacity 640x1390
(`mithardir-deep-tiles.log`).

Monster weapon preferences now include the five new melee weapons and thrown
spikes. Tests exercise actual selection, touch/shield/artifact guards, and
prove that removing the new types leaves the committed native preference
order unchanged (`mithardir-weapon-selection-x64-2.log`, `...-Win32-1.log`).
Donor restrictions still prevent a weak Coure from wielding a two-handed moon
axe. Distinct offhand combat and the remaining combat mechanics still need
completion; loadout and selection passes do not imply those are complete.

### Refreshed compatibility checks

Both latest Release builds pass (`build-mithardir-fey-forms-Weapons-x64.log`
and `...-Win32.log`). Actual wizard probing confirms tiny Coure armor is worn
and retained after save/reload on x64 and Win32 (`mithardir-coure-runtime-x64-1.log`,
`...-Win32-2.log`). The initial probe fixture stopped at the status `--More--`
prompt before the inventory appeared; it now waits for the equipment listing.

The compiled Step 6/7/8/9A, depth-range and ledger/standalone recovery checks
passed on both architectures in `mithardir-cumulative-*-1.log`. Those combined
commands then stopped at older whole-file source contracts which assumed no
Mithardir additions. `test/step9c_source_projection.py` now removes only named
new helpers and exact scoped call sites, the exact saved hero fields, and the
two inert key entries before comparison. Every remaining legacy byte is still
compared against its original milestone; native shop stock/probabilities,
enchantment, wishing, structural/save/recovery code and README remain protected.
The affected Step 7 and Step 8 source/package checks pass on both DLBs
(`mithardir-step7-source-2.log`, `mithardir-step8-source-1.log`). The Dragon Caves
compiled regression also passes both architectures, including 24,576 real
corpse-dispatch cases per architecture (`mithardir-dragon-regression-*-1.log`).
Foundation checks were refreshed for all 22 species on both architectures.

### Offhand combat checkpoint

The pinned donor has seven offhand slots across six imported species: Coure,
the three deep-one forms (slot 1), Bralani (slots 1 and 3), and Alabaster elf
(slot 2). Native `AT_WEAP` remains the dispatch type, with an explicit species
and slot helper selecting the other weapon. Shielded or ranged offhand attacks
are skipped. Without an eligible secondary weapon the permitted melee slot
is unarmed, as in donor `xhity.c`; it never reuses the main weapon.
Damage, coatings, knockback, contact protection and passive corrosion or
disenchantment receive the weapon selected for that attack. Polymorphed heroes
use the same imported slot order, with secondary slots requiring two-weapon
mode. Native species retain their original repeated-weapon behavior.

The local adaptation reselects the offhand from current inventory instead of
adding a saved `MON_SWEP` pointer. It uses native weapon preference and first
eligible object order; the donor instead compares damage between duplicate
types. Donor offhand exclusions for artifacts, curses, excessive weight,
two-handed weapons, shields and unsafe touch are retained. There is no new
save field: `mhitm_data.weapon` exists only during damage resolution.

Explicitly sized branch gear now uses the donor's effective-size handedness
formula. This caught oversized Alabaster sickles and the donor's different
unicorn-horn base size. Ordinary unsized native objects retain their original
handedness. `run_step9c_handedness.py` compares the actual pinned donor function
across 3,072 weapon/wielder-size cases per architecture and checks every
unsized local object (`mithardir-handedness-x64-4.log`, `...-Win32-1.log`).
The first fixture compile failed on a const structure member; corrected fixture
initialization exposed the unicorn-horn mismatch, which was fixed in production.

Offhand selection tests pass on x64 and Win32, including duplicate weapon
types, theft and all eligibility exclusions. Actual x64 Bralani melee uses
both long sword and knife before and after save/restore
(`mithardir-offhand-runtime-x64-1.log`, `...-Win32-1.log`). Expanding the size
comparison to arrows and launchers passes 4,032 cases on x64
(`mithardir-handedness-x64-5.log`). The remaining combat/service checks are
still part of the open Step 9C gate.

### Elder spell audit checkpoint

`AT_MMGC` in pinned `xhity.c` means monster-only casting (the polymorphed hero
cannot cast it), not monster-to-monster-only casting. The elder can cast at
the hero. Native `AT_MAGC` already omits this ability for the elder's player
form. Its donor selection is an eight-entry uniform list: invisibility,
confusion, blindness, sleep beam, distant group healing, nearby group healing,
aggravation and distant group healing again. The initial unused favored-list
coin flip is retained. The general donor spell-list expansion is not imported
for ordinary local monsters.

The elder now has that list. Group healing rolls up to ten d8 for each wounded,
living monster of matching peacefulness within squared distance 10. Nearby
healing includes the caster; distant healing centers on its remembered target
(caster if unset) and excludes the caster. Tame casters also heal the hero.
Healing messages name the recipient rather than the donor's accidental caster
name. The beam uses the native sleep/reflection/resistance system. Existing
native casting cooldown, usefulness checks and the reused native spell effects
remain the compatibility layer; monster-versus-monster casting still needs
completion and validation.

The donor's `MAX_BONUS_DICE` is 10. Its later redundant limit of 15 does not
raise that cap. The imported mummy formula has been corrected from 15 to 10
and reused for the elder, retaining Krau's 1.5 multiplier on die size. Timed
Vaul now halves the ordinary `castmu` damage path after native half-spell
damage; its other beam/explosion/environment paths remain under audit.

`run_step9c_spells.py` extracts the pinned donor selection, damage and healing
blocks and compares actual local helpers and RNG consumption. Initial fixture
compile/link failures were corrected (a macro name and missing saved globals).
The comparison then caught a production `min` macro evaluating a healing roll
twice; the roll now occurs once before HP capping. The x64 comparison passes
4,096 selections, 14,336 damage cases and 1,024 mass-heal cases
(`mithardir-spells-x64-4.log`). These new spell paths still require the full
runtime gate; this checkpoint is not a completed Step 9C milestone.

Both architectures now pass the expanded 4,032-case handedness comparison and
the elder selection/damage/healing comparisons (`mithardir-handedness-Win32-2.log`,
`mithardir-spells-x64-5.log`, `mithardir-spells-Win32-1.log`). The spell build
initially rejected the donor's renamed `Sleeping` property; it now uses native
`Sleepy`, preserving the donor predicate. The refreshed compiled Step 9B and
earlier regressions pass both architectures (`mithardir-offhand-regressions-*-1.log`).
Step 7 and Step 9A/B source/package contracts also pass both DLBs.

Live elder validation exposed the native range split: distant `AT_MAGC` went
through `buzzmu`, which rejects spell-list damage types. Elder and syllable
mummy attacks now retain `castmu` dispatch at range; other native casters keep
their existing dispatch. The first wizard fixture incorrectly expected a human
hero to polymorph into a deep one: native same-race polymorph creates a new
person instead. The fixture now uses a vampire for sleep resistance. Another
fixture run displayed healing but its raw ANSI assertion lost unchanged screen
characters; full message snapshots now preserve those messages. The enclosure
also closes diagonal exits and uses a non-terrain-breaking attack to wound the
elder. Actual x64 group healing, ranged sleep beam and save/reload pass
(`mithardir-elder-runtime-x64-3.log`). Win32 live spell coverage remains pending.

### Eladrin weapon effects checkpoint

Pinned `xhity.c` applies Coure's physical weapon hit before sleep, and Noviere's
rust effect before its physical hit. Native special-damage handlers alone
did not include that weapon damage. A scoped dispatch prefix now retains both
parts for those two imported attacks. Rust that destroys an iron form exits
before further physical damage. Cancellation suppresses rust but still permits
the weapon hit. Coure sleep runs after HP damage, skips monster deaths and
lifesaving returns, and uses hero mortality to avoid adding sleep after a saved
death. Contact armor reduction and coating effects receive the actual weapon.

Coure's donor sleep chance is `1/(7-level)` with a 1–10 turn duration. The local
denominator clamps to at least one for unusually elevated levels, avoiding the
donor's nonpositive RNG range. Native magic cancellation/resistance checks are
used. `run_step9c_fey_attacks.py` exercises the actual new dispatch prefix and
post-hit helper, with native physical/rust handlers as fixture boundaries.
Both architectures pass dispatch/order/cancellation/lethal-exit checks and
286 chance/duration/guard cases (`mithardir-fey-attacks-*-1.log`). The rebuilt
x64 and Win32 Release solutions pass. Live damage now passes on both
architectures (`mithardir-fey-weapon-runtime-x64-2.log`,
`mithardir-fey-weapon-runtime-Win32-1.log`): a wizard-supplied +30 rapier
produces damage beyond the maximum bare attack dice for Coure and Noviere,
including a cancelled Noviere. The fixture saves and restores successfully.
The first x64 fixture read a partially rendered intrinsic menu; it now waits
for the actual sleep-resistance entry before selecting it. This isolates the
physical weapon contribution; generated loadouts have separate donor tests.
The full Step 9C gate remains open.

Win32 elder healing, ranged sleep and save/reload also pass
(`mithardir-elder-runtime-Win32-2.log`). The first run observed healing but no
sleep beam reaching the hero. Moving the fixture hero immediately outside the
enclosure's bars prevents other monsters from intercepting the beam while
retaining a range-two cast. Failure messages now reference the saved full
screen transcript instead of copying megabytes into the console log.

### Sized weapon dice checkpoint

The pinned `src/weapon.c:dmgval_core` audit found that the initial size adapter
scaled only a weapon's first die and omitted its bonus dice. Explicitly sized
or material-altered branch weapons now retain the donor's scaled bonus dice
and flat bonuses. Crystal swords receive their second die and `spe/3` bonus;
moon axes retain the donor's five phases. Material changes adjust die size
for the selected native materials using the donor's direction-dependent rule.
No unrelated materials, exploding dice, artifact exceptions or lightsaber
systems are imported. Shared native weapon definitions remain authoritative
for their base dice; ordinary unmodified native weapons keep their complete
existing base-damage implementation.

`run_step9c_weapon_damage.py` extracts the pinned donor core, material rules,
moon-axe block, applicable bonus cases and final dice normalization. It
compares both rolls and RNG consumption across target sizes, item sizes,
materials, phases and positive/negative enchantments: **377,568 cases per
architecture pass** (`mithardir-weapon-damage-x64-1.log`,
`mithardir-weapon-damage-Win32-1.log`). A separate exact source comparison
confirms the native base-damage block is unchanged. The donor's elven-sickle
plant-bonus comment has no corresponding `ELVEN_SICKLE` combat implementation
in this pin; no new bonus is inferred from that comment.

### Topology, merchants and Vaul checkpoint

Both Release builds pass after the weapon-dice correction. Actual paid tile
identification, pronunciation, merchant/service persistence and save/reload
now pass for sea garden, fishery and spa as well as the previously checked
sand-walker shop (`mithardir-shop-{garden,fishery,spa}-x64-1.log`). These
transaction checks use isolated geometry and actual production stock/merchant
creation; they do not substitute for the remaining portal-guidance and
weapon-service runtime checks.

`run_step9c_topology.py` extends all earlier packaged topology contracts.
Eight fresh x64 and eight fresh Win32 games pass: exactly one ten-level
Mithardir, DoD110 portal, `chalv2` approach at110, seven internal fixed maps
at logical110–116 and generated floors117–119. The preceding Sheol/Caves,
Moria/Tomb/Temple, enrichment occurrence and collision checks also pass
(`mithardir-topology-{x64,Win32}-1.log`). Entrances remain deterministic for
Step9 testing; prior randomized branches remain randomized.

Timed Vaul now also halves actual beam damage (including breath), explosion
damage and iron-form rust-trap damage. Native half-spell/half-physical effects
apply first; native acid-explosion handling remains intact. This follows the
pinned `zap.c`, `explode.c` and `trap.c` damage paths, without altering the
global `losehp` or half-damage properties. `run_step9c_vaul.py` compiles these
actual source blocks and donor counterparts: **65,600 comparisons pass on
each architecture**, including zero damage, stacked protections and
invulnerability (`mithardir-vaul-{x64,Win32}-1.log`). Both Release builds pass.

The live Vaul fixture remains under validation. Its first run failed to
answer native "step onto" confirmation after restore; it now uses the
explicit move prefix. The second encountered the native 1/5 known-trap escape
and correctly rejected unchanged HP as proof of protection. It now retries
an avoided trap and requires actual HP loss while retaining iron form. Other
Vaul paths and the full Step9C gate still require completion.

### Leader entourage and growth checkpoint

Pinned `mondata.c` includes Alabaster elf → Alabaster elf-elder growth, now
restored locally. The actual growth function passes that transformation and
all existing deep-soul/native growth checks on both architectures
(`mithardir-elf-growth-{x64,Win32}-1.log`).

The ordinary group flags were already present, but pinned `makemon.c` also
creates leader companions on random selection. A deeper one requests 4–13
deep ones; a deepest one requests 4–6 deeper ones and 11–20 deep ones. A
random elder requests one elf and its large group, with a 50% extra elder
and small group. Creation can fail when no legal space remains. Explicit
spawns, pets, `MM_NOGRP` callers and native species receive no new entourage.
Native group placement and peacefulness checks are reused.

`run_step9c_entourage.py` extracts the three pinned leader blocks and compares
spawn order, counts, RNG and failed creation handling at the placement/group
API boundary: **131,072 comparisons pass per architecture**
(`mithardir-entourage-x64-2.log`, `mithardir-entourage-Win32-1.log`). The first
fixture compile collided with native `rn1`; it now uses that existing macro.
This addition still requires the refreshed build/live gate.

The refreshed x64 ten-floor traversal passes with current weapon changes,
including both top-row vision edges, representative save/reload, all forward
connections, the directed Spire shortcut and return to DoD110
(`mithardir-runtime-x64-9.log`). The entourage changes subsequently pass the
x64 Release build; Win32 is being refreshed.

Actual pronounced Vaul now passes rust-trap control/protection and save/reload
on both architectures (`mithardir-vaul-runtime-x64-3.log`,
`mithardir-vaul-runtime-Win32-1.log`). The unprotected iron form is destroyed;
the protected restored form takes damage and survives. An avoided trap is
retried, not counted as a successful protection test. The earlier failed
fixture logs and their Lua assertion diagnostics are retained.

### Elder monster-target casting checkpoint

Native `mattackm` had no `AT_MAGC` spell-list case. Imported elders now dispatch
their list against another monster, including confusion/blindness, close/far
healing, invisibility, sleep beams and aggravation. The entry point excludes
all other species, so it grants no new spells to native monsters. Syllable
mummy monster-target casting remains to be completed. Spellcasting does not
invoke contact passive retaliation.

The adapter retains the already documented native cooldown/fumble framework.
Monster resistance and native sleep/reflection are reused. Far healing uses
the actual monster target's coordinates. Blindness updates both native vision
fields. The donor's `ucast_spell` lacks a sleep case even though the elder can
select it; the local implementation uses its actual sleep beam against either
kind of target. Aggravation wakes opposing monsters and may release paralysis;
it does not overwrite their remembered hero location with another monster's
coordinates or force a hero screen interruption. Those donor behaviors do not
fit native target tracking. No donor-wide faction/targeting system is imported.

`run_step9c_elder_mm.py` exercises the actual new dispatch and usefulness
guards, with existing beam/heal/resistance APIs as fixture boundaries. Both
architectures pass effects, cooldown/cancellation/fumble, target coordinates,
resistance, beam death flags and native exclusion
(`mithardir-elder-mm-x64-2.log`, `mithardir-elder-mm-Win32-1.log`). The first
fixture link lacked a saved-global definition; that fixture was fixed. The
x64 Release build passes; live conflict and Win32 build checks are running.

The cumulative x64 focused/source/depth/ledger/recovery runner also passes
after the entourage/growth changes
(`mithardir-entourage-regressions-x64-1.log`). This includes the Moria package,
classic Tomb, earlier scheduler protections and invalid checkpoint rejection.

The actual x64 elder conflict test now passes a status spell applied to an
iron golem and save/reload (`mithardir-elder-mm-runtime-x64-1.log`). Both elder
builds passed. Monster-target sleep now uses native `dobuzz` with monster-target
flags, as the native monster-versus-monster breath path does.

The shared entry point now also supports syllable mummies. They retain the
native general wizard list, with target-aware usefulness checks rather than
hero status/peacefulness checks. The local adapter supplies psychic damage,
healing, haste, stun, invisibility, maximum-HP drain, armor destruction, item
curse, aggravation, summoning and death touch. Krau spell dice and Naen
cooldown/fumble behavior are shared with the already checked hero-target path.
Native monsters remain excluded.

Compatibility boundaries: the donor-wide expanded generic spell catalog is
not imported. Summoning reuses native `nasty` and its native count/candidate
limits, with a temporary caster proxy carrying the monster target coordinates;
the real caster's remembered hero position is unchanged. Peaceful/tame casters
do not use this hostile native summoning path. The item-curse subset includes
ordinary inventory, Magicbane and intelligent-artifact resistance; unrelated
donor artifacts/factions are excluded. Armor destruction preserves native
quest artifacts and imported chromatic disintegration protection. Maximum-HP
drain cannot leave a nonpositive maximum after lifesaving. Death touch uses
the caster's level rather than the donor's accidental hero-form level.
Native `monkilled` handles death/lifesaving. No new fields are added.

The expanded `run_step9c_elder_mm.py` tests both casters, actual local list
selection, target proxy, native exclusions, all mummy spell-effect branches,
Naen, cancellation, immunity and lifesaving. Both architectures pass
(`mithardir-mummy-mm-{x64,Win32}-1.log`). The x64 combined build passes;
live mummy conflict and refreshed Win32 build are running. The remaining
Step9C combat, services and environment gate is still open.

### Vaul path and dust callback checkpoint

Both combined Release builds and both live monster-target conflict tests now
pass for elders and syllable mummies, including save/reload. The final live
Win32 logs are `mithardir-{elder,mummy}-mm-runtime-Win32-1.log`.

Timed Vaul now also reaches the native spell helpers that reroll damage or
status duration after the initial spell roll, blindness duration, psychic
blasts, monster wand striking, falling piercers, swallowed explosions,
carnivorous bags, ordinary poison HP damage and inventory-curse attempts.
Each modifier is applied after that path's existing protection. Native poison
retains towel protection and its separate severe/attribute-loss branches;
the donor's global physical-protection change is not imported. Monster-read
fire scrolls are disabled by native `#if 0`, so no inactive code is changed.
Unrelated donor artifact/song attacks and the expanded global spell catalog
remain excluded; the Cthulhu-specific path belongs to the later 9D audit.

`run_step9c_vaul.py` passes on both architectures: 65,600 pinned beam,
explosion and rust comparisons, 57,400 actual rerolled spell modifiers plus
blindness stacking, and 106,600 actual native damage-path cases plus pinned
curse bounds (`mithardir-vaul-paths-{x64,Win32}-1.log`). It checks native
source equivalence after removing only the scoped Vaul guards. Both refreshed
Release builds pass (`build-mithardir-vaul-paths-{x64,Win32}-1.log`).
The refreshed Win32 cumulative Step6/7/8/9A, depth, ledger and recovery gate
passes, including packaged Tomb/Moria bytes
(`mithardir-vaul-regressions-Win32-1.log`).

`run_step9c_dust.py` extracts the actual creation and inside/expiry callbacks
and the local anhydrous classification. On each architecture it passes
13,440 geometry/TTL/flag checks, 53,760 pinned drift/growth/shrink comparisons
and 61,440 hero/monster effect comparisons. These cover map edges, RNG order,
blindness, spores, existing sickness, salt, immunity, sentinel healing and
lifesaving (`mithardir-dust-x64-3.log`, `mithardir-dust-Win32-1.log`).
The first fixture compile used the wrong return type for native `isok`; the
fixture was corrected. Region allocation/message APIs are fixture boundaries.
The donor comparison maps breathlessness and sickness resistance to the
documented native species checks, without importing donor resistance fields.
Live natural-storm save/restore validation is running. The full 9C gate is
still open; this checkpoint does not authorize beginning 9D.

The actual natural Wastes storm test now passes on both architectures
(`mithardir-dust-runtime-x64-3.log`, `mithardir-dust-runtime-Win32-1.log`).
It observes naturally spawned regions through wizard `#timeout`, saves and
restores their exact strength, bounds and remaining lifetime, then confirms
continued expiry/drift. Native `rest_regions` removes already expired ttl0
regions without callbacks; the test preserves that existing rule. The first
fixture tried a cross-branch named teleport (unsupported by native NetHack),
and the second incorrectly expected ttl0 regions to survive restore. Both
fixture assumptions were corrected; no region production change was needed.

`run_step9c_word_combat.py` passes on both architectures: 30,720 pinned First
Word damage/multiplier/RNG comparisons, 2,048 Dividing combat/terrain cases
and 3,072 Nurturing target/tree/lifesaving cases. Cancellation, engulfment,
fruit renewal and movement loss are also covered
(`mithardir-word-combat-x64-2.log`, `mithardir-word-combat-Win32-1.log`).
The first fixture named a nonexistent generic demon; it now uses the native
horned devil. The x64 live Word runner now additionally passes undead light
combat, Dividing knockback/bisection and Nurturing undead death/tree creation,
along with all prior study, terrain and save/restore checks
(`mithardir-words-combat-runtime-x64-1.log`).

Living-armor audit found that the local loop allowed one attack while its
wearer was helpless. Pinned `dosymbiotic` uses `magr_can_attack_mdef`, whose
`cantmove` check forbids that. The local helper now skips sleeping/paralyzed
wearers, checks clear paths and preserves the native grid-bug diagonal attack
restriction. Occupations still permit attacks when the wearer is able to
move. It still stops immediately if retaliation incapacitates the wearer or
destroys the armor. `run_step9c_living.py` passes 31,744 enchantment/limit cases
on both architectures plus path, helplessness, petrification, hunger,
retaliation, swallowing and grid-bug guards
(`mithardir-living-{x64,Win32}-1.log`). Refreshed builds and worn-armor live
checks are in progress; the full 9C gate remains open.

The live Word combat extension now also passes on Win32
(`mithardir-words-combat-runtime-Win32-1.log`). Both builds with the
living-armor eligibility guards passed. The first live worn-armor test
revealed a distinct object-parser bug: `living armor` was treated as a
generic armor-class request and produced an ordinary cloak. The parser now
recognizes the complete `living armor`/`barnacle armor` names and their
`giant sea anemone`/`giant shell armor` descriptions before class/monster-name
stripping. It then uses ordinary native object creation and wish restrictions.
The runtime fixture now asserts the created object's identity before wearing
it; `run_step9c_armor_names.py` covers both new types and native armor aliases.
These fixes require refreshed builds and live passes before this issue is
closed. The failed fixture log is `mithardir-living-runtime-x64-1.log`.

The exact-name/description parser test now passes all24 cases on x64 and
Win32 (`mithardir-armor-names-runtime-{x64,Win32}-1.log`). Both armor-name
Release builds passed. Correctly worn living armor now passes actual tentacle
combat before and after save/reload on x64
(`mithardir-living-runtime-x64-2.log`). The Win32 barnacle-armor live check is
running. Native wish restrictions and the existing armor alias cases remain
unchanged.

The Aspect spawn path now includes the missing donor common message00202,
"A terrible silence has fallen!", through native `com_pager` and a named
entry in `dat/quest.lua`. The existing initial warning remains. This restores
branch presentation without importing donor quest/endgame structure. The
Mithardir package checker now also requires current `quest.lua` bytes;
builds are being refreshed for this addition.

Correctly worn barnacle armor now also passes live tentacle combat and
save/reload on Win32 (`mithardir-barnacle-runtime-Win32-1.log`).

The projectile audit found missing calls to the imported physical-defense
helper in native `thitu` and `ohitmon`. Both now apply it to physical missile
damage; acid bypasses it, and native poison additions remain outside it.
Hero-thrown weapon hits already use the scoped `hmon` path. The native
combined damage representation and half-physical handling remain authoritative.
No extra displacement roll is added to projectile hits: the pinned donor's
`projectile.c` invokes `tohitval`/`hmon2point0` directly, bypassing the
`xmeleehity` independent displacement check.

The defense runner now passes20,200 actual hero/monster projectile dispatch
cases per architecture, checking single defense/RNG application and acid
exclusion, alongside the existing armor, native-neutrality, resistance and
lighting checks (`mithardir-projectile-defense-{x64,Win32}-1.log`). Builds
and remaining encounter validation are being refreshed.

Both projectile Release builds and the refreshed eight-map/dungeon/quest
package check pass (`build-mithardir-projectile-{x64,Win32}-1.log`,
`mithardir-package-2.log`). Sheol/Dragon Caves source/package regressions
pass; refreshed Win32 Dragon Caves focused tests include24,576 corpse/drop
cases (`mithardir-projectile-step9b-Win32-1.log`).

The first live mummy/Aspect reward test exposed a missing donor guarantee:
native `corpse_chance` still applied its ordinary random roll to an Alabaster
mummy, skipping its tile/corpse path. Pinned `mon.c` explicitly guarantees that
path for Alabaster mummies. The local predicate now does too. The existing
putrefaction, petrification, disintegration and level-specific exclusions
remain separate. The focused soul/growth runner now additionally passes
55,040 corpse-chance comparisons per architecture, proving all other species'
results and RNG unchanged (`mithardir-mummy-corpse-{x64,Win32}-1.log`).
The failed live run is `mithardir-deaths-runtime-x64-1.log`; its paniclog is
the deliberately asserted missing reward, not an unexplained native crash.
Builds and the actual alive/dead reward save/restore check are being rerun.

The AI audit corrected both a missing implementation and imprecise audit
wording. The donor's explicit Mithardir hostility is Alabaster elves/elders
and sentinels versus pudding/blob/umber classes, in both directions. It is
now included in native `mm_2way_aggression`; the unchanged enclosing native
dispatcher preserves pet protection. The broader donor civilian/undead
faction system is excluded. `run_step9c_aggression.py` passes184,900 species
pairs on each architecture, matching72 directed enemy pairs from the pinned
branch rule (`mithardir-aggression-{x64,Win32}-1.log`). Build/runtime refresh
and the remaining 9C gate checks are still pending.

### Reward, Alabaster iron and natural form-change follow-up

Both Release builds were refreshed with the hostility rule. The actual
mummy/Aspect reward runner now passes on both architectures
(`mithardir-deaths-runtime-x64-3.log`, `...-Win32-1.log`): the mummy gives
exactly one tile and a cursed mask; the Aspect warning appears, exactly one
Third Key is created, and its death produces one slab. Alive/dead saves and
restores preserve these counts. The earlier warning assertion was a test
observer defect: the base wait loop dismissed the pager directly, bypassing
the observer's send method. Recording reconstructed screens from the wait
loop fixes that test without changing the warning.

Pinned `monst.c` marks the living Alabaster elf and elder as iron-hating.
The port now retains `xhityhelpers.c:hatesobjdmg`'s 1d(defender level) iron
damage in native weapon/projectile damage, improvised weapon hits, worn
contact equipment/rings and iron-golem contact. Bare-handed Alabaster
monsters reject iron through native touch/selection checks; gloves permit
handling. No native species gains iron vulnerability. Local native contact
and bonus ordering remain authoritative; this does not import the donor's
global material framework, holy/unholy systems or unrelated khakkhara.
The local defensive floor makes a drained level-zero victim use d1 rather
than passing an invalid zero die to RNG. Native ring/contact selection is
retained instead of introducing donor hand bookkeeping.

`run_step9c_iron.py` passes 8,668,800 damage/RNG comparisons per architecture,
all species' handling with/without gloves, hero forms and contact-cover
checks (`mithardir-iron-{x64,Win32}-2.log`). The native touch function is
byte-identical after removing the scoped iron guard. Existing weapon
selection checks still pass on Win32; the complete 9C gate remains open.

The natural form-change wizard test exposed a production integration bug
after all three elemental forms returned to humanoids: `mith_fey_shift`
passed deferred `NEED_WEAPON` to the immediate native wield routine. That
routine accepts `NEED_HTH_WEAPON`. The call now uses that state. The focused
fixture additionally checks the native callee's accepted states, so its stub
can no longer mask this mismatch. Both architectures pass the 413,696 pinned
decision/RNG comparisons and equipment/light/state checks
(`mithardir-forms-wield-{x64,Win32}-1.log`). Refreshed live tests are pending.
The first live form fixture also used an invalid Lua terrain argument;
that fixture-only failure was corrected before the production issue was found.

The corrected live form test now passes on both architectures
(`mithardir-forms-runtime-x64-3.log`, `...-Win32-1.log`): all three full-health
elemental forms are saved/restored, naturally return to their humanoid forms,
and are saved/restored again without the invalid weapon-state diagnostics.
The refreshed x64/Win32 builds and cumulative Step6/7/8/9A focused/source,
depth and ledger/recovery suites also pass
(`build-mithardir-forms-wield-{x64,Win32}-1.log`,
`mithardir-forms-regressions-{x64,Win32}-1.log`). Native weapon selection passes
on both architectures after the iron handling extension.

A projectile audit found that monster-thrown coated weapons bypassed
`mith_weapon_effects`, although melee and hero-thrown weapons reached it.
Both native projectile hit paths now apply the existing scoped handler to
weapons/weapon-tools. The monster path runs after wakeup and only for a living
target actually hit; it therefore preserves sleep/paralysis effects and
does not coat a harmless pass-through or petrified target. Native damage,
kill credit, lifesaving and corpse handling still belong to the caller.
`run_step9c_coatings.py` compiles the actual handler and both hit-dispatch
fragments. Both architectures pass 800 dispatch cases plus direct acid,
sleep, paralysis, blindness, filth, resistance, coating depletion and native
no-property RNG checks (`mithardir-coatings-x64-3.log`, `...-Win32-1.log`).
Those unit fixtures stub native resistance/status APIs; they do not claim a
complete live coated-projectile encounter. The first two fixture builds
failed on native API signatures/link dependencies and were corrected without
changing production behavior. Package refresh is in progress.

### Subsequent live and compatibility audit results

- Both coating Release builds and both DLB byte checks passed
  (`build-mithardir-coatings-{x64,Win32}-1.log`, `mithardir-package-4.log`).
- The live First Wraithworm check passes on both architectures: no wind at
  six squares, wind within five after restoring the encounter, and another
  save/restore afterward (`mithardir-worm-runtime-{x64,Win32}-1.log`). The
  monster is paralyzed in this radius test; it does not claim bite coverage.
- Actual paid portal guidance, uncursing, proofing, enchantment and acid
  coating pass on x64 after eight naturally rolled merchants, followed by
  item-state save/restore (`mithardir-shop-works-runtime-x64-2.log`). The first
  fixture incorrectly assumed the portal stayed at its requested coordinate;
  it now inspects native generated portal positions. Win32 is sampling.
- The expanded dust runner passes on both architectures, retaining all prior
  pinned callback comparisons and adding actual terrain-turn engraving
  erosion, occupied-pit filling/release/burial, unearth/bury outcomes and
  nearby portal discovery (`mithardir-dust-terrain-x64-2.log`, `...-Win32-1.log`).
  Its first link failed on two fixture dependencies, subsequently supplied.
- Pinned `xhity.c` sends pure elemental damage directly to `xdamagey` and
  applies physical defense in `hmon2point0`. No universal DR multiplier is
  appropriate for pure fire/cold/magic contacts. The local combined-damage
  adaptation remains explicit. The audit did find that improvised physical
  blows bypassed imported defenses. They now take the blunt defense path
  without accessing an object that may already have broken. Both defense
  suites pass (`mithardir-improvised-defense-{x64,Win32}-1.log`).

Living mirages previously set their puddle appearance directly, bypassing
native shape-protection checks, and their valid furniture disguise triggered
the native non-mimic sanity warning. Birth now uses `set_mimic_sym`, with the
pinned puddle case ahead of ordinary mimic choices. The sanity exception is
limited to this species in this exact disguise. Native restoration and
protection logic remain unchanged. Source projection passes
(`mithardir-mirage-source-1.log`), and both live tests pass puddle appearance,
save/restore, protection revealing an existing mirage, protected creation
remaining visible, and enabled wizard sanity checks
(`mithardir-mirage-runtime-{x64,Win32}-1.log`). The preserved earlier binary
reproduces the non-mimic warning (`...-x64-before5.log`). Earlier negative
fixtures first needed a TTY column correction and the complete runtime
templates; those setup failures are not counted as production failures.

Explicit imported armor sizes were enforced for monsters but not heroes.
`mith_armor_size_fits` now checks both, preserving pinned flexible hats,
one-size cloak/elven-suit tolerance, dragon-scale and shield exceptions.
High-elven plate and elven toga are included in the donor elven-suit set.
Unmarked native gear remains unchanged; native anatomy, layering and
polymorph restrictions remain authoritative rather than importing donor body
shape/race systems. Both architectures pass 831,600 comparisons of the pinned
wear conditions (`mithardir-armor-size-{x64,Win32}-1.log`). Builds and live
hero/Coure equipment checks are being refreshed. The full 9C gate is open.

### Gate refresh: armor, services, Aspect and remaining kick path

Both armor-size Release builds passed. The live hero wear tests pass all six
cases on both architectures: small/large jackets rejected, native jacket
accepted, oversized cloak accepted, flexible small cornuthaum accepted and
rigid small high-elven helm rejected. Accepted equipment survives save/restore.
Logs: `mithardir-armor-size-runtime-x64-2.log` and
`mithardir-armor-size-runtime-Win32-1.log`. The initial x64 fixture tried a
nonexistent wish-parser size prefix; it now uses the production `des.object`
size metadata API. No wish-parser change was made.

Coure's three tiny worn items pass native probing and save/restore on both
architectures (`mithardir-coure-sizing-runtime-{x64,Win32}-2.log`). A sleeping
fixture could wake and wander before probing, so it now uses a counted
paralysis for this equipment-only check. Natural form transitions have their
separate awake runtime tests.

All paid service paths also pass on Win32 after two naturally rolled
merchants, including item-state save/restore
(`mithardir-shop-works-runtime-Win32-2.log`). Native placement could occupy the
fixture's requested teleport square; its position check now accepts a legal
interior square within payment range. Merchant service rolls and payment
mechanics were not changed to force coverage.

`run_step9c_aspect.py` compiles actual turn processing, the native moving
light-source function and the production spell-suppression fragment. Both
architectures pass exact energy loss per live Aspect, zero-floor/dead-monster
handling, Naen ordering, 202,398 pinned donor spell-chance comparisons and
moving radius3/radius2 darkness with blocked paths and map edges
(`mithardir-aspect-{x64,Win32}-3.log`). Native light and the hero-position
`COULD_SEE` path are covered. Initial fixture attempts needed the native local
light flag definition and proper separation of hero-position vision from
ordinary path visibility; production code did not change for these tests.

Both live death/reward tests now also verify passive energy loss across six
actual world turns, both before and after restore, while allowing native
Wizard energy regeneration. They still pass mummy tile/mask drops, Aspect
warning, exactly one Third Key, slab drop and alive/dead restore
(`mithardir-deaths-energy-runtime-{x64,Win32}-1.log`).

Pinned `xhity.c` includes `unarmed_kick` in the Aesh general damage bonus.
The audit found that local ordinary `kickdmg` bypassed both that bonus and
imported physical defenses. It now calls the existing scoped handlers before
committing damage. Both defense tests pass actual kick-dispatch cases and
verify that removing those two calls recovers the entire baseline `dokick.c`
byte-for-byte (`mithardir-kick-defense-{x64,Win32}-1.log`). Unaffected native
kicks retain damage and RNG behavior; polymorph kicks already use `damageum`.
The x64 build with this fix passed; Win32 refresh is running.

Both refreshed cumulative Step6/7/8/9A focused/source/depth/ledger/recovery
suites pass (`mithardir-size-regressions-{x64,Win32}-1.log`). Eight fresh games
per architecture pass all topology checks
(`mithardir-topology-gate-{x64,Win32}-1.log`), retaining randomized earlier
branches, Castle200 and the temporary reservations. Both DLB byte checks and
the complete tile-table/reused-art/bitmap-capacity check passed. Full ten-floor
traversal refresh and the awake First Wraithworm bite encounter remain in
progress. These pending checks keep the Step9C gate open; Step9D has not begun.

## Manual validation route and checklist

Use a **new EDITLEVEL4 wizard game** from `binary/Release/x64/NetHack.exe`
or `binary/Release/Win32/NetHack.exe` (`-D`); `NetHackW.exe -D` provides the
tiled window port. Do not use pre-Step9 saves or assume root-level binaries
are current. The checkpoint remains deliberately deterministic.

1. Use Ctrl-V (`#wizlevelport`), enter `110`, and use `#wizmap` if needed.
   This is the `chalv2` approach in the ordinary DoD. Its central structure,
   surrounded by water, contains the **magic portal** to Elshava. Keep track
   of the normal DoD stairs; the portal, not those stairs, enters Mithardir.
2. Elshava is branch level1 (`ossa1`). Inspect all eight merchants and their
   species, stock and prices. Use `p` inside a shop to reach its other-service
   menu; available services are randomized and persist after saving. Test
   identification/pronunciation, uncursing, weapon work when offered, and
   the 500-gold portal guide. Find the hidden **western** portal to Wastes1;
   the separate arrival/return portal leads back to DoD110. Save/reload here.
3. Follow Wastes1 (`mith1`) east to Wastes2 (`mith2`), then east to Wastes3
   (`mith3`). Examine white dust, stone islands, ceramic tiles, shallow water,
   storms, burial/unearthing and pit filling. Desert portals reveal within
   two squares. The shallow-water map symbol is `Q` in Lua, displayed as
   water in-game; it is separate from deep water and Moria bogs. Test cold/fire
   freezing, thawing and evaporation. Save during an active dust cloud.
4. On Wastes3 enter the central Last Spire and take the down stair. Last Spire1
   (`cat1`) has pools, faceless historic statues and a **secret eastern side
   chamber** containing the portal onward. Search its door and open it.
   Last Spire2 (`cat2`) holds the unique First Wraithworm and the **floor**
   Second Key of Chaos. Test five-square biting, paralysis gaze and wind.
   The eastern portal advances to Last Spire3 (`cat3`); its return portal
   goes to `cat1`. Save/reload around the First Wraithworm encounter.
5. On `cat3`, inspect both equipment piles, ceramic tiles and the slab.
   Search the eastern hidden chamber for the down stair into the generated
   Catacombs. The western hidden portal is a deliberate **shortcut back to
   Wastes3**, not a return to `cat2`. The resource table above gives exact
   donor-map coordinates; map alignment can offset screen coordinates.
6. Descend generated branch levels8, 9 and 10. Inspect dark connected rooms,
   locked doors, pools, niches and Alabaster mummies. Verify their syllable
   behavior and a single ceramic drop on death. Some Alabaster deaths instead
   putrefy into a blob/pudding. The terminus has a slab chamber and river,
   and **no down stair**. Save/reload an ordinary Catacomb and the terminus.
7. Test the Aspect's darkness, energy loss, suppressed spell success and
   attacks. Its birth warning is “A terrible silence has fallen!” The donor
   does not mark this species unique: terminal appearances and the two-learned-
   Words trigger are retained. The Third Key artifact exists once; slabs are
   generated without replacement. Save/reload before and after an Aspect death.
8. Identify a ceramic tile with `#wizidentify` (or the relevant paid service)
   and read it with `r`. Compare uncursed/blessed/cursed duration and permanent
   effects; cursed tiles do not add permanent counters. Test Aesh on ordinary
   attacks and kicks, Uur AC/speed, Hoon/Naen regeneration, Krau spell damage
   and Vaul protection. Wear living/barnacle armor and watch autonomous
   tentacles against adjacent hostiles, hunger use and removal. Put on a
   living mask with `P`: it permits breathing without blinding the wearer.
9. Read each slab, finish its 99-turn study, and use **`#word`** to select a
   learned Word. Check light/searing, directional water parting and vegetation,
   the passive bonuses, cooldown refusal and persistence after restore.
   The two Keys remain ordinary key tools and do not unlock a new endgame gate.
10. Return up from level10 through 9 and 8 to `cat3`, use its western shortcut
    to Wastes3, then the portals back through Wastes2, Wastes1 and Elshava to
    **DoD110**. Check both TTY names and tiled names/art at each stage. Reused
    artwork is intentional and catalogued by `test_step9a_tiles.py`; it needs
    visual inspection, not a claim that new bespoke artwork was imported.

Native NetHack perception and tracking remain authoritative. Imported
Alabaster elves/elders, humanoid Eladrin and seals have eyes and use native
`can_track`; seals also satisfy native `olfaction`. Donor `MV_SCENT`'s separate
six-square around-corners detection and hero scent-vision framework are not
imported. This is an explicit sensory adaptation alongside the already
documented native infrared/night-vision and excluded low-light3 layer.

### Final content and encounter refresh

Both final kick-fix Release builds pass
(`build-mithardir-kick-{x64,Win32}-1.log`), and their eight Mithardir resources,
`dungeon.lua` and `quest.lua` match source bytes in both DLBs
(`mithardir-package-kick-1.log`). Both refreshed Dragon Caves suites pass,
including all 24,576 corpse-dispatch cases; the combined Sheol/Dragon Caves
pinned-source and package checks also pass
(`mithardir-gate-step9b-{x64,Win32}-1.log`,
`mithardir-gate-step9ab-source-1.log`).

The additional awake First Wraithworm encounter passes five-square biting
both before and after save/restore on both architectures
(`mithardir-worm-bite-runtime-{x64,Win32}-1.log`). An iron-bar enclosure keeps
the hero in place against wind; it does not paralyze the monster or suppress
its AI. The separate six-versus-five-square wind test remains applicable.

`test_step9c_content.py` reads immutable `chaos2.des` and checks all eight Lua
resources' exact geometry, alignment, flags, cellular initialization, stairs,
maze walks, doors, portal destinations/bounds and shop regions. It also executes
every real map script across 128 controlled seeds per resource, comparing
object/monster decisions, chances, coordinates, shuffled placements, named
keys and scoped material/weapon properties against parsed donor directives
(`mithardir-content-gate-2.log`). These are controlled script primitives,
not a claim that engine-wide RNG consumption is identical. The separate
native cellular/Catacomb generator and packaged traversal tests cover engine
side effects. The full Elshava region and statue-identity Lua checks also pass.

Both final traversal runs passed all ten branch floors, representative saves,
terminal save/restore, the directed Last Spire shortcut and return to DoD110
(`mithardir-runtime-gate-{x64,Win32}-1.log`). These runs used isolated copies
of the immediately preceding armor-size builds; the subsequent two-call kick
fix does not affect traversal and passed its focused checks and both builds.
No traversal result is being relabelled as a different executable version.
The final package bytes, tile checks, focused regressions and `git diff --check`
passed for the historical fixed-parent checkpoint. **Historical Step9C gate:
PASS. Step9D remains canceled.**

The statements above are historical checkpoint evidence. The current working
tree routes Mithardir through the randomized DL30–199 parent scheduler. The
final native, Release, package and non-PTY comparison gates pass on x64 and
Win32; the combined Step 9 commit, tag and push are the remaining publication
actions for this revision.
