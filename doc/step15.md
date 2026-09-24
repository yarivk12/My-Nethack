# Phase 1 Step 15 â€” forge foundation, recipes, inheritance and gemstone affixing

Step 15A implements forge terrain, natural placement, and war-hammer activation.
Step 15B adds the locked recipe catalogue, native menus, exact ingredient
allocation, and atomic crafting. Step 15C adds deterministic equipment-state
inheritance. Step 15D adds gemstone affixing and shared equipment effects.
There are no repairs, charges,
cooldowns, smithing skills, recipe learning, or persistent forge provenance.

## Step 15D implementation and validation (2026-09-19)

This section supersedes earlier format and deferred-affixing statements below;
those sections retain the historical Step 15A/B/C evidence.

### Implemented behavior

Applying a war hammer on a forge offers **Socket gemstone** beside crafting,
using the `s` shortcut. A completed socketing attempt can remain on the same
target: a failed attempt may ask `Try again?`, a successful fill with an empty
socket may ask `Socket another gem?`, and a successful full target may ask
`Replace a gem?`. Each prompt appears only when another valid operation exists
for a directly carried gemstone; container contents never enable continuation.
Answering `n` exits the socketing workflow, while `y` re-enters the appropriate
gem/socket selection without reapplying the hammer or selecting the target.
Existing activation gates, terrain, generation, and all 12 recipes remain.
Native menus select carried, unequipped equipment and a gemstone, then an
occupied socket only when full. The first empty socket takes precedence.
Artifacts, ammunition, weapon-tools, unpaid objects, the activating hammer,
and active equipment are excluded; inactive alternate weapons are allowed.
Inventory letters select items and IDs are revalidated at commitment.

All 22 real gemstone mappings are explicit, including black opal at T2.
T1/T2/T3/T4 chances are 70/60/50/40 percent, plus 10 for a blessed hammer or
minus 10 for a cursed hammer. Gem BUC has no effect. Exact chances appear only
when tier and hammer BUC are known. Unknown gems/glass remain selectable;
hidden glass or exhausted pools consume one unit with the same generic failure
as an ordinary failed roll, without identifying material or hammer BUC.
Identified glass and exhausted known tiers are rejected before commitment.
Uniform exact-tier candidate selection excludes inherent capabilities and
exact identities across retained sockets and ordinary enhancements. Different
tiers remain distinct; socket Trueflight is launcher-only.

**Approved stack correction:** failure filling an empty socket leaves the
entire target stack unchanged. Committed replacement first separates one
physical weapon and destroys only its selected old socket. Every replacement
failure leaves that socket empty, preserving the remainder. Success installs
one known result and its acquired value. Commitment consumes one gemstone and
one action; cancellation/rejection consumes neither. Full-pack preflight checks
every possible magnitude and merge, considering only a known tier or all tiers
when hidden. No affixing RNG occurs during preflight. Native `splitobj` also
uses `rnd(2)` for object-ID allocation, separate from affixing draws; RNG-tail
tests explicitly account for it.

The approved weapon, armor and jewelry catalogues are complete. Elemental dice
roll on hits, with separately resisted Primordial components. Numeric values
roll once at acquisition. Eight ordinary weapon STR/DEX properties use bits
25–32, approved names/dice, and equal catalogue participation under the existing
tier system. Ordinary armor, generation opportunities, bands, property counts,
and ordinary Trueflight eligibility retain their previous rules.

Effective attributes use active weapons, including actual dual wielding, and
normally worn armor/jewelry. Encoded STR is handled deliberately: 16 + 4 displays
20. Protection lowers AC. HP/Pw maxima use reversible applied-bonus ledgers,
never heal on equip, and clamp current values on removal. Level gain, polymorph,
new-form maxima, cloning and splitting exclude temporary bonuses when computing
underlying maxima. Repeated golem healing does not reapply equipment HP.
Monsters receive native binary, AC, combat and HP effects where meaningful.
Flight, breathing, see-invisible and telepathy changes invoke native transition
updates. Telepathy works sighted; Very Fast matches speed boots.

`doname` adds one physical `[used/actual-capacity]` marker while preserving
ordinary names and native annotations. **`#inspect`** is a free general command
with a native carried-inventory menu and text window. Its detail window uses
the following presentation contract:

    <full item name> [sockets] (equipped state if applicable)

    Quality:
        <known quality> - <recipient-specific impact>

    Enhancements:
        <known property> - <actual impact>

    Sockets: <used>/<actual capacity>
        Socket 1: <empty, unknown, or known property> [- <actual impact>]

The quality and property impacts are concise descriptions of the shared
mechanics, including recipient-specific quality behavior, confirmed-hit dice,
stored static values, native worn capabilities, and all three independent
Primordial components. Socket occupancy appears only in the single `Sockets:`
heading. Naming uses object copies; inspection performs no identification.
Per-socket knowledge is independent; full identification reveals all,
attributable effects reveal the relevant source, and bones/acquisition
forgetting preserves actual values. Source gemstone identity is never stored.

### `#inspect` certainty and impact display (2026-09-20)

The enhancement engine keeps actual properties and ordinary knowledge in
separate masks. Full identification already sets ordinary knowledge to the
complete property set allowed for that object, including known absence when the
actual property mask is empty. The earlier defect was only in the empty-list
wording in `doinspect()`: it treated zero visible actual properties as `none
known` even when the knowledge mask was complete.

The fix uses the shared allowed-property mask to distinguish complete knowledge
from an unknown or partial view. An unknown plain eligible item still reports
`none known`; a partially known item reports only known properties and their
impacts; a fully identified plain item reports definitive `none`; and a fully
identified enhanced item, including numeric values, reports its actual known
properties and stored values. Unknown quality remains `unknown`, and unknown
sockets remain `unknown` without revealing property, tier, value, gem identity,
or impact. Known Standard quality explicitly reports `no additional quality
bonus`. Socket knowledge remains independent, and `#inspect` continues to
render from object copies without mutating object or knowledge state.

The native Step 13 and Step 15 regressions cover Standard, Fine and Exceptional
quality impacts for melee/direct throw weapons, launchers, fired ammunition and
armor; runtime and static ordinary properties; binary ordinary and socket
properties; Primordial's three formulas; one- and two-socket equipment; empty,
unknown, partial and fully known states; the long-sword acceptance shape; naming
capitalization; and explicit nonmutation assertions.

Socket tier surcharges add to ordinary surcharges before quality, including
jewelry and hidden shop valuation. Crafting destroys consumed socket state and
creates full-capacity empty outputs. Ordinary STR/DEX inheritance selects the
highest contributed stored value per selected identity, without rerolling or
socket contributions. Output comparisons and preflight include these values.

### `#overview` coverage fix and custom-feature audit

The focused audit found one real omission: a known forge was not included in
`#overview`. The native overview remembers terrain through `svl.lastseentyp`,
recalculates feature state in `recalc_mapseen()`, filters levels through
`interest_mapseen()`, and formats the feature sentence in `print_mapseen()`.
Forge terrain had no case in that shared counting path, so it could be visible,
persistent, and usable while remaining absent from the overview.

The fix reuses the existing one-bit `mapseen_flags.spare1` slot as `forge`.
`recalc_mapseen()` clears it, `count_feat_lastseentyp()` sets it for remembered
`FORGE`, and the native interest and output paths include one singular
`Forge`. Multiple known forges remain one presence annotation. This preserves
the existing struct size and mapseen save/restore codec; no new field or format
epoch is introduced. Forge removal before recalculation also clears stale
overview state.

The project-wide custom dungeon audit used native overview semantics, not a new
custom annotation system:

| Feature family | Result | Native coverage or reason |
|---|---|---|
| Authored and natural forge terrain | Fixed omission | Persistent furniture now follows the same remembered-terrain path as fountain, throne, sink, grave, and tree. |
| Big Room, Oracle, Castle, Valley, Moloch's Sanctum, Fort Ludios, Sokoban, Rogue level, quest annotations, and vibrating-square state | Already covered | Existing `mapseen_flags` annotations and native overview branches print these states. |
| Shops, temples, altars, fountains, thrones, sinks, graves, and trees | Already covered | Existing `mapseen_feat` counts and `print_mapseen()` output remain authoritative. |
| Sheol, The Dragon Caves, Mithardir, Neutral Quest, The Lost Tomb, The Ruins of Moria, and The Lost Cities | Covered indirectly | Native dungeon headings and remembered branch or portal lines identify these dungeons; their ordinary terrain and content are not overview feature types. |
| Giant Court, Real Zoo, Dragon Lair, Wizard Study, Storeroom, Super Honeycomb, Dragon Hall, and Library | Not an overview feature | These are custom room/content identities. Native overview does not enumerate arbitrary room themes, and no separate line is justified by existing semantics. |
| Moria, Mithardir, Sheol, and other custom ordinary terrain, objects, traps, services, and merchants | Not an overview feature | They are level content rather than persistent overview furniture or native level annotations; branch and dungeon identity remain covered where applicable. |

The native runtime regression captures the actual overview menu and covers a
known authored forge, multiple known forges without duplicate text, and a
cleared level without stale forge text. It also round-trips the mapseen chain
through the native dungeon save/restore codec before reopening the overview.
Existing natural-generation and forge-persistence tests cover the equivalent
natural terrain and level transition paths. `test_step15_overview_source.py`
locks the reused flag and unchanged mapseen codec boundary.

Focused evidence: `PY test/run_step15.py --out
_qa/overview-native-20260919-c` passed the overview cases, all retained Step
15A/B/C/D native regressions, and the 1,000-level corpus. The production
`run_step15_save.py` and `run_step15d_save.py` sessions passed transitions,
save/restore, recovery, crafting, and affixing. The x64 Release solution build
and ZIP/DLB/resource checks passed afterward.

### Representation, ownership, and format

`struct obj` has actual capacity, two inline `{ property, value, known }`
records, eight ordinary value bytes, and 64-bit ordinary actual/known masks.
Canonical property IDs 1–32 refer to the ordinary catalogue; further IDs cover
socket-only entries. Bit 7 stays retired. Native x64 object size changes from
**112 to 144 bytes**. Hero HP/form-HP/Pw and monster HP applied-bonus ledgers are
inline in their existing serialized structures.

**EDITLEVEL 9 → 10** rejects previous development saves/bones, without migration.
Native object/hero/monster serialization carries state through save, level,
bones and checkpoint/recovery. Restore validates metadata deterministically
without equipment callbacks. Inert ordinary known-mask bits from the retained
low-byte codec corpus remain round-trippable; explicit normalization masks the
public valid range.

There is no separately owned socket allocation. Zeroed constructors and
`enhancement_created` initialize full empty capacity independently of ordinary
generation permission. Copies/splits copy inline state; merging compares every
field/value. Destruction needs no extra free. Same-type polymorph preserves
metadata; true type changes clear it and initialize new capacity. Artifact
conversion removes prior worn effects and clears capacity. Starting racial
substitutions, bones conversions and unseen acquisition use shared lifecycle
rules. Enchantment, BUC, erosion, coatings and names remain separate.

Primary code: `include/{obj,enhance,you,monst,patchlevel}.h`, `src/enhance.c`,
`src/apply.c`, and existing naming, command, inventory, attribute, wear/wield,
polymorph, monster, shop, restore and bones hooks. New tests are
`test/test_step15d*.c`, source/projection gates, and `test/run_step15d_save.py`;
the retained Step 13/14/15 harnesses execute them.

### Validation record

Starting checkout: `phase1/equipment-enhancement`,
`a4585f85f1acc56f489cf9dbc3eecc3c6a2679d5`, no tracked edits and untracked
`.codegraph/` preserved. Before edits,
`C:/Python311/python.exe test/run_step13.py --out _qa/step15d/baseline13`
passed the complete native suite and generation/codec corpora.

Commands below run from the repository root. `PY` is
`C:/Python311/python.exe`; `MSBUILD` is
`C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/MSBuild/Current/Bin/MSBuild.exe`.
Builds sharing generated headers must run sequentially.

| Executed command | Result |
|---|---|
| `PY test/run_step13.py --out _qa/step15d/step13` | PASS final run: 110,000 generation traces, 1,000,000 generated objects, 27,648 low-byte and 5,345,280 finalized codec states, combat/knowledge, shop/billing, level/bones, 959 new mixed socket/value codec states, and 1,280 socket combat/resistance/RNG outcomes. |
| `PY test/run_step15.py --out _qa/step15d/step15` | PASS final run: retained A/B/C, all 12 recipes, 480 deterministic affixing outcomes, glass/exhaustion, pack boundary, inheritance, five cancellation boundaries, recipient gates and four chance-disclosure states. The unchanged 1,000-level corpus has 116 selections/placements, zero invalid and zero duplicates; the seeded count changed with approved acquisition draws. |
| `PY test/test_step13_source.py`; `PY test/test_step14_source.py`; `PY test/test_step15_source.py`; `PY test/test_step15b_source.py`; `PY test/test_step15c_source.py`; `PY test/test_step15d_source.py`; `PY test/test_phase1_audit_source.py` | PASS. 15D frozen projection covers 26 production files and rejects 102 mutations; retained 15C rejects 4 and audit rejects 52. Historical manifests unchanged. |
| `MSBUILD sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal /nologo` | PASS x64 console, GUI, recover, resources/DLB and ZIP packaging. |
| `PY test/test_step11_resources.py _qa/step13-donors/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip` | PASS pinned donors, 199 protected files, exact packaged binaries/DLB, and 186 Lua resources. |
| `PY test/test_step9a_tiles.py`; `PY test/test_step9c_package.py binary/Release/x64/nhdat500` | PASS glyph/tile mapping and bitmap capacity; three resource manifests and packaged dungeon/quest bytes. |
| `PY test/run_step13_save.py _qa/step15d/step13/bin _qa/step15d/enhanced-save` | PASS full game save/restore and native recovery, socketed items across eight ownership paths and container/monster backlinks. |
| `PY test/run_step15_save.py binary/Release/x64 _qa/step15d/production-craft _qa/step15d/step13/bin` | PASS production menus, cancellation, one-turn crafting, named ingredients, two-craft chain, enhanced inheritance, level/save/recovery and diagnostic inspection of three production saves. |
| `PY test/run_step15d_save.py binary/Release/x64 _qa/step15d/step13/bin _qa/step15d/production-affix-final` | PASS byte-identical final Release executable: activation, STR 16 +4 =20 and removal, one-weapon committed replacement failure, successful affixing, free inspection, level/save/recovery, and diagnostic actual-state verification of three production saves. |

Older source gates were also run with pinned donors:
`PY test/test_step9a_source.py _qa/step13-donors/unnethack.git binary/Release/x64/nhdat500`,
`PY test/test_step9b_source.py _qa/step13-donors/unnethack.git binary/Release/x64/nhdat500`,
`PY test/test_step9c_mirage_source.py _qa/step13-donors/dnethack.git`, and each
`test/test_step10*source.py` (each invoked as `PY test/<filename>`):
`test_step10b2_1_source.py`, `test_step10b2_2_source.py`,
`test_step10b2_3_source.py`, `test_step10b2_4_source.py`,
`test_step10b3_1_source.py`, `test_step10b3_2_source.py`,
`test_step10b3_3_source.py`, `test_step10b3_4_source.py`,
`test_step10b4_source.py`, `test_step10b5_source.py`, `test_step10b_source.py`,
`test_step10c_b_source.py`, `test_step10c_c_source.py`,
`test_step10c_d_source.py`, `test_step10qa2_source.py`.
Explicit epoch assertions were updated to 10;
protected donor, scope and projection assertions were retained. All passed after
those explicit format expectations were updated.

Final release artifacts (SHA-256):

| Artifact | SHA-256 |
|---|---|
| `binary/Release/x64/NetHack.exe` | `667747ec10348b3e4e15e34bef3e8f51c83d5a51726b50ec6a4e1c71e162fa09` |
| `binary/Release/x64/NetHackW.exe` | `4653c79c537c2c14c88ad0da5828bbd79a7f65eef964bcd671569d195cfe25f2` |
| `binary/Release/x64/recover.exe` | `5cfd6fa4b58737ea079b299b4f99cf18ec032d91a4ed3d5d2aee7b3db63c5001` |
| `binary/Release/x64/nhdat500` | `0b30de5d278930e4eb18f61c3dce0cda6b2718ecbdd495c631a3a3c0321894b0` |
| `vspackage/nethack-500-win-x64.zip` | `2a66bd4904d2838b0fdbbf2dbcc914a90d3910cb9d70393363409d76524706d6` |

The ZIP/resource check was repeated after the final Release build. Full command
logs and isolated games are under `_qa/step15d/`; these are local ignored QA
artifacts. No staging, commit, tag, push, branch switch, reset, clean or stash was
performed. `git diff --check` passes; branch and HEAD remain the starting values.

Initial failures were corrected: obsolete mask/size/epoch/source expectations,
test split-linkage cleanup, a terminal transcript parser losing unchanged console
cells (replaced by native inventory-screen parsing), and a diagnostic compilation
racing Release regeneration of the shared Lua header (rerun sequentially).
The untouched baseline also emits existing `mkmap.c` C4701 x/y warnings.
No failing baseline test is claimed. GUI/manual gameplay has not been run;
production gameplay checks above are automated Windows terminal sessions.

Manual smoke path: start a fresh epoch-10 game; carry a war hammer, unequipped
eligible equipment and gemstones; stand on a forge; apply the hammer and choose
Socket gemstone. Use `#inspect`, fill and replace sockets, verify the destruction
warning, equip/unequip numeric gear and check reversible stats, then save/restore.
Native fixtures supply known states for otherwise random acquisition cases.

## Terrain and compatibility

`FORGE` is appended as terrain ID 45. It is accessible, transparent, ordinary
object-bearing furniture, with its own classification and descriptions. It is
not a fountain, sink, altar, wall, pool, or lava. The default display is an orange
`{`, consistent with the existing colored furniture symbols (white sink, blue
fountain); `S_forge` has a dedicated stone-hearth tile in `win/share/other.txt`.
The glyph, farlook description, `look here`, surface description, terrain-status
name, and wizard/Lua terrain names all identify a forge.

Forges are permanent and unlimited. Normal terrain replacement and trap creation
cannot overwrite them; digging, including object-caused holes, cannot destroy
them. They remain lit after dark Lua regions, magical darkness, and a Shadow's
post-move darkening. This is illumination of the forge square, not an added
radius-based light source. Ordinary objects can rest on it normally.

Lua uses the previously unused map character `f`, through both `des.map` and
`des.terrain({ x=..., y=..., typ='f', lit=0 })`. Forge lighting overrides `lit=0`.
Explicit placement is independent of depth, branch, probability, and natural
room restrictions, except that a forge may never exist inside a shop. Authored
placement on an already-trapped square is rejected: the trap, underlying terrain,
lighting, and square metadata remain unchanged. This applies to coordinate and
selection terrain requests, terrain replacement, and map fragments. Ordinary
non-forge terrain placement gains no new restriction; shop resolution is unchanged.
A standalone `des.finalize_level` does not select forges.

**EDITLEVEL 8 â†’ 9** rejects old saves and bones through the existing version gate.
Appending the terrain leaves existing real terrain IDs intact, but adding
`S_forge` before the final dungeon symbol shifts subsequent glyph identities,
including glyphs stored in map memory and mimic state. No `struct rm` layout,
object layout, level codec, recovery codec, level compiler, or serialized
scheduler is added or changed. Native level/save/recovery/bones paths carry the
terrain, lighting, glyph, and floor objects. Step 13/14 enhancement fields and
behavior remain unchanged.

## Natural generation

The production generation wrapper runs forge selection once, after room typing,
Lua postprocessing, ordinary/special filling, mineralization, and topology
finalization. Accepted bones bypass the wrapper. Standalone Lua finalization
only finalizes topology.

Eligibility is main Dungeons of Doom, absolute DL20â€“199 inclusive, excluding
special/authored identities, branch junctions, other dungeon branches, and
prototype/filler-authored levels. DL19 and DL200 are excluded. There is no
room/maze/cavern eligibility test. Each eligible level executes exactly one
`rn2(100) < 12` selection, with no quota, compensation, or retry.

After selection, reservoir sampling chooses uniformly among legal squares:

- Terrain must be exactly `ROOM`, with an interior ordinary `OROOM` owner.
- Exclude shops, temples, vaults, themed/special/custom rooms, irregular rooms,
  subrooms and their parent rooms, and room edges.
- Exclude authored map fragments and scripted rooms even if their type is
  `OROOM`. Lua's default random ordinary-room backend remains eligible; explicit
  geometry, lighting, unfilled rooms, or content callbacks mark a scripted area.
- Exclude stairs/ladders, corridors, doors, all traps including portals, water,
  lava, and every existing terrain feature.

A coordinate mask records scripted provenance only while generating the level;
it resets with the level structures and is never serialized. Coordinates remain
valid when room descriptors are sorted. This is placement metadata, not a
per-game feature scheduler.

At most one natural forge is placed. If a selected topology has no legal square,
selection succeeds but placement fails, without relaxing restrictions or rolling
again. `forge_generate` returns a transient outcome: excluded, missed selection,
selected with no candidate, or placed. Production needs no saved outcome state;
the diagnostic build captures it for invariant checks.

## Activation and extension point

Stand on a forge and apply the existing war hammer. The apply picker suggests
war hammers there; away from a forge the original picker and apply behavior are
unchanged. `forge_interact` is the single operation entry point. It validates the
terrain and object type, then rejects confusion, stun, or current Strength below
4. STR 4 permits activation. Ordinary, enhanced, cursed, eroded, negatively
enchanted, and artifact war hammers all use the same object-type rule.

The forge dispatch precedes artifact retouch/speech, preventing artifact
alignment/touch restrictions or side effects from becoming forge requirements.
Native apply hands/capacity preflight is retained. State rejections use
`ECMD_OK` (no time), following `do_break_wand`'s strength rejection and existing
`use_pole` preflight gates. Invalid entry-point terrain/type returns `ECMD_FAIL`.
Valid activation opens the native forge menu. Opening, browsing, inspection,
cancellation and transaction rejection return `ECMD_OK`, without a turn.
Only successful crafting or socketing returns `ECMD_TIME`; socketing can repeat
on the same target after that action without reopening the forge activation.
This supersedes Step 15A's temporary one-turn activation message.

## Step 15B catalogue and workflow

The action menu offers **Forge an item** and **Socket gemstone**. Categories are
always Weapons, Armor, Tools, Other; categories with recipes show the number of
distinct currently forgeable recipes, and recipe-less categories remain
nonselectable. Category derives from the output's native object class, with
weapon ammunition under Weapons. Recipes are known from the beginning and list
true output/ingredient names and complete quantities. Currently forgeable
recipes appear first with contiguous selection letters. Remaining recipes are
shown afterward under `Not available:` without selection letters, availability
tags, or selection IDs. The heading is omitted when every recipe is forgeable.
No inventory object is identified by reading a recipe. The category count and
lettered recipe rows use the same `forge_available()` result, and a zero-count
category remains inspectable through its unlettered unavailable rows.

The central `forge_recipes` table in `src/apply.c` has one output type and exactly
two fixed ingredient slots, each an exact type and positive quantity. There is
no output-quantity field: each craft makes one object. Catalogue validation
rejects invalid, unique/protected and progression-slab types, nonpositive counts,
and duplicate unordered input formulas. Same-type quantities are aggregated for
duplicate checking; different formulas may share an output. Invalid static
catalogues are development errors (`panic`), not random crafting failures.

The curated order groups the sword chain, other weapon conversions, then the
armor progression:

| # | Output | Ingredients |
|---|---|---|
| 1 | katana | 2 long swords |
| 2 | two-handed sword | long sword + broadsword |
| 3 | tsurugi | katana + two-handed sword |
| 4 | battle-axe | axe + broadsword |
| 5 | dwarvish mattock | pick-axe + dwarvish short sword |
| 6 | trident | scimitar + spear |
| 7 | athame | dagger + stiletto |
| 8 | runesword | broadsword + dagger |
| 9 | chain mail | 2 ring mails |
| 10 | splint mail | scale mail + chain mail |
| 11 | plate mail | splint mail + chain mail |
| 12 | elven shield | elven dagger + small shield |

There are no object imports. The engine permits other normal classes in future
explicit recipes. Importing a future object must separately decide its source
policy (normal generation, forge-only, or another source); it must not implicitly
enable random generation or bring in unrelated donor content.

## Ingredients, knowledge and allocation

Only top-level carried objects participate. Floor and contained items are never
candidates. Base-type knowledge uses native `dknown` and `oc_name_known` gates;
unknown objects contribute neither availability nor diagnostic reason counts.
The same inspection text is produced when an unknown matching item is absent.
Inspection reports required and eligible amounts and known exclusions.

Exclude all equipped slots, the activating hammer, artifacts, unique/progression
objects and Rider corpses, unpaid objects, non-empty containers, in-use items,
cursed loadstones and attached leashes. Native `is_unpaid`, `Has_contents`,
equipment flags and `canletgo` supply the rules. The side-effecting cursed
loadstone/welded branches of `canletgo` are guarded before invoking it, preserving
knowledge on cancellation. Ordinary cursed unequipped weapons remain usable.
Quality, enhancements, enchantment, BUC, material, erosion, names, poison and
grease do not affect exact-type matching.

Native `NHW_MENU`, `PICK_ONE` and menu counts let the player choose each exact
stack and contribution. A slot can take several contributions; an overlarge
count is rejected and reprompted. Without an explicit count, a selected stack
contributes up to that slot's remaining requirement. Even a sole candidate must
be selected. One stack can supply both same-type slots without double-counting.
Normal inventory names are formatted on a display copy with observation
suppressed, so selection does not change the original objects' knowledge.

Transient descriptors contain object IDs and two allocated quantities. Selection
never splits, unlinks, consumes, reserves or changes inventory quantities. Final
confirmation lists the output and every exact stack, inventory letter and
combined consumed quantity, with explicit Yes/No choices.

Navigation: cancel action/category exits; cancel recipe list returns to
categories; cancel allocation, decline confirmation or inspect an unavailable
recipe returns to that category's recipes. Every such path is free and preserves
inventory. A successful craft exits immediately and consumes one normal action.
The activating hammer's state is unchanged.

## Step 15B transaction foundation (historical output policy)

The transaction design below remains in use. Step 15C, specified next,
supersedes only the completed result's plain-state policy.

After confirmation, preflight checks allocation IDs, types, uniqueness, bounds
and slot totals; it does not rerun ingredient eligibility. Output construction
precedes consumption. Pack acceptance uses the native `inv_cnt(FALSE)`,
`invlet_basic` and `mergable` rules, subtracting fully consumed non-coin stacks
and testing merges only against surviving objects. At a full pack, a released
slot or compatible surviving output stack permits crafting. Otherwise the free
output is destroyed and ingredients remain intact, with no turn spent.

Commit reacquires each unique object ID immediately before consumption, uses
`useupall` for exhausted stacks and the native `useup` quantity/weight pattern
for partial stacks, then `addinv` for the prepared output. It never follows a
freed object pointer or uses floor fallback. Normal inventory merging, weight
and burden updates apply; no forge carrying-capacity subsystem exists. Native
allocation failure is fatal globally, but the diagnostic construction-failure
hook proves that an unexpected null output aborts without consumption.

`mksobj(type, FALSE, FALSE)` supplies canonical native default initialization.
The existing enhancement context is temporarily set to `ENH_CONTEXT_NONE` and
restored because even the non-random constructor reaches `enhancement_created`.
No enhancement fields/extensions are fabricated or cleared by forge code.
The result inherits no ingredient state: no quality, properties, enchantment,
BUC, material, erosion, names, poison or grease. `makeknown` and `dknown` expose
the base type only; full identification is not called. The result is an ordinary
non-artifact item usable in later recipes, with no saved forged flag.

No persistent state, object layout or save codec changed in 15B. `EDITLEVEL`
remains 9 from 15A. Real equipment-state inheritance is exclusively Step 15C;
gemstone affixing is exclusively Step 15D.

## Step 15C finalized inheritance contract

Every physical object with a positive combined consumed allocation contributes
its actual state, including partially consumed stacks. Unselected matching
inventory, contained items, and the activating hammer do not contribute. A
shared stack contributes once even when it supplies both requirements. There
is no primary donor or additional identification gate. Quantities have no
weight: inventory/allocation/ingredient order and splitting equivalent amounts
across identical-state stacks cannot change the result.

The unchanged `forge_output` calls `mksobj(type, FALSE, FALSE)` under
`ENH_CONTEXT_NONE`, restores the exact previous context, and marks only base
type visibility. That fresh object is **not** a contributor. `forge_gather`
captures only transient scalar values and a property mask; `forge_inherit`
finalizes the supported categories before capacity preflight:

| Actual state | Reduction |
|---|---|
| Quality | Highest Standard/Fine/Exceptional rank |
| Properties | Union exact identities; filter target legality; tier descending, catalog-order ties; first two |
| `spe` | Highest signed value, subject only to existing native safety limits |
| BUC | Blessed > uncursed > cursed |
| `oeroded`, `oeroded2` | Independent lowest numeric severity |
| Erosion-proof | Logical OR |

Initialization comes from the first consumed object. Thus two cursed inputs
remain cursed; -5/-2 produces -2; two severity-1 contributors produce severity
1. Proofing is independent of severity. Source damage descriptions do not
transfer: an iron input's rust severity can become wooden output burn severity.
No damage-type conversion, quantity averaging, property fusion, tier promotion,
replacement roll, generated enhancement, or forge-specific balance cap exists.

Property legality is tested before selection. Rejected high-tier candidates
cannot hide legal lower-tier candidates, and rejecting every candidate still
permits crafting. `enhancement_catalog` supplies identity, tier and order;
`enhancement_property_allowed` supplies shared target legality, followed by
`enhancement_set(..., FALSE)`. The finalized engine has no opposing-element or
pairwise exclusions beyond exact identity deduplication and the two-property
limit. Distinct tiers of the same element, Primordial plus another weapon
property, and Magic Resistance plus Reflection remain legal.

### Source-backed target capabilities

These checks operate on the fresh output's actual type and native default
material. Unsupported fields retain constructor defaults, including unrelated
native meanings of `spe`; the forge does not widen eligibility.

| Category | Existing native/shared rule used in `forge_inherit` |
|---|---|
| Quality/properties | `enhancement_eligible` and `enhancement_property_allowed` in `src/enhance.c`: non-artifact weapons/armor only; Trueflight uses native launcher/ammo/missile/spear classification; armor filters primary and secondary native powers (blue/white dragon armor, alchemy smock, chromatic armor). Weapon-tools remain ineligible for generic enhancements. |
| Enchantment `spe` | `include/obj.h`'s native meanings and `src/objnam.c:readobjnam`: weapons, armor, `is_weptool`, charged rings. Signed values retain the full native `[-SPE_LIM, SPE_LIM]` range (`SPE_LIM=99`), not generation/wishing balance ranges. |
| Charge `spe` | Wands and `TOOL_CLASS` with `oc_charged`, after weapon-tool handling; `src/read.c:charge_ok` and `readobjnam` distinguish charge state from age-based lamps. `include/obj.h` defines the charge floor as -1 for wands/charged tools; ceiling is `SPE_LIM`. Non-wizard wishing limits and cancellation defaults are not hard bounds and are not copied. No recharge roll or recharge-count inheritance. |
| Other `spe` meanings | Preserve fresh defaults for noncharged rings, lamps, statues/figurines/corpses (gender), fruit/tins/eggs, containers, towels and other unsupported types. Field existence alone grants no capability. |
| BUC | Native `bless`/`curse` in `src/mkobj.c`; coins are excluded by those setters. Fresh uncursed output remains uncursed for an uncursed reduction. Setters maintain native BUC-dependent behavior. |
| `oeroded` | `src/objnam.c:readobjnam` gates: `erosion_matters` and flammable, rustprone or crackable (glass armor). |
| `oeroded2` | `erosion_matters` and corrodeable or rottable. |
| Proofing | `erosion_matters` and (`is_damageable` or `CRYSKNIFE`), preserving the native fixed-form crysknife exception. |

`erosion_matters` supports weapons, armor, weapon-tools, balls and chains;
material predicates live in `include/objclass.h` and `src/mkobj.c`. Consequently
copper armor supports secondary corrosion without primary damage, glass armor
supports primary cracking without secondary damage, and silver weapons support
neither. Merely metallic non-weapon tools do not inherit erosion/proofing.
The generic recipe engine still accepts permitted normal classes; test-only
recipes exercise rings, wands, tools, food, potions, statues and coins without
adding production recipes.

### Allowlist, knowledge and transaction boundary

Everything else retains fresh-output state. No material override, individual
name, coating/poison, grease, input knowledge, worn/in-use state, inventory
letter, identity, ownership/billing state, age/timer, runtime flag, linkage or
future field is copied. Native creation and insertion assign legitimate output
identity and inventory bookkeeping. Actual inheritance is identical for unknown
and fully identified inputs. Normal Step 15B `dknown`/`makeknown` base knowledge
remains; hidden enhancement, enchantment, BUC and proofing knowledge does not
transfer. Normal later identification/observation, including hero-use-only
elemental observation, remains unchanged.

`forge_commit` revalidates the confirmed ID/type/quantity ledger and combined
per-object consumption first. It then captures all contributor values, creates
and finalizes a temporary output, and checks native capacity/merge compatibility
against surviving inventory. `inv_cnt(FALSE)` and `invlet_basic` remain native;
only completely consumed non-coin objects release slots. `mergable` is read-only
and currently never reads quantity, so residual quantities affect survival but
require no temporary object/inventory mutation. A fully consumed object cannot
serve as a surviving merge target. Actual **and knowledge** state participate
in the finalized output comparison; the forge never clears knowledge to merge.

Only after preflight succeeds does the original consumption/`addinv` path run.
There are no donor pointers in the reduction record, no donor reads after
consumption, no preflight splitting/relinking/merging, no cloned inventory graph,
and no floor fallback. Cancellation/rejection remains free and non-mutating;
success consumes exact confirmed quantities and one turn. Inheritance/filtering
consume no RNG. Constructor RNG remains native, with exact outer-context
restoration. Native first-time base discovery can exercise Wisdom and consume
its existing RNG draw; constructor/inheritance RNG checks isolate that behavior.

No recipe, object/catalog identity, natural-generation rule, shared mechanic,
object layout, save codec or version epoch changed. `EDITLEVEL` remains 9.
Current-format persistence uses the normal object fields with no provenance or
inheritance serializer. Step 15A and Step 13/14 behavior remain protected.

### Step 15C validation (2026-09-19)

Baseline: `phase1/equipment-enhancement`, HEAD
`439f459b1afbe42fa7118874d4e22f772229d7e9`. Evidence is separately recorded under
`_qa/step15c/`; all earlier 15B/audit evidence below is historical and retained.

Commands use `C:/Python311/python.exe`. MSBuild was located through `vswhere`
at `C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/MSBuild/Current/Bin/MSBuild.exe`.

| Command/suite | Step 15C result |
|---|---|
| `test/run_step15.py --out _qa/step15c/step15` | PASS all 12 production recipes; 1,880 new cases: 1,591 quality/BUC/signed-spe/erosion/proof reductions, 42 property selections, 115 target capabilities, 48 equivalent allocations/knowledge/allowlist snapshots, 16 finalized-output capacity cases, 64 zero-inheritance-RNG/context cases plus 4 construction-failure context controls. Later natural construction yields 7 enhanced objects in 100 fixed-seed controls. |
| Retained Step 15B native coverage in the same runner | PASS 192 independent allocation cases (105 successes), 16 original capacity cases, 40-stack selection/count/cancellation, construction/allocation failure hooks, navigation, and 2,000 plain constructors across all contexts. Only completed-craft plain-output expectations changed. |
| Retained Step 15A coverage in the same runner | PASS terrain, activation, trap/shop/light/permanence/provenance and level/bones fixtures; unchanged 1,000-level corpus: 114 selections/placements, no invalid/multiple placements. |
| `test/run_step13.py --out _qa/step15c/step13` | PASS full affected enhancement/knowledge/combat/naming/value/lifecycle/codec suite: 108 native observation impacts, 5,724 price checks, 110,000 exact generation/RNG traces, million-object corpus, 5,345,280 finalized codec states plus 27,648 earlier cases. Large corpora ran because the authoritative aggregate includes them, not as added volume. |
| `test/test_step{13,14,15,15b,15c}_source.py`; `test/test_phase1_audit_source.py` | PASS representation and shared-code identity; recipe order; capture/finalization/preflight/RNG boundary. New exact projection rejects 4 mutations; unchanged audit projection rejects 52 mutations across 14 files. Original historical manifests/baselines remain immutable. |
| `MSBuild.exe sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal /nologo` | PASS console, GUI, utilities, resources, tiles, DLB and ZIP. First overlapping build hit a shared `hacklib.lib` lock; serialized retry passed. Both logs retained. |
| `test/test_step11_resources.py _qa/step13-donors/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip` | PASS 199 protected files, 186 Lua resources, exact packaged console/GUI/DLB bytes and pinned donor definitions. |
| `test/test_step9a_tiles.py`; `test/test_step9c_package.py binary/Release/x64/nhdat500` | PASS 2,868 tiles, 640x1574 bitmap capacity, unchanged forge artwork, Mithardir/dungeon/quest packaged data. |
| `test/test_step9{a,b}_source.py _qa/step13-donors/unnethack.git binary/Release/x64/nhdat500`; `test/test_step9c_mirage_source.py _qa/step13-donors/dnethack.git`; Step 10 source scripts | PASS pinned donors, protected mechanics and historical projections. Initial missing-argument invocations were corrected and rerun with the pinned local donors. |
| `test/run_step13_save.py _qa/step15c/step13/bin _qa/step15c/enhanced-save` | PASS full-game save/restore and native checkpoint recovery across eight enhanced ownership paths. |
| `test/run_step15_save.py binary/Release/x64 _qa/step15c/production-reviewed _qa/step15c/step13/bin` | PASS final byte-identical production executable: explicit named selection, free cancellation, one-turn success, inherited +4 katana-to-tsurugi chain; cursed -2, severity-1/1, proofed Exceptional Fire I/Primordial athame from controlled enhanced inputs; level transition, save/restore, native `recover.exe`, and three independent diagnostic save inspections with hidden knowledge preserved. |
| `git diff --check`; baseline/index/package hash checks | PASS; branch/HEAD unchanged, original index SHA256 `84a944c856f0fcad3617da57ce45d6eb85a39f546ee8652a6bfab83951831017` preserved, no staged changes. `workspace-package-check.json` records final package hashes. |

The enhanced dagger originates in an explicitly diagnostic-created current-format
save (`STEP13_GAME_FIXTURE` with `STEP15C_GAME_SEED`), not natural acquisition.
Subsequent menu/gameplay commands run the unmodified production executable.
Separate save copies after crafting, level traversal, and checkpoint recovery
are restored by the diagnostic executable with `STEP15C_GAME_CHECK`, which checks
actual fields and knowledge without identifying or modifying the forged items.
The fixture-only additions were incrementally rebuilt after the full Step 13
run; the production save checks exercise that final fixture binary. The live
terminal checks use Lua's existing native-field view; no production debug API
or enhancement-field export was added.

The first updated completed-craft assertion failed on Step 15B's plain output,
then passed after production integration (`red-plain-output.log`). A new RNG
test initially included first-time discovery's native Wisdom draw; its corrected
setup pre-discovers the synthetic target before comparing constructor RNG.
Independent inheritance-only tail checks remain separate. No production RNG
behavior was changed to satisfy the fixture.
Review also removed MSVC qualifier/signedness warnings and verified the charge
lower bound directly against `obj.h`, rather than applying the narrower
non-wizard wishing limits. The final native suite, source/projection gates,
Release build/package checks and production terminal/persistence replay passed
after these corrections. Logs from earlier runs remain separate.

Historical exceptions remain Step 7's `src/mklev.c` and Step 8A's
`include/dungeon.h` identity mismatches; projected current bytes equal projected
Step 15B HEAD bytes for both. The documented Win32 GUI startup residual is
outside scope: GUI build/package success is not visual gameplay validation.
Existing `mkmap.c` uninitialized-variable and unused-parameter build warnings
remain. No Step 15C production warning, contract conflict or new persistent
representation is introduced.

Production changes: `src/apply.c`. Regression/integration changes:
`test/test_step15b.c`, new `test/test_step15c.c`, `test/run_step15_save.py`,
`test/test_step13_runtime.c` (diagnostic fixture/restore inspection only), and
the narrow Step 15C projection/source gates with audit-projection composition.
Documentation: this file and `doc/phase1.md`. The pre-existing index and local
`.codegraph/` state are preserved; Step 15C remains unstaged, uncommitted and
unpushed for review. No blocker or contract deviation remains; historical
exceptions and the GUI manual residual above are not counted as passing gates.

## Shop invariant

The terrain setter rejects forge placement inside existing shops. Lua map
placement checks before clearing room membership, including irregular shops.
Lua region completion and final level topology remove a pre-existing forge if
the area subsequently becomes a shop: shop designation takes precedence and
the invalid forge square becomes ordinary room terrain. This narrow generation
repair is the exception to forge permanence. Rectangular shop geometry is
checked even before topology assigns room numbers. Forging never bills goods.

## Historical audit through Step 15B

The [Phase 1 audit through 15B](phase1-audit.md) records the earlier validation,
confirmed corrections and finalized closeout contracts. It adds exact
allocation/capacity ledgers, native count/quantity boundaries, stronger terrain
and provenance tests, and production repeated crafting. The original evidence
below is retained as a historical snapshot; current commands and counts are in
the audit report.

## Step 15B preaudit validation

- `python test/test_step15b_source.py`: exactly the locked 12 formulas and curated
  order, native transaction integration, no splitting or floor fallback.
- `python test/run_step15.py --out _qa/step15b-diagnostic`: all 12 crafts; default
  output despite altered inputs; duplicate formula rejection; unknown/absent
  diagnostics; eligibility exclusions; exact multi-stack counts; selection and
  cancellation nonmutation; native menu/back paths; confirmation; capacity
  rejection, merging and freed slots; invalid-allocation and construction faults;
  2,000 outputs without enhancements under an active natural-generation context;
  reforging; Lua shop exclusion; and all affected 15A activation/level/bones gates.
- Its unchanged 1,000-level corpus produces 114 selections and 114 valid forge
  placements, zero invalid/multiple placements, as before 15B.
- Step 13/14 structural and native regressions pass: 50,000 exact RNG traces,
  one million generated objects, stacking, knowledge, billing, combat, names,
  and 5,345,280 finalized persistence states. Step 15's protected enhancement
  and codec source-identity gate remains unchanged and passes.
- Authoritative x64 Release solution build passes, including console, GUI,
  utilities, tiles, DLB and `vspackage/nethack-500-win-x64.zip`. Resource validation
  confirms 199 protected files, 186 packaged Lua resources and exact ZIP equality
  for the freshly built EXEs/DLB. Existing `mkmap.c` and unused-parameter compiler
  warnings remain; forge production and test code add no warnings.
- `python test/run_step15_save.py binary/Release/x64
  _qa/step15b-production-2` passes through the real production terminal: named
  +4 and -3 swords explicitly selected, exact final confirmation, one plain
  katana, zero-turn cancellation, exactly one turn on success, level transitions,
  full save/restore and native `recover.exe` checkpoint recovery. Lua-created
  ingredients are first viewed in inventory, since `u.giveobj` bypasses native
  pickup observation and does not set `dknown` by itself.
- The current-code `STEP11_TEST` x64 build and focused Step 11/12 suite pass:
  selectors, every custom feature, recurrence, coexistence with shops, room
  metadata, level/bones codecs, and clean/partial generation failures.

Evidence: `_qa/step15b-diagnostic`, `_qa/step15b-enhancement`,
`_qa/step15b-release-build.log`, `_qa/step15b-production-2`,
`_qa/step15b-step11-focused`. No implementation blocker remains. The production
terminal path is automated; interactive GUI gameplay remains a manual review
item, as requested.

15B changes are in `src/apply.c`, forge/shop placement integration in
`src/mklev.c`, `src/mkmaze.c`, `src/sp_lev.c`, declarations in `include/extern.h`,
`test/test_step15b.c`, `test/test_step15b_source.py`, the updated Step 15 native
and production terminal fixtures, and this document. All changes remain
uncommitted/unpushed. The existing untracked `.codegraph/` is preserved.

## Step 15A historical validation evidence

The dedicated `STEP15_TEST` executable links the real engine, native RNG,
apply dispatch, Lua parser, and codecs. It is separate from production builds.

- `python test/test_step15_source.py`: integration wiring, contiguous symbol IDs,
  epoch 9, and exact protected enhancement/codec identity against the pre-Step-15
  commit.
- `python test/run_step15.py`: depth/branch/authored boundaries; every possible
  selection draw and native RNG tail on a no-room level; all terrain/room/trap
  exclusions; actual scripted Lua `OROOM` exclusion; map/glyph/look/status;
  permanence, darkness/Shadow lighting, objects; actual apply gates, timing,
  ordinary/enhanced/artifact hammer nonmutation; native level and accepted bones
  roundtrips; no selection from standalone Lua finalization.
- `C:/Python311/python.exe test/run_step15_save.py binary/Release/x64
  _qa/step15-production-final`: production Lua map placement, walking onto a forge,
  applying the hammer, departure/return through real level files, full save/restore,
  and `recover.exe` checkpoint recovery. Passed.

### Exactly 1,000 eligible generation cases

The corpus runs in a fresh process, separate from other fixtures, using seeds
150000â€“150999 (display seeds 160000â€“160999). Requested depths cycle over 20â€“199;
known excluded identities advance to the next eligible depth before generation.
The fixture fixes the deferred Ludios junction at DL18 so a sampled ordinary
level cannot become a branch junction partway through generation. No forge
selection or placement is forced. Results are aggregated only.

| Measure | Result |
|---|---:|
| Cases generated | 1,000 |
| Successful 12% selections | 114 |
| Successful placements | 114 |
| Selected with no legal square | 0 |
| Observed selection rate | 11.4% |
| Observed placement rate | 11.4% |
| Invalid placements | 0 |
| Levels with more than one natural forge | 0 |

No-placement behavior is covered deterministically; this corpus did not happen
to encounter a selected case without a candidate. Observed rates are a sanity
check, not an assertion that the sample must equal 12%.

### Regressions and Windows release

- Step 13/14 source and native diagnostics pass, including 50,000 exact RNG
  traces, the unchanged million-object enhancement corpus, combat/name/price/
  stacking/transform tests, 5,345,280 finalized codec states, and epoch-8 rejection.
- `C:/Python311/python.exe test/run_step13_save.py
  _qa/step15-enhancement/bin _qa/step15-enhanced-save` passes full-game save,
  restore, and checkpoint recovery for seven enhanced-object ownership chains.
- The Step 11/12 focused suite passes: all features, recurrence, coexistence,
  room identities, actual level/bones codecs, clean and partial failures.
- Existing production terrain tests pass for bog freeze/thaw/evaporation, tree
  conversion, boulders, ice/crystal walls and picks, shallow water, soil and grass.
- Step 9 donor/source/resource gates and Step 10 source gates pass.
- Two historical whole-file gates retain their documented pre-existing failures:
  Step 7 `src/mklev.c` and Step 8A `include/dungeon.h`. Their projected current
  bytes equal projected pre-task HEAD bytes; no assertion was weakened.
- Existing tests' explicit epoch assertions now expect 9. Historical scope tests
  remove only frozen exact forge hunks in `step15_historical_changes.json` before
  their existing projections. The manifest is never generated during tests.
- Authoritative `MSBuild.exe sys/windows/vs/NetHack.sln
  /p:Configuration=Release /p:Platform=x64 /m` passes, including console, GUI,
  tile generation, utilities, DLB, and `vspackage/nethack-500-win-x64.zip`.
- Resource validation confirms 199 protected files, 186 packaged Lua resources,
  and exact ZIP equality for console EXE, GUI EXE, and DLB.
- Remaining compiler warnings are the previously documented unused `trop`/GUI
  `fmt` parameters and `mkmap.c` potentially uninitialized coordinates.

Local evidence is under `_qa/step15-diagnostic` (`runtime.log`, `corpus.log`),
`_qa/step15-production-final`, `_qa/step15-enhancement`,
`_qa/step15-step11-focused`, `_qa/step15-regressions`,
`_qa/step15-release-build.log`, and `_qa/step15-resources.log`.

## Changed areas and review status

- Representation/display: `include/rm.h`, `include/defsym.h`,
  `include/patchlevel.h`, `src/display.c`, `src/botl.c`, `src/cmd.c`,
  `src/dungeon.c`, `src/invent.c`, `win/share/other.txt`.
- Generation/Lua/permanence/light: `src/mklev.c`, `src/mkmaze.c`, `src/nhlua.c`,
  `src/sp_lev.c`, `src/dig.c`, `src/read.c`, `src/monmove.c`.
- Activation/interface: `src/apply.c`, `include/extern.h`.
- Diagnostic build: `sys/windows/windmain.c`,
  `sys/windows/vs/NetHack/NetHack.vcxproj`.
- New tests: `test/test_step15_runtime.c`, `test/test_step15_source.py`,
  `test/run_step15.py`, `test/run_step15_save.py`; historical projection manifest
  and epoch-only updates to existing gates, including native version rejection.
- Documentation: this file. Steps 13/14 remain authoritative for enhancements.

The pre-existing untracked `.codegraph/` content is preserved; there were no
pre-existing tracked modifications. Build
outputs and local evidence are ignored. Manual gameplay and GUI tile appearance
remain useful review items; no implementation blocker is known.
