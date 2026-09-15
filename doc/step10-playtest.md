# Step 10D user manual validation guide

This is the user-executable handoff for the Step 10 Neutral Quest, Lost
Cities, and Dispensary playtest. Automated prerequisites are green, but every
manual result below starts as **PENDING**. Only the user may change a manual
row to `PASS`, `FAIL`, or `BLOCKED` and approve Step 10D.

## 1. Prerequisites and frozen artifacts

| Automated prerequisite | Exact value | Status |
|---|---|---|
| Repository | `E:\Codex\My_Nethack`, branch `phase0/dod-length`, HEAD/origin `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe` | PASS |
| Console executable | `E:\Codex\My_Nethack\binary\Release\x64\NetHack.exe`; 6,020,096 bytes; SHA-256 `CE2A42BDE23F7676FC5C5035D9C3242402705C1955CAF22394A59B3208FC108F` | PASS |
| Tiled executable | `E:\Codex\My_Nethack\binary\Release\x64\NetHackW.exe`; 7,340,544 bytes; SHA-256 `89A2742F72A23B37F0727BA74CFE86968654AF890E9913B40F11234D479B7179` | PASS |
| Data archive | `E:\Codex\My_Nethack\binary\Release\x64\nhdat500`; 1,775,240 bytes; SHA-256 `EC5448960207CBF9FBD8C0A0C95BF16DDCFC8DE23CC4DDDD088FBD9E03B3AE03` | PASS |
| Distribution package | `E:\Codex\My_Nethack\vspackage\nethack-500-win-x64.zip`; 5,230,714 bytes; SHA-256 `B62EB2A192496F9355C884134CC51A82A0C046FB17DF216E5D74F37FBC616C95` | PASS |
| Generated identities | `NUMMONS=503`; `NUM_OBJECTS=547`; artifact sentinel/count `47/46`; `EDITLEVEL=5`; `MAX_GLYPH=12410`; tiles `2867`; `MAXDUNGEON=18`; actual dungeons `17`; `MAXLINFO=3600` | PASS |
| Focused pre-manual gate | Fresh native matrix: 26 resources, five Dispensary parents, four alternate groups/eight variants; scheduler/topology and save/restore/revisit smoke passed | PASS |

Use a new EDITLEVEL 5 wizard game. Pre-Step-10 saves are unsupported. Do not
use root-level binaries, Win32, or a primary non-wizard save for this test.
The package stores Lua resources inside `nhdat500`; they are not loose ZIP
entries.

## 2. Wizard startup and verified commands

Launch from the directory containing `NetHack.exe`, `NetHackW.exe`, and
`nhdat500`:

```powershell
$gameDir = 'E:\Codex\My_Nethack\binary\Release\x64'
Start-Process -FilePath "$gameDir\NetHack.exe" -ArgumentList '-D','-u','wizard' -WorkingDirectory $gameDir -Wait
# Use this instead for the tiled window port:
Start-Process -FilePath "$gameDir\NetHackW.exe" -ArgumentList '-D','-u','wizard' -WorkingDirectory $gameDir -Wait
```

`-D` enables wizard/debug mode; current Windows startup then authorizes and
sets the player name to `wizard`. There is no production RNG-seed option.

| Purpose | Verified command/binding | Exact use |
|---|---|---|
| Show topology | `#wizwhere` | Lists placed special levels and branches; no control-key binding |
| Level teleport | `#wizlevelport` or `Ctrl-V` | At the prompt enter a number/name, or `?` for the selectable dungeon menu. Prefer the extended command in Windows Terminal. |
| Reveal map | `#wizmap` or `Ctrl-F` | Reveals terrain, traps, and engravings on the current level |
| Create monster | `#wizgenesis` or `Ctrl-G` | Enter an exact monster name or symbol; a numeric command prefix creates several |
| Wish/create item | `#wizwish` or `Ctrl-W` | Enter an exact object or artifact description; `m` prefix shows wish history |
| Recreate current level | `#wizmakemap` | Discards and regenerates the current level; use only in a disposable game |
| Load one map resource | `#wizloaddes` | Enter an exact resource filename such as `leth-a-2.lua`; use only in a disposable game and never count it as traversal |
| Identify inventory | `#wizidentify` or `Ctrl-I` | Identifies inventory for reward/property inspection |
| Save and exit | `S` or `#save` | Save normally, exit, relaunch, and restore |

The command table is source-verified in `src/cmd.c`; implementations are in
`src/wizcmds.c` and the level-teleport number/name/`?` behavior is in
`src/teleport.c`. `src/wizard.c` was also inspected. The known automated
WinPTY `#wizloaddes` prompt race does not affect deliberate human input.

## 3. Save location, backup, and restore

The actual `--showpaths` runtime probe reports:

| Runtime item | Actual value |
|---|---|
| Save/level directory | `C:\Users\yariv\AppData\Local\NetHack\5.0\` |
| Wizard save filename | `wizard.NetHack-saved-game` |
| Complete wizard save path | `C:\Users\yariv\AppData\Local\NetHack\5.0\wizard.NetHack-saved-game` |
| Data directory | `E:\Codex\My_Nethack\binary\Release\x64\` |
| Runtime working convention | Launch with the executable directory as `-WorkingDirectory`; Windows locates data beside the executable and uses the directories above |

Back up any same-name wizard save before starting. These commands never
overwrite it without first creating a timestamped copy:

```powershell
$saveDir = 'C:\Users\yariv\AppData\Local\NetHack\5.0'
$save = Join-Path $saveDir 'wizard.NetHack-saved-game'
$backupDir = Join-Path $saveDir ('step10d-backup-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
if (Test-Path -LiteralPath $save) {
    Copy-Item -LiteralPath $save -Destination (Join-Path $backupDir 'wizard.NetHack-saved-game')
}
$backupDir
```

When restoring a wizard save, relaunch with the exact command above, choose
the existing `wizard` game, and answer **yes** to “Do you want to keep the save
file?” when continued repeat testing is desired. After testing, preserve the
playtest save before restoring the original:

```powershell
$playtestDir = Join-Path $saveDir ('step10d-playtest-save-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $playtestDir -Force | Out-Null
if (Test-Path -LiteralPath $save) {
    Move-Item -LiteralPath $save -Destination (Join-Path $playtestDir 'wizard.NetHack-saved-game')
}
$backup = Join-Path $backupDir 'wizard.NetHack-saved-game'
if (Test-Path -LiteralPath $backup) {
    Copy-Item -LiteralPath $backup -Destination $save
}
```

No external playable save is supplied. In particular,
`step10c-e-save.bin` is a native codec-test artifact, not a launchable player
save and must not be copied into the save directory.

## 4. Topology discovery

Start a new game, run `#wizwhere`, and record the actual parent `P`, selected
alternates, and Dispensary parent `N`. Use `#wizlevelport` → `?` when an exact
cross-dungeon destination is needed; numeric teleporting alone is ambiguous
when branch display depths overlap ordinary DoD depths.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| TOP-01 | `neulev` | DoD parent `P`, where `30 <= P <= 199`; ordinary DoD up/down plus Neutral portal | `#wizwhere`; record `P=____` | PENDING |
| TOP-02 | Gate / `gatetwn` | Neutral N1 at `P`; portal back to `neulev`, portal onward | Record from `#wizwhere` | PENDING |
| TOP-03 | `out1` | Neutral N2 at `P+1`; two portals; possible Dispensary parent | Record actual depth/branch | PENDING |
| TOP-04 | `out2` | Neutral N3 at `P+2`; two portals; possible Dispensary parent | Record actual depth/branch | PENDING |
| TOP-05 | `out3` | Neutral N4 at `P+3`; two portals; possible Dispensary parent | Record actual depth/branch | PENDING |
| TOP-06 | `out4` | Neutral N5 at `P+4`; two portals; possible Dispensary parent | Record actual depth/branch | PENDING |
| TOP-07 | Spire / `spire` | Neutral N6 at `P+5`; return portal and down stair; possible Dispensary parent | Record actual depth/branch | PENDING |
| TOP-08 | Sum / `sumall` | Neutral N7 at `P+6`; up stair and branch stair to LC2 | Record actual depth | PENDING |
| TOP-09 | `leth-a-{1|2}` | LC1 at `P+6`; exactly one selected alternate; down stair to LC2 | Record selected name | PENDING |
| TOP-10 | `lethe-b` | LC2 at `P+7`; branch return to Sum, up to LC1, down to LC3 | Record depth | PENDING |
| TOP-11 | `leth-c-{1|2}` | LC3 at `P+8`; exactly one selected alternate; up/down stairs | Record selected name | PENDING |
| TOP-12 | `leth-d-{1|2}` | LC4 at `P+9`; exactly one selected alternate; up/down stairs | Record selected name | PENDING |
| TOP-13 | `lethe-e`, `lethe-f`, `lethe-g` | LC5–LC7 at `P+10` through `P+12`; ordinary up/down stairs | Record depths | PENDING |
| TOP-14 | `lethe-z` | LC8 at `P+13`; up stair and four holes to LC9 | Record depth | PENDING |
| TOP-15 | `nkai-a-{1|2}` | LC9 at `P+14`; one selected alternate; up stair/four holes | Record selected name | PENDING |
| TOP-16 | `nkai-b`, `nkai-c`, `nkai-z` | LC10–LC12 at `P+15` through `P+17`; up stair/four holes each | Record depths | PENDING |
| TOP-17 | `rlyeh` | LC13 at `P+18`; fall landing and one working up stair | Record depth | PENDING |
| TOP-18 | `lbyrnth` | Dispensary at `P+N`; its parent is exactly one of Neutral N2–N6 | Record `N=____`, parent name/depth | PENDING |
| TOP-19 | No fixed DL111 | `P` is the actual randomized legal result; no rule depends on DL111 | Confirm listing and do not assume 111 | PENDING |
| TOP-20 | Castle | Castle remains ordinary DoD DL200, independent of branch display depth | Confirm in `#wizwhere` | PENDING |

## 5. Complete live traversal

Do not substitute teleporting or a `#wizwhere` listing for these rows. Use
level teleport only to reach `neulev` initially; then walk every connector.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| TRV-01 | Ordinary DoD around P | Main route is intact through `P-1 -> neulev(P) -> P+1`, with Castle still at 200 | Walk both ordinary stairs and return to `neulev` | PENDING |
| TRV-02 | `neulev -> gatetwn` | Portal enters Gate at displayed depth P | Walk onto Neutral portal | PENDING |
| TRV-03 | `gatetwn -> out1 -> out2 -> out3 -> out4` | Each paired portal advances one Neutral floor and remains usable | Walk all four forward portal edges | PENDING |
| TRV-04 | `out4 -> spire -> sumall` | Portal reaches Spire; down stair reaches Sum | Walk portal, then descend | PENDING |
| TRV-05 | `sumall -> lethe-b` | Branch stair enters LC2 at `P+7` | Descend branch stair | PENDING |
| TRV-06 | Headwaters | `lethe-b` up reaches selected `leth-a-{1|2}` LC1; its down stair returns to the same `lethe-b` | Walk up then down | PENDING |
| TRV-07 | LC3–LC8 | Down stairs traverse selected `leth-c`, selected `leth-d`, `lethe-e`, `lethe-f`, `lethe-g`, `lethe-z` | Walk every down stair | PENDING |
| TRV-08 | LC8–LC13 holes | One hole per level reaches LC9 `nkai-a`, LC10 `nkai-b`, LC11 `nkai-c`, LC12 `nkai-z`, then LC13 `rlyeh` | Use a hole on each level; record landing | PENDING |
| TRV-09 | R'lyeh terminal | Complete terminal layout, no down exit, working up stair | Inspect, then take up stair | PENDING |
| TRV-10 | Reverse N'Kai | Up stairs return `rlyeh -> nkai-z -> nkai-c -> nkai-b -> nkai-a -> lethe-z` | Walk every up stair | PENDING |
| TRV-11 | Reverse Lost Cities | Up stairs return `lethe-z -> lethe-g -> lethe-f -> lethe-e -> leth-d -> leth-c -> lethe-b` | Walk every up stair | PENDING |
| TRV-12 | Return to DoD | Branch stair returns `lethe-b -> sumall`; up to Spire; portals reverse through out4..out1, Gate, then `neulev` | Walk the entire reverse route | PENDING |
| TRV-13 | Dispensary forward | Actual Neutral parent N2–N6 has a down branch stair to `lbyrnth` | Enter via the discovered stair | PENDING |
| TRV-14 | Dispensary return | `lbyrnth` up stair returns to the same Neutral parent, not another floor | Ascend and compare parent/name/depth | PENDING |

## 6. All 26 resources and four alternate groups

For the selected live resources, traverse and use `#wizmap` plus normal look.
To inspect an unselected alternate, start a separate disposable wizard game,
run `#wizloaddes`, enter the exact filename, then `#wizmap`; quit without
saving. A directly loaded resource proves its map body only, not production
topology, identity dispatch, or traversal.

| Test ID | Resource | Depth / connector | Major expected feature or visual | Inspection command/action | Status |
|---|---|---|---|---|---|
| MAP-01 | `neulev.lua` | P; DoD up/down + portal | `nommap`; two shops, nine barracks, throne, mines, mind flayer/Deep Ones; no placeholder residue | Live traversal and `#wizmap` | PENDING |
| MAP-02 | `gatetwn.lua` | P; arrival/forward portals | Hardfloor town; eight fixed shops, temple, beehive; functional rooms | Live traversal and `#wizmap` | PENDING |
| MAP-03 | `out1.lua` | P+1; paired portals | Lit forest base plus plausible procedural Outlands content | Live; `#wizmap`; see Outlands table | PENDING |
| MAP-04 | `out2.lua` | P+2; paired portals | Progressively lit forest base plus procedural content | Live; `#wizmap` | PENDING |
| MAP-05 | `out3.lua` | P+3; paired portals | Unlit forest behavior plus procedural content | Live; `#wizmap` | PENDING |
| MAP-06 | `out4.lua` | P+4; paired portals | `nommap`, hardfloor, shortsighted forest plus procedural content | Live; `#wizmap` | PENDING |
| MAP-07 | `spire.lua` | P+5; portal + down stair | `nommap`, hardfloor, shortsighted, unlit maze/spire; clay golem, Ferrumach, Plumach | Live traversal | PENDING |
| MAP-08 | `sumall.lua` | P+6; up + branch down | Lit hardfloor terminal plane; Rilmani and argentum golems | Live traversal | PENDING |
| MAP-09 | `leth-a-1.lua` | P+6; down only | Complete headwaters variant 1; water, loot, traps, mixed population | Live if selected; otherwise isolated load | PENDING |
| MAP-10 | `leth-a-2.lua` | P+6; down only | Complete headwaters variant 2; distinct geometry/population/loot | Live if selected; otherwise isolated load | PENDING |
| MAP-11 | `lethe-b.lua` | P+7; return/up/down | Complete entry map; 26-member eldritch set, chests, water, rust traps | Live traversal | PENDING |
| MAP-12 | `leth-c-1.lua` | P+8; up/down | Ogre stockade, giant mimic, spiked/level-teleport traps | Live if selected; otherwise isolated load | PENDING |
| MAP-13 | `leth-c-2.lua` | P+8; up/down | Undead stockade with lich/mummy/kraken population | Live if selected; otherwise isolated load | PENDING |
| MAP-14 | `leth-d-1.lua` | P+9; up/down | Zoo/thrones, Alhoon, Necronomicon | Live if selected; otherwise isolated load | PENDING |
| MAP-15 | `leth-d-2.lua` | P+9; up/down | Swamp/morgues, Alhoon, Necronomicon | Live if selected; otherwise isolated load | PENDING |
| MAP-16 | `lethe-e.lua` | P+10; up/down | Complete bridge temple and valid designated bridge priest | Live traversal | PENDING |
| MAP-17 | `lethe-f.lua` | P+11; up/down | Old Gods settlement; witches, goats, Hmnyw-Pharaoh, Good Neighbor | Live traversal | PENDING |
| MAP-18 | `lethe-g.lua` | P+12; up/down | Mi-go area, goat population, coherent darkness/loot | Live traversal | PENDING |
| MAP-19 | `lethe-z.lua` | P+13; up/four holes | Transition map; Sword of the Deeps; inactive donor branch residue creates no connector | Live traversal | PENDING |
| MAP-20 | `nkai-a-1.lua` | P+14; up/four holes | N'Kai variant 1; complete gulf geometry, eldritch population/traps | Live if selected; otherwise isolated load | PENDING |
| MAP-21 | `nkai-a-2.lua` | P+14; up/four holes | N'Kai variant 2; distinct complete geometry; inactive marker creates no connector | Live if selected; otherwise isolated load | PENDING |
| MAP-22 | `nkai-b.lua` | P+15; up/four holes | Alhoon, parasitized doll, Deep Ones; complete landing/holes | Live traversal | PENDING |
| MAP-23 | `nkai-c.lua` | P+16; up/four holes | Flayers/Deep Ones/priests; inactive marker creates no connector | Live traversal | PENDING |
| MAP-24 | `nkai-z.lua` | P+17; up/four holes | Gulf terminus population; complete landing/holes | Live traversal | PENDING |
| MAP-25 | `rlyeh.lua` | P+18; landing/up only | `nommap`, hardfloor, no-teleport; blue terminal city, bosses, Silver Key | Live traversal | PENDING |
| MAP-26 | `lbyrnth.lua` | P+N; return stair | `nommap`, no-teleport labyrinth; Illurien, minotaur/priestess, 13 rust traps, six spellbooks | Live Dispensary traversal | PENDING |

For every row also check complete geometry, accessible connectors, no missing
sections, intended lighting/darkness, hardfloor/`nommap`/no-teleport behavior,
fixed content, and absence of debug placeholders.

## 7. Outlands generated features

Inspect the four live Outlands floors first. If a listed visual is absent, use
`#wizmakemap` only in a disposable game on an actual Outlands level. Cap this
at 12 regenerations per floor; this is visual coverage, not a probability
test. If a feature is still absent, record `BLOCKED` with evidence rather
than treating its frequency as a failure; C–E already validated 1,000 native
instances.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| OUT-01 | Kamerel towers | Coherent shallow-water/mirror tower complex; route remains open | Inspect live or capped `#wizmakemap` | PENDING |
| OUT-02 | Minor spire | Recognizable mirror spire with valid surrounding terrain | Inspect live or capped regeneration | PENDING |
| OUT-03 | Fishing village | Plausible shore, huts, water fauna/objects; no catastrophic overlap | Inspect live or capped regeneration | PENDING |
| OUT-04 | Well | Well placement is reachable and visually integrated | Inspect with fishing feature | PENDING |
| OUT-05 | Plumach homestead | Reachable structure with plausible occupants/loot | Inspect representative instance | PENDING |
| OUT-06 | Plumach village | Buildings register sensibly as shop/temple/barracks/court/tool rooms | Inspect rooms and occupants | PENDING |
| OUT-07 | Ferrumach tower | Locked barracks/tower remains reachable and coherent | Inspect representative instance | PENDING |
| OUT-08 | Inverted ziggurat | Concentric structure, altar/chest/monsters, no overlap blocking portals | Inspect representative instance | PENDING |
| OUT-09 | Neutral river / liquification | Shallow/deep water looks plausible, lights coherently, and leaves a nonswimming portal route | Walk the protected route | PENDING |
| OUT-10 | Overall placement | Both portals accessible; monsters/objects plausible; no structure collision or soft lock | Check all four actual floors | PENDING |

## 8. Visible mechanics

Use disposable wished items for destructive Lethe checks; leave valuable
artifacts on dry ground first. Manual coverage is representative, not a
duplicate of deterministic C tests.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| MEC-01 | Gate/out1–4/Spire/Sum | Step 10 context-specific behavior appears only on its intended levels | Observe terrain/spells/objects during traversal | PENDING |
| MEC-02 | Ordinary DoD control | Ordinary water, trees, colors, pits, projectiles, shops, and priests remain ordinary | Compare a nearby DoD level | PENDING |
| MEC-03 | Lethe description | Looking at Lost Cities water says `sparkling water` | Use normal look on a water cell | PENDING |
| MEC-04 | Lethe immersion/amnesia | Entering Lethe reports sparkling waters sweeping away cares and causes bounded forgetting | Step into water in a disposable state | PENDING |
| MEC-05 | Lethe inventory effects | Representative scroll/book rewrite or destruction, BUC stripping, enchantment/charge loss, or marker drain is visible; protected artifacts remain valid | Wish expendable items, identify, immerse, re-identify | PENDING |
| MEC-06 | Lethe persistence | Lethe level identity and affected inventory survive save/reload | Save and restore on a Lost Cities level | PENDING |
| MEC-07 | Ordinary water control | Water outside Lost Cities/R'lyeh does not use Lethe message or transformations | Repeat with expendable control item in DoD | PENDING |
| MEC-08 | Outlands trees | Kicking/cutting uses Outlands behavior without ordinary fruit/swarm side effects | Exercise one tree; compare ordinary tree | PENDING |
| MEC-09 | Mirror-shard spiked pit | Outlands spiked-pit presentation/damage is visible and survivable in wizard testing | Trigger representative pit and record message/damage | PENDING |
| MEC-10 | Projectile materials | Representative Outlands arrow/dart material is named/behaves plausibly | Inspect dropped projectile where practical | PENDING |
| MEC-11 | Palettes | Outlands brown/black, Lost Cities black/gray, R'lyeh bright-blue/blue terrain is coherent | Compare TTY color screens | PENDING |
| MEC-12 | R'lyeh terminal | Terminal geometry is complete and the return up stair works despite no-teleport/`nommap` | Inspect and ascend | PENDING |
| MEC-13 | Gate pet separation | A separated tame pet remains tame and does not suffer ordinary absence hunger while Gate context applies | Test with a disposable pet where practical | PENDING |

## 9. Shops, temples, barracks, and NPC roles

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| NPC-01 | Gate fixed shops | Eight fixed rooms: armor, potion, four food, tool, and general shops; shopkeepers and stock function | Enter rooms; buy/pay for a cheap item | PENDING |
| NPC-02 | Gate temple/beehive | Temple altar/priest and beehive register sensibly | Inspect and interact without destroying test route | PENDING |
| NPC-03 | `neulev` | Weapon and wand shops plus nine barracks and throne exist | `#wizmap`, enter representative rooms | PENDING |
| NPC-04 | Outlands Plumach shop | Representative generated shop has a Plumach-designated shopkeeper with valid ESHK state | Enter, price, buy/pay, leave | PENDING |
| NPC-05 | Plumach ownership | Ordinary billing, ownership, stock, and payment remain functional | Pick up/return/pay for an item | PENDING |
| NPC-06 | `lethe-e` bridge temple | Native temple has designated hostile blasphemous-lurker bridge priest with valid priest state | Inspect altar, role, hostility, and temple behavior | PENDING |
| NPC-07 | Ordinary controls | Unrelated ordinary shopkeepers and priests retain ordinary species/state/services | Test one non-Step-10 shop and temple where practical | PENDING |

## 10. Bosses, rewards, and artifacts

Use live fixed encounters first. `#wizgenesis` may create an exact named
monster for inspection in a disposable game; do not wait for Center of All's
rare natural generation. Use `#wizidentify` for exact object inspection.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| BOS-01 | Dispensary / Illurien | Illurien is present with coherent identity, attacks, and encounter presentation | Live `lbyrnth`; optional `#wizgenesis` control | PENDING |
| BOS-02 | Lost Cities / Alhoons | Fixed Alhoons on both `leth-d` variants and `nkai-b`; coherent undead-flayer behavior | Inspect live plus unselected variant map | PENDING |
| BOS-03 | `lethe-f` uniques | Hmnyw-Pharaoh and The Good Neighbor are present and distinct | Inspect/encounter both | PENDING |
| BOS-04 | R'lyeh / Father Dagon | Correct unique boss presentation and encounter | Live R'lyeh | PENDING |
| BOS-05 | R'lyeh / Mother Hydra | Correct unique boss presentation and encounter | Live R'lyeh | PENDING |
| BOS-06 | R'lyeh / Great Cthulhu | Correct terminal boss, psychic/combat/death presentation where safely tested | Live R'lyeh; retain evidence | PENDING |
| BOS-07 | Center of All | Correct identity/equipment can be inspected without waiting for rare generation | `#wizgenesis`, enter `Center of All` in disposable game | PENDING |
| REW-01 | Necronomicon | Named artifact uses `SPE_SECRETS`; reading (`r`) opens bounded local menu; ordinary spellbook of secrets remains ordinary | Inspect fixed `leth-d` copy; compare wished ordinary base | PENDING |
| REW-02 | Silver Key menu | `#invoke` opens native destination menu containing only sensible visited Step 10 destinations | Acquire R'lyeh key after visiting route; invoke | PENDING |
| REW-03 | Silver Key travel/cancel | One positive destination works; Escape cancellation is safe; no arbitrary/endgame bypass appears | Invoke twice, once cancel and once select | PENDING |
| REW-04 | Sword of the Deeps | Exact map item: base `LONG_SWORD`, `OBP_DEEP`, +12, cursed, named `The Sword of the Deeps` | Recover at `lethe-z`, identify, inspect | PENDING |
| REW-05 | Neutrality Keys | First/Second/Third Keys appear only through intended Center/Alhoon paths and do not duplicate improperly | Inspect reachable drops/equipment | PENDING |
| REW-06 | Mirror artifacts | Representative `Infinity's Mirrored Arc`, `The Staff of Twelve Mirrors`, `The Sansara Mirror`, `Mirror Brand`, or `Soulmirror` has coherent name/material/effect | `#wizwish` one representative artifact | PENDING |

## 11. Save, reload, and revisit

For each row: record the level name/depth and connectors, press `S`, exit
normally, relaunch the same executable/options, restore `wizard`, answer yes
to keep the save when continuing, then re-run `#wizwhere` and revisit one
adjacent level. Do not corrupt a save or bones file for this phase.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| SAV-01 | Neutral / Outlands | Same logical level, P, portals, selected generated content, and any Plumach shopkeeper state | Save/restore/revisit adjacent Neutral floor | PENDING |
| SAV-02 | Lethe Lost Cities | Same level/alternate, Lethe identity/effects, stairs, objects, and bridge-priest state where applicable | Save/restore/revisit | PENDING |
| SAV-03 | N'Kai hole section | Same selected `nkai-a` alternate, up stair, holes, landing/content; no reroll | Save/restore/revisit by up stair and hole | PENDING |
| SAV-04 | R'lyeh | Same terminal layout, bosses/key state, up stair, Silver Key destinations | Save/restore, invoke/cancel as applicable, ascend | PENDING |
| SAV-05 | Dispensary | Same `lbyrnth`, Illurien/content state, and exact same Neutral parent return | Save/restore, ascend, re-enter | PENDING |
| SAV-06 | Sword/reward state | Sword remains named, cursed, +12 and deep; collected artifacts/keys remain stable | Identify before and after restore | PENDING |

## 12. Deterministic P=199 / displayed-depth-217 procedure

The accepted extracted-production scheduler harness uses input seeds 1–1000.
The first input that yields maximum parent depth is:

```text
SEED|65|P=199|A=1|C=1|D=0|N=0|DISP=5
```

Thus harness input `65` selects P=199, `leth-a-2`, `leth-c-2`,
`leth-d-1`, `nkai-a-1`, and Dispensary parent N5. Reproduce the automated
evidence with:

```powershell
& 'E:\Codex\My_Nethack\_qa\step10c-tests\x64\step10c-e-final2\e3\scheduler\step10c_b_scheduler.exe' |
    Select-String -Pattern '^SEED\|65\||^PASS '
```

Production does **not** expose an RNG seed command-line option. The harness
seed drives its fixture LCG and cannot safely be passed to the production
ISAAC RNG. No playable P=199 save was manufactured: doing so would require a
test-hook build or save surgery, and the available E6 codec artifact is not a
full player save. This is the exact approved alternative: retain the fresh
automated P=199 evidence and leave these manual rows PENDING until a naturally
encountered or separately authorized safe production fixture is available.
Do not brute-force production launches and do not modify the scheduler.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| HIGH-01 | Scheduler evidence | Input 65 reports P=199 and the exact variants/N5 above | Run command and retain output | PENDING |
| HIGH-02 | `neulev` / Gate | Both display depth 199 in a real P=199 game | `#wizwhere`, then inspect live | PENDING |
| HIGH-03 | R'lyeh | Displays depth 217 and has coherent topology | Traverse/inspect in real P=199 game | PENDING |
| HIGH-04 | Deep save/reload | Deep descendant restores at same logical/depth identity | Save/restore below Neutral | PENDING |
| HIGH-05 | Return traversal | Full return reaches `neulev`/ordinary DoD | Walk reverse route | PENDING |
| HIGH-06 | Main DoD/Castle | DoD continues downward from P=199 to Castle at DoD200, independent of branch depth 217 | Walk ordinary down stair and confirm Castle | PENDING |

## 13. TTY and tiled visual inspection

Run representative levels first with `NetHack.exe`, then save/exit and open
the same wizard save with `NetHackW.exe` (close one process before opening the
other). Existing intentional tile reuse is valid; no new artwork is required.

| Test ID | Location / Target | Expected Result / Depth / Visual | Command / Action | Status |
|---|---|---|---|---|
| VIS-01 | TTY symbols/colors | Walls/floors, monsters, objects, portals, holes, water, shops, and temples are readable; no stale donor display-class symbols | Inspect Gate, Outlands, Lost Cities, R'lyeh | PENDING |
| VIS-02 | Graphical tiles | Same entities are visible with coherent tile aliases; intentional reuse is acceptable | Repeat representative route in `NetHackW.exe` | PENDING |
| VIS-03 | Outlands palette | Brown walls/lit floor and black dark floor remain legible in both interfaces | Compare `out1`–`out4`/Spire | PENDING |
| VIS-04 | Lost Cities palette | Black walls/dark floor and gray lit floor remain legible | Compare headwaters, bridge, N'Kai | PENDING |
| VIS-05 | R'lyeh palette/layout | Bright-blue walls/blue floors, bosses, Silver Key, holes/landing and up stair are visible | Inspect terminal in both interfaces | PENDING |
| VIS-06 | Services/connectors | Shop/temple room cues, portals, stairs, holes, shallow/deep water are not missing or misleading | Inspect representative cells with normal look | PENDING |

## 14. Defect logging and stopping rule

For a manual failure, preserve the save, screenshots, and transcript; add a
row below; stop that failing path; and do not approve Step 10D. Remediation is
a separate implementation pass followed by affected automated gates. Use
`BLOCKED` only when the expected result cannot be exercised, not as a pass.

| Test ID | Status | Level / Target | P / Coordinates | Action | Expected | Observed | Evidence | Severity | Notes |
|---|---|---|---|---|---|---|---|---|---|
| DEF-01 | PENDING | — | — | — | No defect recorded yet | — | — | — | Add rows; do not overwrite prior evidence |

## 15. User sign-off

STEP 10D MANUAL TOPOLOGY VALIDATION: PENDING

STEP 10D MANUAL COMPLETE-TRAVERSAL VALIDATION: PENDING

STEP 10D MANUAL MAP/VISUAL VALIDATION: PENDING

STEP 10D MANUAL OUTLANDS VALIDATION: PENDING

STEP 10D MANUAL LETHE VALIDATION: PENDING

STEP 10D MANUAL SHOP/TEMPLE VALIDATION: PENDING

STEP 10D MANUAL BOSS/REWARD VALIDATION: PENDING

STEP 10D MANUAL SAVE/RELOAD VALIDATION: PENDING

STEP 10D MANUAL HIGH-PARENT/DEPTH-217 VALIDATION: PENDING

STEP 10D USER APPROVAL: PENDING

## 16. Final Step 10D approval record

The user approved Step 10D as **PASS** for final Step 10 closeout. This is a
user-supplied approval record; no row-level observations or unperformed manual
actions are being invented in this document. The automated closeout evidence
is recorded separately in the final Step 10 report.

STEP 10D MANUAL VALIDATION: PASS

STEP 10D USER APPROVAL: PASS

## QA2 fresh-generation retest addendum

QA2 remediation is complete, but this does not approve Step 10D. Start a new
wizard game and generate each level afresh; do not reuse an old generated
level as proof of the corrected room-fill or payload behavior. Preserve any
existing user save and use an isolated save for this retest.

Retest the corrected high-value cases in addition to the existing matrix:

- Gate Town: enter all eight shops, verify a keeper, stock, pricing/billing,
  the beehive, temple, and both portals.
- `neulev`: verify both shops, representative stock/billing, all nine
  barracks, fixed Deep Ones/shriekers/mines, and that the authored throne
  remains intentionally unfilled.
- `leth-d-1`/`leth-d-2`: verify the zoo/courts and swamp/morgues in fresh
  variants without duplicate authored payload.
- `lethe-z`: verify the barracks, morgue, five corpse species by actual
  identification (not display text), and the exact Sword of the Deeps.
- `rlyeh`: verify all seven temple regions, `has_temple`, the two required
  explicit hostile unknown priests, no additional auto-priest, the three
  bosses, Silver Key, traps, and return stair.
- `nkai-b`: verify both cursed create-monster scrolls have numeric `spe=0`
  and no custom name `"0"`; verify the intentional morgue remains unfilled.

Save, reload, and revisit representative corrected state, especially Gate
Town shop state, `neulev`, Lethe-Z, R'lyeh, and N'Kai-B. Continue with the
broader topology, visual, mechanics, shops, bosses, rewards, and P=199
manual checklist above. Any failure remains a Step 10D defect and must keep
Step 10D pending.

## Portal-arrival hotfix retest

Use a fresh isolated wizard game. From the DoD Approach portal, verify that
arrival in Gate Town lands on the portal whose destination is the Approach;
step away and back once to confirm it does not immediately retrigger. Traverse
Gate Town → out1 → out2 → out3 → out4 → Spire, confirming each arrival portal
returns to the level just left. Then traverse Spire → out4 → out3 → out2 →
out1 → Gate Town → the DoD Approach using the paired return portals. Save and
reload at a representative middle floor before continuing. The permanent
automated equivalent is:

```powershell
python -B test/run_step10_portal_arrival.py `
  E:\Codex\My_Nethack\binary\Release\x64 `
  E:\Codex\My_Nethack\_qa\portal-arrival-manual-support
```

This automated check is evidence for the hotfix only; it does not approve
the remaining manual Step 10D checklist.

## Lurking-one combat retest

Use a fresh isolated wizard game and approach a lurking one without shock
resistance. Confirm that its tentacle/brain attack still occurs, followed by
the shocking gaze, and that neither `Gaze attack N?` nor `Program in disorder!`
appears. Repeat with shock resistance active; the shocking gaze should report
that the zap does not shock the hero, with no internal-error diagnostic.
Preserve the save and transcript if any failure occurs. This focused retest
is required for Step 10D and is separate from the automated QA3 evidence.
