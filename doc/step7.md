# Step 7: classic NerfHack Lost Tomb

Manual validation completed: the user confirmed that testing with the temporary
DL106 entrance passed. That override and its fixed-depth test assertions have
been removed; the original randomized DL30-199 scheduler and variation checks
are restored. Existing test saves keep their saved DL106 topology; start a
fresh game to use random placement. The temporary-build audit remains in
`../step7-dl106-audit/`.

Restoration validation passed: x64 and Win32 Release rebuilds; 2,000 scheduler
samples per architecture covering 168 distinct Tomb depths; three fresh games
per architecture (x64 entrances DL179/192/146, Win32 DL186/33/194); source/DLB
comparisons; and `git diff --check`. The scheduler, topology tests and README
exactly match their pre-override versions. Logs are in
`../step7-random-restored-audit/`.

Implemented and validated 2026-09-07. NetHack General Public License;
see `dat/license`.

## Baseline and authority

Repository: `H:\app\mynethack\YarivNetHack`. Starting branch:
`phase0/dod-length`. Starting HEAD:
`9191de7079624e94acdde19d07814a0f03ce9450`, tagged
`step6b-nerfhack-dungeon-enrichment`. Working tree and index were clean.

The sole donor is `elunna/NerfHack`, branch `dev`, pinned revision
`0cb8781b0929b4617590a3b9fe78f972ef42c25f`. Files were read with
`git show REVISION:path` from sibling clone `../step6a-donor-NerfHack`.
Neither its current checkout nor another variant was used as authority.

## Donor dependency audit

| Donor files traced at the pinned revision | Dependency and local disposition |
| --- | --- |
| `dat/tomb-1.lua`, `dat/dungeon.lua` | Classic map and branch metadata imported. Local topology uses the existing Step 6 scheduler. `tomb-2.lua` is absent and cannot be selected. |
| `include/monsters.h` | Only Shadow imported. L-class guardian, Z-class zombies, M-class mummies and all other map monster dependencies already exist locally. |
| `include/mondata.h`, `include/display.h` | Shadow identity, shade-like contact behavior, light sensitivity, telepathy/warning exclusion. Local equivalents used. |
| `src/makemon.c`, `src/mon.c`, `src/monmove.c` | Permanent invisibility, ghost-class handling, light avoidance and darkening the square left behind. |
| `src/dokick.c`, `src/dothrow.c`, `src/mhitu.c`, `src/uhitm.c`, `src/weapon.c` | Relevant shade-like combat exceptions extended to Shadow. |
| `src/mhitm.c`, `src/mondata.c` | Audited adjacent shade behavior; no changes imported. |
| `include/objects.h`, `include/obj.h`, `src/mkobj.c` | Only Magic Candle imported; normal tool entry, candle classification and initialization. |
| `src/apply.c`, `src/timeout.c`, `src/light.c` | Apply, permanent lighting, snuffing, light-source lifecycle, finite Candelabrum attachment. Existing local light-source infrastructure reused. |
| `src/objnam.c`, `src/shk.c`, `src/zap.c` | Candle naming/wish classification, shop charges/selling, cancellation and polymorph behavior. |
| `src/iactions.c`, `src/olookup.c` | Audited item actions/light-source classification; no donor-wide action or wish changes imported. |

Local Lua already provides the required map, selection, secret-door, nondiggable,
no-teleport, trap, monster, object and container APIs. Spiked pits, chests, gold,
wax candles, magic marker, wand of death and wand of polymorph already exist.
No unrelated branch, monster, object, or donor subsystem was imported.

## Map and branch

Local resource: `dat/tomb-1.lua`. It preserves the donor 44-by-20 geometry,
four randomized secret-path decisions, secret/locked doors, non-diggable
structure, no-teleport flag, five spiked pits, eight chests and their locked/
trapped state and contents, gold/object caches, three wax candle caches,
14 Shadows, one L-class guardian, 19 Z-class and 11 M-class creations.

The special chest keeps the donor's successive 30% checks: Magic Candle 30%,
magic marker 21%, wand of death 14.7%, wand of polymorph 34.3%. These are not
four equal-weight outcomes. Population, rewards and difficulty were not reduced.

The only Lua adaptations are a provenance comment, ordinary quoted rows with
explicit newlines to preserve every map byte without trailing source whitespace,
and removal of the donor's explicit up-stair at (1,9). The existing branch
levregion creates that same return stair; keeping both creates a duplicate in
this engine. Trailing comment whitespace was removed. A trace comparison of
all 16 secret-path combinations and four reward outcomes confirms every other
map/content creation call matches the pinned donor.

`dat/dungeon.lua` declares exactly one optional dungeon named `The Lost Tomb`,
one level, chaotic/mazelike, bonetag `Z`, and a direct `tomb-1` level entry.
There is no variant pool. Its downward entrance is in the DoD.
`src/dungeon.c:step6b_schedule` chooses its final entrance after the Step 6
features, using the same occupied-depth table and `step6b_pick_depth` helper.
The eligible range is DL30-199 and excludes existing reservations, special
levels and branch entrances. No independent per-floor roll or deterministic
production test placement was introduced.

The selected ordinary `branch` endpoint is reinserted into the branch list;
the child's `depth_start` uses the existing branch direction/entry-level formula.
An entrance at DoD123 therefore displays Tomb level 1 as Dlvl124. Normal
branch/dungeon persistence stores this topology; no extra save fields or custom
depth display were added. DoD remains 200 levels, Castle DL200; Big Rooms remain
3-5 total (maximum 5), Giant Court 1, Real Zoos 2-3, Dragon Lair 1, Temple 1.
NerfHack Big Room 18 remains unchanged as local `bigrm-14.lua`.

## Shadow

Shadow is inserted in the existing S_GHOST class before ghost. It retains
level 10, speed 9, AC -2, MR 0, neutral alignment, difficulty 10, two 4d4 touch
attacks (strength poison and cold), human statue weight, no nutrition, wail
sound and human size. It resists cold, disintegration, sleep, poison, stone,
acid and electricity; flies, passes walls, is breathless, humanoid, unsolid,
sees invisible, and has infravision. It is undead, hostile, wandering/stalking,
not polymorphable, permanently invisible and leaves no corpse.

`G_NOGEN` is preserved; the actual `uncommon()` random-generation exclusion
rejects it. Explicit Lua creation by name works and the Tomb census showed
14 Shadows. Shade-like combat handling, immunity to ordinary telepathy/warning,
light sensitivity, and uncancelled darkening after movement were carried over.
Explicit monster detection remains available.

Compatibility: donor `MH_UNDEAD` maps to local `M2_UNDEAD`; the donor's empty
extra flag field needs no local subsystem. Donor color `DRAGON_SILVER` uses
the local bright-cyan definition. Existing post-move infrastructure handles
darkening. Tiled ports reuse the existing ghost male/female artwork; no new
donor artwork was available/imported. Text appearance uses the S_GHOST glyph.

## Magic Candle

The normal `TOOL_CLASS` entry is magical, mergeable, wax, white, unidentified
appearance `candle`, generation weight 5, weight 2, base price 500. It is not
restricted to Tomb loot. Local generation derives class-weight totals from
the object table, so no other probability or Step 5 shop class rule changes.
Initialization shares the donor magic-lamp-style `spe=1`, unlit state and
bless/curse handling, without changing the existing magic lamp entry.

Normal apply lights it with radius 3 and nominal age 300, without a burn timer;
time does not consume fuel. Normal snuff/relight, dropping/pickup, containment
and light-source cleanup are reused. It is classified as a candle/ignitable
object, but its age is not a relative burn-timer value and its name does not
become "partly used". The local candle fuel-age merging restriction explicitly
exempts Magic Candle so prior lighting does not prevent otherwise valid stacks.

Attaching to an empty Candelabrum converts it to ordinary finite candle fuel
(600 turns), with the donor "very ordinary" message. Attaching to a partially
filled Candelabrum preserves that object's existing fuel, and a lit Candelabrum
uses its normal finite burn timer. It never acquires permanent light.

Shop use costs the wax-candle base price (20). Lighting does not incorrectly
bill it as an ordinary candle that will burn completely away; selling is not
rejected on that ordinary-candle fuel test. Other local pricing remains intact.
Cancellation retains its magic; a polymorph result of Magic Candle is converted
to a wax candle with age 400 as in the donor. Vanilla magic-lamp polymorph,
lighting, rubbing/djinni charges and wish paths remain intact. `dorub`,
`djinni_from_bottle` and `makewish` are byte-equivalent to the baseline after
newline normalization. Magic Candle does not grant wishes.

## IDs, save compatibility and packaging

NetHack derives monster/object enums and tables by repeated inclusion of
`include/monsters.h` and `include/objects.h`; no generated IDs were edited.
Source tile lists receive matching ghost/wax duplicates and subsequent numeric
labels shift. Build tooling regenerates the tile mapping and other outputs.
Generated sources, binaries, packages, saves and test logs are not part of
the source change.

Inserting these definitions shifts later monster/object IDs; adding the Tomb
also shifts the Temple dungeon number. `EDITLEVEL` increases from 1 to 2 using
the existing project convention (incarnation `0x05000002`). The normal version
gate rejects older saves/bones/checkpoints; no custom migration or backwards
compatibility is claimed. Same-build save/reload was tested. The ledger fixture
assertion is updated to the new epoch without weakening its checks.

`sys/unix/Makefile.top`, `sys/windows/Makefile.nmake` (both resource lists) and
`sys/windows/vs/files.props` include `tomb-1.lua`. Both final Release DLBs were
parsed and the exact packaged dungeon, Tomb, Moloch and Big Room bytes compared
with current source. Neither contains `tomb-2`.

## Moloch regression found during validation

The baseline's whitespace cleanup used long-string concatenation at four map
rows. Lua discards the initial newline of each new long string, joining those
rows: evaluation produced a 168-by-5 map instead of the intended 56-by-9 map.
The earlier Step 6 documentation's claim that this cleanup preserved the map
was incorrect. `dat/moloch.lua` now explicitly adds those four newlines.
This restores the intended Step 6 geometry without changing its population,
rewards or placement. Both Release builds were rerun after this correction;
the evaluated-map test and actual Temple generation/traversal passed.

## Tests and executed results

| Test/gate | Result |
| --- | --- |
| `test/run_step7.py` + `test/test_step7_runtime.c`, x64 and x86 | PASS: 2,000 scheduler samples per architecture, 168 distinct Tomb depths, no reservation collisions, all Step 6 counts and branch depth semantics; real monster/object tables, Shadow non-random exclusion, 100 lighting cycles with cleanup, finite Candelabrum, candle usage charge and unchanged magic-lamp charges. |
| `test/test_step7_content.lua` | PASS: 64 pinned-donor/local executions comparing geometry and every content call, all path/reward outcomes; evaluated Moloch map 56x9. |
| `test/test_step7_source.py` | PASS: protected Step 5/6 sources and wishing functions, only intended monster/tool additions, one classic Tomb, manifests, epoch, and final DLB resource byte comparisons on both architectures. |
| `test/run_step7_topology.py` | PASS: five fresh packaged x64 games (Tomb DL112,54,93,128,82) and three Win32 games (DL139,110,32); exactly one Tomb/Temple, varying legal entrances, 3-5 Big Rooms, Castle200 and ordinary depth display. |
| `test/test_step7.lua`, real x64 wizard game | PASS: one return stair to DoD123, 8 chests, 8 locked, 2 trapped, 3 wax candles, 5 pits in the tested seed; checks passed inside Tomb before and after reload. Actual census: 14 Shadows. |
| Real branch traversal/save | PASS: entered from DoD123, saved inside Tomb with a lit Magic Candle, reloaded, candle still lit without timer, returned by the actual stair to DoD123. |
| `test/test_step7_candle.lua`, `test/test_step7_container.lua` | PASS in the prepared game: normal apply/snuff/relight, lit save/reload, dropped/picked up while lit, then put into sack and confirmed snuffed with no timer. |
| `test/test_step7_candelabrum.lua` | PASS: seven attached Magic Candles became finite fuel, age600 unlit; normal apply produced a finite burn timer. |
| Existing Step 6B runtime checks | PASS: actual `test/test_step6b.lua` on x64 in corrected Temple (9 chests, wax caches, one return stair), Big Room68 and Tomb-parent DoD123; actual Temple return to DoD44 succeeded. The Step 7 native harness additionally checks Step 6 scheduling counts on both architectures. |
| Existing depth-range tests rebuilt via `test/run_depth_range.py` | PASS x64/x86 at current incarnation `0x05000002`, including the original serialization and depth tests. |
| Existing `test/run_ledger_runtime.ps1` with current `recover.exe` | PASS x64/x86: 225 packed pairs, numeric ordering, teleport endpoints, round trips through ledger3199, internal recovery, standalone recovery, invalid bounds and old-version rejection. |
| Full Visual Studio solution Release build | PASS x64 and Win32, including console, GUI/tile and packages. Executable timestamps were checked in `binary/Release/x64` and `binary/Release/Win32`; the final documentation-only dungeon comment was repackaged afterward. |
| `git diff --check` and final source audit | PASS; new files were also checked for trailing whitespace. No generated output is included. |

The native harness extracts current production functions and links the actual
monster/object databases, with fixture RNG/list/light adapters. It is not a
whole-game combat simulation. Fresh packaged game tests complement it. Compiler
conversion/unreachable-code warnings occurred in the harness; compilation and
assertions passed.

The initial Tomb inspector allocated userdata for every map square and forced
collection, producing Lua `__gc` memory warnings. It now walks `obj.next()`'s
floor chain once, recursively inspecting contents, preserving all assertions.
Repeated final runs passed without new GC warnings. No production Lua allocator
change was made. Inventory Lua tests have explicit setup preconditions; running
the container check immediately after restoring the earlier pre-container save
correctly failed that precondition, then it was rerun after normal sack setup.

Audit logs, topology JSON and isolated wizard-game recordings are outside the
repository in `../step7-audit/`. No real player save was used. Test-only wizard
setup removed monsters after the Tomb census to isolate inventory operations;
the production level content was not changed.

### Reproduction

From an x64 or x86 Visual Studio developer shell, use a separate output directory
for each architecture:

```text
python test/run_step7.py ../step7-audit/focused-ARCH
python test/run_depth_range.py ../step7-audit/depth-current-ARCH
powershell -NoProfile -File test/run_ledger_runtime.ps1 -OutputDirectory ../step7-audit/ledger-ARCH -RecoverExecutable binary/Release/PLATFORM/recover.exe
lua test/test_step7_content.lua PINNED_DONOR_TOMB dat/tomb-1.lua dat/moloch.lua
python test/test_step7_source.py binary/Release/x64/nhdat500 binary/Release/Win32/nhdat500
python test/run_step7_topology.py binary/Release/PLATFORM NEW_OUTPUT_DIRECTORY 5
MSBuild sys/windows/vs/NetHack.sln /m /p:Configuration=Release /p:Platform=x64 /v:minimal
MSBuild sys/windows/vs/NetHack.sln /m /p:Configuration=Release /p:Platform=Win32 /v:minimal
git diff --check
```

`ARCH` is x64/x86, `PLATFORM` is x64/Win32. Export `PINNED_DONOR_TOMB` with
`git show` at the revision above. Content tests require Lua5.4; topology runner
requires `pywinpty` and `pyte`. In a fresh wizard game, use `#wizwhere` to locate
the Tomb parent, then the actual branch stair. Load the Lua runtime checks with
`#wizloadlua` after satisfying their file-header setup instructions.

## Changed-file inventory

Documentation: `README`, `doc/step7.md`.

Map/topology: `dat/dungeon.lua`, `dat/tomb-1.lua`, `dat/moloch.lua`,
`src/dungeon.c`.

Shadow: `include/monsters.h`, `include/mondata.h`, `include/display.h`,
`src/makemon.c`, `src/mon.c`, `src/monmove.c`, `src/dokick.c`, `src/dothrow.c`,
`src/mhitu.c`, `src/uhitm.c`, `src/weapon.c`.

Magic Candle: `include/objects.h`, `include/obj.h`, `src/mkobj.c`, `src/apply.c`,
`src/timeout.c`, `src/objnam.c`, `src/shk.c`, `src/zap.c`, `src/invent.c`.

IDs/art/packaging: `include/patchlevel.h`, `win/share/monsters.txt`,
`win/share/objects.txt`, `sys/unix/Makefile.top`, `sys/windows/Makefile.nmake`,
`sys/windows/vs/files.props`.

Tests: `test/test_ledger_runtime.c`, `test/run_depth_range.py`,
`test/run_step7.py`, `test/run_step7_topology.py`, `test/test_step7.lua`,
`test/test_step7_candle.lua`, `test/test_step7_container.lua`,
`test/test_step7_candelabrum.lua`, `test/test_step7_content.lua`,
`test/test_step7_runtime.c`, `test/test_step7_source.py`.

## Remaining manual playtest recommendations

Long-form combat against Shadows, visual inspection in the tiled GUI, and
ordinary shop buying/selling/stacking play are useful follow-up playtests.
These were not exhaustively exercised interactively. Definition/function tests
and the recorded console traversal/item tests cover the implementation gates;
this is not a claim of exhaustive balance, visual or every-state gameplay testing.

## Final Git state

The 43 paths listed above are the complete Step 7 closeout change set. No
generated artifacts are included. The closeout commit and annotated milestone
tag are created from this change set after the validation gates above pass.
