# Phase 1 Step 15A — forge foundation

Step 15A implements forge terrain, natural placement, and war-hammer activation.
Step 15B is the future recipe and crafting-transaction phase. No recipes,
ingredients, crafted objects, inheritance, repairs, charges, cooldowns, smithing
skills, environmental forge interactions, or recipe UI are implemented.

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
room restrictions. A standalone `des.finalize_level` does not select forges.

**EDITLEVEL 8 → 9** rejects old saves and bones through the existing version gate.
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

Eligibility is main Dungeons of Doom, absolute DL20–199 inclusive, excluding
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
Valid activation says “You know no forge operations yet.” and returns
`ECMD_TIME`, one normal action. It changes no item, enhancement, or terrain.
There is no placeholder menu.

## Validation evidence

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
150000–150999 (display seeds 160000–160999). Requested depths cycle over 20–199;
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
