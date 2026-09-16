# Step 11: recurring custom rooms

## Scope and starting point

Implemented on `phase0/dod-length`, starting from clean commit
`fdc824dfd95e96fbafcbf66177b2fd674818f35b`. CodeGraph exploration preceded
direct source inspection. Changes remain uncommitted and unpublished. User
playtesting has not occurred. Only x64 is a mandatory validation target.

The source inventory found Giant Court, Real Zoo and Dragon Lair. Dilapidated
Armory and Lemure Pit have no implementation in `src`, `include` or `dat`;
both remain deferred. No other donor rooms or donor systems were imported.

| Saved ID | Feature | Backend/base type | Additional inclusive eligibility |
|---:|---|---|---|
| 1 | Giant Court | classic / COURT | DoD DL30–199, before Medusa |
| 2 | Real Zoo | classic / ZOO | DoD DL30–199, before Medusa |
| 3 | Dragon Lair | classic / ZOO | DoD DL30–199, before Medusa |
| 4 | Wizard Study | Lua / THEMEROOM | difficulty ≥14 |
| 5 | Storeroom Vault v1 | Lua / THEMEROOM | difficulty ≤14 |
| 6 | Super Honeycomb | Lua / BEEHIVE | difficulty ≥13 |
| 7 | Dragon Hall | Lua / THEMEROOM | difficulty ≥21 |

Every candidate uses the inherited **300/10,000** probability. There is no
global DL30 minimum. Difficulty, DoD dlevel and logical depth are snapshotted
separately. Special/proto/filler maps, Big Rooms, Rogue levels, maze paths,
branch interiors and parents, quest/endgame maps, Castle and Medusa never
enter this lottery. The ordinary random path below Medusa can admit the new
Lua rooms; the three classic rooms retain their pre-Medusa restriction.

## Selection and placement

`include/customroom.h` defines stable IDs and descriptor/context/state types.
`src/mklev.c` owns the seven-entry registry, pure production categorical
partition, single generation draw and backend coordination. Each eligible
candidate owns exactly 300 disjoint outcomes; none owns the remainder.
Empty sets select none. Validation rejects duplicate IDs, invalid targets,
bounds and probabilities. The complete eligible mass is validated before
returning a result, so an overfull registry fails rather than normalizing or
favoring earlier entries. Zero in a descriptor's probability field inherits
the default; it is not a disable flag.

Selection happens once after committing to an ordinary DoD generation path.
There are no occurrence quotas, consumed bits, cooldowns, failure debts,
later reservations or alternate-candidate selection. Different levels may
repeat the same feature indefinitely. Selected ID, attempts, committed
emissions, progress and vanilla opportunity/placement counts are transient.
States distinguish idle, selected, clean failure, fill pending, complete
and partial error. No diagnostic counter consumes random numbers.

Lua features execute first through the existing per-dungeon themed Lua state
and level-description machinery. Their separate local descriptor table is
absent from the ordinary weighted room/fill pools. All target names are
resolved; missing resources or targets are fatal integration errors.
Ordinary themed-room generation continues afterward. Stack depth, Lua flags,
placement origin and failure status are restored between attempts.

The classic adapter runs after the unchanged vanilla special-room chain and
before fillable-room counting, bonus loot and deferred filling. It uses the
native bounded host search and then an exhaustive search of remaining joined,
ordinary rooms without stairs. It returns the actual converted room. The
old emergency stair-room fallback is deliberately removed. Vanilla `mkzoo`
never uses a custom fallback. Successful custom rooms fill exactly once.

A selected Lua feature may make at most three top-level attempts. A failed
attempt may retry only if terrain, room counts and payload lists show no
commit. For these four constructors, failed fit happens before their contents
callbacks. A committed terrain/room/payload failure is fatal, never a clean
retry. Classic exhaustive failure establishes that no legal host exists and
stops after one attempt. No whole-level regeneration is used. At most one
custom feature may be committed; repeated backend dispatch cannot emit again.

The former `x6b-*` custom-room reservations, level-wide subtype query and
generation precedence are removed. The remaining `step6b_schedule()` still
owns its valid branch/Big Room/rebasing responsibilities. Removing reservations
changes historical seeded branch locations, as allowed by the Step 11
contract. The 200-level DoD, Castle DL200, Medusa DL196–199, capacities,
branch counts/connections, 3–5 Big Rooms and Steps 7–10 content remain intact.
The shop decision expression, stock/type tables and 10% mimic cap are unchanged.

## Durable identity and compatibility

`struct mkroom` now has a dedicated saved `unsigned char custom_id`. Zero is
vanilla; IDs 1–7 above are explicit values independent of array order. Normal
constructors and level clearing reset it, including subroom storage. Sorting
moves the field with its owning record. Recursive room codecs save/restore it
with the room, reconstructing subroom pointers normally. No retained pre-sort
pointer supplies identity.

The actual `Sfo_mkroom`/`Sfi_mkroom` codec serializes the room structure;
`save_rooms`/`rest_rooms` recurse through subrooms. Level saves, full saves,
revisit, recovery and accepted bones preserve the field. Loading and accepted
bones clear transient generation state and do not select again. Custom fill,
rewards and entry text consult the owning room, so another COURT/ZOO/BEEHIVE
remains vanilla. `rtype`/`orig_rtype` and native discovery/shared flags retain
their roles; persistent identity does not repeat first-entry messages.

**A new game is required. `EDITLEVEL` advances from 5 to 6.** Epochs 0–5 and
future incompatible versions are rejected by the native version gate,
including bones/recovery headers. There is no migration and no compatibility
claim based on unchanged padding or structure size. Existing user saves and
bones were neither removed nor overwritten. Tests use isolated fixture files.
No monster, object, artifact, dungeon, terrain, trap or base room-type IDs change.

## Donor and adaptations

Source: [copperwater/xNetHack, pinned themerms.lua](https://github.com/copperwater/xNetHack/blob/6eef39403f16f65e13f5d57242ee8d036307687a/dat/themerms.lua),
commit `6eef39403f16f65e13f5d57242ee8d036307687a`.
Downloaded file SHA256:
`09ea97b7603212af1181f4ef9630f79ca95bdc410656ff773db6daf0c45d5c11`.
Pasi Kallinen's copyright and NetHack license notice are retained.

- `Wizard study`: unchanged disconnected 3×3 body, one teleport trap, three
  books, scrolls and other donor items. The center stays available. Native
  antimagic or an active stasis effect can suppress teleport traps as usual.
- `Storeroom vault`: unchanged first 2×2 variant, 1–3 native chests and the
  minimal donor `way_out_method(true)` helper. The helper supplies a hole,
  teleport trap, digging tool/wand, teleport scroll or ring. Its item table
  is locally scoped. Native dig/teleport/fall restrictions remain in force.
- `Super Honeycomb`: exact ASCII map and spanning-tree algorithm connect all
  seven cells; one joined irregular region gets native BEEHIVE filling.
- `Dragon hall`: exact map and loot/egg/trap bodies. The requested fixed
  difficulty 21 replaces `nh.mon_difficulty`. `loot` and the color list are
  local. Gold dragons are absent locally, so the list uses the nine existing
  native colors. Intersecting the hoard circle with the owned floor prevents
  monster/loot coordinates outside the irregular room. Hoard probabilities,
  quantities, recursive loot, eggs, traps, waiting flags and 4–6 baby / 6–10
  adult creation requests are preserved. Native extinction/genocide and
  creation failure may reduce actual monster counts; no bypass is added.

The donor's existing note about dragons later picking up hoard loot still
applies; no AI or balance change is included. Storeroom v2, Library and all
other donor templates are excluded.

## Automated validation

Evidence is retained locally under `_qa/step11/` (ignored generated output).
Native fixtures link the real generator, native ISAAC RNG, Lua interpreter,
constructors, fillers, codecs, entry and billing code. Extracted legacy
fixtures supplement these tests; they are not the gameplay evidence alone.

From the repository root, use a Visual Studio x64 developer shell and Python:

```powershell
$msbuild = 'C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/MSBuild/Current/Bin/MSBuild.exe'
& $msbuild sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /p:STEP11_TEST=true /p:STEP9_TOPOLOGY_TEST=true /p:STEP10C_C_TEST=true /p:STEP10C_D_TEST=true /p:STEP10QA2_TEST=true /v:m /m:4
python test/run_step11.py binary/Release/x64 _qa/step11/gates-final
python test/run_step11.py binary/Release/x64 _qa/step11/probability-final --phase probability
```

Gate A passes exhaustive enumeration of all 10,000 outcomes across an 18×18
depth/difficulty boundary matrix, including differing difficulty/depth,
empty/mixed/reordered sets, a 137-bp test override, duplicates, malformed
entries and 34×300-bp overflow. Native exclusions cover explicit maps, every
branch family, proto/filler, quest/endgame, maze, Rogue, Big Rooms and parents.
Repeat backend calls do not emit twice. Actual accepted `mklev/getbones`
loading retains identity with zero lottery draws.

Gates B/C pass all seven forced features, three different levels per variant,
Giant+court, Zoo+zoo, Lair+zoo, Honeycomb+beehive, and each feature with a shop.
Checks include connected owned geometry, stairs, disconnected walls/doors,
native teleport escape, fall/dig restrictions, population, waiting flags,
objects/stack quantities, giant coffer contents, gold, chests, eggs/traps,
fill completion, shopkeeper/stock and actual add/remove billing operations.
Honeycomb has 68 connected owned floor cells across its seven chambers.
Hall request counts are checked at native monster creation, independently
of successful creation counts. First-entry discovery does not erase IDs,
repeat entry does not rediscover the room and same-base level flags remain
set while another room still needs discovery.

Native level and bones codec roundtrips occur before and after discovery.
A separate constructor/sort/recursive-subroom test verifies zero initialization,
identity ownership, reconstructed pointers and reused records. Real bounded
Lua placement searches are deliberately obstructed: three clean attempts,
no payload, no replacement candidate. A failure after actual geometry commit
reports partial error on attempt one and stops (expected fixture exit 86).

The full wizard-game test uses ordinary executable save/restore, later
generation, leave/revisit and actual `recover.exe` output restoration:

```powershell
# Dependencies: pyte and pywinpty (installed locally in _qa/step11/python).
$env:PYTHONPATH = "$PWD/_qa/step11/python"
python test/run_step11_save.py binary/Release/x64 _qa/step11/normal-save
```

Earlier full-game evidence is in `pty-save.log`, `actual-recovery.log` and
`actual-recovery-restore.log`; the reusable script includes those recovery
steps. Isolated test scaffolding corrections were required for fruit IDs,
hero form/billing state, merged item stack quantities, the move-1 stasis
boundary, and avoiding reuse of entered-shop hero state for teleport probes.
These changes did not adjust selection probabilities, production room loot,
geometry or the predeclared probability corpus.

Gate E's affected regression commands and evidence:

```powershell
$tests = 'run_step10c_b_scheduler','run_step7','run_step10b_compatibility','run_depth_range','run_step10b_ogre','run_step10b2_1','run_step10b2_2','run_step10b2_3','run_step10b2_4','run_step10b3_1','run_step10b3_2','run_step10b3_3','run_step10b4'
foreach ($t in $tests) { python "test/$t.py" "_qa/step11/regression/$t" }
python test/run_step9_topology_native.py _qa/step11/diagnostic-final/NetHack.exe _qa/step11/protected/native-topology 16
python test/run_step10c_c_focused.py _qa/step11/diagnostic-final/NetHack.exe _qa/step11/protected/outlands-complete
python test/test_step10c_b_source.py
python test/test_step10c_c_source.py
python test/test_step10c_d_source.py
python test/test_step10qa2_source.py
python test/test_step9_scheduler_contract.py
python test/test_step9a_tiles.py
powershell -NoProfile -ExecutionPolicy Bypass -File test/run_ledger_runtime.ps1 -OutputDirectory _qa/step11/protected/recovery-final -RecoverExecutable binary/Release/x64/recover.exe
```

All listed regression groups pass. Scheduler fixtures retain Castle/Medusa,
depth, branch, collision, persistence and 3–5 Big Room assertions; only obsolete
custom quotas become zero. The native topology run adds 16 complete generated
games. Protected content fixtures exercise all 26 Step 10 maps and the
topology save matrix using the diagnostic executable:

```powershell
# Run in its isolated directory with matching nhdat500; clear each variable afterward.
$env:NETHACK_STEP10QA2_TEST='1'; ./NetHack.exe
Remove-Item Env:NETHACK_STEP10QA2_TEST
$env:NETHACK_STEP10C_D_TEST='1'
$env:NETHACK_STEP10C_E_MATRIX='1'
$env:NETHACK_STEP10C_E_PERSISTENCE='1'
$env:NETHACK_STEP10C_E_SAVE='H:/app/mynethack/YarivNetHack/_qa/step11/protected/topology-save.bin'
./NetHack.exe
Remove-Item Env:NETHACK_STEP10C_D_TEST,Env:NETHACK_STEP10C_E_MATRIX,Env:NETHACK_STEP10C_E_PERSISTENCE,Env:NETHACK_STEP10C_E_SAVE
```

See `regression/results.json`, `protected/results-final.json`, `10qa2.log`,
`10cd.log`, `native-topology.log`, `outlands-complete.log` and
`recovery-final.log`. Old epoch-5 fixture expectations were updated to 6.
Outlands retains its original nine seeds and all payload assertions; updated
depth-dependent vectors plus fixed seeds 100046, 100052 and 100123 restore
the same focused coverage after reservation removal. Exploratory seed 100090
selected a village without placing its residents; it was not adopted as a
payload fixture. This coverage search is separate from the untouched natural
probability corpus. No production Outlands function was changed.

## Adding a future compatible room

For a hypothetical Library (not implemented): reserve a new explicit nonzero
saved ID, adapt only its constructor/content and dependencies, add its named
target to the separate custom Lua table or a narrow classic adapter, then add
one registry descriptor with default probability and independent eligibility
bounds. Preserve base room classification and owned-room identity. Validate
target resolution, total eligible mass, clean-failure behavior, content,
coexistence, persistence and placement rates. The selector algorithm needs no
change. A constructor emitting multiple native room records would also need
an explicit ownership adapter and tests; the current Lua adapter deliberately
requires one room record, which covers all four included templates.

## User playtest guide

Extract the normal ZIP into a **new directory**, create `sysconf` there with
`WIZARDS=*` and `PORTABLE_DEVICE_PATHS=1`, then start a new wizard game:

```powershell
$env:CUSTOMROOM='4'  # 4 Study, 5 Storeroom, 6 Honeycomb, 7 Hall
./NetHack.exe -D -u wizard
```

Use Ctrl-V to enter an unvisited ordinary DoD level. Try DL14–20 for Study,
DL2–14 for Storeroom, DL13+ for Honeycomb, or DL21+ for Hall; difficulty still
controls eligibility. Special/branch-parent levels remain excluded, so try
another fresh depth if necessary. These are convenient playtest ranges, not
fixed production reservations. `#wizcustomrooms` reports only current-level
IDs, bounds and current/original types, without RNG or generating any levels.
Wizard map reveal/teleport can be used to inspect the reported room.

Check Study's books and teleport exit (remove antimagic when testing a trap),
Storeroom's 1–3 chests and escape provision, the Honeycomb's seven connected
chambers and native bees, and Hall's hoard/eggs/traps/waiting dragons. For
recurrence, keep the same forcing value and visit several eligible fresh
levels. Save before entering a room, restore, enter/discover it, save again,
restore, leave and revisit; identity/content should persist without repeat
entry text. Inspect ordinary stairs and corridors as well.

For same-base coexistence, launch fresh games with `CUSTOMROOM=1,2,3,6` and
`SHOPTYPE=t,z,z,b` respectively (one pair per game). For shops, use
`SHOPTYPE=g` with any custom ID. The classic rooms require DL30+ before
Medusa. A native vanilla constructor may occasionally find no host; inspect
another eligible level. Check that only the custom room gets custom messages
and rewards and that shop billing works. Remove `CUSTOMROOM` and `SHOPTYPE`
from the launching environment for randomized play. `THEMERM` and
`THEMERMFILL` are not production custom-room interfaces.

Human visual/gameplay review, long campaigns, every possible role/extinction
state, and Win32 runtime remain outside the completed automated sample.
Statistical bounds below apply to the declared generation strata and game
profile; they are not a proof of perfect placement for every possible game.


## Probability and placement results (Gate D)

The predeclared corpus uses seeds **110001–111000**, one independent fresh
process/game per seed. Each generates ordinary eligible DoD levels 1–14 and
40–57 in order, with hero level 1. Protected levels are skipped, producing
25,830 ordinary opportunities. Wizard mode, CUSTOMROOM, SHOPTYPE, THEMERM,
THEMERMFILL and unrelated test overrides are disabled. The diagnostic hook
only records counts and validates generated content; it adds no RNG draws.
The fixed corpus supplies shallow vault and deep mixed-backend strata.

The separate inexpensive 100,000-draw production-selector check at difficulty
50 yielded: none 82,212; Giant 3,055; Zoo 2,930; Lair 2,962; Study 2,880;
Honeycomb 2,987; Hall 2,974. Vault is ineligible in that stratum; exhaustive
boundary enumeration proves its 300 outcomes in shallow eligible contexts.

| Feature | Eligible | Selected | Attempts | Completed | Failures | Selection / appearance | Conditional success |
|---|---:|---:|---:|---:|---:|---:|---:|
| Giant Court | 16,865 | 545 | 545 | 545 | 0 | 3.232% | 100% |
| Real Zoo | 16,865 | 501 | 501 | 501 | 0 | 2.971% | 100% |
| Dragon Lair | 16,865 | 510 | 510 | 510 | 0 | 3.024% | 100% |
| Wizard Study | 17,677 | 516 | 516 | 516 | 0 | 2.919% | 100% |
| Storeroom Vault v1 | 8,965 | 278 | 278 | 278 | 0 | 3.101% | 100% |
| Super Honeycomb | 18,447 | 588 | 588 | 588 | 0 | 3.188% | 100% |
| Dragon Hall | 16,865 | 469 | 469 | 469 | 0 | 2.781% | 100% |

There were **3,407 / 25,830 = 13.190%** any-custom levels, zero double emissions
and zero natural placement failures. Every selected feature completed in one
top-level attempt. The vanilla decision chain attempted conversion on 21,244
levels and converted 16,664 room records (a swamp may convert several).
Shop attempts (base type 14): 6,714; successful shop conversions: 3,408. These counts describe this corpus, not a probability change.

Selection intervals use the predeclared z=3.2 Wilson interval and a game-cluster
ratio standard error (one residual per game). Both contain 3% for every room.
The conservative z value accounts for the seven comparisons. For placement,
zero-failure one-sided exact lower bounds use Bonferroni alpha=0.05/7. To
avoid treating repeated placements in one game as independent evidence, a
second bound uses only the first naturally selected room in each game.
All seven first-per-game lower bounds exceed the initial 98% engineering
target; the vault is closest, at 98.011%. Bounds describe finite-sample
uncertainty, rather than requiring an observed rate of exactly 3.000%.

| Feature | Wilson selection interval | Game-cluster selection interval | First-selected games | All-trial placement lower | First-per-game placement lower |
|---|---|---|---:|---:|---:|
| Giant Court | 2.823–3.696% | 2.810–3.654% | 439 | 99.097% | 98.881% |
| Real Zoo | 2.580–3.418% | 2.557–3.384% | 401 | 99.018% | 98.775% |
| Dragon Lair | 2.630–3.475% | 2.599–3.449% | 398 | 99.036% | 98.766% |
| Wizard Study | 2.540–3.352% | 2.496–3.342% | 394 | 99.047% | 98.754% |
| Storeroom Vault v1 | 2.567–3.742% | 2.513–3.689% | 246 | 98.238% | 98.011% |
| Super Honeycomb | 2.799–3.628% | 2.778–3.597% | 449 | 99.163% | 98.905% |
| Dragon Hall | 2.404–3.216% | 2.375–3.187% | 376 | 98.952% | 98.694% |

Recurrence distribution: number of games with 0, 1, 2, 3 or 4 completed
occurrences in the sampled levels (each row sums to 1,000):

| Feature | 0 | 1 | 2 | 3 | 4 |
|---|---:|---:|---:|---:|---:|
| Giant Court | 561 | 349 | 75 | 14 | 1 |
| Real Zoo | 599 | 313 | 76 | 12 | 0 |
| Dragon Lair | 602 | 301 | 83 | 13 | 1 |
| Wizard Study | 606 | 296 | 77 | 18 | 3 |
| Storeroom Vault v1 | 754 | 217 | 26 | 3 | 0 |
| Super Honeycomb | 551 | 326 | 110 | 10 | 3 |
| Dragon Hall | 624 | 295 | 69 | 12 | 0 |

Evidence: `probability-final/corpus.json`, `levels.json`, `results.json` and
all 1,000 individual seed logs; `gates-final/selector.log` holds the selector
check. The earlier run exposed a fixture assertion that counted item stacks
instead of units; the corrected complete corpus was rerun without omitting
seeds. No probability, acceptance threshold, seed range or denominator was
adjusted after observing the results. No confirmation batch was needed.

## Normal x64 Release and final checks

After diagnostic builds, the ordinary solution was rebuilt without test
properties. The user-facing executables do not contain the STEP11_TEST entry
point or clean/partial failure hooks. The saved diagnostic executable remains
only in `_qa/step11/diagnostic-final` for reproducing native tests.

```powershell
& $msbuild sys/windows/vs/NetHack.sln /p:Configuration=Release /p:Platform=x64 /v:m /m:4
python test/test_step11_resources.py _qa/step11/donor/themerms.lua binary/Release/x64 vspackage/nethack-500-win-x64.zip
git diff --check
Get-FileHash binary/Release/x64/NetHack.exe,vspackage/nethack-500-win-x64.zip -Algorithm SHA256
```

The resource check compares the four complete donor bodies with only explicit
Hall adaptations, checks 199 protected source/resource files against the
starting commit, verifies the unchanged vanilla decision expression, compares
all 186 packaged Lua resources (including map whitespace), and checks ZIP
executable/GUI/DLB bytes against the ordinary Release directory. Recovery is
built/tested separately; the established ZIP manifest does not include
`recover.exe`. Build logs include existing warnings in unchanged `mkmap.c`
about possibly uninitialized local x/y; Step 11 adds no build errors.

Evidence: `build-normal-final.log`, `resources-final.log`, `normal-save.log`.
Normal artifact SHA256 values:

- `H:/app/mynethack/YarivNetHack/binary/Release/x64/NetHack.exe`
  SHA256 `057ecc3a52c63f1d4e4cc11aa858f902d7a831ca6be50d27afcf9c5c095aa78d`
- `H:/app/mynethack/YarivNetHack/vspackage/nethack-500-win-x64.zip`
  SHA256 `2a8724610c3dae9e13d76a55a5e70b8f79ea75d69f9390f6ac44799e1f274b01`
