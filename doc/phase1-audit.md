# Phase 1 audit through Step 15B — 19 September 2026

## Result and scope

The audit reviewed Steps 13, 14, 15A and the uncommitted 15B implementation,
including affected native callers, object ownership, combat, knowledge, shops,
generation, terrain, persistence and Windows packaging. Confirmed defects were
corrected against finalized contracts. No inheritance, affixing, new recipes,
properties, imports, balance changes or Phase 0 redesign was introduced.

Final native enhancement and forge diagnostics, source/resource gates (apart
from two proven historical gates), Windows x64 Release/package, and eight-owner
save/recovery validation pass. Production terminal and final focused regression
results are recorded in the validation matrix below. This is bounded source and
automated execution evidence, not a proof of every gameplay history or GUI view.

Closeout authority: the subsequent user request resolves the two former policy
questions and authorizes one commit of Step 15B plus the audit, without a push.
The original audit evidence below is retained as a historical snapshot; new
closeout evidence is recorded separately at the end.

Original audit authority: the audit request, then finalized `step13.md`, `step14.md`, `step15.md`,
then `phase1.md`. Existing tests were assessed as evidence, not specifications.
Independent subsystem reviews were reconciled against source and failing tests.
The user elected to retain these reviews and tests rather than add an external
cross-model review.

## Entry state and protection

- Branch: `phase1/equipment-enhancement`.
- HEAD: `17d070c1290cafe64c7f40d52a2888521477ecd0` (Step 15A).
- Frozen Phase 0: annotated tag `phase0-dod-expansion-complete`, peeled commit
  `216bf60903cbde8b3ef1de33aeb7368a09ba6719`. The tag object itself is
  `8566470c6527c5e38c8762b40a82e5108ad5b55a`.
- Index: no staged changes. Entry index SHA256:
  `529663F5B7149EC35A780D6C630A032B7FB599F8F12853F1208BEDEDE5871B88`.
- Eight already-modified tracked files: `doc/step15.md`, `include/extern.h`,
  `src/apply.c`, `src/mklev.c`, `src/mkmaze.c`, `src/sp_lev.c`,
  `test/run_step15_save.py`, `test/test_step15_runtime.c`.
- Already-untracked files: `test/test_step15b.c`, `test/test_step15b_source.py`,
  `.codegraph/.gitignore`. The first two were existing 15B work, not new audit
  implementation. The CodeGraph workspace was preserved and used for discovery;
  it was not regenerated, staged or incorporated into the product.
- The existing tracked delta was 779 insertions/18 deletions, predominantly
  15B crafting and shop ordering. No unrelated product work was identified.
- Exact entry status, index listing, binary diff and copies of the untracked
  15B tests were captured before edits in
  `C:/Users/yariv/AppData/Local/Temp/my-nethack-phase1-audit-20260919/`.

During the original audit, no checkout, reset, clean, stash, staging, commit,
tag, rebase or push was used.
The final status and index check are recorded at the end of this report.

## Confirmed production findings

| Finding and observable failure | Root cause, correction and affected paths | Regression/evidence |
|---|---|---|
| Same-base polymorph erased actual/known properties and quality | `zap.c:poly_obj` recreated the object without copying the four fields. Copy after native type adjustments when the final base type matches, before billing/wear. Genuine type changes remain plain. | `step13-red/runtime.log`; same-type floor and worn replacement tests. |
| Polymorph could acquire natural enhancements under an outer scope | Constructors/retries inherited the caller context. Their scope is now NONE, with exact prior restoration. | Source-confirmed; 128 same/different-type replay pairs across four prior contexts compare native RNG tails. A separate pre-fix runtime failure was not captured for this subcase. |
| Enhanced shield identified despite redundant reflection | `muse.c:ureflects` unconditionally marked its property after the shared attribution check declined. Removed that duplicate assignment. | Two captured red cases (intrinsic/native amulet), plus unique-source positive control. |
| Vaul displacement falsely identified enhanced boots | `enhancement_observe_worn` omitted the existing displacement timer from redundant sources. The observer now checks it. | Active/expired timer pair; active-timer red captured. |
| Monster retained enhanced speed after mail cancellation | `cancel_item` removed extrinsics while the old source still appeared worn to speed recalculation. Temporarily clear the mask during removal and restore it before new armor effects. | Captured red; eight glowing/chromatic mail cases with normal, fast, slow and redundant speed. |
| Croesus's extra sword bypassed acquisition provenance | Early `makemon` extra-item creation preceded the normal scoped equipment region. Apply the same MM_NATURAL/NONE scope locally and restore the caller. | Captured red under outer SHOP; 200 natural/unflagged cases pass. |
| Native random exterior maze stock stayed plain | `fill_empty_maze` used three ordinary constructors despite the approved random-fill contract. Use existing floor/monster wrappers. Explicit authored objects remain unchanged. | Captured red; 128 real native fills now yield 29 enhanced floor and 56 enhanced monster items. |
| Forge availability/diagnostic totals overflowed | `forge_quantity` added native `long` quantities directly. Use the existing saturating `nowrap_add` helper. | Captured `LONG_MAX + 1` red; availability and displayed reason totals now remain valid. |
| Invalid negative allocation counts were interpreted as all | `forge_allocate` accepted every negative count as the native default. Only `-1` now has that meaning; zero, other negatives and overlarge counts reprompt. | Captured scripted `-2` failure; 40-stack count/cancellation test. Native UI normally produces only the valid sentinel. |
| Direct terrain writers destroyed permanent forges | Lua stairs/ladders/graves/doors/maze walking, drawbridges, terrain wishes, room construction, invocation rings and swamp conversion bypassed the common setter. Narrow guards preserve the forge before their side effects. Existing trap replacement also bypassed the no-new-trap guard; `maketrap` now rejects it early. | Separate pre-fix logs; 45 terrain/light/postprocessing cases, including 25 room positions and seven invocation distances. |
| Dark irregular regions extinguished forge squares | Native `flood_fill_rm` assigned `lit=0` directly. Preserve forge lighting. | Actual Lua irregular-region red/green. |
| Explicitly aligned scripted rooms admitted natural forges | `lspo_room` provenance omitted `xalign`/`yalign`. Explicit alignments now mark authored geometry. | Captured red; default backend versus explicit left/top alignment and mask sorting/reset checks. |

All confirmed Phase 1 defects with authoritative fixes were implemented. The
two policy boundaries left open at that checkpoint were subsequently resolved
by the closeout request, as recorded below. The later GUI startup observation
is a separate native-port limitation, also recorded below.

## Coverage traceability

| Request area | Contract and reviewed production paths | Independent evidence / limits |
|---|---|---|
| 3.1 C/resource safety | Allocation/zeroing, `splitobj`, bill copies, `oname`, `poly_obj`, native free/relink routines; forge menus and transaction consumption | New quantity boundary, semantic state comparisons, stable-ID multi-stack ledger; reviewed window/free paths. No general sanitizer claim. |
| 3.2 Representation/lifecycle | `obj.h`, `enhance.h`, `enhancement_set/normalize/change_type`, artifact hooks, `mergable`, billing copies, native codecs | 4/4/1/1 widths, 112-byte x64 object, retired bit 7, maximum two properties, magical equipment eligibility and tool/gem/artifact exclusion; split/merge matrix, polymorph and eight-owner persistence. |
| 3.3 Combat/worn effects | `hitval/dmgval`, `hmon_hitmon`, `hitmu`, `mdamagem`, enhanced missile impacts, `m_throw`, `setworn/setnotworn`, monster extrinsics/speed | Nine real attack pipelines, exact component dice/RNG, source stacking, Anarchic/Concordant independence, shade contact; all seven hero and monster slots × 13 supported armor properties; cancellation integration. |
| 3.4 Knowledge/name/value | Shared observers, `fully_identify_obj`, bounded prefix/suffix/xname formatting, `shk` price/billing paths | Actual-state mechanics, resisted/Primordial knowledge, Trueflight non-ID, attribution fixes; known/unknown names and long buffers; 5,724 independent price checks. Monster-use observation was subsequently finalized as hero-use-only; see closeout below. |
| 3.5 Creation/context | Opt-in callers in `allmain`, `mklev`, `mkmaze`, `mkroom`, `shknam`, `mplayer`, `makemon`, native maze filler; `mkobj` hook; forge output | Four prior scopes, constructor failure/NO_MINVENT, 100 NONE/filter RNG tails, Croesus and maze fixes. Explicit scripted rewards, wishes, starting objects, gifts and unflagged monsters remain non-opted-in; movement/restore do not reroll. |
| 3.6 Exact generator | `enhancement_depth_band/generate/created`, native secondary filtering | 110,000 exact reference result+next-RNG cases, all depth boundaries/clamps and absolute-depth branch fixture; independent pools exercise Trueflight, exhausted tiers and downward fallback. Million corpus retained with conditional accepted marginals. |
| 3.7 Persistence/version | Native save/restore/sfstruct/version, bones, recovery; no second serializer | Current epoch 9 passes; previous epoch 8 rejected. 5,345,280 independent codec states + 27,648 retained low-bit cases; level/bones and eight full-game ownership paths including migrating monsters/backlinks. No old-format compatibility imposed. |
| 3.8 Forge terrain | Terrain macros/arrays, glyph/symbol/tile boundaries, display/farlook/surface, mimic/vision/movement, setter and raw writers | FORGE=45, accessible/transparent furniture; setter matrix, 45 mutation/light cases, native level/bones, production terminal persistence, generated tile/resource checks. Interactive GUI appearance remains manual. |
| 3.9 Natural forge generation | Production finalization wrapper, accepted-bones early return, `forge_eligible/candidate/generate`, transient coordinate mask | Exactly one eligible-level gate; 1,000 exact reservoir tails; retained deterministic no-candidate cases; independent 1,000-level postconditions (114 placements); six additional candidate exclusions, sorting/reset and Lua provenance. |
| 3.10 Activation/turns | Real `doapply` preflight and forge dispatch before artifact retouch/speech | Terrain/type/state/STR3–4, altered/artifact hammer, away-from-forge behavior; diagnostic return codes plus production terminal cancellation and success turns. Hammer state checked semantically. |
| 3.11 Catalog/eligibility | Locked 12-formula order, validation/normalization, `forge_reason/quantity`, guarded `canletgo`, display copies | Duplicate unordered formulas and aggregated slots; actual top-level/type-knowledge predicates, unknown-versus-absent diagnostics, exclusions; independent synthetic potion/food/tool recipe shapes. No production catalog additions. |
| 3.12 Menus/transient state | `forge_menu/pick/allocate/confirm`, native menu counts and ID descriptors | 192 ledger cases, 40 stacks, shared slots, partial cancellation, invalid/maximum counts, explicit sole selection, real native terminal selections. No tentative splitting or mutation. |
| 3.13 Atomic transactions | `forge_commit/find`, native `inv_cnt/mergable/useupall/addinv`, capacity preflight | 16 independent capacity cases plus existing faults/duplicate IDs/construction-failure hook; consumed and surviving merge targets; exact survivors and quantity ledger. Construction precedes irreversible consumption; no floor fallback. |
| 3.14 Outputs/shops | `forge_output` native default constructor under NONE, exact context restore, base-type-only knowledge; forge/shop geometry ordering | 2,000 plain outputs across all four outer contexts; all 12 native crafts; real repeated crafting and enhanced-input fixture; corrected multi-character irregular-shop map test exercises the actual parser. Unpaid exclusion is native diagnostic coverage, not claimed as a live terminal shop scenario. |

Detailed reconciled review records are under `_qa/phase1-audit/`:
`enhancement-review.md`, `generation-persistence-review.md`,
`forge-terrain-review.md`, `historical-review.md`. These local evidence files
are ignored build/audit artifacts; this document is the durable summary.

## Test reliability and new coverage

- Replaced the catalog-derived pricing oracle with explicit contract identities
  and surcharges: 159 legal masks × three qualities × six base boundaries × two
  knowledge states = **5,724 checks**.
- Added **78** monster armor cases (now **91**, all seven slots × 13 properties),
  **six** lifecycle/attribution cases, **128** polymorph replay pairs, **one** worn
  replacement, and **eight** monster-cancellation cases.
- Added **60,000** generator traces (total **110,000**) using manually declared
  class/native-secondary pools; **100** NONE/filter RNG checks; **28** nested
  scope cases; **200** Croesus provenance cases; **128** native maze fills.
- Retained exactly **1,000,000** generation corpus objects. Added independent
  accepted quality/count marginals with denominator
  `10000 - standard * (100 - presence)`. Statistical tolerances supplement exact
  tests; raw pre-rejection probabilities are not used as accepted expectations.
- Retained **5,345,280** codec states, but populate fields from independent
  specification pools rather than the production catalog/setter. Expanded real
  save/recovery from seven counted chains to **eight named ownership paths**,
  verifying owner identities, object locations and container/monster backlinks.
- Added **192** six-stack eligibility/allocation ledgers (**105 successful
  allocations/crafts**) across three recipe shapes, **16** capacity ledgers,
  **one** aggregation-limit case, and a **40-stack** invalid-count/cancellation/
  success scenario. Snapshots compare semantic fields and topology rather than
  padding; cancelled allocation also checks the native RNG tail and context.
- Extended the existing **2,000** forge constructions to all four prior scopes.
- Added **45** terrain/light cases, **five** provenance cases, **six** candidate
  exclusions and **1,000** exact reservoir result/RNG traces. The original
  **1,000-level** corpus now uses explicit independent postconditions rather
  than production `forge_candidate` as its placement oracle.
- Fixed a vacuous existing map test: a 1×1 `des.map` establishes coordinate
  context without loading map terrain, so the irregular-shop test now uses a
  two-character fragment to exercise actual placement.
- Preserved original source gates. A static **14-file/38-hunk** reviewed manifest
  reverses only accepted 15B/audit changes before original historical equality
  checks. **52 mutation checks** verify in-hunk and outside-hunk changes cannot
  pass those comparisons. Tests never regenerate the manifest. The large
  `apply.c` hunk includes preexisting 15B implementation, not 494 new audit lines.
- Corrected the Step 9 tile test's obsolete all-new-art-must-be-reused premise:
  forge pixels must exactly match the immutable Step 15A asset; dimensions,
  generated reachability and all other donor-art checks remain intact.

Initial diagnostic additions exposed several fixture mistakes (room-state
isolation, native function/global names and delayed monster cleanup). Those
were corrected before relying on the production failures. Their compile and
runtime failures are retained and are not counted as product defects.

## Validation matrix

Commands are run from the repository root with `C:/Python311/python.exe`.
`MSBuild.exe` is Visual Studio 18 BuildTools' `MSBuild/Current/Bin/MSBuild.exe`.
Evidence paths below are relative to `_qa/phase1-audit/`. The directories named
`*-red2` preserve separate red logs but their final runtime/build logs are green.

| Command/suite | Result, count and what it proves | Evidence |
|---|---|---|
| `test/run_step13.py --out _qa/phase1-audit/step13-maze-red2` | PASS final full native suite; 110,000 exact RNG traces, million corpus, 5,345,280 + 27,648 codec cases, combat/worn/names/value/lifecycle/version/level/bones | `step13-maze-red2/build.log`, `runtime.log`; `production-red.log` retains maze red |
| `test/run_step15.py --out _qa/phase1-audit/step15-red2` | PASS final full forge suite and unchanged 1,000-level corpus: 114 selections, 114 placements, zero invalid/multiple; all added cases above | `step15-red2/{build,runtime,corpus}.log` |
| `test/run_step13_save.py _qa/phase1-audit/step13-maze-red2/bin _qa/phase1-audit/enhanced-save` | PASS real full-game save, restore, native checkpoint recovery and second restore, eight ownership paths | `enhanced-save/step13-game-results.txt`, `recovery.log`, terminal captures |
| `MSBuild.exe sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal /nologo` | PASS console, GUI, utilities, generated resources/tiles, DLB and ZIP | `release-build.log` |
| `test/test_step11_resources.py _qa/step13-donors/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip` | PASS 199 protected files, 186 Lua resources, pinned donor bodies, exact packaged console/GUI/DLB bytes | `package.log` |
| `test/test_step9a_tiles.py`; `test/test_step9c_package.py binary/Release/x64/nhdat500` | PASS 2,868 generated tiles/capacity, exact forge artwork; eight Mithardir resources plus dungeon/quest | `tiles-final.log`, `step9c-package.log` |
| All `test/*_source.py`, scheduler contract and tiles with required pinned donor arguments | 25 PASS / 2 proven historical failures; includes Steps 13/14/15/15B and new audit-scope gate | `source-final/results.json` and individual logs |
| Current-source isolated x64 `STEP11_TEST=true` build; `test/run_step11.py _qa/phase1-audit/step11/bin _qa/phase1-audit/step11-focused --phase focused` | PASS selectors, all eight features including Library, recurrence, shop coexistence, metadata, level/bones, clean/partial failure | `step11-build.log`, `step11-focused.log`, `step11-focused/` |
| Eight focused Step 9C donor/native runners: weapon damage, coatings, defense, equipment, fey equipment, handedness, armor size, services | PASS respectively 377,568; 800; 20,200; 180; 28,672; 4,032; 864,864; 1,689,120 cases, with additional native no-effect/credit checks | `regressions/results.json` and named logs |
| `test/run_step15_save.py binary/Release/x64 _qa/phase1-audit/production-forge _qa/phase1-audit/step13-maze-red2/bin` | PASS production terminal: exact named selection, free cancellation, one-turn success, katana-to-tsurugi chain, enhanced dagger-to-athame craft, level transitions, save/restore and checkpoint recovery | `production-forge.log`, terminal/save/recovery evidence directory |
| `test/run_step11_save.py binary/Release/x64 _qa/phase1-audit/production-step11-save 4` | PASS production full-game save/restore, leave/revisit, stable custom identity/recurrence and native checkpoint recovery | `production-step11-save.log` and evidence directory |

The production crafting fixture first creates an explicitly diagnostic
current-format save containing an enhanced dagger, then replaces its executable
with the byte-identical production EXE before any forge gameplay. This tests
production decoding and consumption of enhanced ingredients without adding a
production enhancement-creation command. It must not be described as natural
production acquisition or as GUI gameplay.

Release warnings match the preaudit release log exactly: C4100 for `trop` and
GUI `fmt`, and C4701 for the existing `mkmap.c` coordinates `x`/`y`.
`warnings.json` records the comparison; no new warning remains. Early fixture
compile errors were resolved. No sanitizer/static-analyzer run is claimed.

## Historical failures and differential evidence

The two legacy gates remain failed and unchanged: Step 7 first reports
`src/mklev.c`; Step 8A first reports `include/dungeon.h`. Investigation went
beyond the first failed assertion: all **16** protected-file comparisons were
evaluated separately. Seven ancient assertions differ: Step 7's `mklev.c`,
`mkroom.c`, `shknam.c`, `dungeon.h`, `restore.c`; Step 8A's `dungeon.h` and
`restore.c`. For every comparison, projected final bytes equal both entry HEAD
and the frozen Phase 0 commit. These are evidenced historical mismatches, not
new audit exemptions. Step 11's custom registry/obsolete enum changes, prior
Plumach shopkeeper integration and reset hook explain the differences.

Step 10B's additional entry failure was **not** historical: the entry worktree's
33 accepted Step 15B shop lines broke two exact Step 15 projection contexts.
The new narrowly frozen projection restores those contexts and the original
Step 10B gate now passes. The forge artwork failure was a stale test premise,
not a missing resource. Exact hashes, entry reconstruction, diffs and commands
are under `historical/` and summarized in `historical-review.md`.

Unenhanced/native behavior is supported by zero-state combat/RNG tests, native
body/equipment donor regressions, default-NONE generation tails, away-from-forge
apply tests, ordinary terrain/generation invariants and exact historical source
comparisons. A separate frozen-Phase-0 executable was not built. Save comparison
uses the current format only; no pre-Phase-1 compatibility requirement was added.

## Resolved policy questions

The original audit left authored forge placement over existing traps and visible
monster-use identification undecided. The closeout request finalized both:
placement is rejected without changing the trap or square, and elemental
identification is hero-use-only. The small placement correction and dedicated
regressions are recorded below. No monster-combat production change was needed.

## Coverage limits and manual checks

1. Interactive GUI appearance/accessibility was manual at the original audit
   checkpoint. The closeout observation result is recorded separately below.
   GUI build, resources, glyph/tile mappings and package bytes passed automation.
2. Full-pack and unpaid rejection are covered in linked native transaction/
   menu fixtures; a separate production-terminal full-pack/unpaid shop scenario
   was not added. This is a production-path coverage limit, not a claimed pass.
3. Statistical corpora do not prove every seed/topology. No new bulk samples were
   added to the existing million-object or 1,000-level corpus.

## Original audit changed-file accounting and Git state (historical)

The audit preserved the entry 15B implementation. Audit-only production changes
are the defects enumerated above: two small `apply.c` boundary fixes, lifecycle/
attribution/provenance corrections and narrow terrain protections. Additional
tests strengthen the existing native suites and production runner; source guards
retain immutable baselines with exact audited deltas. Documentation updates
clarify current milestone status, historical epoch progression and evidence.
`include/extern.h` and `test/test_step15b_source.py` retain their preexisting
changes without audit edits. CodeGraph remains local/untracked.

Final validation completed against stable production/test source. The source
hash inventory is `_qa/phase1-audit/final-source-sha256.json`; package comparison
used the newly built production files. Final Git evidence follows.

Index SHA256 is byte-identical to entry; staged diff is empty. HEAD and branch
are unchanged. All 309 recorded source/test hashes still match the final
validation state. `git diff --check` passes. No commit or push occurred.

Exact `git status --short --untracked-files=all`:

```text
 M doc/phase1.md
 M doc/step13.md
 M doc/step14.md
 M doc/step15.md
 M include/extern.h
 M src/apply.c
 M src/dbridge.c
 M src/enhance.c
 M src/makemon.c
 M src/mklev.c
 M src/mkmap.c
 M src/mkmaze.c
 M src/mkroom.c
 M src/muse.c
 M src/objnam.c
 M src/sp_lev.c
 M src/trap.c
 M src/zap.c
 M test/run_step15_save.py
 M test/step13_source_projection.py
 M test/test_step13_runtime.c
 M test/test_step14_combat.c
 M test/test_step14_generation.c
 M test/test_step14_names.c
 M test/test_step14_persistence.c
 M test/test_step15_runtime.c
 M test/test_step15_source.py
 M test/test_step9a_tiles.py
?? .codegraph/.gitignore
?? doc/phase1-audit.md
?? test/phase1_audit_historical_changes.json
?? test/phase1_audit_projection.py
?? test/test_phase1_audit_source.py
?? test/test_step15b.c
?? test/test_step15b_source.py
```

## Closeout: resolved contracts and new validation

The closeout began on the same branch and HEAD with an empty staged diff and
the exact index SHA256 above. All existing Step 15B/audit work was preserved.
Only the two authorized residual contracts were revisited; no later feature,
large-corpus expansion, or broad exploratory audit was undertaken.

- Authored forge placement over a trap now returns the existing rejection
  result from `set_levltyp`. `lspo_map` checks the same conflict before clearing
  square metadata. The trap, terrain, lighting and room metadata are preserved.
  Natural exclusion, trap-on-forge rejection, permanence, shop resolution and
  ordinary non-forge placement retain their contracts.
- Elemental identification is hero-use-only. Inspection confirmed the hero
  hit caller alone invokes the observer; native monster melee and missile
  paths use the knowledge-independent damage calculator. No production combat
  change was made. Visible hero use still learns effective supplying properties,
  including Primordial; fully resisted or unseen manifestations do not.

Closeout production edits are limited to `src/mkmaze.c` and `src/sp_lev.c`.
Tests changed in `test/test_step15_runtime.c`, `test/test_step14_combat.c`,
`test/test_step13_runtime.c`, and `test/run_step13.py`. The exact accepted
`after` text of the two existing manifest hunks was updated in
`test/phase1_audit_historical_changes.json`; its baseline and 14-file/38-hunk
scope remain fixed. Contract/status documentation changed in `doc/step13.md`,
`doc/step14.md`, `doc/step15.md`, `doc/phase1.md`, and this report.

New evidence lives under `_qa/phase1-closeout/`, separately from the original
audit results. Commands below use Python 3.11 and the same MSBuild as above.

| New closeout run | Result |
|---|---|
| `test/run_step13.py --combat-only --out _qa/phase1-closeout/step13` | PASS focused combat/armor/knowledge gates, including 108 impacts across nine native paths and four visibility/resistance conditions, ordinary/unknown/identified replay, identical damage and RNG regardless of knowledge, and 40 surviving native monster projectiles with preserved enhancement/ownership state. |
| `test/run_step15.py --out _qa/phase1-closeout/step15` | PASS complete forge suite, 12 new real Lua coordinate/selection/replacement/map rejections across pit/arrow/teleport traps, unchanged trap identity/fields and nonzero terrain metadata, ordinary-terrain/untrapped-forge controls; unchanged 1,000-level corpus, 114 selections/placements, zero invalid/multiple. |
| Steps 13/14/15/15B source gates and `test/test_phase1_audit_source.py` | PASS all five; exact projection scope and all 52 rejected mutations retained. |
| `test/test_step10b_source.py` | PASS historical source projection dependent on the updated manifest. |
| Windows x64 Release solution command above | PASS console, GUI, utilities, resources, tiles, DLB and ZIP. Only the previously recorded C4100/C4701 warnings remain. |
| `test/test_step11_resources.py _qa/step13-donors/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip` | PASS 199 protected files, 186 packaged Lua resources, exact console/GUI/DLB ZIP content and pinned donor bodies. |
| `test/test_step9a_tiles.py`; `test/test_step9c_package.py binary/Release/x64/nhdat500` | PASS 2,868 tiles, 640×1574 bitmap capacity, pinned forge art, eight Mithardir resources and dungeon/quest bytes. |
| `test/run_step15_save.py binary/Release/x64 _qa/phase1-closeout/production-forge _qa/phase1-closeout/step13/bin` | PASS production named ingredient selection, free cancellation, one-turn crafting, two-craft chain, enhanced ingredient consumption, level transition, save/restore and native checkpoint recovery. The enhanced input again comes from an explicitly diagnostic-created current-format save. |
| `git diff --check` | PASS. |

The new trapped-square regression first failed against the old production guard;
`step15/trapped-square-red.log` retains that result. One Lua fixture call was
corrected to the supported selection-table form. Actual monster flight also
exposed missing diagnostic display initialization (`bot before init`); the
test harness now defers screen flushing with the native `flush_screen(-1)`
control. This changes no visibility or combat state and is diagnostic-only.
`step13/runtime-display-init-failure.log` retains the failed run; final runtime
logs pass. These fixture issues are not production combat defects.

The million-object and 5,345,280-state codec suites were not repeated: closeout
does not change enhancement generation, shared enhancement mechanics or codecs.
Their original full-audit evidence remains above. The proven historical Step
7/8A expectations remain untouched. Independent closeout reviews found no
actionable issue in the placement guards, visible combat tests, or commit scope.

### Actual GUI attempt and remaining manual residual

Windows automation launched a byte-identical current Release `NetHackW.exe`
in the isolated `gui-forge` copy of the production checkpoint. The welcome
dialog was visually observed, but the app exited before gameplay. Its paniclog
reports `condition_field->mask == (1 << ci)` at `win/win32/mswproc.c:2930`.
The native condition table has 30 initializers for 31 conditions: FROZEN is
missing, leaving its mask zero. This is independent of the forge and save state.

Both the GUI source (Git blob `976b8d76553023d18a06ff350ada380b59cc345a`)
and `include/botl.h` (`b92fede355cab23daeeb51986738819b8ef5b8b9`) exactly
match the frozen Phase 0 baseline. The only Phase 1 `src/botl.c` changes are
forge terrain labels. This pre-existing native GUI startup defect was not
expanded into an unrelated port repair during closeout. Forge appearance and
interactive GUI accessibility remain **unverified/manual**, not a GUI pass.
The user explicitly made that residual nonblocking; automated GUI build and
package integration pass. The isolated GUI process exited; no user game was used.

### Commit scope

The complete tracked and intended untracked changes were reviewed as Step 15B,
the completed Phase 1 audit, or these closeout corrections. The authorized
single normal commit includes those 35 project files, including this durable
report and source-gate support. `.codegraph/`, `_qa/` evidence, generated build
outputs and unrelated local files are excluded. The historical precommit status
above is deliberately retained, not presented as the post-closeout Git state.
The final commit hash and exact postcommit status are reported with delivery.
No push is authorized or performed.
