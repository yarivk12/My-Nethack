# Step 9A–9C wizard verification report — historical validation record

Historical status: Step9A, Step9B and Step9C automated gates and user manual
validation passed at temporary DoD108/109/110 parents. The production code now
randomizes all three parents persistently in DL30–199. The final x64/Win32
Release, native-topology, focused regression, package and non-PTY Mithardir
matrix passes; [step9.md](step9.md) records the authoritative closeout
evidence. Step9D was canceled before production integration, and DL111 is not
reserved.

The ordered entries below preserve findings, fixes and test outcomes from the
implementation process. Earlier pending-work and publication restrictions are
historical and superseded by the current status above; failed or incomplete
runs are not counted as successful validations. No four-branch completion is
claimed for the canceled scope.

Checkpoint publication found one validation-coverage issue: `git diff --check`
before staging did not inspect new map files. The staged check flagged
intentional trailing ASCII map cells in nine files. A path-specific Git
whitespace rule now preserves their exact validated bytes, while the source
test rejects trailing whitespace outside map literals. Staged checks and
source/package checks were rerun; no gameplay or map data was changed.

## Method

Release executables run in actual wizard-mode Windows TTY sessions, driven by
the test scripts under `test/`. Each run copies the relevant architecture's
Release directory to a new external fixture directory. Saves, panic logs,
terminal transcripts, generated arenas, build output and donor exports stay
outside the repository in `../_qa/step9-audit`.

Traversal fixtures inspect generated content before removing combatants to
isolate connector and save integrity. Separate encounters exercise combat and
terrain; wizard slaying tests the real death/drop path without claiming a
normal-character balance evaluation. These are automated wizard playtests,
not a claim that a human has inspected every tiled interaction.

## Findings and fixes so far

| Finding | Cause and correction | Verification |
| --- | --- | --- |
| Sheol filler could become a fallback maze | A one-cell Lua map at x=0 is outside the local map API's legal coordinates. Use x=1; preserve the donor full-map coordinate frame and middle-map offset. | Actual generated terrain checks, traversal, and 192 donor/local native-generator seed comparisons on each architecture |
| Crystal pick could not break palace crystal | The generic nondiggable-wall check ran before the crystal-pick exception. Preserve the donor exception for crystal ice specifically. | Actual ordinary-pick, crystal-pick and magic-dig arena tests, including nondiggable palace-style crystal |
| Native dragons absent from Sheol eligibility | Donor global dragon renaming prevented name matching. Recognize native dragon-class equivalents without importing global renaming or changing dragons outside Sheol. | Focused production generation tests and full donor definition/boundary checks |
| Ice restraint survived engulfing | Missing donor release path. Engulfing now breaks the ice independently of ordinary traps. | Source audit against pinned donor; final encounter coverage pending |
| Frozen status could stay stale | Restraint creation, movement expiry and fire thaw must request status refresh. | Focused creation assertion and live freeze/save/teleport encounter; expanded thaw checks pending |
| Weeping angels lacked two donor interactions | Add mutual-gaze petrification and defensive scroll restrictions. | Pinned source comparison and successful compilation; focused encounter coverage pending |

## Test-fixture corrections

These fixes change test control, not gameplay:

- Windows console redraws can leave a stale `--More--` prompt. Repeated quick
  spaces answered the following restore question prematurely; its intended
  `n` became a movement command. Prompt pacing now prevents that extra input.
- TTY output omits unchanged spaces and overwrites earlier Lua output. The
  reader reconstructs cursor-position gaps and retains the command transcript.
- Lua `nh.pline` does not implement printf formatting. Probe scripts now use
  `string.format` where numeric output is required.
- Intrinsic wizard teleport consumes 100 nutrition even with ordinary hunger
  disabled. Long stair-traversal fixtures now eat ordinary food before starving.
- NetHack 5 refuses an unforced rest beside a hostile monster. Combat fixtures
  use `m.` to allow the intended attack.
- Lua terrain edits must establish the map coordinate frame before calculating
  offsets. The terrain arena now initializes that frame explicitly.
- A fire beam can melt both walls on its path. The independent crystal-dig test
  now restores its crystal wall before checking magical digging.
- The ledger fixture still expected EDITLEVEL3. Its assertion now requires the
  combined Step 9 EDITLEVEL4; capacity and rejection checks remain unchanged.

## Recorded successful runs

- Sheol x64 six-level traversal: complete descent and return to DoD108, with
  representative filler and terminal save/restore.
- Actual fire melting, magical-dig immunity, pick restrictions, and persistent
  changed terrain/inventory in the x64 wizard arena.
- Blue-slime contact attack, movement restraint, save/restore and teleport
  release in the x64 wizard arena.
- Executioner unique birth, alive/dead save state, real death/equipment drops,
  and seven chests/two crystal picks/marker. Actual melee damage also passes.
- Four-map Lua conversion: 256 seeds each, geometry, content counts and
  conditional probability checks. Native generator: 192 donor/local comparisons
  on each architecture.
- Fresh-game prior topology remains variable and legal on both architectures;
  Step 9's four temporary depths are reserved before prior enrichment.
- Prior Lost Tomb and Temple traversal/save checks, all ten Moria map variants.
- Depth and ledger/recovery checks pass on both architectures, including high
  ledger boundaries and rejected invalid/old-version checkpoints.
- Latest x64 and Win32 cumulative focused/source/depth/ledger/recovery suites
  pass, as do both Release console/tiled solution builds and both DLB checks.
- Complete six-level Moria round trip returns to the sampled DoD176 parent.
- Final four-game topology samples per architecture pass Sheol DoD108 and all
  prior random placement/reservation checks. The verifier was corrected to
  accept indented TTY columns, then rerun on the original game captures.
- Generated tile names/glyph indices and bitmap capacity pass; bitmap visual
  inspection found no corruption. Reused artwork is listed in `step9a.md`.

**Step9A gate passed.** The entire requested goal is still in progress.

## Step9B findings and evidence in progress

Actual fresh-game startup exposed a production loader capacity problem: adding
the four Dragon Caves prototypes exceeded the old 50-entry temporary table.
The table now holds 128 prototypes, and both level and branch bounds checks run
before writes. This changes no saved structures or ledger/depth limits.
Both architectures subsequently completed the four-level traversal and returned
to DoD109, including entrance and terminal save/reload checks.

Actual wizard object creation exposed a second production issue: the native
Caveman boss's name prefix consumed the imported chromatic armor names during
wish parsing. The two exact armor names now resolve before monster-prefix
parsing, with non-wizard restrictions still applied. The native boss and role
quest map remain unchanged. Live chromatic armor tests have exercised all ten
powers, enchanting scales into mail, cancellation back into scales while worn,
save/reload and removal. Glowing armor's light and conversions worked; the final
removal check is being rerun after correcting the harness's extra inventory
letter following NetHack's automatic selection of the sole worn armor item.

Additional passing evidence:

- Four fresh games per architecture preserve the fixed Sheol/Caves parents,
  all temporary reservations and earlier random placement/occurrence rules.
- Both Release solutions and preceding focused/depth/ledger/recovery suites.
- All four exact donor map conversions over 256 seeds per map, complete pinned
  dragon definitions and packaged map byte comparisons.
- Actual production armor/body, resistance add/remove, native/imported breath
  selection, scale probabilities, and lava obstacle functions.
- Actual `montraits` through four revivals: saved identity and HP restoration,
  freeze release, saturating revival count and exhausted later scale drops.
- Wizard glowing-dragon breath melts stone to lava and both ice-wall types to
  ice, preserves nondiggable walls, and retains altered terrain after restore.
- Actual terminal map population and hoard inspection finds 22 random eligible
  dragons, three imported chromatic cave dragons, six worms and the required
  gold/gems/tools/weapons/potions/scrolls; its save/reload check passes.

Harness fixes retain all original assertions: multi-page topology capture,
waiting for complete attribute menus, preserving unchanged Windows terminal
cells, and capturing the native startup panic diagnostic. Failed attempts are
retained outside the repository and are not counted as passing checks.
Step9B's gate remains open until the outstanding live mechanics and affected
validation refresh finish. Steps9C/9D and the final whole-project playtest
remain pending.

The death-path review also found a real omission in the post-release build's
explicit corpse dispatcher: the six Moria species, fifteen Sheol species and
baby glowing dragon were absent. Their ordinary corpse cases are now present,
with every original `G_NOCORPSE` exclusion preserved. The actual dispatcher
passes focused checks for all 22 species. The same test exercises 24,576 actual
cave-dragon death/drop calls and verifies preserved traits, the donor scale
drop rates and no scales after the second revival. This fixes an existing
Moria issue as well as Step9 content; it adds no new rewards or balance rules.

The corrected combat fixture now passes actual chromatic melee damage, death,
corpse save/restore, two wand revivals and subsequent deaths. It restores the
map's absolute coordinate frame when creating the isolated opponent after
reload. The complete chromatic/glowing armor test also passes, including
turning off glowing armor's light on removal. The current-build Sheol round
trip and Win32 bog/tree regression both pass.

Final Step9B refresh: both Release builds, focused/source/depth/ledger/recovery
suites, donor/package comparisons and diff checks pass. The actual reflection
test passes after restoring its prepared arena to refresh the visibility grid;
no reflection gameplay correction was needed. The earlier fixture failures
were misses, stale visibility and coordinate-frame setup, all retained in the
external evidence. **Step9B gate passed.** Mithardir and Neutral Quest remain
the next implementation phases, followed by the full-project wizard playtest.

Final architecture/package results, the complete project playtest matrix,
remaining issue disposition and the manual inspection checklist will be filled
in as their gates finish. No pending check in this report is a passing result.

## Step9C foundation findings (implementation in progress)

- Corrected incomplete terrain/status lookup tables: Sheol ice walls lacked
  status entries, shifting later labels, and the debug table omitted imported
  terrain names. Both-architecture compiled tests now verify every real terrain
  index and synthetic status boundary.
- Corrected initial Mithardir definition placement: a mechanical import had
  placed seal inside optional `CHARON` and some other species in quest-specific
  table regions. All20 entries are now enabled, NOGEN, and below `SPECIAL_PM`;
  both-architecture compiled assertions pass.
- Corrected the Windows tile generator's fixed2500-entry reference table,
  exceeded by the new monster/statue/item art. Capacity now follows glyph count
  with a bounds check. The x64 build and tile verification pass at2546 tiles.
- Exact syllable weights, all slab uniqueness states, Aesh fractional rounding,
  and90-turn regeneration arithmetic pass compiled x64/Win32 checks, including
  late-game turn counts.

These are foundation results, not a completed Mithardir playtest. Full Step9C,
Step9D and the final cumulative wizard verification remain pending.

The x64 wizard terrain check also passed shallow-water freeze/save/thaw/fire
behavior and persistence of the new dry terrain. Compiled actual weight tests
passed material density, size scaling and all moon-axe phases. Focused actual
combat handlers passed dehydration, watery/dry targets, capped healing, both
wraithworms' life drain/poison, cancellation and lethal/life-saving dispatch.

Reward wizard checks exposed fixture assumptions: Lua-created tiles were not
yet seen, Uur grants extra hero actions per world turn, queued console input
needs settling, and the living mask's native feedback is “survive without air.”
The individual identification, permanent/timed AC, cursed-tile and breathing
observations succeeded; the final combined restore check is still pending.
No incomplete Mithardir subsystem is counted as a passing phase gate.


## Additional Step9C findings (integration still in progress)

- Word study/reward runtime checks now pass with world-turn accounting and
  single-letter menu selections. The stronger run verifies actual First Word
  lighting and deep-water pits, not only completion messages.
- Fixed Last Spire's legacy statue flag interpretation:31 blessed statues
  used donor `spe=9` for historic/faceless state. Native flags differ. Added
  explicit history/faceless conversion and the30-way donor identity selection,
  with documented inscriptions/native bodies for excluded global figures.
- Added the Aspect's armor-eroding disintegration and overflow-only cold HP
  growth. Focused lethal/resistance/life-saving tests pass. The native death
  API replaces the unavailable donor-specific disintegration death enum.
- Scoped defense tests pass native damage/RNG preservation, imported armor,
  Vaul's permanent five-slot distribution, creature physical resistances and
  the complete normal-sight darkness/light truth table. The active Vaul
  damage paths, complete boss encounters and the full-project wizard playtest
  remain pending; these focused checks do not constitute those gates.


### Mithardir connection findings

- Real Elshava loading caught a type mismatch in converted shop regions:
  `des.region` requires integer lighting. The eight regions now use `lit=1`.
  The failed run correctly stopped on the panic log, rather than accepting
  NetHack's fallback maze as a successful branch floor. Rerun pending.
- The Catacomb river's donor depth arithmetic can become nonpositive in the
  extended dungeon. Added positive bounds only to those three probability
  denominators. Seeded river path/RNG and monster-pool comparisons pass;
  full floor/traversal testing remains pending.
- Both defense-checkpoint Release builds and Win32 focused defense/combat
  suites pass. The complete Step9C and cumulative project gates are unfinished.


The next packaged traversal (`mithardir-runtime-x64-2`) loaded Elshava but
failed its panic-log check: the hidden portal could not be placed on shallow
water. The pinned donor's `bad_location()` explicitly admits PUDDLE, SAND,
SOIL and GRASS; the port had not extended the native region-placement check.
Those four imported walkable types now have the same eligibility, while
occupied squares and excluded regions still reject placement. The foundation
test exercises every terrain type, occupied/excluded squares, and native maze
corridors. A rebuilt packaged traversal is required before this finding is
closed. The earlier `lit=1` correction resolved the Lua load error.

All twenty current Mithardir monster difficulty fields now match NetHack 5.0's
actual `mstrength()` implementation in `src/mondata.c`, verified by
`mithardir-difficulty-x64-1.log`. This replaces provisional hand-entered values;
future imported attack changes must rerun this calculation.


Elshava stock checks found and fixed a category mismatch: pick-axes selected
from the donor weapon list were missing silver/acid because native NetHack
classifies them as tools. `mithardir-shops-x64-3.log` now passes. Portal route
tests also needed to wait out native dizziness and follow actual arrival
portals on reflected maps; the production terrain eligibility fix is distinct
from those test-driver corrections. The fifth traversal reached Wastes 1–3.
Merchant services have been implemented but their validation is still pending.


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
# Step 9C continuation: boundary lighting and deep-one growth

The seventh x64 Mithardir traversal found `newsym(0,0)` when teleporting to a
top-row portal approach. Donor cleanup lit column zero, a non-playable native
vision boundary. The cleanup now leaves that column dark; playable lighting
and terrain still match the donor across 256 cleanup maps. Full traversal is
being rerun; this finding is not being suppressed in the panic-log checks.

Deep-one death pulses and both growth forms now pass focused x64 checks,
including 14,688 ordinary growth comparisons with the committed baseline.
The implementation bounds HP arithmetic and retains the native current/max-HP
invariant. These deliberate donor adaptations are recorded in `step9c.md`.

The eighth x64 traversal and second Win32 traversal subsequently passed all
ten Mithardir floors, five saves/restores, the directed Last Spire shortcut,
and return to DoD110 with no panic-log entries. Win32 also deliberately
teleported to both top-row map edges. The first boundary test's return step
attempted controlled teleport onto its arrival portal, which native teleport
rejects; the test now tracks arrival portals independently of hero position.

Deep-one growth and equipment comparisons now pass on both architectures.
Fey gear matches 28,672 pinned-donor loadouts per architecture, and form
decisions match 413,696 cases per architecture. The generated tile check passes
all 2,554 entries and bitmap capacity. These are Step 9C checkpoints; remaining
combat/service checks, Step 9D and the final full-project playtest are pending.

The refreshed prior-step compiled, depth and ledger/recovery tests passed on
both architectures. Old source tests then rejected the new Mithardir helpers,
saved hero fields and keys because they compared whole files to pre-Mithardir
versions. Their replacement projection removes only the explicit Step 9C
additions and retains byte comparisons for the entire legacy remainder.
The affected Step 7/8 source and package checks pass. Dragon Caves compiled
checks also pass, including 24,576 corpse/death cases per architecture.
Actual x64/Win32 probing confirms that tiny Coure armor is equipped and saved.

The offhand audit found that converted donor `AT_XWEP` slots reused the primary
weapon. They now select a distinct inventory weapon and pass it through combat
and passive retaliation. Shields and range suppress those slots; native species
retain their attack sequences. Actual x64 Bralani melee uses both a long sword
and a knife before and after save/reload (`mithardir-offhand-runtime-x64-1.log`).
Both architectures pass the focused eligibility checks and 3,072 comparisons
against the pinned donor handedness function. The comparison caught a sized
unicorn-horn mismatch, now fixed; ordinary unsized native gear is unchanged.
The first handedness fixture failed to compile due to a const-member assignment;
the fixture was corrected before running the donor comparison. Step 9C's full
combat/service gate, Step 9D and the final cumulative wizard playtest remain open.

Win32 Bralani offhand melee and save/reload also pass
(`mithardir-offhand-runtime-Win32-1.log`). The spell audit corrected the mummy
bonus-dice limit to the pinned value of ten and added the elder's eight-entry
support list, including sleep rays and close/far group healing. A donor/RNG
comparison caught double evaluation of a healing roll through NetHack's `min`
macro; healing now rolls once before capping HP. After correcting two fixture
compile/link issues, x64 comparisons pass 4,096 spell selections, 14,336 damage
cases and 1,024 group-healing cases (`mithardir-spells-x64-4.log`). Live spell
coverage and the remaining Step 9C gate remain pending.

The elder live test found a production range-dispatch gap: native distant
`AT_MAGC` passed spell-list attacks to the beam-only handler. A scoped fix keeps
Mithardir elders and syllable mummies on their spell-list path at range. The
fixture also needed two corrections: native same-race polymorph did not turn
a human wizard into a deep one, and raw terminal deltas lost characters in a
healing message. A vampire fixture and complete message snapshots resolve
those issues. Actual x64 elder group healing, ranged sleep and save/reload now
pass (`mithardir-elder-runtime-x64-3.log`).

Coure and Noviere special attacks now include their weapon damage, respecting
the donor's sleep-after-physical and rust-before-physical order. Their focused
tests pass on both architectures, including cancellation, lethal exits and
286 sleep chance/guard cases. The donor's invalid sleep RNG range at level 7+
is clamped to one. Both Release builds pass. Actual Coure/Noviere weapon
damage now passes on both architectures, including a cancelled Noviere and
save/reload (`mithardir-fey-weapon-runtime-x64-2.log`,
`mithardir-fey-weapon-runtime-Win32-1.log`). The first fixture selected an
intrinsic before its menu finished rendering; it now waits for that entry.

Win32 elder healing, ranged sleep and save/reload pass after isolating the
beam path from intervening monsters (`mithardir-elder-runtime-Win32-2.log`).
The preceding failed run observed healing but did not observe a sleep beam
reaching the hero; it is retained as a failed fixture run.

The weapon audit found omitted size-scaled bonus dice and the crystal sword's
missing second die/small enchantment bonus. Those are corrected for imported
and explicitly modified branch weapons. Both architectures pass 377,568
comparisons with pinned donor dice/material/phase rules and RNG consumption;
the native base-damage source block remains identical. See
`mithardir-weapon-damage-x64-1.log` and `mithardir-weapon-damage-Win32-1.log`.
The remaining Step 9C gate, Step 9D and full cumulative wizard playtest are
still in progress; this is not a final project verification report.

The expanded merchant checks pass paid identification and save/reload for
sea garden, fishery and spa. All four merchant types now have that live
coverage. Eight fresh games per architecture also pass Mithardir's complete
topology and all preceding occurrence/reservation contracts
(`mithardir-topology-{x64,Win32}-1.log`).

Vaul protection was missing from beam, explosion and iron rust-trap damage.
Those scoped paths now pass 65,600 actual source-block comparisons per
architecture with the pinned donor; both builds pass. The live rust fixture
needed correction for the native "step onto" confirmation and then for the
1/5 chance to avoid a known trap. It requires measured damage before claiming
protection; its rerun is pending.

The audit also restored Alabaster elf-to-elder growth and the pinned random
leader entourages for elders/deeper/deepest ones. Existing group flags alone
did not create these companions. Both architectures pass the growth tests
and 131,072 entourage order/RNG/failure comparisons. Explicit single-monster
creation, pets, no-group callers and native species are excluded. Refreshing
the build/live gate and completing remaining Step9C mechanics is ongoing.

The refreshed x64 traversal passes all ten floors, boundary vision,
save/reload, directed return shortcut and DoD110 exit
(`mithardir-runtime-x64-9.log`). Vaul's live rust-trap test also passes on
both architectures after save/reload: native unprotected iron form is
destroyed, while restored pronounced Vaul permits actual damage and survival.
These are Step9 validation results. Following the updated goal, the remaining
work is the attached Step9A–9D implementation and its required regression
gates; this report does not claim a completed project-wide verification.

Native monster-versus-monster combat lacked spell-list dispatch. Scoped
casting for imported elders now passes an actual x64 conflict encounter and
save/reload. Syllable mummies also have a monster-target adapter using the
native general spell list, preserving Naen/Krau and native death/lifesaving.
Both architectures pass the focused casting checks; live mummy validation is
running. The donor compatibility boundaries and excluded global spell and
faction systems are recorded in `step9c.md`.

Both architectures now pass actual elder and mummy conflict casting plus
save/reload. The expanded Vaul audit found protection missing after native
spell rerolls and in several independent native damage paths. Scoped guards
now cover those paths, with the original behavior retained when Vaul is
inactive; both Release builds and the focused comparisons pass. The disabled
monster fire-scroll code was correctly excluded from changes. See the Vaul
checkpoint in `step9c.md` for exact paths and native compatibility boundaries.

The dust callback suite passes 128,640 cases per architecture against actual
local callbacks and the pinned donor. Geometry, map edges, drift, growth,
blindness, sickness, salt damage, sentinel healing and lifesaving are covered.
A test-only `isok` declaration error was fixed before these passes. Live
natural-storm persistence is still being checked, and the full Step9C gate
remains incomplete.

Natural dust persistence now passes on both architectures. The test retains
native restore behavior for already expired regions; it does not restart
them. The live x64 Word test also passes actual undead light damage,
Dividing combat and Nurturing death/tree creation, plus study and save/reload.

The living-armor audit found a production mismatch: one tentacle could attack
while its wearer was helpless. The donor forbids this. Scoped guards now
prevent such attacks and enforce clear paths/grid-bug attack directions.
Both architectures pass the focused loop and passive-interruption tests;
build and live-equipment validation are being refreshed.

Word combat and save/restore now also pass on Win32. The first live
living-armor test found that the object parser converted the armor's name
into a generic armor request and created an ordinary cloak. Exact recognition
of both new armor names/descriptions has been added ahead of the native
class/monster-name stripping, retaining normal creation/wish restrictions.
The fixture now checks the actual object ID before wearing it. Parser and
live-equipment verification are still in progress.

The armor parser correction passes24 actual name/description/native-alias
cases on each architecture. Correct living armor also passes live tentacle
combat before and after restore on x64. The donor Aspect warning panel was
missing; its exact one-line common message has now been restored via native
quest text. Refreshed build/package validation is in progress.

Win32 barnacle armor now passes actual tentacle combat before and after
restore. The projectile audit found that native monster missile hits bypassed
the imported physical defenses. The missing calls are now added, with acid
excluded and native poison handling retained. Both architectures pass20,200
actual dispatch cases plus the existing defense suite. The donor does not
apply its independent melee displacement roll to projectiles, so that behavior
is preserved. Refreshed build/package and encounter checks remain in progress.

The live mummy reward check found another integration omission: Alabaster
mummies still used native random corpse chance, which could suppress their
syllable tile. The pinned donor guarantees this corpse/drop path. That
species-specific guarantee is restored; both architectures pass55,040 cases
showing that every other species' corpse chance and RNG remain unchanged.
The live test's explicit missing-tile assertion produced its recorded
paniclog. Refreshed builds and reward/save checks are in progress.

The AI audit also found that the documented Alabaster opposition had not
been connected to native aggression. The exact donor rule is now implemented:
Alabaster elves/elders and sentinels oppose pudding/blob/umber classes in both
directions. Both architectures pass the species-pair comparison; native pet
protection remains unchanged. The dependency table was corrected to describe
this rule rather than the donor's broader unrelated civilian factions.

Both architectures now pass the actual mummy and Aspect alive/dead
save/restore reward tests. The earlier missing warning assertion was an
observer error, corrected by capturing screens inside the wait loop.
The required warning, single key, single mummy tile and Aspect slab are
verified (`mithardir-deaths-runtime-x64-3.log`, `...-Win32-1.log`).

The remaining Alabaster audit found missing donor iron vulnerability.
Scoped iron damage/contact and glove-dependent handling are now implemented;
both architectures pass the pinned damage/RNG and handling comparisons.
The change affects only living Alabaster elves and elders. See step9c.md
for native contact/bonus ordering and the level-zero defensive adaptation.

Natural Eladrin return transitions exposed an actual invalid weapon-state
call (`weapon_check 1`) in all three forms. The call now requests the native
immediate melee-weapon state, and the focused test validates that state
against the callee. Both focused suites pass; rebuilt live confirmation is
in progress. The preceding invalid Lua terrain call was confined to the
test fixture. Neither failure has been hidden or counted as a passing run.

Live confirmation of the Eladrin fix now passes on both architectures:
three saved elemental forms naturally return after restore, and the
humanoid state can be saved/restored again without diagnostics. Both
refreshed builds and the cumulative Step6/7/8/9A focused/source/depth/
ledger/recovery checks pass.

The projectile audit found a missing coating dispatch for monster-thrown
weapons. Both hit paths now use the existing scoped effect handler; the
monster path applies status effects after wakeup and skips harmless or
already-dead targets. Both compiled tests pass dispatch, resistance,
depletion and native no-property RNG checks. Rebuilt package verification
is pending; the full Step9C gate remains open.

Both coating builds and DLB comparisons passed. The First Wraithworm's
wind radius/save check and expanded dust terrain-turn tests pass on both
architectures. The x64 live merchant run verified all five requested service
paths after eight naturally rolled merchants; its Win32 counterpart is running.

Further physical-path review fixed improvised blows bypassing imported
defenses. Both focused suites pass, including consumed-object safety and
native no-op behavior. Pure elemental contacts retain their separate path,
as in the pinned donor.

Living mirage disguise had two integration defects: direct appearance setup
bypassed shape protection, and native sanity checks rejected the new
species' puddle disguise. Both are corrected narrowly. The old binary
reproduces the non-mimic warning; both corrected live tests pass appearance,
protection, existing/new mirages and save/restore with sanity checks enabled.
Fixture column/template setup mistakes were corrected separately.

Armor-size review found missing hero enforcement and over-strict monster
handling of donor flexible clothing. The shared explicit-size predicate
preserves the donor exceptions and leaves unmarked native gear unchanged.
Both pinned-condition comparisons pass; rebuilt hero/Coure live checks are
pending. See step9c.md for logs, scope and remaining gate status.

The refreshed hero armor and Coure equipment checks now pass on both
architectures, including save/restore. Test-only corrections replaced an
unsupported wish size prefix with level-data metadata and kept the Coure
stationary during probing. Win32 also passed all paid service paths after
two naturally rolled merchants; its teleport fixture now allows native
adjacent placement inside the shop.

Both Aspect focused suites pass exact passive drain, donor spell suppression
and moving darkness. Both live tests confirm energy loss before/after restore
alongside the existing birth/death/reward checks. Ordinary kicks were found
to bypass Aesh and imported physical defenses; both scoped calls are now
connected, and both focused suites preserve native no-effect damage/RNG.
The x64 rebuild passed; Win32 is being refreshed.

Eight fresh topologies per architecture and both cumulative prior-milestone,
depth and ledger/recovery suites pass. Full traversal and awake Wraithworm
bite checks remain running; no complete Step9C or Step9D result is claimed.

The remaining Step9C refresh completed successfully: both awake First
Wraithworm bite/save tests, both ten-floor round trips back to DoD110,
both final Release builds, package bytes and refreshed Sheol/Dragon Caves
regressions pass. All eight Lua resources also pass pinned geometry,
flags, initialization and connector comparisons plus 128 controlled content
runs each. The Step9C automated gate is now PASS. Step9D proceeds with its
mandatory donor audit; no Step9D implementation success is claimed yet.

The user then canceled Step9D and closed scope at the completed Steps9A–9C.
The preliminary Neutral Quest audit is historical only; no production
integration was started. Final manual-validation/publication instructions
remain separate from these passing automated gates.

Steps9A–9C manual-test closeout is complete. The user retained fixed parents
DL108–110 for the historical manual route. The unused DL111 reservation was
removed; scheduler suites confirm it can again host earlier randomized
enrichment. Both final Release solutions, eight fresh native topologies per
architecture, focused regressions, package-byte checks, tile tables and
whitespace/scope review pass. The final result and log inventory are in
[step9.md](step9.md). Step9D remains canceled.
