# Step 8B: Ruins of Moria final integration

Step 8A was a temporary manual-validation phase.  It pinned the Moria
entrance to DoD107 so the six-floor branch could be traversed and its imported
mechanics inspected.  Step 8B removes every DL107-specific path and makes the
branch use the persistent dungeon-topology scheduler.

## Provenance and scope

- Donor: [UnNetHack](https://github.com/UnNetHack/UnNetHack), `dev` history,
  pinned at `439b8d63d3d1ca78fb08588dd43f61874114b21a`.
- Six upward branch levels are represented by ten Lua maps:
  `moria1-1`, `moria2-1`, `moria3-1`, `moria4-1` through `moria4-4`,
  `moria5-1`, and `moria6-1` through `moria6-2`.
- Exactly one branch is created per game. Its DoD entrance is selected once at
  a legal random depth from DL30 through DL199, then saved with normal branch
  topology. There is no independent roll for any floor.
- The six Moria levels occupy the entrance depth minus one through minus six.
  Four orc halls and two terminus maps retain their donor variant weights, and
  the final downstairs returns to the reserved DoD entrance.

## Imported content and mechanics

The port preserves the donor's maps, terrain, monsters, objects, loot and
special behavior while using NetHack 5's existing save and level machinery.
It includes deep orcs; Durin's Bane; the Watcher in the Water; swamp fern,
sprout and spore life cycles; iron safes; unrefined mithril; Balin's grave and
ordinary cursed battle axe; the Earthstone sapphire artifact and its consumed
unpaired portal; dead trees; muddy bogs; outdoor sky handling; ambient sounds;
and the exact Moria monster-generation tables. The Endless Stair, broken
bridge, regenerating seven-room floor, four orc halls, forest and barracks,
and intact or ruined Doors of Durin maps retain donor geometry, stairs,
populations, containers, engravings and reward cases. Existing magic-lamp and
wishing behavior is unchanged.

The new monster, object, artifact, dungeon and terrain IDs advance
`EDITLEVEL` to 3. No new save-format fields were added; branch identity,
nonpersistent-level state, bog and portal behavior are derived from existing
level, object and trap data. The frozen 200-level Dungeons of Doom, ledger
capacity and ordinary depth semantics remain unchanged, with Castle at DL200.

## Placement and collision contract

At new-game initialization the scheduler reserves Moria's DoD endpoint from
the same free pool used by the Step 5/6/7 features. It excludes all existing
special levels, branch endpoints, Big Rooms, Giant Court, Real Zoos, Dragon
Lair, Temple of Moloch and Lost Tomb reservations. The selected endpoint is
persisted, so revisits and save/reload never reroll it. The final contract is:

- exactly one six-level upward Ruins of Moria branch;
- one legal entrance in DL30–199, varying across fresh games;
- no collision with any other reserved depth or branch entrance; and
- Castle remains DL200.

The Step 8A DL107 fixture and all `DL107`, `DoD107`, and `101–106` production
or test assumptions were removed before this final integration.

## Validation record

The final matrix covers both architectures and the complete imported branch:

- focused source, content and database tests for all ten Lua resources, donor
  variant weights, terrain, monsters, objects, artifacts and mechanics;
- fresh packaged topology sampling on x64 and Win32, confirming exactly one
  Moria, varying legal DL30–199 entrances, six relative depths, no reservation
  collisions, unchanged Step 5/6/7 occurrence rules, and Castle DL200;
- complete six-level traversal in both directions with bridge and terminal
  save/reload checks;
- Durin's Bane and Watcher uniqueness/reward persistence, fern lifecycle,
  bog/tree interactions, Earthstone invocation and portal persistence, iron
  safe occupation, Balin's grave, and Step 7 Tomb/Temple regressions;
- x64 and Win32 depth-range and ledger/recovery tests;
- x64 Release and Win32 Release builds; and
- DLB byte checks proving all ten Moria maps are packaged.

The focused runners are invoked from a Visual Studio developer shell:

```text
python test/run_step8a.py OUTPUT_DIRECTORY
python test/run_step8a_topology.py binary/Release/x64 OUTPUT_DIRECTORY 8
python test/run_step8a_runtime.py binary/Release/x64 OUTPUT_DIRECTORY
```

The same topology, runtime, depth and ledger checks are run against
`binary/Release/Win32`. Packaged fixtures are copied to an external
audit directory and are not part of the repository.

The implementation branch is `phase0/dod-length`. The Step 8 commit and
annotated tag are created after the final diff, `git diff --check`, both builds,
topology sampling, traversal/save checks, regressions and DLB audits pass.
