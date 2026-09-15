# Step 10A — Neutral Quest / Lost Cities donor audit and implementation contract

## 1. STEP 10A EXECUTIVE SUMMARY

This document is the implementation contract for importing the pinned dNetHack Neutral Quest adventure into the finalized NetHack 5.0 expansion. It is an audit only: Step 10A changes no production source, scheduler, data, tests, generated files, or README.

The verified local baseline is branch `phase0/dod-length`, commit `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`, with `origin/phase0/dod-length` at the same commit. Milestone tag `step9-sheol-dragon-caves-mithardir` resolves to that commit. The committed submodules are Lua `6e22fedb74cf0c9b6656e9fce8b7331db847c605`, PDCurses `09cf16db29753305e4241d4ae609aac997fa730d`, and PDCursesMod `6cd9c16900fef82754923c718fab7fe85f761bb6`. The starting tree was clean. The existing 200-level Dungeons of Doom, Castle at DoD DL200, persistent Step 6B scheduler, and Steps 5–9 content are protected architecture.

The authoritative donor is `Chris-plus-alphanumericgibberish/dnethack` at immutable commit `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`, resolved locally at `E:\Codex\My_Nethack\_qa\dnethack-donor-pinned`. Its repository tree is rooted under `dnethack-3.4.3/`. That pin has no `dat/dungeon.def`; the authoritative definitions are `dat/dngnch1.def`, `dat/dngnch2.def`, and `dat/dngnch3.def`. All donor conclusions below come from the pin, not current master or `doc/step9d.md`.

The adventure contains exactly **21 branch floors** in three registered dungeons: Neutral Quest (7), The Lost Cities (13), and The Dispensary (1). A separate `neulev` special map replaces the selected ordinary DoD parent floor and is not a branch floor. There are 26 concrete map bodies/resources when `neulev`, four equal-probability alternate map bodies, and the Dispensary are counted. No donor inclusion/submap files are used.

The audit found no architectural blocker to Step 10B. It did find one mandatory capacity change: the local game already registers 15 dungeons and `MAXDUNGEON` is 16; adding three requires at least 18. Appended serialized monster, object, artifact, dungeon, level, and branch identities plus the capacity/layout change require a new save epoch. Step 10B must advance `EDITLEVEL` from 4; no migration from pre-Step-10 saves is promised.

## 2. AUTHORITATIVE TOPOLOGY

Let `P` be the scheduler-selected DoD parent in DL30–199. `neulev` is generated on that same DoD ledger location and retains ordinary DoD up/down stairs. Its branch portal enters Neutral Quest level 1, Gate Town. Portal branches have zero depth adjustment, so Gate Town displays at depth `P`.

```text
DoD P: neulev (ordinary DoD up/down, portal)
  ↕ portal
Neutral 1 gatetwn (Gate Town)
  ↔ portal Neutral 2 out1
  ↔ portal Neutral 3 out2
  ↔ portal Neutral 4 out3
  ↔ portal Neutral 5 out4
  ↔ portal Neutral 6 spire
                  ↓ ordinary stair
Neutral 7 sumall (Sum of All)
                  ↓ branch stair
Lost Cities 2 lethe-b (entry level)
        ↑ ordinary stair                    ↓ ordinary stair
Lost Cities 1 leth-a-{1|2}       Lost Cities 3 leth-c-{1|2}
                                              ↓
                              Lost Cities 4 leth-d-{1|2}
                                              ↓
                                     5 lethe-e
                                              ↓
                                     6 lethe-f
                                              ↓
                                     7 lethe-g
                                              ↓
                                     8 lethe-z
                                              ↓ four holes
                                     9 nkai-a-{1|2}
                                              ↓ four holes
                                    10 nkai-b
                                              ↓ four holes
                                    11 nkai-c
                                              ↓ four holes
                                    12 nkai-z
                                              ↓ four holes
                                    13 rlyeh (terminal; up stair only)

Neutral level 2, 3, 4, 5, or 6 (one uniformly eligible parent selected by
dungeon branch placement)
                  ↓ branch stair
The Dispensary 1 lbyrnth
                  ↑ return branch stair
```

Important orientation rules:

- Gate Town through the Spire use paired portals. The Spire has its return portal plus a down stair to Sum of All. Sum has the up stair and the branch stair into The Lost Cities.
- The Lost Cities entry is explicitly level 2. `lethe-b` therefore has the return branch to Sum, an up stair to level 1, and a down stair to level 3. Either headwaters variant at level 1 is a side head with only a down stair.
- Levels 3–8 have ordinary up/down stairs. Levels 9–12 have an up stair and four holes, with no down stair; a hole falls exactly one logical Lost Cities level. R'lyeh has only an up stair. Its `TELEPORT_REGION ... down` is the landing region for falls, not a portal.
- The complete return route is available: R'lyeh up through levels 12–2, branch stair to Sum, up to the Spire, portals through the Outlands and Gate Town, then portal to DoD.
- No trapdoor directive participates in progression. The only one-way progression edges are the holes from Lost Cities levels 8–12; each destination retains an up stair.
- The Dispensary is normal topology. Its parent is randomly one of Neutral levels 2–6 (`out1`, `out2`, `out3`, `out4`, or `spire`); it is not a second DoD-scheduled branch and reserves no DoD depth.
- `BRANCH` markers present in `lethe-z`, `nkai-a`, or `nkai-c` map text do not correspond to a branch in the pinned dungeon definitions. They are inactive donor residue and must not create invented subbranches.
- Major fixed encounters are Illurien in the Dispensary; Alhoons on both `leth-d` variants and `nkai-b`; Hmnyw-Pharaoh and The Good Neighbor on `lethe-f`; Father Dagon, Mother Hydra, and Great Cthulhu in R'lyeh. Center of All is runtime-generated, not fixed in a map.

Displayed depth is deterministic relative to `P`: Neutral level `N` is `P + N - 1`; Lost Cities logical level `L` is `P + L + 5`, so `lethe-b` is `P+7`, headwaters level 1 is `P+6`, and R'lyeh is `P+18`. A Dispensary attached to Neutral `N` displays at `P+N`. At the maximum parent, the deepest displayed depth is 217.

## 3. LEVEL / GENERATOR INVENTORY

All paths in this table are below donor root `dnethack-3.4.3/`. `neutrality.des` contains every map body from `neulev` through R'lyeh; `labr.des` contains the Dispensary. The `RNDLEVEL` alternatives are equal-weight donor alternatives. No inclusions or submaps occur.

| Level/region | donor source | type | variants | entry | exit | required local port |
|---|---|---|---|---|---|---|
| DoD approach `neulev` | `dat/neutrality.des`; `dat/dngnch1.def` | static special map rebased onto DoD | 1 | normal DoD up/down | portal to `gatetwn`; normal DoD stairs | `.des` to Lua; retain DoD ledger identity; shops, throne, barracks, mines, monsters, and branch marker |
| Neutral 1 `gatetwn` | same | static hardfloor town | 1 | branch portal from DoD | portal to `out1` | Lua map, six fixed shops, temple, beehive, portal |
| Neutral 2 `out1` | same; `src/mkmaze.c`, `src/mkroom.c` | static forest canvas plus procedural features | 1 | portal from Gate | portal to `out2`; possible Dispensary branch | Lua canvas plus narrow Outlands feature generator |
| Neutral 3 `out2` | same | static forest canvas plus procedural features | 1 | portal from `out1` | portal to `out3`; possible Dispensary branch | same; donor initialization truth values and lighting preserved |
| Neutral 4 `out3` | same | static forest canvas plus procedural features | 1 | portal from `out2` | portal to `out4`; possible Dispensary branch | same; unlit forest behavior preserved |
| Neutral 5 `out4` | same | static forest canvas plus procedural features | 1 | portal from `out3` | portal to `spire`; possible Dispensary branch | same; `nommap`, hardfloor, shortsighted flags |
| Neutral 6 `spire` | same | static maze/spire | 1 | portal from `out4` | return portal; down stair to Sum; possible Dispensary branch | Lua map; maze walk; inaccessible-monster relocation equivalent |
| Neutral 7 `sumall` | same | static terminal plane | 1 | up stair from Spire | down branch stair to Lost Cities entry | Lua map; wallification; Sum generation/spell rules |
| Lost Cities 1 `leth-a` | `dat/neutrality.des`; `dat/dngnch2.def` | static Lethe headwaters | `leth-a-1`, `leth-a-2` | up-route arrival from level 2 via its down stair | down stair to level 2 | two equal Lua variants; Lethe flag and listed populations/loot/traps |
| Lost Cities 2 `lethe-b` | same | static Lethe entry | 1 | branch stair from Sum | branch return, up to level 1, down to level 3 | Lua map; 26-member eldritch set |
| Lost Cities 3 `leth-c` | same | static stockade | `leth-c-1`, `leth-c-2` | up stair | down stair | two equal Lua variants: ogre versus undead stockade |
| Lost Cities 4 `leth-d` | same | static fortress/swamp | `leth-d-1`, `leth-d-2` | up stair | down stair | two equal Lua variants; fixed Alhoon and Necronomicon |
| Lost Cities 5 `lethe-e` | same | static bridge temple | 1 | up stair | down stair | Lua map; `bridge_temple` priest/blasphemous-lurker subset |
| Lost Cities 6 `lethe-f` | same | static Old Gods settlement | 1 | up stair | down stair | Lua map; witches, goat entities, Hmnyw-Pharaoh, Good Neighbor, loot |
| Lost Cities 7 `lethe-g` | same | static mi-go area | 1 | up stair | down stair | Lua map; mi-go and goat populations |
| Lost Cities 8 `lethe-z` | same | static transition | 1 | up stair | four holes to level 9 | Lua map; inactive branch marker ignored; Sword of the Deeps |
| Lost Cities 9 `nkai-a` | same | static Gulf of N'Kai | `nkai-a-1`, `nkai-a-2` | fall landing and up stair | four holes to level 10 | two equal Lua variants; one inactive branch marker ignored |
| Lost Cities 10 `nkai-b` | same | static Gulf level | 1 | fall landing and up stair | four holes to level 11 | Lua map; Alhoon and parasitized doll |
| Lost Cities 11 `nkai-c` | same | static Gulf level | 1 | fall landing and up stair | four holes to level 12 | Lua map; inactive branch marker ignored |
| Lost Cities 12 `nkai-z` | same | static Gulf terminus | 1 | fall landing and up stair | four holes to R'lyeh | Lua map; eldritch generation |
| Lost Cities 13 `rlyeh` | same | static terminal city | 1 | fall landing region; up stair | up stair only | Lua map; hardfloor, no-teleport, `nommap`, Lethe flag, bosses and Silver Key |
| Dispensary 1 `lbyrnth` | `dat/labr.des`; `dat/dngnch3.def` | static labyrinth | 1 | down branch stair from random Neutral 2–6 | return branch stair | Lua map; no-teleport/`nommap`; Illurien, minotaur/priestess, 13 rust traps, six spellbooks |

The Outlands generator is invoked only on `out1`–`out4`. `src/mkmaze.c:place_neutral_features` makes a mutually exclusive primary choice: 1/30 Kamerel towers (with a nested 1/16 fishing-village attempt), otherwise 1/16 minor spire, otherwise 1/16 fishing village. It then independently attempts a neutral river at 1/8, Plumach village at 1/8, inverted ziggurat at 1/16, Ferrumach tower at 1/8, and `rnd(4)+rn2(4)` Plumach homestead placements with probability 1/3. `mkferrufort` exists but its dispatch is commented out and is excluded.

Required generator functions are `place_neutral_features`, `mkkamereltowers`, `mkminorspire`, `mkfishingvillage`, `mkwell`, `mkpluhomestead`, `mkpluvillage`, `mkferrutower`, `mkinvertzigg`, `mkneuriver`, and `neuliquify`. Their narrow ports must preserve connectivity checks, feature collision avoidance, shop/room registration, lighting, water depth, monster/object placement, and source probability order. The river is horizontal one quarter of the time and vertical otherwise; it uses shallow `PUDDLE` across open land and deeper `POOL` through tree bands, avoids shops, lights itself, seeds water fauna/objects/gold, and must not sever connectors. Kamerel structures use puddles and mirror structures; fishing villages use shallow/deep coasts and `mkwell` may place Rakuyo; Plumach villages can contain ordinary shop, temple, barracks, court, Plumach loot, gold-golem treasury, or tool-room buildings; Ferrumach towers are locked barracks; inverted ziggurats use a no-alignment altar, Shattered Ziggurat monsters, and an iron chest. `mkferrufort` and its otherwise reachable technology loot are not part of normal dispatch.

The authored payload trace, used to validate each future conversion, is:

- `neulev`: `nommap`; ordinary up/down and branch portal; nine barracks, weapon shop, wand shop, throne; shriekers, master mind flayer, Deep Ones; land mines and random class loot.
- `gatetwn`: hardfloor; armor, potion, four food, tool, and general shops; temple and beehive; branch/Outlands portals.
- `out1`–`out4`: hardfloor and shortsighted (with `nommap` on `out4`); the exact donor `INIT_MAP` tree/ground truth values and progressive lighting; paired portals; gray unicorns and plains centaurs before procedural content.
- `spire`: `nommap`, hardfloor, shortsighted; tree/floor maze initialized unlit; return portal/down stair; clay golem, Ferrumach, and Plumach.
- `sumall`: `nommap`, hardfloor, lit; up/branch stairs; Argenach, Ferrumach, Cuprilach, Plumach, and argentum golems.
- `leth-a-1`: clay/electric eel/kuker/living doll/living lectern/lurker above/lurking one/nightgaunt/priest of an unknown god/raven/red dragon/trapper/vampire bat; amnesia/blank-paper/chest/egg/enchant-weapon/leather-armor/luckstone/magic-lamp/marker/oil/statue/teleportation loot; board, pit, and rust traps.
- `leth-a-2`: byakhee/electric eel/kuker/living doll/living lectern/lurking one/nightgaunt/ochre jelly/priest of an unknown god/raven/vampire/vampire bat/vampire lord/warg/winter wolf; amnesia/blank-paper/chest/create-monster/fire/full-healing/leather-armor/luckstone/magic-lamp/marker/oil/sickness/slow-monster/speed-monster/statue/teleportation loot; board, magic, pit, and rust traps.
- `lethe-b`: the 26-member eldritch set enumerated in section 5; chests, gain-ability potions, water; rust traps.
- `leth-c-1`: cave spider/electric eel/giant mimic/kobold lord/kuker/living doll/lurking one/ogre/ogre king/ogre mage/wererat; chest and earth/fire/sleep/striking items; level-teleport and spiked-pit traps.
- `leth-c-2`: cave spider/electric eel/elf mummy/ettin mummy/giant mummy/human mummy/ghoul/gnoll ghoul/giant eel/giant mimic/hell hound/kraken/kuker/lich/master lich/living doll/lurking one/shark/skeleton/wererat; chest and death/earth/lightning items; level-teleport and spiked-pit traps.
- `leth-d-1`: zoo/thrones; Alhoon, eels, killer bees, kraken, lurking one, master mind flayer, shark; cancellation/chest/lightning/Necronomicon/sleep/statue; board, magic, and polymorph traps. `leth-d-2`: swamp/morgues; Alhoon, black dragon, eels, giant mummy, killer bee, kraken, lurking one, master lich, shark, shrieker, succubus; the same named object set.
- `lethe-e`: bridge temple with kuker/lurking one/troll; diamond and extra/full-healing potions; transitive hostile blasphemous-lurker priest logic.
- `lethe-f`: apprentice witch, blessed, coven leader, dark young, deep dweller/deep one/deeper one, deminymph, electric eel, energy vortex, three goat-spawn sizes, giant mummy, Hmnyw-Pharaoh, kraken, kuker, mouth of the goat, oread, plains centaur, priest of an unknown god, The Good Neighbor, witch; blessed +3 bone viperwhip, gold magical-breathing amulet, ruby/star sapphire, trapped metal box, death/undead-turning wands.
- `lethe-g`: kuker, migo philosopher/queen/soldier/worker, mouth of the goat; aggravate-monster ring, flying boots, fortune cookie, full-healing/gain-energy potions, statues; board/pit traps.
- `lethe-z`: barbed/horned devils, Deep Ones, kuker, nightgaunt, raven, vampire bat; the **cursed +12 deep long sword named The Sword of the Deeps**, amnesia, life-saving amulet, corpse, create-monster scroll, earthquake drum, fire/full-healing/lightning/long-sword/protection/sleep/statue/taming loot; board, hole, magic, rust, and spiked-pit traps.
- `nkai-a-1`/`nkai-a-2`: the eldritch subset plus byakhee, Deep Ones, gug, nightgaunt, shoggoth; anti-magic, board, hole, magic, pit, and spiked-pit traps. `nkai-b`: Alhoon, parasitized doll, iron golem, green slime, skeleton, Deep Ones, gugs; death/create-monster loot. `nkai-c`: master/mind flayers, Deep Ones, priests. `nkai-z`: eldritch subset and gugs. Each has the up/four-hole pattern described above.
- `rlyeh`: Father Dagon, Mother Hydra, Great Cthulhu, star spawn, Deep Ones, mind flayers, nightgaunts, shoggoths, krakens, priests; the Silver Key, chests, wands, space mead; board/fire/magic/teleport traps.
- `lbyrnth`: Illurien, minotaur, minotaur priestess; 13 rust traps and six random spellbooks.

## 4. COMPLETE DEPENDENCY MATRIX

| Donor element | donor source file(s) | local status | required future action | compatibility adaptation |
|---|---|---|---|---|
| Three-dungeon topology and `neulev` | `dat/dngnch1.def`, `dngnch2.def`, `dngnch3.def`, `neutrality.des`, `labr.des` | NEW | register 3 dungeons, 3 branches, 21 floors, 26 map bodies | Lua conversion and existing persistent scheduler; no ad-hoc parent roll |
| Outlands feature family | `src/mkmaze.c`, `src/mkroom.c` | NEW | narrow C generator port for all eleven dispatched functions | use native rooms, doors, terrain, objects, connectivity and RNG; exclude `mkferrufort` |
| Deep one/deeper one/deepest one | `src/monst.c`, `makemon.c`, `mon.c`, `xhity.c` | REUSE | use Step 9C definitions and hooks | do not duplicate IDs or soul/cold-passive logic |
| 72 absent required species | `src/monst.c`, `makemon.c`, `monmove.c`, `mon.c`, `questpgr.c`, `role.c`, `priest.c` | NEW | append species and narrow birth/combat/death hooks | translate unsupported attacks/flags; no global sanity/insight |
| 48 native exact-name species | `dat/neutrality.des`, `labr.des` | REUSE | use native monster IDs | preserve map quantity/attitude/placement only |
| Neutral runtime monster selection | `src/questpgr.c:neutral_montype`, `rndmonst`; `src/mkroom.c:squadmon` | NEW | branch-scoped selectors and tests | preserve pinned observable weights, including duplicate Cuprilach outcomes |
| R'lyeh runtime selector | `src/questpgr.c:rlyeh_montype`, `rndmonst` | NEW | branch-scoped 1/20 override | preserve pinned inclusive-loop quantities and genocide fallbacks |
| Center of All | `src/monst.c`, `makemon.c`, `monmove.c`, `rndmonst` | NEW | unique species, equipment, branch-local arrival rule | no global 1/5000 spawn, migration, ward footprint, or endgame hook |
| Lethe level flag/water damage | `include/rm.h`, `src/sp_lev.c`, `trap.c`, `teleport.c`, `pager.c` | EXTEND | persistent per-level flag and scoped water behavior | ordinary `POOL`/`MOAT` plus level flag; do not alias to `PUDDLE` |
| Outlands `PUDDLE`, `SAND`, `SOIL` | donor `rm.h`, `mkroom.c`; local Step 9C | REUSE | use existing terrain IDs/iced state | add only branch behavior/color hooks |
| Mirror-shard pits and projectile materials | `src/trap.c`, `mklev.c` | EXTEND | branch-gated spiked-pit damage and arrow/dart material selection | no new trap ID; use existing material metadata |
| Outlands spell gradient | `src/spell.c`, `mcastu.c`, `mcastm.c` | NEW | branch-level spell modifier helpers | native spell system only; no donor global spell framework |
| Outlands trees | `src/dokick.c`, `dig.c` | EXTEND | branch-gated no-fruit/no-swarm kicking and wood-stick cutting | all trees outside branch retain native behavior |
| Gate Town pet separation | `src/dog.c` | EXTEND | branch-gated exception | no global pet hunger/feral change |
| Portals and normal dungeon records | donor maps/definitions; local `dungeon.c`, Lua loader | REUSE | register using standard structures | existing `#wizwhere` supplies location reporting |
| Shops/temples/barracks/court | maps, `mklev.c`, `mkroom.c`, `shknam.c`, `priest.c` | EXTEND | fixed Gate shops and branch-local procedural rooms | never change Step 5 global shop probabilities |
| Plumach shopkeeper morph | `src/shknam.c` | EXTEND | branch-local shopkeeper species substitution | retain normal billing/service engine |
| Blasphemous bridge priest | `src/role.c:god_priest`, `priest.c` | NEW | minimal bridge-temple creation/hostility rule | no donor pantheon or role/race import |
| Object material/size/properties | donor `obj.h`, `objnam.c`, `weapon.c`; local Step 9C | REUSE | encode special materials/sizes/properties in existing saved metadata | append only genuinely new base types |
| Amnesia scroll | donor maps; local `SCR_AMNESIA` | REUSE | use native scroll | Lethe can rewrite 1/10 of damaged scrolls to it |
| Potion of amnesia | `include/objects.h`, `src/potion.c`, `trap.c` | NEW | append potion and native-memory effect | omit donor sanity/insight reductions |
| Special weapon/tool bases and space mead | donor `objects.c`, `mkroom.c`, maps | NEW | append only bases listed in section 6 | use local skills/properties; no lightsaber-form framework |
| Upgrade kit and appearance lookup helpers | `src/mkroom.c`, `o_init.c` | EXCLUDE | substitute native utility loot in Outlands barracks | no clockwork/firearm/upgrade subsystem; silver/iron wand/ring means native type plus material metadata |
| Ten branch-created artifacts | `include/artilist.h`, `src/artifact.c`, `makemon.c`, maps/generators | NEW | append artifacts and narrow effects | Keys are optional loot; Silver Key cannot replace Invocation items or unlock global travel |
| Necronomicon donor invocation suite | `artifact.c`, `read.c`, `spell.c` | EXCLUDE | implement a bounded native reading effect/menu | no seals, wards, madness, insight, special skills, or global summon framework |
| Sanity/insight/madness fields and engine | `you.h`, `sanity.c`, monster flags/hooks | EXCLUDE | translate each required encounter as section 9 specifies | no permanent player mental progression fields |
| Donor wards/seals/alignment-quest state | `ward.h`, `engrave.c`, `you.h`, dungeon/endgame code | EXCLUDE | none | do not affect vanilla progression |
| Map colors and sparkling water descriptions | `src/mapglyph.c`, `pager.c` | EXTEND | branch/dungeon glyph color and description overrides | reuse existing symbols and tiles |
| Save IDs and capacities | local `obj.h`, `display.h`, `global.h`, `dgn_file.h`, `patchlevel.h` | EXTEND | append IDs, raise `MAXDUNGEON`, bump edit level | no old-save migration claim |

## 5. MONSTER / BOSS INVENTORY

### Complete species accounting

The maps name 109 exact species. The following 48 are existing local equivalents and must be reused: barbed devil, black dragon, cave spider, clay golem, deep one, deeper one, electric eel, elf mummy, energy vortex, ettin mummy, ghoul, giant eel, giant mimic, giant mummy, gray unicorn, green slime, hell hound, horned devil, human mummy, iron golem, iron piercer, killer bee, kobold lord, kraken, lich, lurker above, master lich, master mind flayer, mind flayer, minotaur, ochre jelly, ogre, ogre king, plains centaur, raven, red dragon, shark, shrieker, skeleton, succubus, trapper, troll, vampire, vampire bat, vampire lord, warg, wererat, and winter wolf.

The following 61 exact map-named species require new appended entries: alhoon; apprentice witch; Argenach Rilmani; argentum golem; bestial dervish; blessed; blood shower; byakhee; coven leader; crimson writher; Cuprilach Rilmani; dark young; deep blue cube; deep dweller; deminymph; ethereal dervish; Father Dagon; Ferrumach Rilmani; flashing lake; frosted lake; giant goat spawn; gnoll ghoul; goat spawn; Great Cthulhu; gug; hemorrhagic thing; Hmnyw-Pharaoh; Illurien of the Myriad Glimpses; kuker; living doll; living lectern; lurking one; man-faced millipede; many-eyed seeker; many-taloned thing; migo philosopher; migo queen; migo soldier; migo worker; minotaur priestess; mirrored moonflower; Mother Hydra; mouth of the goat; nightgaunt; ogre mage; oread; parasitized doll; pitch black cube; Plumach Rilmani; prayerful thing; priest of an unknown god; radiant pyramid; shoggoth; small goat spawn; smoldering lake; sparkling lake; star spawn; The Good Neighbor; tiny being of light; voice in the dark; and witch.

Normal transitive generation adds 11 absent species: Amm Kamerel, Hudor Kamerel, Sharab Kamerel, Ara Kamerel, Aurumach Rilmani, Shattered Ziggurat cultist, Shattered Ziggurat knight, Shattered Ziggurat wizard, hunting horror, blasphemous lurker, and Center of All. Deepest one is also transitive but already exists from Step 9C. Gold golem, jellyfish, and piranha are native. Total projected new monster entries is therefore **72**. Decorative Kamerel statues use the corresponding imported species; the unrelated donor `serpents[]` decoration list is not a required live-import dependency and native statue species are sufficient.

### Required behavior groups

- **Rilmani:** Plumach (level 4, slow, DR4, weapon/spell, acid/poison/electric resilience), Ferrumach (level 6, DR5), Cuprilach (level 8, fast, two weapons/spell, stalking/backstab), Argenach (level 9, DR4, weapon/spell, see invisible), and Aurumach (level 12, DR4, three weapons/spell, broad elemental/magic resistance and no spell cooldown). Fixed maps, Outlands selector, squads, courts, villages, and Sum use them. Preserve donor selector outcomes: two pinned branches return Cuprilach where comments imply Ferrumach; this is observable pin behavior, not silently “fixed.”
- **Kamerel:** Amm (level 3 clerical weapon user, stone/electric/reflective), Hudor (level 6 stationary amphibious wet touch), Sharab (level 10 flying two-claw cleric), and Ara (level 15 heavily defended artifact-stealing cleric). Towers/spires create them. Preserve reflection and movement distinctions with local flags; do not import holy/unholy alignment frameworks merely for hate flags.
- **Argentum:** argentum golem is level 18, slow, DR9/MR100, uses a 4d8 weapon attack and silver-arrow shot, is breathless/mindless/hostile/waiting, blunt-vulnerable, no-corpse/no-random-generation, and broadly resistant. Existing gold/iron golems remain native.
- **Dolls/lectern:** living doll is a peaceful level-15 mindless golem with 4d4 weapon attack and broad golem resistances; parasitized doll is hostile level 30 with stronger defenses; living lectern is a distinct monster. Step 9C living armor/masks supply patterns but are not substitutes.
- **Eldritch morph set:** bestial dervish, ethereal dervish, flashing/frosted/smoldering/sparkling lakes, blood shower, many-taloned thing, deep blue cube, pitch black cube, prayerful thing, hemorrhagic thing, many-eyed seeker, voice in the dark, tiny being of light, man-faced millipede, mirrored moonflower, crimson writher, radiant pyramid, iron piercer, kuker, living doll, lurking one, deep one, deeper one, and electric eel are the exact 26 direct choices on `lethe-b`; subsets recur below. Preserve attacks, stationary/amorphous/flying/aquatic traits, resistances, peaceful defaults, wait/close behavior, and conventional status effects. Translate only sanity/insight flags as section 9 states.
- **Goat entities:** small goat spawn (level 4), goat spawn (8), giant goat spawn (16), blessed (30), and mouth of the goat (90) retain their escalating weapon/butt/kick, gaze/devour/clerical, or poison/digest/acid-back attacks, movement, tracking, resistances, and no-random-generation rules. The global insight/sanity flags are omitted.
- **Witches:** apprentice witch (level 5), witch (10), coven leader (20), The Good Neighbor (unique level 66, four long-reach shred attacks plus spellcasting), and Hmnyw-Pharaoh (unique level 66, two 8d8 weapon attacks plus spellcasting) retain flight, casting, equipment, attitude, uniqueness, and wait/close rules. Do not import unrelated witch-familiar or role systems unless a pinned birth path invoked by these exact branch creations is demonstrated during Step 10B; the audited map/runtime paths do not require one.
- **Other named branch species:** byakhee is a fast level-11 flying/breathless hostile with 1d6 bite, two 2d4 claws, 1d3 stunning sting, regeneration, and sleep/poison/cold resistance. Dark young is a hostile amphibious level-25 wait/close predator with two 4d4 poison tentacles, 2d8 hug, 2d10 enhanced-acid bite, and acid/cold/fire/electric resistance. Deep dweller is a hostile amphibious level-21 tracker with 3d6 tentacle, weapon resistance, and cold/disintegration/stone/sleep resistance. Deminymph is a level-10 tunneling/teleporting collector with two 1d8 weapons and two seduction claws. Gnoll ghoul is a hostile mindless level-12 undead with 1d8 weapon, 1d4 claw, and 2d2 paralyzing bite. Gug is a hostile tunneling level-15 group monster with 2d6 weapon, two 1d6 claws, 1d12 hug, 3d6 paralyzing bite, and 1d6 item-stealing claw. Illurien is unique level 25, flying/teleporting/breathless/regenerating, hostile and waiting for the book, with 2d12 engulf-memory attack, 1d4 sticky claw, and 0d8 spellcasting; its donor-special memory attack must use bounded native forget/stat loss. Kuker is peaceful level 18 with two 4d8 weapons, 0d6 clerical casting, broad elemental/sleep/poison/magic resistance, and no spell cooldown. Living lectern is a hostile mindless level-7 golem with two 3d4 claws, 0d4 monster-magic attack, breathlessness, and sleep/poison/magic resistance. Lurking one is a flying level-45 waiting rock-thrower with four 1d8 weapon arms, 4d8 tentacle, 4d8 electric gaze, and strong poison/stone/electric/magic defenses; blasphemous lurker is a no-generation level-90 priest entity with range-five 4d8 death/pestilence/famine/conflict attacks plus blasting gaze and corresponding broad defenses. Nightgaunt is a flying/breathless level-15 group stalker with 1d6 tickling claw, regeneration, and sleep/poison/cold resistance. Ogre mage is a level-7 collecting ogre with 2d5 weapon and 2d6 spell attack. Oread is a level-7 wall-walking/teleporting collector with two seduction claws and iron aversion. Minotaur priestess is a hostile level-16 tracker with 2d8 butt and 0d10 clerical attack. Priest of an unknown god is a hostile amphibious level-25 rider-class entity with an unknown passive attack and broad fire/electric/sleep/poison/stone/drain defenses; port only its branch encounter behavior, never global Rider/endgame rules. Shoggoth is a hostile amorphous amphibious level-20 collector with 1d12 acid touch, 1d12 sticky touch, 2d12 sucking hug, 2d12 acid passive, and cold/electric/poison/acid/stone resistance. Star spawn is a flying hostile level-26 R'lyeh entity with two 4d4 weapons, 2d4 brain tentacle, psychic spell action, telepathy, and poison resistance.
- **Mi-go:** worker is a flying/breathless/tunneling level-7 collector with 1d6 claw; soldier is a flying/breathless level 9 group fighter with mist gaze, 1d10 weapon, and 2d3 electric claw; philosopher is a level-9 flying caster with mist gaze, 4d3 brain claw, 1d8 claw, and 4d3 spell; queen is level 25 with mist gaze, 3d3 paralyzing claw, 6d3 brain claw, and 6d3 clerical spell. Preserve their poison/acid/cold/sickness resistance, plus electric resistance where the source grants it, and their worker/soldier/philosopher/queen generation flags; replace insight/sanity presentation only through section 9.
- **Alhoon:** level 17 undead flayer with cold touch, brain attack, two drain-life tentacles, spellcasting, flight, regeneration, strong DR/MR, no spell cooldown, and broad undead resistances. Its inventory creates the Second Key of Neutrality, falling back to the Third if the Second already exists. Step 10B must make repeated fixed Alhoons deterministic without duplicating artifacts: once both artifacts exist, later Alhoons get an ordinary equivalent key, not a duplicate artifact.
- **Father Dagon/Mother Hydra:** unique level-60 amphibious R'lyeh bosses. Dagon uses two 3d8 weapons, 5d6 kick, and soul passive; Hydra uses four 3d6 bites, two 3d8 weapons, and soul passive. Preserve hostile/stalking/collecting behavior and cold/sleep resistance; map donor “soul” to the already imported Step 9C narrow soul-damage framework, not a global soul system.
- **Great Cthulhu:** unique level-100 R'lyeh terminal boss with enormous claw, permanent-Wisdom-drain gaze, poison passive, flight/swimming, broad defenses, and no random generation. Preserve its psychic blast as a monster-local action: 5d15 hero damage subject to native half-spell handling, long stun, and a bounded native mental effect; hostile non-mindless monsters can be confused and take 5d15. On death its `AT_NONE/AD_POSN` passive produces an 8d8 physical noxious explosion and a radius-2, duration-30 gas cloud. No separate Cthulhu revival hook was found in the pinned normal path; do not invent one. Do not port `MAD_DREAMS`, sanity loss, global scary-item exceptions, or unrelated telepath monsters.
- **Center of All:** unique level-18 invisible, hostile, no-corpse/no-random-generation Rilmani with three 2d8 weapons, spell, load/arrow action, broad resistances/reflection, and large +4 gold concordant bardiche, First Key, gold war hat, leather armor, robe, and gauntlets. Donor global `rndmonst` tries it 1/5000 anywhere and `monmove.c` can migrate it or leave ward/endgame state. The local contract instead tries the same 1/5000 only from branch runtime generation while in Neutral Quest or The Lost Cities. It may be peaceful before first entering Sum and hostile afterward, using a saved Sum-entered event bit; uniqueness/extinction prevents duplicates. It never migrates outside these dungeons and never writes a ward or endgame arrival state.

### Runtime pools

`neutral_montype` must be ported as a branch selector, not a global replacement. Gate Town returns no override. Before Sum it chooses among horse/random quadruped, weighted Rilmani, weighted Kamerel, quadruped-or-Shattered-Ziggurat member, and plains centaur in five equal outer branches; at Sum it uses the Aurumach/Argenach/Cuprilach/duplicate-Cuprilach/Plumach distribution. The Outlands barracks `squadmon` source has nominal Ferrumach 80, iron golem 15, argentum golem 4, Cuprilach 1 weights but samples with `rnd(80 + level_difficulty())`, leaving a depth-dependent fallback to the normal selector; implement the formula rather than presenting fixed percentages.

R'lyeh overrides only 1/20 ordinary runtime generation calls. Its selected groups are: 1% hunting horrors `d(2,3)+1` (3–7 due inclusive loop), 4% byakhee `d(2,4)+1` (3–9), 2% one shoggoth, 2% one deepest plus `rnd(4)+1` deeper (2–5) plus `rn1(2,1)+1` deep (2–3), 20% `rnd(3)+1` master mind flayers (2–4), 20% `rn1(2,2)+1` mind flayers (3–4), 20% `rnd(6)+1` deeper ones (2–7), and 31% `rn1(4,3)+1` deep ones (4–7), with donor genocide fallbacks where applicable. Preserve those pinned inclusive-loop quantities.

## 6. OBJECT / ARTIFACT / REWARD INVENTORY

### Direct map objects

The complete named direct set is: potion of amnesia, potion of extra healing, potion of full healing, potion of gain ability, potion of gain energy, potion of oil, potion of sickness, potion of speed; scroll of amnesia, scroll of create monster, scroll of earth, scroll of enchant weapon, scroll of taming, scroll of teleportation; amulet of life saving and a gold amulet of magical breathing; chest; drum of earthquake; magic marker; universal key (the Silver Key base); long sword and deep long sword; flying boots; leather armor; diamond, luckstone, ruby, and star sapphire; wands of cancellation, cold, death, fire, lightning, sleep, slow monster, speed monster, striking, and undead turning; bone viperwhip; class-selected knight/ranger/wizard/priest/rogue corpses; eggs; fortune cookies; statues including the named forgotten-god statue; rings of aggravate monster and protection; space mead; and the Necronomicon spellbook. Random object/class, random spellbook, random wand, chest-content, gold, boulder, and statue directives use native eligible tables; Step 10 must not enlarge them with unrelated donor-global types.

Existing local types cover all of that set except potion of amnesia, universal key, deep long sword, viperwhip, space mead, and the Necronomicon’s `SPE_SECRETS` base. `SCR_AMNESIA` already exists locally. New bases must be appended, never inserted. Potion of amnesia uses native `forget()` semantics and ordinary hallucination/messages but no sanity/insight changes. The +12 cursed Sword of the Deeps is a named deep-long-sword object, not an artifact.

### Procedural objects

The dispatched feature set additionally creates mirrors; robes; shields and amulets of reflection; bucklers; spears; salted-fish slime molds; booze; cram rations; torches; wand of striking; water; iron chests; tin/magic whistles, bells, and bugles; randomized rings/wands/tools/weapons/armor/spellbooks; gold; and material-overridden gold amulets/circlets, silver weapons/wands, and iron rings/wands. These are native equivalents plus Step 9C metadata. New special bases required by normal feature rewards are Kamerel vajra, double lightsaber, mirrorblade, khakkhara, Rakuyo, and torch/Shadowlander’s torch. Do not import donor combat forms or technology trees with them: define bounded native weapon dice/skills and light/reflection behavior.

Outlands barracks can put an `UPGRADE_KIT` in an iron chest. That item’s donor meaning depends on unrelated upgrade/technology systems. It is explicitly accounted for but excluded; substitute an identified native utility/repair tool of comparable value. `find_sawant`, `find_riwant`, `find_iring`, and `find_gcirclet` select randomized appearances, not new semantic item classes: use a legal native wand/ring/helm and apply SILVER, IRON, or GOLD through existing persistent material metadata. `find_sawant` reward sites may use a native silver wand; no donor randomized-appearance subsystem is needed.

### Artifacts and keys

Ten artifacts can be created in normal branch play and must be appended:

| Artifact | creation | donor effect | local contract |
|---|---|---|---|
| The First Key of Neutrality | Center of All inventory | restricted no-generation skeleton key; no active property | optional skeleton-key artifact; no progression gate |
| The Second Key of Neutrality | first eligible Alhoon | same | optional skeleton-key artifact; no progression gate |
| The Third Key of Neutrality | later eligible Alhoon | same | optional skeleton-key artifact; no progression gate |
| The Necronomicon | fixed `leth-d` treasure | `SPE_SECRETS`, donor multi-operation invocation | unique readable spellbook with a small, explicitly tested native effect/menu; no seals/wards/madness/global summons |
| Infinity’s Mirrored Arc | Kamerel tower treasure chance | double lightsaber; reflection/inherit; alternate mode | bounded double weapon, reflection while appropriately active; no lightsaber-form framework |
| The Staff of Twelve Mirrors | Kamerel tower treasure chance | khakkhara; reflection, displacement, +5d6 physical | native pole/staff skill mapping and those properties |
| The Sansara Mirror | Kamerel tower treasure chance | gold mirrorblade; reflection, half spell damage, +8d8 physical | retain named properties and material using local artifact hooks |
| Mirror Brand | minor spire treasure chance | silver long sword; alignment attack/stun/reflection | use native alignment comparison, stun, reflection; no donor alignment-quest dependency |
| Soulmirror | minor spire treasure chance | mithril plate; reflection, drain defense, +7 armor | native armor/property hooks |
| Silver Key | fixed R'lyeh object | silver universal key; energy regeneration, teleport control, portal invocation, polymorph control | optional treasure; retain passive properties and a branch-safe portal invocation only; never substitute for Bell/Invocation, unlock level teleport globally, or affect Castle/Gehennom/ascension |

Artifact existence, discovery, bones, naming, and save serialization use the native artifact array. Hmnyw-related wrappings and a Cthylla hand mirror are not created by audited normal branch paths and are excluded.

Materials required are native PAPER, CLOTH, LEATHER, WOOD, BONE, IRON, METAL, COPPER, SILVER, GOLD, MITHRIL, GLASS, GEMSTONE, and MINERAL. “Concordant” on the Center’s bardiche and “deep” on the Sword are donor-only descriptors: model concordant as an existing narrow object property and deep long sword as its appended base, rather than expanding the global material enumeration. Step 9C saved size/material/property metadata is sufficient for all other overrides.

## 7. TERRAIN / ENVIRONMENT INVENTORY

- **Lethe:** every Lost Cities map has donor `level.flags.lethe`; its visible water is ordinary `POOL`/`MOAT` rendered/described as sparkling. Add a persistent per-level Lethe flag. Entering/being immersed in Lethe water applies native amnesia and then Lethe inventory water damage. Teleport arrival in such water applies the same memory effect. Lethe is not Step 9C `PUDDLE`.
- **Lethe object damage:** strip blessed/cursed state; eligible scrolls become `SCR_AMNESIA` 1/10 and blank paper otherwise; nonartifact spellbooks become blank paper; water potions become potion of amnesia, other potions become uncursed undiluted water, and potion of amnesia remains itself; positive weapon/armor/tool enchantment or charges, including marker charges, are drained according to the pinned `water_damage` path; ordinary erosion/destruction rules still apply. Preserve artifact/Book-of-the-Dead exceptions. The donor’s luckstone and erodeproof-removal blocks are commented out and remain excluded.
- **Water movement:** ordinary drowning, swimming, submergence, teleport landing, and water-damage behavior are reused. No independent current/forced-movement mechanic was found in the pinned normal branch path; do not invent one.
- **Outlands terrain:** use existing `PUDDLE`, `TREE`, room/floor, walls, `POOL`, fountains/wells, altars, doors, iron bars where mapped, and existing iced-puddle state. `SAND` and `SOIL` infrastructure is reusable where maps request it. Mirror structures are constructed from ordinary supported terrain/objects, not a new global terrain family.
- **Trees:** only in the Outlands, kicking a tree yields neither fruit nor swarm and cutting produces wood sticks rather than fruit. Preserve global native tree behavior elsewhere.
- **Traps:** direct maps require land mine, squeaky `board`, pit, spiked pit, rust trap, magic trap, polymorph trap, level teleport trap, `anti magic` trap, fire trap, teleport trap, and hole. There is no progression trapdoor. Outlands spiked pits become mirror-shard pits without a new trap ID: 1d12 physical damage plus 1d20 against silver-hating targets, never poison. Arrow/dart traps choose donor material among METAL/IRON/COPPER/SILVER/GOLD using branch-gated source weights and persistent metadata.
- **Flags:** preserve hardfloor, `nommap`, shortsighted, no-teleport, lighting, maze, town, morgue, zoo, swamp, temple, and Lethe semantics on the exact maps listed in section 3. `nommap` blocks mapping in native fashion; it is not a new discovery system.
- **Colors:** Outlands walls are brown and floors brown/black; R'lyeh walls are bright blue and floors blue; other Lost Cities walls are black and floors gray when lit/black when dark. Implement as branch/dungeon glyph-color overrides with existing symbols/tiles.
- **Spells:** hero casting penalties on Gate/out1/out2/out3/out4 are `-10/-20/-30/-40/-50 × spell level`; Spire has no hero penalty; Sum gives `+100 - 10 × spell level`. Monster casting fumble modifiers are +2/+4/+6/+8/+10 on those five levels, always-fumble on Spire, and -1 on Sum. Implement only in these levels using native spell resolution.
- **Portals:** portals in Neutral are marked seen as in the donor. Use native portal traps and normal branch records; do not add a connector type.

## 8. SHOP / SERVICE INVENTORY

Gate Town has exactly six fixed shops: armor, potion, four food shops, tool, and general store, plus a temple and beehive. `neulev` has a weapon shop and wand shop plus nine barracks. These use native shop types, stock, billing, priests, and room mechanics.

Plumach villages can place an ordinary shop selected within the donor’s ordinary-shop range, a temple, barracks, court, Plumach/loot room, gold-golem treasury, or tool room in their principal buildings; huts and other structures retain source content. In an Outlands procedural shop, the created shopkeeper is changed to a Plumach. Branch barracks use the Neutral squad selector and the iron-chest loot described in section 6. The bridge temple creates a hostile blasphemous lurker/priest relationship through a narrow `bridge_temple` rule.

No new paid service is branch-required. Step 9C’s shopkeeper morph, branch-local stock injection, material overrides, containers, billing, and temple infrastructure are reusable with narrow extensions. All fixed/procedural shop decisions are gated by Neutral level identity. Step 5’s global shop-generation probability and global stock tables must remain byte-for-byte behaviorally unchanged outside the branch.

## 9. SANITY / INSIGHT / MADNESS AUDIT

The donor tags many eldritch monsters with sanity-loss and/or insight flags and uses `u.usanity`, `u.uinsight`, `u.umadness`, `sanity.c`, visibility/generation gates, and madness-specific reactions. None of those global persistent systems is required to express the audited topology, attacks, movement, resistances, loot, or connectors. They are explicitly excluded.

The local encounter contract is:

| donor-visible semantic | minimum required local semantic | narrow adaptation |
|---|---|---|
| unsettling first sight/presence for eldritch forms, goats, R'lyeh entities | encounter feedback and temporary impairment | one monster-local, once-per-instance or cooldown-gated visible-presence check; message plus bounded confusion, stun, hallucination, or temporary Wisdom exercise/save according to severity |
| insight-only reveal/generation flags on morphs | preserve mysterious presentation without persistent progression | monsters remain normally renderable when generated by their audited map/pool; use hallucination-aware descriptions and no insight gate |
| Great Cthulhu telepathic attack and `MAD_DREAMS` | damaging psychic pressure, stun, disorientation | keep 5d15 blast and long stun; replace madness bit/sanity loss with bounded hallucination/confusion and temporary Wisdom penalty using native timers; monster victims take damage/confusion |
| permanent-Wisdom-drain gaze | lasting conventional mental harm already supported | retain native permanent Wisdom drain with resistance/save behavior |
| potion/Lethe sanity and insight reduction | memory loss | call native amnesia/forget behavior only |
| donor sanity-dependent messages/AI | atmosphere, not global progression | preserve source-relevant messages at map/encounter boundaries; omit numeric sanity thresholds |

Step 10B must centralize these adaptations in branch/monster-scoped helpers and test duration, resistance, hallucination text, save/reload, and non-leakage. It must not add `u.usanity`, `u.uinsight`, a madness bitfield, `sanity.c`, permanent hidden mental progression, or an equivalent renamed system. Removing mental behavior entirely is also noncompliant.

## 10. GLOBAL-SYSTEM EXCLUSIONS

The following donor systems are outside Step 10 and must not be imported: alignment-quest selection or Law/Chaos quest variants; alignment keys as endgame gates; Castle, Gehennom, Invocation, Amulet, vanilla Quest, or ascension changes; donor roles, races, gods, pantheons, or quest text framework; global sanity, insight, madness, or `sanity.c`; wards, seals, Center ward footprints, and engraving extensions; global skills, lightsaber forms, spell framework, resistance/intrinsic redesign, armor-slot redesign, or full DR rewrite; donor-wide material/object randomization; global shop probabilities or stock replacement; global tree behavior; Silver Key as Bell substitute or global level-teleport bypass; global Center 1/5000 generation, cross-dungeon migration, or endgame arrival; unrelated dungeons, monsters, hallucination lists, artifacts, and current donor master; `mkferrufort`; inactive branch markers; and any unreferenced technology/upgrade framework.

Donor alignment hostility, holy/unholy hate flags, extramission, insight senses, no-spell-cooldown, unusual attack types, and equipment mechanics are imported only where a named required entity needs an observable effect and only through the smallest local equivalent.

## 11. STEP 9C REUSE MATRIX

| dependency | classification | contract |
|---|---|---|
| deep one, deeper one, deepest one | REUSE AS-IS | same local IDs, growth/death/soul and cold-passive paths; no duplicates |
| `PUDDLE` shallow water and iced state | REUSE AS-IS | Outlands rivers/coasts only; Lethe stays ordinary water plus flag |
| `SAND`, `SOIL`, branch terrain predicates | REUSE AS-IS | use where exact maps/generators require |
| portals, Lua special maps, standard dungeon/branch records | REUSE AS-IS | supports connectors and `#wizwhere` |
| object `obranch_material`, `obranch_size`, `obranch_props` persistence | REUSE AS-IS | gold/silver/iron/copper/bone/mithril/size/property overrides |
| material/size combat, DR, coatings | REUSE WITH NARROW EXTENSION | add only concordant and new base-type effects; reuse existing hit paths |
| branch-local shop stock/morph/billing helpers | REUSE WITH NARROW EXTENSION | Plumach morph and Neutral loot; no global probability changes |
| dNetHack adaptation helper patterns and saved spare-field discipline | REUSE WITH NARROW EXTENSION | use scoped helpers, document any newly allocated bits |
| tile artwork reuse and generated-table tests | REUSE AS-IS | every new entry gets an intentional existing art mapping unless new art is approved |
| living armor and masks | NOT SUFFICIENT | frameworks may guide code, but living doll, parasitized doll, and living lectern are distinct new species |
| Eladrin forms | NOT RELEVANT | no audited normal Neutral generator/map requires them; do not duplicate or add branch generation |
| Mithardir syllables/tiles and living-equipment service | NOT RELEVANT | no Step 10 connector, reward, or generator depends on them |

## 12. SAVE / ID / EDITLEVEL ASSESSMENT

Step 10B/C will append 72 monster IDs, approximately 13 semantic base object types if the excluded upgrade kit uses native substitution (potion of amnesia, universal key, deep long sword, viperwhip, space mead, secrets spellbook, Kamerel vajra, double lightsaber, mirrorblade, khakkhara, Rakuyo, torch, Shadowlander’s torch), 10 artifact IDs, 3 dungeon identities, 26 special-map prototype names, and 3 branch records (DoD→Neutral, Neutral→Lost Cities, Neutral→Dispensary). No new trap or terrain ID is required: Lethe is a saved level flag and mirror-shard pits reuse spiked pits.

Local representations are ample when entries are appended: `struct obj.otyp` is signed `short`; `corpsenm` is `int`; `oartifact` is `char`; monster indices used by objects are `int`; dungeon `dnum` is byte-sized but constrained by `MAXDUNGEON`; and level depths already use the widened Step 6–9 ledger architecture. The current enumerated baseline recorded during audit is `NUMMONS=430`, `NUM_OBJECTS=526`, `AFTER_LAST_ARTIFACT=37`, and `MAX_GLYPH=10762`. Projection is roughly 502 monsters, 539 objects, artifact sentinel 47, and `MAX_GLYPH` about 12,516 (24 glyph slots per added monster and two per object), all far below their integer limits. Exact generated counts must replace estimates after Step 10B.

The blocking compile-time capacity is `include/global.h:MAXDUNGEON 16`. Fifteen dungeons are currently registered; three more require `MAXDUNGEON >= 18`. `LEV_LIMIT=128` and `BRANCH_LIMIT=32` in `include/dgn_file.h` remain sufficient: current definitions plus 26 prototypes and three branch records remain below them, but Step 10C must assert actual compiled counts. `MAXLINFO` automatically scales with `MAXDUNGEON * MAXLEVEL`.

Persistent state includes appended IDs, dungeon topology and `sp_levchn`, the Lethe level flag, Center uniqueness/attitude and Sum-entered eligibility, artifact existence, and existing object metadata. Prefer an existing saved event spare bit for Sum-entered if one is demonstrably unallocated; otherwise add an explicitly named saved bit/field. Do not overload Step 9 monster spare bits. Bones carrying new species/objects/artifacts and recovery files share the same compatibility boundary.

**Recommendation: advance `EDITLEVEL` from 4 in Step 10B before any serialized Step 10 content is accepted.** Reasons are appended enum identities, changed dungeon array capacity/layout and topology, a new persistent level flag, possible event state, and new artifact/object/monster save values. No migration is authorized; pre-Step-10 saves, bones, and recovery files must be rejected by the normal epoch check rather than misread.

## 13. TILE / GLYPH / TABLE CAPACITY AUDIT

**Step 10B correction (2026-09-10):** the original projection in sections 12–13
below used 24 glyphs per added monster. Compiling the actual local glyph enum
and inspecting every offset proves **22**, not 24: 9 monster/body/ridden slots,
8 swallow slots, 2 statue slots, and 3 piletop corpse/statue slots. The exact
formula with the current terrain/trap families is
`MAX_GLYPH = 22 * NUMMONS + 2 * NUM_OBJECTS + 250`. Thus the unchanged projected
72/13 additions would yield **12,372**, not 12,516. The earlier audit text is
retained below as historical material; this correction supersedes its estimate.
Actual B1 counts remain 430/526/10,762 because no new declarations have landed.

`include/display.h` derives glyph offsets from `NUMMONS` and `NUM_OBJECTS`; there is no fixed 2,500-glyph table. `glyphmap[MAX_GLYPH]`, `win/share/tilemap.c:tilemap[MAX_GLYPH]`, and `tilelist[MAX_GLYPH]` scale at compile time. Each added monster contributes 24 glyph positions across male/female, pet, detected, body, ridden, swallow, statue, and piletop families; each object contributes two. With 72 monsters and about 13 bases, projected `MAX_GLYPH` is approximately `10762 + 1728 + 26 = 12516`. Glyph values and loop indices are `int`-scale.

The narrowest representation is `glyph_map.tileidx`, a signed 16-bit `short` in `include/wintype.h` (and unsigned short in X11). That indexes unique artwork, not glyph count. Step 9C’s final audit generated 2,554 tile mappings in a 640×1390 bitmap, so reuse leaves very large headroom below 32,767. All 72 new species, 13 bases, 10 artifacts, and branch-color/terrain appearances must receive explicit mappings to existing semantically suitable artwork under the project convention. Artifacts normally reuse base-object art; gender/body/statue/piletop glyphs reuse associated tiles. No new terrain artwork is required. If any genuinely new artwork is later approved, keep final `total_tiles_used < 32767` and prove the bitmap dimensions contain all pixels.

Step 10B must run the tile generator and the Step 9 `test_step9a_tiles.py` style audit, checking every generated glyph index, tile reference, 16×16 pixel payload, reused-art declaration, generated `src/tile.c`, and `win/win32/tiles.bmp` capacity. It must test that appending IDs shifts generated offsets consistently without aliases or truncation. Generated resources remain build products unless existing policy says otherwise. x64 and Win32 must both compile; signed `char oartifact` remains safe at projected ID 46, but add a compile-time assertion that the artifact sentinel fits the field.

Conclusion: glyph and tile capacity is safe under the required reuse policy, but dungeon-table capacity is not and must be raised. No ID renumbering or insertion is allowed.

## 14. FINAL SCHEDULER SPECIFICATION

Step 10C must integrate Neutral at `src/dungeon.c:step6b_schedule`, using the same persistent randomized topology pass as Sheol, Dragon Caves, and Mithardir.

1. Add lookup of the DoD→Neutral branch, Neutral dungeon number, and `neulev` approach special level after dungeon definitions are loaded. Missing or duplicate records are fatal diagnostics in debug/tests.
2. Generalize `step6b_step9_branch`/`step6b_step9_approach` names or add equivalent internal predicates so the provisional DoD→Neutral endpoint and `neulev` do not count as occupied while candidates are gathered. Do not alter public constants in Step 10A.
3. Schedule Neutral once, alongside the existing Step 9 branch group and before Temple/Moria/room placements, from the shared collision ledger over DL30–199 inclusive with `ordinary=FALSE`. Rebase both `neulev` and the branch endpoint to the chosen DoD level exactly as Mithardir’s approach/portal pair is rebased, then mark the parent used.
4. Do not make an independent RNG call in Lua or branch generation, do not reserve DL111, and do not use a validation-only fixed parent. DL111 is an ordinary legal candidate whenever no prior scheduled feature occupies it.
5. The selected parent and all resulting records are saved through normal dungeon state. Exactly one Neutral branch exists per game. The Dispensary parent is resolved inside Neutral from its definition and does not consume a DoD ledger slot.
6. Preserve occurrence/count contracts for Big Rooms, Giant Court, Real Zoo, Dragon Lair, Temple of Moloch, Lost Tomb, Ruins of Moria, Sheol, Dragon Caves, Mithardir, and Castle at DL200. Tests compare their counts and collision invariants before/after Neutral integration.

The maximum parent remains 199 even though descendants display through depth 217; Step 6–9 widened ledger/save code, not the DoD parent picker, handles descendants.

## 15. #WIZWHERE / MANUAL LOCATION CONTRACT

No new command is required. Register all special levels normally so vanilla `#wizwhere` lists `neulev`, `gatetwn`, `out1`, `out2`, `out3`, `out4`, `spire`, `sumall`, the chosen `leth-a/c/d` variants, `lethe-b/e/f/g/z`, the chosen `nkai-a` variant, `nkai-b/c/z`, `rlyeh`, and `lbyrnth`.

Given any listing and known logical number:

- `neulev` is the DoD parent itself: displayed depth is `P`.
- Gate Town also displays at `P` because the connector is a portal; the parent is neither plus nor minus one.
- Neutral level `N` displays `P+N-1`, so derive `P = displayed-(N-1)`.
- Lost Cities level `L` displays `P+L+5`, so derive `P = displayed-(L+5)`. `lethe-b` at level 2 is `P+7`; R'lyeh is `P+18`.
- Dispensary level 1 uses a downward branch stair from a randomly selected Neutral level 2–6 and displays one deeper than that selected parent, namely `P+N`.

Manual validation must follow the bidirectional portals from `neulev` through Spire, stairs into Sum/Lost Cities, holes downward through N'Kai, and every up-stair return; separately identify and traverse the Dispensary branch. `#wizwhere`’s branch table should reveal its actual Neutral parent.

## 16. STEP 10B IMPLEMENTATION CONTRACT

Step 10B is engine/data/tile work only; it must not port the branch maps or register the final topology.

- Append the 72 new species and implement the grouped attacks, defenses, movement, AI, birth inventory, uniqueness, death/revival, selector, and narrow mental behavior in section 5. Reuse the three Deep Ones.
- Append the 13 projected semantic base types and ten artifacts in section 6; replace upgrade-kit loot and appearance helpers as specified. Implement optional keys and bounded artifact effects without progression hooks.
- Add the persistent Lethe level flag and scoped water/amnesia/object-damage helper; branch spell gradient, tree rules, mirror-pit damage, projectile material rules, colors/descriptions, portal-seen behavior, pet exception, priest rule, and shop/generator support primitives.
- Add only the minimum saved event state required for Center/Sum, raise `MAXDUNGEON` to at least 18, append IDs, add assertions, and advance `EDITLEVEL` from 4. Do not add migration.
- Add explicit reused artwork mappings and validate generated glyph/tile/bitmap tables.
- Add primitive/unit fixtures for every selector boundary, inclusive group count, birth inventory, unique/key fallback, Lethe transformation, artifact effect, mental-duration/resistance path, spell modifier, trap damage/material distribution, and non-branch non-leakage.
- Stabilize x64 Release and Win32 Release. Step 10B completion requires no placeholder monster/object behavior and no maps/topology beyond isolated fixtures necessary to test primitives.

## 17. STEP 10C IMPLEMENTATION CONTRACT

Step 10C ports all 26 concrete `.des` map bodies into Lua resources, with four equal variant choices producing 21 logical branch floors plus the `neulev` DoD approach. It implements all eleven dispatched procedural functions and exact call ordering/probabilities, with connectivity and placement safeguards. It registers Neutral Quest (7), The Lost Cities (13, entry 2), The Dispensary (1), DoD→Neutral portal, Sum→Lost Cities down branch, and random Neutral 2–6→Dispensary down branch.

Every flag, message, fixed/random region, room, monster, object, trap, portal, stair, hole, landing region, inactive-marker exclusion, and boss location in sections 2–8 is required. The scheduler integration follows section 14 and is the only DoD parent selection. Focused tests must prove exactly one branch, DL30–199 distribution, collision freedom, no DL111 reservation, persistent reload, every variant reachable across seeds, correct display-depth arithmetic, hole destinations, complete return path, Dispensary parent range, no inactive subbranches, and unchanged Step 5–9 occurrence contracts.

## 18. STEP 10D VALIDATION CONTRACT

Step 10D performs no new design. It must validate x64 Release and Win32 Release; native/non-PTY fresh-game topology sampling; all map variants and procedural feature probabilities using deterministic fixtures plus broad samples; complete live traversal from DoD entry through R'lyeh and back; separate Dispensary traversal; boss/unique/reward encounters; Lethe inventory and immersion behavior; shops/temples/barracks; tile and TTY display; save/reload on representative Neutral, Lethe, N'Kai, R'lyeh, and Dispensary floors; recovery; bones compatibility boundary; DLB/package presence for every resource; and cumulative Step 5–9 regressions.

Force/sample a legal parent near DL199 and validate descendant ledger depths through 217, `#wizwhere`, save/reload, return traversal, and Castle DL200 independence. Produce `doc/step10-playtest.md` only in Step 10D with exact wizard/manual instructions and leave the requested dirty-tree handoff for user validation. Do not stage, commit, tag, or push without a later explicit instruction.

## 19. OPEN RISKS / ARCHITECTURAL QUESTIONS

There are **no unresolved architectural blockers** to starting Step 10B. The following are bounded implementation decisions which must be resolved against the same pin and locked by tests:

- Choose the exact native replacement/value for the excluded upgrade kit and exact native candidates for appearance-selected silver/iron wand/ring/circlet rewards. This does not affect topology or save design.
- Define the Necronomicon’s bounded native menu and Silver Key’s safe portal destination set while preserving their identity and excluding all progression gates.
- Translate donor-only attack/defense flags (artifact theft, soul passive, shred/reach, wet, devour, reflection, displacement, spell cooldown, concordant) to the existing local subset one entity at a time. Unsupported donor-wide semantics must not be smuggled in.
- Implement and test the pinned Cthulhu death explosion/gas cloud and absence of a special revival path without importing the donor’s global gas/state machinery.
- Determine an actually unallocated saved event bit for `sum_entered`; add a named field if none exists. Either path already requires the edit-level bump.
- Recompute generated enum/glyph/tile counts after the final Step 10B list. The capacity conclusion is robust, but the estimates in sections 12–13 are not substitutes for generated assertions.

These are ordinary engine implementation tasks, not reasons to repeat the topology/dependency audit.

## 20. SOURCE INVENTORY

All donor files were queried at `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` through `git show`, `git grep`, or `git ls-tree`, under `dnethack-3.4.3/`:

- Definitions/maps: `dat/dngnch1.def`, `dat/dngnch2.def`, `dat/dngnch3.def`, `dat/neutrality.des`, `dat/labr.des`.
- Headers/tables: `include/align.h`, `include/artilist.h`, `include/artifact.h`, `include/decl.h`, `include/dungeon.h`, `include/extern.h`, `include/monattk.h`, `include/monflag.h`, `include/objects.h`, `include/rm.h`, `include/sp_lev.h`, `include/you.h`.
- Dungeon/generation: `src/dungeon.c`, `src/sp_lev.c`, `src/mklev.c`, `src/mkmaze.c`, `src/mkroom.c`, `src/questpgr.c`, `src/o_init.c`, `src/mkobj.c`.
- Monsters/combat/AI: `src/monst.c`, `src/makemon.c`, `src/mon.c`, `src/monmove.c`, `src/mcastu.c`, `src/mcastm.c`, `src/xhity.c`, `src/weapon.c`, `src/wield.c`, `src/zap.c`, `src/wizard.c`, `src/attrib.c`.
- Objects/artifacts/environment/UI: `src/artifact.c`, `src/objnam.c`, `src/objects.c`, `src/potion.c`, `src/read.c`, `src/trap.c`, `src/teleport.c`, `src/dig.c`, `src/dokick.c`, `src/dog.c`, `src/pager.c`, `src/mapglyph.c`, `src/spell.c`, `src/shknam.c`, `src/priest.c`, `src/role.c`, `src/allmain.c`.

Important local files inspected against the finalized baseline:

- Architecture/history: `doc/step9c.md`, `doc/step9d.md` (leads only), `dat/dungeon.lua`, `src/dungeon.c`, `include/patchlevel.h`.
- IDs/save/capacity: `include/global.h`, `include/dgn_file.h`, `include/dungeon.h`, `include/display.h`, `include/wintype.h`, `include/obj.h`, `include/objclass.h`, `include/objects.h`, `include/monsters.h`, `include/rm.h`, `include/you.h`.
- Existing reusable implementation: `src/makemon.c`, `src/mon.c`, `src/mondata.c`, `src/mkobj.c`, `src/objnam.c`, `src/sp_lev.c`, `src/shknam.c`, `src/weapon.c`, `src/invent.c`, `src/trap.c`.
- Tiles/tests: `win/share/tilemap.c`, `test/test_step9a_tiles.py`, and the Step 9C focused tile/topology test references documented in `doc/step9c.md`.

The obsolete `doc/step9d.md` was not used as authority. Where its historical expectations differ from the files above, the pinned source and finalized local implementation control.

## 21. STEP 10B IMPLEMENTATION STATUS — PARTIAL B1, BLOCKED RUNTIME GATE

**Historical checkpoint, superseded by section 22 below.** The user restored
the quarantined files and reported an exclusion; the resumed runtime checks
now pass. Preserve this section as the original failure/backup evidence.

Recorded 2026-09-10. **Step 10B is NOT complete and is NOT ready for Step 10C.**
Only compatibility scaffolding and build/generated-data verification have landed.
No monster batch was started, and no placeholder content was added. This is a
compiling partial handoff, not a passing B1 runtime/persistence gate.

### Starting state and authority

- Branch `phase0/dod-length`; HEAD and `origin/phase0/dod-length` both
  `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`. The annotated milestone tag
  `step9-sheol-dragon-caves-mithardir^{commit}` resolves to the same commit.
- No tracked changes initially; exactly `doc/step10.md` intentionally untracked.
- Initial document and `E:/Codex/My_Nethack/_qa/step10a-audit.md.backup` SHA-256 both
  `4B7DAF27621BF4A5EDF8AF803EF2F38D0E72FCBBCE842050495E0C747A9C1A41`.
  The initial document was read completely before production edits.
- Pinned donor commit `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` verified in
  `E:/Codex/My_Nethack/_qa/dnethack-donor-pinned`; no donor content was modified. No new donor
  behavioral adaptation has been attempted in this partial B1.
- Submodules unchanged: Lua `6e22fedb74cf0c9b6656e9fce8b7331db847c605`,
  pdcurses `09cf16db29753305e4241d4ae609aac997fa730d`, pdcursesmod
  `6cd9c16900fef82754923c718fab7fe85f761bb6`.

### Verified Windows generated-data pipeline

Authority is `sys/windows/vs/NetHack.sln`, its project dependencies, each
`.vcxproj`, and `aftermakedefs.proj`, `aftertilemap.proj`, `aftertile2bmp.proj`,
and `afterdlb.proj`. Do not invoke imaginary object-file solution targets.

1. Solution dependencies provide `fetchprereq`, `lualib`, `nhlua_h`, and
   `hacklib`. `nhlua_h` supplies the generated Lua wrapper header.
2. `makedefs` is a verified solution target. Its project compiles `monst.c`,
   `objects.c`, `date.c`, `alloc.c`, and `util/makedefs.c`. Its after-build
   project runs the built executable with `-d`, `-r`, `-s`, and `-h`.
3. **Serialized IDs are compile-time enums, not generated ID headers here.**
   `permonst.h` includes `monsters.h` with `MONS_ENUM`; `objclass.h` includes
   `objects.h` with `OBJECTS_ENUM`; `hack.h` includes `artilist.h` with
   `ARTI_ENUM`. `src/monst.c` and `src/objects.c` instantiate the same tables.
   `pm.h` and `onames.h` do not exist in the current checkout. `files.props`
   still lists `onames.h` as a legacy output, but the after-build project
   does not run `makedefs -o`; this is not a missing-ID source defect.
4. `tilemap` compiles the actual monster/object tables plus `drawing.c` and
   `win/share/tilemap.c`; its after-build runs the tool from `src` to generate
   `src/tile.c`. `tile2bmp` compiles those tables, `tiletext.c`, and its
   TILETEXT variant of `tilemap.c`; its after-build generates then moves
   `tiles.bmp` into `win/win32`. Both are verified solution target names.
5. `dlb` depends on `makedefs` and packages the current data resources.
   `NetHack` builds the console engine through its solution dependencies;
   `NetHackW` additionally depends on the tile tools. The full solution builds
   both game executables, recovery, data, and packages.

Executed successfully in the VS 18 BuildTools developer environment:

```text
MSBuild.exe sys\windows\vs\NetHack.sln /t:makedefs /p:Configuration=Release /p:Platform=x64 /v:m /nologo
MSBuild.exe sys\windows\vs\NetHack.sln /t:makedefs,tilemap,tile2bmp /p:Configuration=Release /p:Platform=x64 /v:m /nologo
MSBuild.exe sys\windows\vs\NetHack.sln /p:Configuration=Release /p:Platform=x64 /v:m
MSBuild.exe sys\windows\vs\NetHack.sln /p:Configuration=Release /p:Platform=Win32 /v:m
```

No generated source/resource was hand-patched. `src/tile.c`, `tiles.bmp`,
tools, binary outputs, and packages follow existing Git ignore policy.

### Compatibility changes and saved-state ownership

- `EDITLEVEL` advanced **exactly 4 -> 5**. This is the shared Step 10B/C epoch;
  do not bump again merely when Step 10C adds the audited topology.
  `VERSION_COMPATIBILITY` remains disabled; no migration was added.
- `MAXDUNGEON` advanced **exactly 16 -> 18**; `MAXLINFO` is now **3,600**.
  No new dungeon or scheduler entry accompanies this capacity change.
- `u.uevent.sum_entered` owns the first previously free event bit after
  `amulet_wish`. Baseline `u_event` declares 17 occupied bits and explicitly
  documents seven free bits; repository field/spare searches show no other
  owner of this bit. Six free bits remain. This is saved inside `struct you`
  and initialized to zero by the existing full-`u` reset in `u_init()`.
  No Step 9 `uspare1` or monster `mspare1` bits were reused.
- `svl.level.flags.lethe` is a named bit after `stormy` in `struct levelflags`.
  `clear_level_structures()` explicitly resets it to zero. The production
  save and restore paths already use `Sfo_levelflags` / `Sfi_levelflags`; the
  historical codec writes/reads the complete structure. Lethe water behavior
  and a map flag setter are NOT implemented by this allocation.
- `hack.h` has a compile-time typedef assertion that the highest artifact ID
  fits signed char (127), safely covering Windows `obj.oartifact` and ports
  where plain char is unsigned.
- Existing depth/ledger tests retain exact capacity/version assertions, now
  at 3,600, 28,808-byte choice layout, and epoch 5; ledger pair fixtures add
  3,599 and 3,600 while retaining the old 3,199/3,200 cases.
- Historical source tests use exact, one-occurrence B1 reverse projections
  before whole-file comparisons. They still reject every unrelated byte change.

### Actual IDs, tables, tiles, and builds

No monster, object, or artifact entry was changed or added. Both current
Release executables report identical complete enum dumps:

| quantity | actual partial B1 value |
|---|---:|
| NUMMONS | 430 |
| NUM_OBJECTS | 526 |
| AFTER_LAST_ARTIFACT | 37 |
| actual artifacts | 36 |
| MAX_GLYPH / generated glyphmap entries | 10,762 |
| total_tiles_used | 2,554 |
| bitmap | 640 x 1390, 8 bpp |

`test_step10b_generated.py` checks both Release dumps, the exact glyph formula,
every glyph index in sequence, every tile index, signed tile capacity, full
bitmap pixel payload, and room for every 16x16 tile. The Step 9 reused-art
audit additionally checks all source tile payloads and existing artwork reuse.
These PASS results cover the **current B1 table**, not absent Step 10 entities.

Full solution builds both returned exit 0; current binaries:

- x64 `binary/Release/x64/NetHack.exe`: 5,880,832 bytes,
  **2026-09-10 07:28:59.9757058 UTC**.
- Win32 `binary/Release/Win32/NetHack.exe`: 4,766,208 bytes,
  **2026-09-10 07:30:35.1358281 UTC**.

Builds report warnings in unchanged `timeout.c`, `worn.c`, `mkmap.c`,
`u_init.c`, and Win32 `mswproc.c`, plus Win32 Lua `/Oi` option override.
They were not hidden or treated as new B1 source changes. Logs are outside
the repository in `E:/Codex/My_Nethack/_qa/step10b-tests/b1-generators-x64.log`,
`b1-release-x64.log`, and `b1-release-Win32.log`.

### Tests, failure evidence, and exact stopping point

PASS:

- `py -3 -B test/test_step10b_source.py`: exact B1 production diff,
  unchanged declarations/save routing/topology/scheduler/README.
- `py -3 -B test/test_step10b_generated.py`: both binaries, all 10,762
  glyph entries, all tile references, bitmap payload and capacity.
- `test/test_step9a_tiles.py`: reused-art and bitmap audit.
- `test/test_step7_source.py` and `test/test_step8a_source.py`, including
  `binary/Release/x64/nhdat500` and `binary/Release/Win32/nhdat500` arguments:
  protected source and exact current packaged resource bytes.

BLOCKED / NOT PASS:

- `run_step10b_compatibility.py` compiles all three enum declaration headers
  from a `git archive` of the exact baseline into an external fixture, and
  compiles the current headers with actual extracted historical save macros,
  native pointer normalizers, and the production `check_version()` body.
  The current C fixture compiled successfully with `/std:c11 /W4 /WX` after
  fixture-only linkage/annotation corrections. It covers all four Sum/Lethe
  combinations with adjacent Step 9 state, native serialized bytes, old/future
  epoch rejection, and every old serialized ID. **Its runtime assertions have
  NOT passed.** The runner now requires explicit output evidence even on exit 0.
- Malwarebytes quarantined
  `E:/Codex/My_Nethack/_qa/step10b-tests/x64/step10b_compatibility.exe` at
  **2026-09-10 07:25:34 UTC** and again at **07:26:55 UTC**. The latter
  diagnostic event is
  `C:/ProgramData/Malwarebytes/MBAMService/RtpDetections/0025700e-ace9-11f1-91bf-7c10c942688a.json`:
  `Malware.AI.3985130059`, successful quarantine, SHA-256
  `D8B05298F5320EC86DF350A5E1671AE8F32808E1375C3D0806539902B09D9759`.
  Execution yielded no output even though the process exit was zero, and the
  file disappeared. No protection was disabled, no exclusion added, and no
  fixture rewrite/rebuild was attempted after confirming the quarantine.
  User review of the endpoint-protection event is needed before this gate
  can run. Source compilation is not substituted for save/restore evidence.
- A Step 9A source test invocation without its required donor-clone argument
  failed argument parsing; it was not counted as a passing regression.
- Depth/ledger/recovery runtime gates, Win32 compatibility fixture, complete
  Step 9C runtime matrix, and representative live save/reload remain unrun.
  The overall focused and regression gates therefore remain **FAIL/incomplete**.

The debugging skill's stop-the-line rule halted additions at this B1 gate.
Incremental/test-first discipline prevented proceeding into unverified monster
batches. The source test initially rejected the old epoch/capacity/missing
fields; compatibility runtime was never claimed green.

### Outstanding Step 10B work (not Step 10C wiring)

1. Resolve the blocked fixture execution and run B1 runtime/save/recovery
   tests on both architectures. Review/update this B1-only source gate as
   later explicitly scoped batches land; never weaken its unrelated-byte guard.
2. B2: all 72 projected new monster species and their required mechanics,
   inventories, selectors, mental adaptations, Center/Sum transition and
   generation, Alhoon key fallback, and Cthulhu psychic/death behavior.
   The three native Deep Ones remain unchanged, not duplicated.
3. B3: all projected object declarations/behaviors and ten artifacts; verify
   exact necessities against the pin first. Necronomicon and Silver Key
   designs, upgrade-kit replacement, appearance substitutions, and Sword of
   the Deeps implementation remain pending, not silently chosen.
4. B4: actual Lethe transformations/immersion/amnesia, and every branch-support
   helper from sections 5–8 and 16. Saved bits alone are not Lethe or Center
   implementation. No complete helper is yet waiting *only* for Step 10C wiring.
5. B5: append-only integration, class/order assumptions, every new artwork
   mapping, and final generated counts after the complete declarations.
6. B6: complete focused/regression/runtime matrix and repeat both full builds,
   docs, hygiene, and backups after final content. No topology workaround.

No global sanity/insight/madness system, map, `neulev`, live Outlands generator,
dungeon definition, branch record, scheduler entry, fake dnum/depth, or DL111
reservation was added. No `doc/step10-playtest.md` was created.

### Dirty handoff and external safety backup

Tracked changes: `include/global.h`, `include/hack.h`, `include/patchlevel.h`,
`include/rm.h`, `include/you.h`, `src/mklev.c`, `test/test_depth_range.c`,
`test/test_ledger_runtime.c`, `test/test_step7_source.py`,
`test/test_step8a_source.py`, `test/test_step9a_source.py`.

Intentional untracked files: this document, `test/run_step10b_compatibility.py`,
`test/test_step10b_compatibility.c`, `test/test_step10b_source.py`, and
`test/test_step10b_generated.py`. Two generated Python bytecode files from
earlier test invocations were removed by exact path; subsequent source checks
use `-B`. No source or user data was removed.

Backup locations: `E:/Codex/My_Nethack/_qa/step10b-engine.patch`,
`E:/Codex/My_Nethack/_qa/step10b-untracked-intentional.txt`, and
`E:/Codex/My_Nethack/_qa/step10b-untracked/` with `manifest.txt` and repository-relative copies.
The final backup manifest records the actual status, Git machine-oriented
untracked list, copied paths, patch path, and timestamp. Generated binaries,
logs, bytecode, and temporary baseline headers are not source backup contents.
HEAD remains unchanged; nothing was staged, committed, tagged, or pushed.

## 22. STEP 10B RESUMED CHECKPOINT — B1 VERIFIED, FIRST SMALL B2 BATCH

Recorded 2026-09-10 after the user's restoration/exclusion confirmation.
**Step 10B remains incomplete; NOT ready for Step 10C.** This supersedes
section 21's current status, counts, timestamps, and antivirus blocker, not
its historical evidence. The current checkpoint contains tested compatibility
scaffolding and one appended species, **ogre mage**, of the projected 72.
Execution stops at this compiling/tested partial batch; no placeholder monster,
object, artifact, or fabricated topology was inserted to fill the remaining scope.

### Resolved environment gate and retained authority

The previously quarantined compatibility fixture now executes with explicit
PASS output on x64 and Win32. No further antivirus action was taken by the
agent. The starting branch, HEAD, remote, peeled tag, initial contract hash,
and submodule revisions remain as recorded in section 21. The original
`E:/Codex/My_Nethack/_qa/step10a-audit.md.backup` still has the exact initial SHA-256.
All donor inspection used the same pinned commit through `git show`/`git grep`;
the donor working tree was not edited or updated.

### Implemented declarations and native behavior

- Stable append order so far: **PM_OGRE_MAGE = 430**, following the old final
  `apprentice` entry and preceding the generated terminator. All **993 old
  monster/object/artifact enum declarations** retain their baseline values.
  No old entry was relocated to keep monster classes contiguous.
- Ogre mage declaration follows pinned `src/monst.c`: level 7, speed 10,
  AC 5, MR 0, alignment -3, weapon 2d5 and wizard spell 2d6, weight 2200,
  nutrition 500, large humanoid carnivore, strong/greedy/jewel/item collector,
  orc ancestry, infravisibility and native infravision adaptation of donor
  low-light vision. No invented unique, no-corpse, hostile, peaceful, or
  no-polymorph flag was added. Genocide eligibility remains present.
- Add `G_NOGEN` while retaining donor frequency 1. The existing global
  random/class selectors retain their pre-Step-10 range and behavior.
  Because new IDs are beyond the old `SPECIAL_PM` boundary, future branch
  selectors must explicitly choose appended IDs; widening the legacy range
  would also expose old quest/role entries and is not authorized.
- Reuse the existing native `S_OGRE` birth equipment: one `rn2(12)` roll,
  battle-axe on zero, club otherwise. This matches the ordinary pinned
  donor path; unrelated Mordor/emperor special cases were not imported.
  Native generic inventory initialization remains intact.
- Extend only two existing `src/mcastu.c` gates: allow this species in the
  established monster-victim spell dispatcher, and use the existing donor
  dice helper for hero-target spell damage. The dice are
  `(min(10, m_lev / 3 + 1) + attack.damn)d(attack.damd)`. Native generic
  wizard selection/effects, cancellation, fumble, and cooldown are reused.
  Full pinned-source `PM_OGRE_MAGE` search in `src`/`include` finds only the
  explicit generation reference in `src/mkroom.c:5957`; there is no separate
  species-specific boss, death, or spell-selector hook to import.
- Review found the hero combat dispatcher still sent ranged ogre-mage wizard
  attacks to `buzzmu()`, which rejects `AD_SPEL`. Add this species to the
  narrow `src/mhitu.c` exception already used by the elder/syllable casters.
  An extracted-production routing regression failed on the ranged mage case
  before the fix, then passed for adjacent/ranged mage casts and unchanged
  ordinary-monster routing. No ordinary caster's ranged behavior was changed.
- Intentional tile reuse: source monster tiles 882/883 copy native ogre
  male/female payloads (the native female artwork is identical to the male).
  Invisible-monster tile shifts 882 -> 884. The generator handles corpse,
  statue, and downstream object/effect offsets; no generated file was edited.

**Inventory at this checkpoint:** one new species; 71 projected new species
still absent. Existing equivalents, including all three Deep Ones, are reused
unchanged. Zero new base objects and zero new artifacts. Existing object IDs
0..525 and artifact IDs 1..36 are unchanged; none of the ten Step 10 artifacts
has been declared. This is not the final Step 10B inventory.

The append-only cutoff adaptation is explicit: named controlled polymorph and
individual/class genocide can reach the ogre mage, but ordinary random and
class-based polymorph selection does not select appended IDs. This preserves
existing global selection probabilities as required by the phase boundary;
it is not a donor-identical global polymorph pool. The legacy bones name rule
also retains names on its corpses because their ID is beyond `SPECIAL_PM`.
These narrow inherited cutoff consequences must be considered for later B2
species; do not describe the entire old ID tail as newly random-generation-safe.

### Verified corrections and dependency discoveries

These are targeted implementation-source checks, not a repeated topology audit:

- The glyph slope correction already recorded in section 13 is confirmed by
  production enum dumps: **22 glyphs per monster**, not 24. The exact present
  formula is `22 * NUMMONS + 2 * NUM_OBJECTS + 250`. The original 72-monster,
  13-object projection would therefore be **12,372**, not 12,516 glyphs.
  Neither estimate substitutes for final generated counts.
- Rilmani/Kamerel magic must not be filled with arbitrary generic wizard
  spells. Pinned `src/mcastu.c`/`src/mcastm.c` assign Plumach `SOLID_FOG`
  and self-cancellation after casting; Ferrumach uses
  `rn2(4) ? HAIL_FLURY : SOLID_FOG`; Amm/Hudor/Ara Kamerel use `OPEN_WOUNDS`,
  Sharab uses `PSI_BOLT`; Cuprilach's six outcomes are drain life, acid blast,
  solid fog, disappear, poison cloud, and make visible. These implementations
  are **pending**, not covered by the ogre's generic native spell reuse.
- Pinned Plumach birth inventory chooses mace on one of three outcomes,
  otherwise axe/sickle/scythe through its nested rolls. Semantic `SICKLE`
  and `SCYTHE` base types are absent locally and were omitted from the
  approximately 13-base-object projection. Record them as additional
  transitive dependencies for B3; do not silently replace them with the
  existing elven sickle or another unrelated base. Other exact material/base
  equivalences still require verification before a final object count exists.
  The recommended first Rilmani batch was consequently deferred in favor of
  a small ordinary species whose native dependencies already exist. The
  user's batch grouping is operational, not a relaxation of the full contract.

### Persistence and exact scope still outstanding

The section 21 allocation is unchanged: `MAXDUNGEON=18`, `MAXLINFO=3600`,
`EDITLEVEL=5` exactly once, named `u.uevent.sum_entered` bit and named/reset
`svl.level.flags.lethe` bit. Actual historical save macros, native pointer
normalizers, and production version checker pass all four Sum/Lethe bit
combinations, adjacent Step 9 field preservation, byte round-trip, epoch
0..4 rejection, current 5 acceptance, and future 6 rejection on both ABIs.

**These are saved-state scaffolding, not completed Lethe or Center mechanics.**
There are no complete new branch primitives awaiting only Step 10C wiring.
The following remain Step 10B work, not acceptable deferrals to Step 10C:

- Remaining monster declarations/mechanics, exact Neutral/R'lyeh/squad
  selectors and inclusive counts, Rilmani/Kamerel/dolls/eldritch/goat/witch/
  Mi-go groups and required transitive species.
- Center equipment/First Key, unique generation and Sum transition primitive;
  Alhoon Second/Third Key assignment and ordinary-key fallback; Cthulhu
  psychic/gaze/death/gas behavior. None is implemented.
- All required new base objects, ten artifacts, Necronomicon and Silver Key
  bounded behaviors, upgrade-kit and appearance-helper substitutions,
  potion of amnesia, and non-artifact Sword of the Deeps work.
- Actual Lethe water transformations/immersion/amnesia/exception behavior;
  spell-gradient/fumble, tree, mirror-pit, projectile-material, pet separation,
  priest/shopkeeper, loot/material, portal, and branch description helpers.
- Recurring local mental/eldritch adaptations and non-leakage tests. No
  global sanity, insight, madness, or renamed persistent mental system was added.
- Full content-set tests, final tables/artwork coverage, and the complete B6
  regression matrix after all content is implemented.

Step 10C remains solely the future map/generator/topology/scheduler and real
identity wiring phase. None of its production maps, dungeon definitions,
branch records, scheduler entries, fake depths/dnums, or DL111 reservation
exists in this diff. Ordinary water, shops, trees, and vanilla progression
code are unchanged. README and `dat/dungeon.lua` are unchanged.

### Generated data and current Release artifacts

The authoritative pipeline in section 21 was followed after the append:
verified `makedefs` target first, then `tilemap,tile2bmp,NetHack` targets,
then both full Release solution builds. IDs remain header-generated enums;
no manual `pm.h`, `onames.h`, `src/tile.c`, or bitmap patch was introduced.

| quantity | observed current value |
|---|---:|
| NUMMONS / monster declarations | 431 |
| NUM_OBJECTS / object declarations | 526 |
| AFTER_LAST_ARTIFACT / actual artifact count | 37 / 36 |
| MAX_GLYPH / generated glyphmap rows | 10,784 |
| total_tiles_used | 2,558 |
| generated bitmap | 640 x 1393, 8 bpp |

Both executable enum dumps are identical. Every glyph row index, tile index,
signed tile/ID capacity, and the complete bitmap payload/16x16 coverage pass.
The +1 monster accounts for +22 glyphs and +4 generated tiles including its
statue variants. Existing and new source-art reuse audits pass.

Full solution builds returned exit 0 with current final executables:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: **5,881,856 bytes**,
  **2026-09-10T08:32:11.2150030Z**.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: **4,766,720 bytes**,
  **2026-09-10T08:32:21.7551353Z**.

Logs: `E:/Codex/My_Nethack/_qa/step10b-tests/ogre-makedefs.log`, `ogre-engine-x64.log`,
`ogre-release-x64.log`, and `ogre-release-Win32.log`. Unchanged-file warnings
remain as recorded in section 21; none was suppressed through source edits.
SHA-256 comparisons of both console and GUI executables inside each final
x64/x86 release ZIP match their respective current Release binaries.

### Observed tests and limits of the passing evidence

PASS on **both x64 and Win32**:

- `run_step10b_compatibility.py` + `test_step10b_compatibility.c`: all old
  enum identities and actual native codec/version/persistent-state tests.
- `run_step10b_ogre.py` + `test_step10b_ogre.c`: full native declaration,
  12 equipment outcomes, 127 spell levels, adjacent/ranged hero spell routing,
  target/dead/helpless gates and
  ordinary-ogre isolation. Extracted production code, not reimplemented logic.
- `test_step10b_ogre_cast.c`: actual full monster-target dispatcher for
  psychic damage, haste, healing, cooldown, cancellation, and fumble, together
  with all pre-existing elder/mummy dispatch regressions.
- `run_depth_range.py`: widened depth/ledger/version checks.
- `run_ledger_runtime.ps1`: 225 packed pairs, 12 valid ledger recovery cases
  using each Release `recover.exe`, and invalid/old-version rejection.
- `run_step9c_foundation.py`: existing imported-species/declaration, terrain,
  reward selectors, object weight, Aesh bonus, regeneration, watery/anhydrous
  and bad-location foundation fixtures; not the entire DR/coating suite.
- `run_step9c_spells.py` against the pinned donor: 4,096 elder selections,
  14,336 spell-dice comparisons, 1,024 mass-heal cases.
- `run_step9c_elder_mm.py`: existing native elder/mummy spell effects and
  resistance/control-path regression suite.
- `run_step9c_defense.py`: 20,200 real hero/monster projectile cases, scoped
  material/DR/resistance, physical hits/kicks, vision predicates and native
  unchanged damage/RNG cases; `run_step9c_coatings.py`: 800 real projectile
  property/depletion cases with native no-property RNG preservation.

PASS source/generated/package checks:

- `test_step10b_source.py`: exact reversible B1/ogre scope; no unrelated
  source/save/topology/scheduler/README changes. Historical source checks
  project only these exact additions before their old whole-file guards.
- `test_step10b_generated.py`: both current Release executables, all 10,784
  glyph rows, 2,558 tile bounds and complete bitmap.
- `test_step9a_tiles.py`: all existing and ogre-mage intentional art reuse.
- `test_step7_source.py`, `test_step8a_source.py`: both current packaged
  `nhdat500` files match protected map/dungeon source bytes.
- `run_step10b_ogre_runtime.py` on both x64 and Win32 packaged games: actual ogre-mage
  spell hit on an iron golem under conflict in an isolated `#wizloaddes`
  fixture, observed after 16 turns, followed by successful save/restore.
  No production topology was introduced for this test.

Focused logs are `E:/Codex/My_Nethack/_qa/step10b-tests/ogre-focus-x64.log` and
`ogre-focus-Win32.log`; fixture directories and all compiler intermediates,
native saves, transcript evidence, and runtime resources are external under
`E:/Codex/My_Nethack/_qa/step10b-tests/`. Test commands use `py -3 -B` to avoid bytecode leaks.

The earlier missing-argument Step 9A source invocation is not promoted to
PASS; its full separate historical-donor suite was not run. More importantly,
the full Step 10B content coverage and final materially affected regression
matrix do not yet exist. Thus the **overall Step 10B focused and regression
gates are FAIL/incomplete**, while the individual current-batch checks above
pass. There is no remaining known antivirus blocker at this checkpoint.

Code-review follow-up approved the scoped ranged-casting fix and tests with
no further findings. Its review did not substitute for the parent-run builds
or runtime fixtures and does not approve the unfinished Step 10B as complete.

Final simultaneous live-fixture reruns both timed out on the initial
`#levelchange` prompt, before the arena or new species was created. The
retained terminal transcripts show the initial welcome/status redraw but
no command echo. The fixture now calls its existing `settle()` routine
before its first command; final live checks are run sequentially. This is
test startup synchronization, not a production source or generator fix.
Both sequential checks passed on the final rebuilt binaries, including
actual monster-target spell effects and save/restore. Evidence is retained
in `E:/Codex/My_Nethack/_qa/step10b-tests/ogre-live-settled-x64/` and
`E:/Codex/My_Nethack/_qa/step10b-tests/ogre-live-settled-Win32/`; the failed startup
transcripts remain in the separate `ogre-live-final-*` directories.

### Exact dirty-file and recovery inventory

Tracked modifications (16): `include/global.h`, `include/hack.h`,
`include/monsters.h`, `include/patchlevel.h`, `include/rm.h`, `include/you.h`,
`src/mcastu.c`, `src/mhitu.c`, `src/mklev.c`, `test/run_step9c_elder_mm_runtime.py`,
`test/test_depth_range.c`, `test/test_ledger_runtime.c`,
`test/test_step7_source.py`, `test/test_step8a_source.py`,
`test/test_step9a_source.py`, `win/share/monsters.txt`.

Intentional untracked files (9): `doc/step10.md`,
`test/run_step10b_compatibility.py`, `test/run_step10b_ogre.py`,
`test/run_step10b_ogre_runtime.py`, `test/test_step10b_compatibility.c`,
`test/test_step10b_generated.py`, `test/test_step10b_ogre.c`,
`test/test_step10b_ogre_cast.c`, `test/test_step10b_source.py`.

External backup uses the same requested paths from section 21, refreshed
for this checkpoint: tracked binary diff `E:/Codex/My_Nethack/_qa/step10b-engine.patch`,
nine-path list `E:/Codex/My_Nethack/_qa/step10b-untracked-intentional.txt`, and all nine
repository-relative copies in `E:/Codex/My_Nethack/_qa/step10b-untracked/`. Its manifest
records actual Git status, machine-oriented untracked list, copied paths,
patch path/size, and timestamp. Copied contents are SHA-256 checked and the
tracked patch is reverse-apply checked without changing the worktree.

No source was staged, committed, tagged, pushed, or merged. HEAD remains
`48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`; submodule revisions are unchanged.
All untracked repository files are intentional source/tests/documentation;
no donor dumps, native saves, logs, binaries, or IDE artifacts are included.

## Step 10B2-1 checkpoint — Rilmani, Kamerel, and argentum golem

This bounded checkpoint implements the five ordinary Rilmani, four Kamerel,
argentum golem, and dormant Neutral selector primitives audited from dNetHack
commit `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. It does not implement or
activate Step 10B2-2 monsters, Step 10B3 objects/artifacts, maps, topology,
scheduling, or a fixed DL111 reservation. The branch and HEAD remain
`phase0/dod-length` and `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`.

The append-only monster block is Plumach, Ferrumach, Cuprilach, Argenach,
Aurumach, Amm, Hudor, Sharab, Ara, and argentum golem at IDs 431 through 440;
`NUMMONS` is now 441. All pre-existing monster IDs and the object/artifact
sentinels remain unchanged. The generated totals are `NUM_OBJECTS=526`,
`AFTER_LAST_ARTIFACT=37` (36 real artifacts), `MAX_GLYPH=11004`, and 2,598
tiles, from the preceding 431 monsters, 10,784 glyphs, and 2,558 tiles.

The local monster declarations preserve the audited levels, speed, defenses,
attacks, resistances, attitude and no-generation/no-corpse constraints. The
five Rilmani and four Kamerel use the native human display class locally;
argentum remains a golem. Narrow helpers provide species natural DR,
Kamerel/argentum reflection, Aurumach/Ara magic resistance, Sharab
displacement, Cuprilach paired attacks and helpless-target backstab damage,
Ara's native object-theft attack, Hudor's inventory-wetting touch, and
argentum's native silver-arrow shot. Slash/pierce resistance and blunt
vulnerability reuse the existing golem damage path only for Ara and
argentum. Aurumach's donor no-cooldown behavior resets only its own spell
cooldown. The donor `OPROP_CONCW` behavior is represented as a saved,
narrow `OBP_CONCORDANT` property which adds the weapon's base damage against
non-neutral targets; it is not a concussive property.

The species spell selectors retain their pinned weights: Plumach always
solid fog; Ferrumach 3/4 hail flurry and 1/4 solid fog; Cuprilach uniformly
drain life, acid blast, solid fog, disappear, poison gas, or reveal;
Argenach 3/4 silver rays with the remaining quarter split between ice storm,
solid fog, disappear, and reveal; Aurumach 3/4 golden wave with the remaining
quarter split between ice storm, acid rain, solid fog, disappear, poison gas,
reveal, and prismatic spray. Amm, Hudor, and Ara use open wounds; Sharab uses
psi bolt. Solid fog is a bounded native gas cloud and Plumach cancels itself
after casting it. No general donor spell framework was imported.

Birth equipment is exact where all base objects exist: Ferrumach receives a
minimum +1 iron halberd or battle axe; Cuprilach receives +2 copper short
sword equipment with the pinned shield/second-sword split; Argenach receives
the pinned +3 silver weapon and broadsword shield distribution; Aurumach
receives large +4 gold concordant halberd and two-handed sword, plus its 1/3
matching plate chance; Hudor and Sharab receive mirrors; argentum receives
20–27 native silver arrows and one of the twelve audited silver weapons.
Three exact dependencies do not exist before the authorized object phase, so
no substitutes were created: Plumach requires `SICKLE` and `SCYTHE`; Amm
requires `MIRRORBLADE` and `ROUNDSHIELD`; Ara requires `KAMEREL_VAJRA`.
Consequently the Rilmani and Kamerel sets are explicitly incomplete under the
Step 10B2-1 birth-inventory completion rule, although their authorized core
combat behavior is present and it is safe to proceed to Step 10B2-2.

`step10b_neutral_montype` implements the five equal outer branches without a
global call site: quadruped/horse, pinned weighted Rilmani (including the
duplicate Cuprilach outcome), Kamerel, quadruped-or-deferred Shattered
Ziggurat member, and plains centaur. The unavailable Shattered Ziggurat
cultist is returned only as an explicit dormant sentinel for Step 10B2-2.
`step10b_sum_montype` preserves the exact 5/15/35/65 boundaries and duplicate
Cuprilach branch. `step10b_neutral_squad` preserves the strict cumulative
Ferrumach 80, iron golem 15, argentum 4, Cuprilach 1 thresholds while sampling
through `80 + level_difficulty()` and returning the supplied normal-selector
fallback for the depth-dependent tail. These helpers are tested directly and
are not wired into production generation.

Focused production-extraction tests cover declarations/IDs, selectors and
their boundaries, natural DR, reflection, magic resistance, spell cooldown,
Cuprilach backstab/offhand behavior, spell selection/effects, wet damage,
silver-arrow routing, equipment/defer guards, and concordant damage. Existing
Step 10B codec/ID/version checks, Step 10B ogre spell tests, Step 9C defense,
weapon, coatings and elder/mummy tests, protected Step 7/8/10 source checks,
and tile/glyph generation checks pass. Final Release builds produced
`binary/Release/x64/NetHack.exe` at 2026-09-10 12:57:26 and
`binary/Release/Win32/NetHack.exe` at 2026-09-10 12:58:21. README and all
production topology/map/scheduler sources remain outside this checkpoint.

## Step 10B2-2 checkpoint — dolls and remaining eldritch morph population

This bounded checkpoint implements the remaining 24 direct members of the
audited 26-member `lethe-b` population slice from pinned dNetHack commit
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. The existing iron piercer,
deep one, deeper one, and electric eel are reused; no duplicate declarations
were added. Step 10B overall remains **INCOMPLETE**: the later monster groups,
object/artifact phase, complete content matrix, and all Step 10C production
maps/topology/scheduling remain future work.

### Append-only identity and population

The exact new append order and generated IDs are:

| ID | species | ID | species |
|---:|---|---:|---|
| 441 | living doll | 453 | pitch black cube |
| 442 | living lectern | 454 | prayerful thing |
| 443 | parasitized doll | 455 | hemorrhagic thing |
| 444 | bestial dervish | 456 | many-eyed seeker |
| 445 | ethereal dervish | 457 | voice in the dark |
| 446 | flashing lake | 458 | tiny being of light |
| 447 | frosted lake | 459 | man-faced millipede |
| 448 | smoldering lake | 460 | mirrored moonflower |
| 449 | sparkling lake | 461 | crimson writher |
| 450 | blood shower | 462 | radiant pyramid |
| 451 | many-taloned thing | 463 | Kuker |
| 452 | deep blue cube | 464 | lurking one |

All 993 serialized declarations from the frozen checkpoint retain their
values. `NUMMONS` moves 441→465 while `NUM_OBJECTS` remains 526 and artifact
sentinel/count remains 37/36. `MAX_GLYPH` is 11,532. The source tile table
adds 48 gender rows (904–951), each intentionally reusing native iron-golem
art; invisible-monster moves to row 952. The generated total is 2,694 tiles,
and both Release enum dumps, every glyph row/tile bound, and the complete
640x1474 8-bpp bitmap agree.

### Species behavior and pinned corrections

The three dolls retain pinned level, speed, conventional AC, attacks,
attitude, golem/body flags, resistances, generation restrictions, natural DR,
and exact fixed/rolled golem HP: living lectern 50, living doll `45+5d8`, and
parasitized doll `45+20d8`. Living lectern uses the existing narrow innate
magic-resistance path. The donor AC fields were confirmed to be conventional
AC values, not values requiring inversion; the declarations were corrected
accordingly.

Every morph has its own declaration rather than a cosmetic shared block.
This includes distinct lake elements, cube touch strengths, prayerful
elemental touch, hemorrhagic rend, moonflower reflection, voice drain/sickness
resistance, radiant-pyramid DR, and the many-taloned repeated `AT_DEVA` chain.
The latter repeats until a miss or terminal combat result and loses four
points of accuracy after each hit in both monster-vs-hero and
monster-vs-monster dispatch. `AT_REND` is guaranteed only after the preceding
two hits. Tiny being of light locally implements the pinned avoid-the-line and
flee-when-adjacent-if-faster behavior without adding a saved global monster
flag.

The pinned passive trace corrected an initially tempting native mapping:
ordinary cold/fire/electric lake retaliation happens at the end of an attack,
requires a successful hit and a living uncancelled lake, then passes its 2/3
roll. Full matching resistance nullifies damage. Magic-missile retaliation
remains the donor exception: it occurs on hit or miss and even if the defender
dies, with magic resistance nullifying it. Zero-dice imported passive damage
uses `(level/3+1)d(die)` while every old native passive keeps `level+1` dice.
Both hero and monster attacker production paths use the same narrow trigger
predicate; monster death attribution remains in the native passive dispatcher.
The four lakes also have a bounded monster-target elemental cast path with the
same level-scaled dice and full resistance semantics as their hero-target path.

Kuker is peaceful, carries two 4d8 weapon attacks and a 0d6 clerical cast,
retains its pinned broad elemental/sleep/poison plus narrow magic resistance,
and shares Aurumach's species-local zero cooldown. Its exact uniform six-way
selector is confuse, reveal, evil eye, curse items, protection, or punishment.
The two otherwise donor-global effects are local adaptations: evil eye applies
the target status/luck effect, and protection clears/heals the caster; no
general donor spell framework was imported.

Lurking one retains level 45, flight/wait behavior, four 1d8 weapon slots,
4d8 tentacle, 4d8 electric gaze, rock-throwing giant behavior, and
poison/stone/electric plus narrow magic resistance. Each extra arm rescans the
current inventory and selects a distinct eligible non-mainhand weapon; there
is no saved secondary pointer. Shields, cursed/artifact/two-handed/unsafe
objects are rejected, and partial, empty, and removal-between-attacks states
terminate without a repeated or infinite selection loop. The same selector is
called from monster-vs-hero and monster-vs-monster combat; ordinary repeated
weapon attacks and the existing two-weapon species retain their old behavior.

### Mental adaptation and object dependencies

The audited mental matrix is intentionally narrow. Sanity-loss presence is
assigned to bestial dervish, blood shower, many-taloned thing, hemorrhagic
thing, many-eyed seeker, man-faced millipede, crimson writher, and lurking one.
Insight-only presence is assigned to ethereal dervish, all four lakes, both
cubes, prayerful thing, mirrored moonflower, radiant pyramid, and Kuker. Dolls,
living lectern, parasitized doll, voice in the dark, and tiny being of light
need no presence adaptation. On the first visible encounter within radius 8,
the former group applies a small native confusion/Wisdom exercise and the
latter a one-turn native stun. Hallucination changes only the message. The
saved `mspare1` bit `0x01` records that one encounter; an ownership audit found
it free below the existing syllable bits 7–9 and shop bits 10–16. This prevents
stationary monsters from retriggering every player step. No `u.usanity`,
`u.uinsight`, `u.umadness`, `sanity.c`, global madness state, visibility gate,
generation gate, or arbitrary numeric cooldown was imported.

No new object is declared in this slice. The exact unresolved doll object
dependencies are deferred to Step 10B3: living doll needs the donor
`LIFELESS_DOLL`/doll-type object mechanics, and parasitized doll death debris
needs `EYEBALL` plus `LIFELESS_DOLL`. Living lectern, Kuker, and lurking one
have no species-specific pinned birth item to add; lurking one continues to
use native giant equipment/rock handling. The earlier Plumach, Amm, and Ara
deferrals recorded above remain unchanged.

### Verification and final artifacts

Focused production-extraction gates passed on both x64 and Win32 under
`/std:c11 /W4 /WX`. They cover all 24 declarations and IDs, doll HP/DR,
distinct morph attacks/body/resistance/attitude flags, passive dice/trigger
truth tables, all six Kuker selections/no cooldown, once-only mental marking,
and lurking-one fully equipped, partial, empty, shielded, cursed, artifact,
two-handed, unsafe, and removal-between-arm inventory cases. Static integration
checks confirm both real passive call sites, resistance/death routing, both
multiweapon call sites, repeated Deva/Rend routing, mental pacing, exact append
order, and unchanged objects/artifacts/topology/scheduler/README.

Both architectures also passed: the 993-ID compatibility/codec/version gate;
Step 10B ogre and Step 10B2-1 regression gates; Step 9C defense (20,200
projectile cases), coatings (800 cases), elder/mummy/lake monster-target
spells, and weapon-selection/two-weapon isolation; protected Step 7/8 source
and both packaged-data checks; reversible Step 10B source projection; tile
reuse; and generated enum/glyph/bitmap checks. Final full solution builds
returned exit 0:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,926,912 bytes,
  `2026-09-10T12:25:33.7237967Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,802,048 bytes,
  `2026-09-10T12:26:57.1970374Z`.

At this checkpoint there are 38 tracked modifications and 15 intentional
untracked source/test/documentation files. The Step 10B2-2-specific additions
are `test/run_step10b2_2.py`, `test/test_step10b2_2.c`, and
`test/test_step10b2_2_source.py`; all other dirty files preserve and extend the
documented Step 10A/B history. No build products or runtime fixtures are
untracked. README, `dat/dungeon.lua`, production dungeon/topology/scheduler
sources, and fixed DL111 reservation remain unchanged. Nothing is staged,
committed, tagged, pushed, reset, stashed, or cleaned. HEAD/upstream remain
`48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`.

## Step 10B2-3 checkpoint: Goat, Witch, Mi-go, remaining species and selectors

Step 10B2-3 was implemented from pinned dNetHack revision
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. The append-only declaration
order is now: 465 small goat spawn, 466 goat spawn, 467 giant goat spawn,
468 blessed, 469 mouth of the goat, 470 apprentice witch, 471 witch,
472 coven leader, 473 The Good Neighbor, 474 Hmnyw-Pharaoh, 475 migo worker,
476 migo soldier, 477 migo philosopher, 478 migo queen, 479 byakhee,
480 dark young, 481 deep dweller, 482 deminymph, 483 gnoll ghoul, 484 gug,
485 Illurien of the Myriad Glimpses, 486 nightgaunt, 487 oread,
488 minotaur priestess, 489 priest of an unknown god, 490 shoggoth,
491 star spawn, 492 Shattered Ziggurat cultist, 493 Shattered Ziggurat
knight, 494 Shattered Ziggurat wizard, 495 hunting horror, and
496 blasphemous lurker. Every prior ID through lurking one 464 is unchanged.

The Goat family retains its increasing paired weapon, butt, and kick dice,
with the blessed's wide gaze/composite attack, clerical cast, full elemental
defenses, flight, and displacement, and the mouth's poison tentacles,
digestion, enhanced-acid composite strike, regeneration, and stationary
body. Composite Goat attacks are resolved in `getmattk` only when attempted,
preserving their runtime RNG boundary. Available native gear is supplied to
the three spawn sizes. The bounded mental encounter classification is extended
to the Goat family; no sanity, insight, madness, visibility, or generation
framework is imported.

The three Witch ranks retain their escalating caster level, magic resistance,
flight, female identity, and no-cooldown spell behavior. The Good Neighbor has
four independently dispatched two-square reach/shred attacks and a spell;
reach is recognized by both hero and monster combat, and remote hits cannot
trigger contact passives. Hmnyw-Pharaoh retains paired 8d8 weapon attacks,
spellcasting, broad defenses, covetous wait behavior, and its available cursed
+9 quarterstaff. Available Witch/Good-Neighbor armor is created by the native
inventory path.

The four Mi-go castes retain their distinct generation/group flags, movement,
resistance, physical/brain/paralysis/spell attacks, and equipment. Their mist
gaze keeps the pinned 1-in-5 activation and creates the caste-specific native
fog, ice, or steam manifestation next to the hero; monster-target use is a
bounded confusion adaptation. Byakhee, dark young, deep dweller, deminymph,
gnoll ghoul, gug, nightgaunt, oread, and minotaur priestess retain their
audited body, attacks, flags, defenses, grouping, and native-compatible status
or clerical paths. Gug receives its native club. Imported paired weapon slots
rescan inventory rather than persisting object pointers.

Illurien is unique and non-generating, with engulfing memory drain, sticky
claw, spellcasting, teleport/flight, regeneration, and broad defenses. The
native adaptation derives memory-loss probability from damage as a bounded
1–10 percent value and applies narrow native amnesia without donor-global
memory state. The unknown god's passive remains an inert marker: the pinned
source contains no implementation for that passive, so inventing damage would
be a donor divergence. Shoggoth uses the native acid passive path, including
the pre-alive passive phase, resistance/corrosion handling, attacker death
flags, and no recursive passive call. Star spawn's `AD_PSON` is routed through
the actual native psionic spell selector/dispatch. Hunting horror receives a
fixed two-segment native worm body without adding the excluded donor tail
species. The blasphemous lurker retains four five-square Rider/conflict attacks
and a 4-in-5 bounded blasting gaze which damages, confuses, and stuns without
importing pantheon/endgame state.

All three Shattered Ziggurat representatives are present with their pinned
weapon/caster progression and group flags. The dormant Neutral selector now
returns the actual cultist in the pinned one-in-four detail outcome; its
Rilmani, Kamerel, quadruped, plains-centaur, and Sum branches are unchanged.
The complete dormant R'lyeh primitive rolls `d(1,100)` before its `rn2(20)`
gate, preserves every threshold, exact `d`/`rnd`/`rn1` expression and inclusive
loop, and checks `G_GENOD` but not extinction before its class fallback. It has
no production caller: R'lyeh identity/topology activation remains Step 10C.

No object ID was added. Exact absent transitive dependencies remain deferred
to Step 10B3: Witch familiar species creation; `BLACK_DRESS` and `WITCH_HAT`;
Hmnyw-Pharaoh's `SICKLE`; Illurien's `SPE_SECRETS`; and Shattered Ziggurat
`TORCH`, `SHADOWLANDER_S_TORCH`, and faceless-robe variants. Available native
equipment was implemented rather than deferred. No birth/death path stores a
new pointer or expands save state.

Focused production-extraction tests pass on x64 and Win32. They verify IDs,
declarations, Goat/Witch/Mi-go attacks, Good Neighbor reach/shred, natural DR,
innate magic, no-cooldown and Star Spawn spell selection, bounded Illurien
memory percentage, mental classification, and the resolved Neutral outcome.
Static integration verifies exact R'lyeh call order/RNG primitives, eight
inclusive loops, genocide-only fallbacks, dormant activation, reach routing,
passive lifecycle ordering, Mi-go/blasting gazes, Hunting Horror initialization,
and absence of global sanity state. An isolated live conflict fixture on both
architectures observed an actual Star Spawn psychic spell affect an iron golem
and then completed save/restore.

The compatibility/save gate preserved all 993 baseline declaration IDs and
reported `NUMMONS=497`, `NUM_OBJECTS=526`, artifact sentinel/count 37/36, and
`MAX_GLYPH=12236`. Both generated-data gates validated every glyph index,
tile bound, and the complete 640x1551 8-bpp bitmap. The final source tile is
1016 (invisible monster); generated total tile count is 2,822. Prior ogre,
Step 10B2-1, Step 10B2-2, defense (20,200 projectile cases), coatings (800
cases), elder/mummy/lake spell, attack, weapon/offhand, protected Step 7/8,
and reversible Step 10B source gates pass.

Both Release solutions completed with exit 0:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,959,168 bytes,
  `2026-09-10T13:54:23.8475117Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,826,624 bytes,
  `2026-09-10T13:55:28.4205274Z`.

Step 10B remains incomplete. Step 10B2-4 remains exactly Alhoon, Center of
All, Father Dagon, Mother Hydra, and Great Cthulhu. Step 10B3 objects and
artifacts remain pending. Step 10C maps, production topology, scheduler,
R'lyeh activation, and any fixed DL111 reservation remain untouched. README
is unchanged.

## Step 10B2-4 checkpoint: Alhoon, Center, Dagon, Hydra, and Cthulhu

Step 10B2-4 appends exactly five authorized declarations: 497 alhoon,
498 Center of All, 499 Father Dagon, 500 Mother Hydra, and 501 Great Cthulhu.
All earlier monster IDs remain unchanged. The mandatory preflight found that
the donor's witch's familiar is a distinct level-5 rodent species with real
production references, not an alias or pseudo-identity. It was outside this
phase's five-monster authorization and remains deferred; therefore the Step 10
monster inventory is not complete and readiness for Step 10B3 is false.

Alhoon retains cold touch, brain drain, two life drains, spellcasting,
no-cooldown casting, natural DR 8, pierce-only resistance, regeneration,
telepathy, and the bounded sanity-loss encounter classification. Its `AD_DRIN`
attack uses the unified attack-type brain dispatcher, so `is_mind_flayer` was
not widened and ordinary mind flayers and unrelated liches keep their native
behavior. An artifact-independent primitive preserves the exact key decision:
try Second when absent, otherwise Third when absent, otherwise an ordinary
skeleton key. The Second and Third Keys themselves remain Step 10B3 deferrals.

Center of All retains three weapon attacks, no-cooldown Aurumach spell
selection, an internal cursed loadstone shot, natural DR 4, permanent
invisibility, innate magic resistance/reflection, and the exact pre-Sum
peaceful/post-Sum hostile rule. Its 1-in-5000 selector is dormant, accepts only
explicit Neutral or Lost Cities context, and rejects a gone unique; it has no
`rndmonst`, migration, map, topology, or scheduler caller. Exact available
large concordant leather armor, robe, and low boots are created. The absent
First Key, bardiche, war hat, and gauntlets are explicitly deferred without
substitution.

Father Dagon and Mother Hydra follow the pinned `G_NOGEN|G_NOHELL` declarations;
the audit's earlier unique assumption was incorrect because the pinned records
do not carry `G_UNIQ`. Father has paired weapon attacks and a kick. Mother has
four bites and paired weapon attacks. Her donor soul marker would be a seventh
attack, so the native six-slot adaptation recognizes her species in the same
Step 9C corpse-processing soul path instead of widening `NATTK`. Either death
grants the pinned eight-point pulse only to living deep-one family recipients.
Their offhand slots rescan current inventory (Father slot 1, Mother slot 5),
with no persisted object pointer.

Great Cthulhu retains its 100d4 claw, uncancellable wide Wisdom gaze, noxious
death marker, natural DR 21, broad defenses, telepathy, and bounded
sanity-loss encounter classification. Its separate 1-in-20 level-wide psychic
action deals 5d15 to selected hero or hostile non-mindless monster targets;
native half-spell and Vaul reductions apply to hero damage, followed by long
stun and capped native confusion in place of donor madness. The permanent gaze
uses conventional Wisdom/maximum-Wisdom loss down to the racial minimum,
native memory loss, a four-turn monster-local cooldown, and overflow damage at
minimum Wisdom; monster targets receive the pinned confusion conversion.
Actual death, after life-saving but before detachment, sets a species-scoped
exactly-once bit and passes captured coordinates to a pointer-free helper. That
helper produces one 8d8 physical noxious explosion and a duration-30 gas cloud.
The donor radius-two footprint maps to five cells in native
`create_gas_cloud` size units. No revival or recursive death path was added.

The five-monster mental matrix is: Alhoon 2, Center 0, Father Dagon 2, Mother
Hydra 2, Great Cthulhu 2, where 2 is the existing bounded sanity-loss presence
classification and 0 means no generic presence effect. No sanity, insight,
madness, hidden progression, or global psychic framework was imported.

Focused declaration/helper and dispatcher-sensitive production-extraction
tests pass under x64 and Win32. Runtime extraction exercises the real deep-one
soul helper, Cthulhu explosion/gas helper, and loadstone dispatcher. Static
integration covers brain non-leakage, Center dormancy/attitude/inventory,
multiweapon rescans, Wisdom-gaze ordering, psychic targeting, exactly-once
death ordering, and no revival. Compatibility reports `NUMMONS=502`,
`NUM_OBJECTS=526`, artifact sentinel/count 37/36, and `MAX_GLYPH=12346`, with
all 993 baseline declaration IDs unchanged. The authoritative generated total
is 2,842 tiles; final source tile 1026 is the invisible monster, and the
640x1563 8-bpp bitmap covers all 12,346 glyphs.

Both Release configurations build successfully. Materially affected ogre,
Step 10B2-1/2/3, soul, defense, projectile, coating, attack, spell,
multiweapon, compatibility/save, protected source-scope, and generated-data
regressions pass. Step 10B3 still owns the Second, Third, and First Keys plus
the bardiche, war hat, and gauntlets (along with the earlier documented object
dependencies). Step 10B4 remaining branch support, Step 10B5 closeout, and
Step 10C maps/topology/activation all remain pending. README,
`dat/dungeon.lua`, production branch definitions/scheduler, and fixed DL111
reservation remain untouched. Step 10B remains incomplete.

### Immediate completion correction: Witch's familiar

The Step 10B2-4 checkpoint above is preserved as historical evidence, but its
five-monster authorization has now received the requested immediate completion
addition. `witch's familiar` is a distinct append-only declaration after Great
Cthulhu at ID 502. No earlier monster ID moved. This corrects the cumulative
Step 10B append block from 72 to 73 monsters and completes the Step 10 monster
inventory without beginning Step 10B3 objects or artifacts.

The pinned donor definition was audited at
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. The local declaration preserves
the level-5, speed-6, AC-0, MR-0, neutral, tiny brown rodent; 1d3 vampiric bite;
0d6 spell attack; squeak; animal/omnivore/female identity; no-polymorph and
infra-visible behavior; no intrinsic resistances; and `G_NOGEN`. Its male and
female source tiles are 1026 and 1027, both deliberately reusing the existing
giant-rat male art. The invisible-monster tile consequently moves to 1028.
There is no ordinary rodent, pet, random-generation, selector, or map leakage.

All apprentice witches, witches, and coven leaders create one adjacent familiar
through the normal production birth path with the donor's adjacent/no-birth-
count flags. The familiar receives its owner's level, maximum/current HP, and
peaceful state. Its owner relationship uses the existing saved `mspare1` field;
normal saves retain the ID directly, and bones restoration remaps or safely
clears it through the existing monster-ID mapping. No pointer or save-structure
field was added.

The production cast gates now require each Witch rank to have its linked living
familiar. The familiar itself always selects open wounds and receives zero spell
cooldown. A witch with a missing familiar attempts replacement with the pinned
1-in-20 chance; a coven leader uses 1-in-4. Successful replacement heals the
owner by its level up to maximum HP, synchronizes the new familiar, and ends
fleeing once the owner is above half health. A familiar death finds its linked
owner: apprentice witches and witches flee, ordinary witches also receive the
pinned ten-turn spell cooldown, while coven leaders receive the pinned four-turn
cooldown and the closest bounded native rage state (`mavenge`) because this
codebase has no donor `encouraged` combat field. The donor Good Neighbor repair
reference was also audited; it belongs to a broader off-map/unseen migration
framework absent here, so that framework was not imported for this narrow
familiar completion.

Focused declaration, relationship, creation, cast-gating, resummon, death,
bones-remap, source-scope, and tile tests pass on x64 and Win32. The cumulative
Step 10B compatibility, ogre live conflict/save, Step 10B2-1/2/3, R'lyeh,
protected Step 7/8 source, and generated-data regressions also pass. The
compatibility gate preserves all 993 baseline declaration IDs and reports
`NUMMONS=503`, `NUM_OBJECTS=526`, artifact sentinel/count 37/36, and
`MAX_GLYPH=12368`. Generation produced 2,846 tiles and a complete 640x1566
8-bpp bitmap covering all 12,368 glyphs.

Both final Release configurations completed with exit 0:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,971,456 bytes,
  `2026-09-11T03:46:47.9713689Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,835,328 bytes,
  `2026-09-11T03:53:48.5959486Z`.

The current checkout has 41 tracked modifications and 24 intentional untracked
source/test/documentation files. README, `dat/dungeon.lua`, production branch
definitions/scheduler, and the fixed DL111 reservation remain unchanged.
Nothing is staged, committed, tagged, pushed, reset, stashed, or cleaned;
HEAD/upstream remain `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`.

STEP 10 MONSTER INVENTORY COMPLETE: YES
READY FOR STEP 10B3: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10C: NO

## Step 10B4 environment and branch-support checkpoint

Step 10B4 is complete.  This checkpoint implements every remaining B4-owned
engine/environment primitive while keeping real dungeon and special-level
identity in Step 10C.  The implementation uses an explicit nonserialized
`step10b_level_context` argument.  Every production hook currently passes
`STEP10B_CTX_NONE`; Step 10C must replace only those arguments with its real
identity resolver.  No dungeon number, special-level record, fixed ledger,
map, generator, scheduler, or persistent context field was introduced.

### B4 ownership accounting

Implemented in B4:

- Lethe immersion memory loss at strength 10, sparkling-water presentation,
  and a deletion-safe inventory/container walk which preserves native wetting
  protections and erosion ordering, strips BUC, protects artifacts/mail/Book
  of the Dead, rewrites scrolls, blanks books, transforms/destroys potions,
  drains positive enchantment/charges, runs magic markers, and refreshes
  transformed weights.  `drown()` is the single immersion seam reached by
  ordinary movement and `teleds()` through `spoteffects(TRUE)`.
- Exact context-parameterized hero spell gradients (Gate through Outlands 4,
  no Spire hero penalty, Sum bonus) before the final percentile clamp, and
  exact monster fumble-threshold modifiers including Spire forced fumble.
- Outlands tree kick suppression and donor-style 2d4-1 wooden club/quarterstaff
  cutting results without fruit; ordinary tree RNG remains on its old path.
- Mirror-shard spiked-pit descriptions and 1d12 plus conditional 1d20 silver
  damage for hero, monsters, and steed, with no poison in that context and the
  existing `Hate_silver`/`mon_hates_silver` predicates.
- The pinned nested arrow/dart material selector: METAL 2/3, IRON 1/6,
  COPPER 1/12, SILVER 1/16, GOLD 1/48.  Rolls remain conditional, material is
  stored in `obranch_material`, and weight is recomputed.
- Outlands, Lost Cities, and R'lyeh wall/lit-floor/dark-floor colors applied in
  the late glyph-info path while preserving `NO_COLOR` and unrelated cmaps.
- Gate Town pet separation handling which suppresses tameness loss and extends
  dietary-pet hunger time through the native catch-up path.
- Idempotent Plumach-shopkeeper and blasphemous-lurker-priest designators which
  require valid native ESHK/EPRI ownership before morphing.  The former keeps
  name, billing, room, and level ownership; the latter keeps shrine ownership,
  becomes awake/hostile, and suppresses ordinary temple intoning.

Proven reuse as-is in B4:

- Lua `des.trap({seen=true})` already reaches `MKTRAP_SEEN` and `tseen`; normal
  portal records and the Step 9C paid portal guide remain authoritative.
- PUDDLE, SAND, SOIL, ordinary shops, temples, barracks, courts, narrow rooms,
  standard loot placement, and the existing object material/size/property
  metadata need no new identifiers or parallel systems.
- Native complete-object save/bones codecs own branch material/size/properties;
  native mextra codecs own ESHK/EPRI; native shop naming, billing, cleanup, and
  temple movement remain authoritative.

Step 10C wiring only:

- Resolve Gate, Outlands 1..4, Spire, Sum, Lost Cities, and R'lyeh identities
  and supply them to the dormant B4 context arguments.
- Set the already-live Lethe level flag from authorized maps/generation, call
  the two role designators after native shop/temple creation, request seen
  portals in the maps which need them, and create the actual rooms/terrain.
- Register all maps, topology, runtime generators, scheduler activation,
  genuine Silver Key destinations, and any authorized Approach placement.

There are no B4-owned unresolved dependencies.  B5 remains a separately
authorized final Step 10B closeout; it owns no deferred B4 implementation.

### Donor/current-engine comparison and intentional variances

The pinned donor commit remains
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`.  Its `In_outlands()` means the
whole Neutral dungeon, so the local explicit Outlands context spans Gate Town
through Sum but excludes Lost Cities/R'lyeh.  NetHack 5.0 creates trap missiles
when they fire rather than storing donor trap ammunition, so material is set in
`t_missile()`.  NetHack 5.0 renders through `display.c`, so branch palette
selection is applied in `map_glyphinfo()` rather than donor `mapglyph.c`.

The current contract explicitly says that Spire has no hero penalty, so the
donor's contradictory `chance=0` line was not imported; monster casting still
always fumbles there.  Donor `SCR_RESISTANCE` has no local object declaration,
and B4 is forbidden to add an object ID, so its exception is represented by an
explicit non-object sentinel and is presently non-applicable.  Artifact scrolls,
mail, artifact spellbooks, and the Book of the Dead retain their applicable
protections.  Donor global quest identities, maps, random-monster changes,
shop probabilities/stock replacement, global tree rules, pet exceptions for
other branches, and persistent mental/progression systems remain excluded.

### Files and verification

B4 production changes are confined to `include/global.h`, `include/extern.h`,
`src/questpgr.c`, `src/spell.c`, `src/mcastu.c`, `src/dokick.c`, `src/dig.c`,
`src/trap.c`, `src/display.c`, `src/pager.c`, `src/dog.c`, `src/shknam.c`, and
`src/priest.c`.  Tests are `test/test_step10b4.c`,
`test/test_step10b4_state.c`, `test/test_step10b4_source.py`,
`test/run_step10b4.py`, and `test/run_step10b4_final.py`; the historical B1
projection was extended only to project the new nonpersistent declaration.

The x64 deterministic gate covers every spell threshold, context boundary,
tree policy, mirror damage, all five nested material outcomes, branch palette,
Lethe type rewrite, charge drain, and ordinary non-leakage.  The state gate
uses real monster/mextra structures to cover ordinary versus Gate catch-up,
ESHK shop/name ownership, EPRI shrine ownership, idempotence, hostility, and
invalid-role rejection.  The source gate covers real call sites, deletion-safe
list traversal, nested containers, immersion/teleport convergence, native
portal-seen support, persistence ownership, and the Step 10C boundary.

All cumulative Step 10B source and focused gates, B2-4 runtime helpers, ogre
packaged runtime, B3-2 artifact/key packaged runtime, B3-3 artifact packaged
runtime, generated identity/tile/glyph validation, and the affected Step 9C
shop/portal/save runtime pass on x64.  Generated values remain NUMMONS 503,
NUM_OBJECTS 547, artifact sentinel 47/count 46, MAX_GLYPH 12410, and 2867
tiles.  EDITLEVEL remains 5.  README and `dat/dungeon.lua` remain unchanged.
Win32 was not run because it is not a B4 completion gate.

STEP 10B3 COMPLETE: YES
STEP 10B4 COMPLETE: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10B5: YES
READY FOR STEP 10C: NO

## Step 10B3-4 integration and closeout

Step 10B3-4 is the integration/closeout slice for the already-delivered
B3-1, B3-2, and B3-3 content. The pre-existing Step 10 working tree was
preserved. No production C, header, object-data, monster-data, map, Lua,
dungeon, topology, scheduler, README, or save-schema change was required in
this slice; the B3-4 production-defect count is zero.

The final B3 object range remains `526..546` (21 objects), with the ordinary
base classes and B3-1 equipment paths intact. B3-2 and B3-3 retain their
artifact records through `AFTER_LAST_ARTIFACT=47` and
`NROFARTIFACTS=46`. The integrated source gates cover the B3-dependent
equipment references, doll drops, artifact material and carried-property
paths, native object/artifact save and bones lifecycle, Necronomicon menu and
passage dispatch, Silver Key passive/no-destination behavior, and the
explicit deferral of positive portal travel to Step 10C. No B3-owned
unresolved dependency remains.

The new B3-4 source gate and master runner are
`test/test_step10b3_4_source.py` and `test/run_step10b3_4.py`. The master
runner re-executed the B2/B3 source and focused x64 gates, B2-4 runtime
helpers, the Ogre packaged runtime, B3-2 and B3-3 packaged runtime suites,
and generated-resource validation. The final post-build packaged checks
also passed directly against the rebuilt x64 Release directory, including
fresh B3-2 and B3-3 runtime fixtures.

The final x64 Release rebuild completed successfully on
`2026-09-11T11:45:28.3638451Z`:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,989,888 bytes.
- `E:/Codex/My_Nethack/binary/Release/x64/nhdat500`: 1,609,361 bytes,
  regenerated at `2026-09-11T11:44:51.5928865Z`.

Generated validation remains `NUMMONS=503`, `NUM_OBJECTS=547`,
`AFTER_LAST_ARTIFACT=47`, `MAX_GLYPH=12410`, all 12,410 glyph indices in
range, 2,867 tiles, and a complete 640x1574 8-bpp bitmap. The affected Step
9C x64 regressions also pass: foundation, armor-size, coatings, defense,
weapon damage, weapon selection, handedness, and spell compatibility.

Two test-only expectation corrections were needed and are intentionally
retained: the Step 9A tile parser now accepts the existing inter-header tile
metadata form, and the Step 9A historical projection excludes the already
documented B3-3 artifact additions before comparing the pinned baseline.
Neither correction changes production behavior. The x64-only requirement was
met; the historical Win32 result above was not rerun for B3-4 and no optional
Win32 diagnostic was performed.

Ownership remains unchanged: B4 owns remaining environment/branch support,
B5 owns the final Step 10B closeout, and Step 10C owns the Sword of Deeps
map instance, positive Silver Key topology wiring, production map/Lua and
dungeon registration, and scheduler/activation. No Step 10 map, production
topology, scheduler, fixed DL111, or new identity was added here. The
working tree remains intentionally dirty and uncommitted; the external
integration patch, intentional-file list, copied-file backup, and per-file
hash verification are the checkpoint safety record for this slice.

STEP 10B3-1 COMPLETE: YES
STEP 10B3-2 COMPLETE: YES
STEP 10B3-3 COMPLETE: YES
STEP 10B3-4 COMPLETE: YES
STEP 10B3 COMPLETE: YES
ALL 10 STEP 10 ARTIFACTS DECLARED: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10B4: YES
READY FOR STEP 10C: NO

## Step 10B3-1 checkpoint: base objects and deferred monster equipment

The pinned B3-1 inventory audit produced 21 genuinely missing semantic base
objects. They are append-only IDs 526 through 546: 526 sickle, 527 scythe,
528 mirrorblade, 529 kamerel vajra, 530 viperwhip, 531 rakuyo, 532 khakkhara,
533 roundshield, 534 witch hat, 535 white faceless robe, 536 black faceless
robe, 537 smoky violet faceless robe, 538 universal key, 539 torch,
540 shadowlander's torch, 541 double lightsaber, 542 eyeball, 543 potion of
amnesia, 544 space mead, 545 spellbook of secrets, and 546 lifeless doll.
Every extension probability is zero, so none enters a vanilla random object
pool.

The duplicate audit reuses the existing `BARDICHE`, `WAR_HAT`, `BLACK_DRESS`,
and `ARCHAIC_GAUNTLETS` bases. Center of All applies large/gold/leather and
concordant instance metadata where the pinned equipment requires it. The pin
also corrects the historical “deep long sword” candidate: there is no donor
base ID. It is an ordinary `LONG_SWORD` with a deep property and special name,
so B3-1 adds only `OBP_DEEP`; creation of the cursed +12 named map instance
remains Step 10C. Local dwarvish roundshield and elven sickle semantics do not
match the pinned roundshield and sickle, so those are genuine new bases rather
than reuse candidates.

The old 0..525 table remains byte-order stable and the legacy class-contiguous
boundary remains `bases[MAXOCLASSES] == 526`. Initialization stops the legacy
range scan at that frozen boundary, validates the appended extension's
ascending class groups and zero probabilities, and exposes explicit all-object
class iterators for wishing and discovery. `mkobj`, `rnd_class`, shops,
polymorph, and the other vanilla probability consumers continue using the
unchanged legacy `bases[]` ranges. The compatibility runner compares all 993
frozen monster/object/artifact declarations against the checkpoint and proves
that every old ID is unchanged.

Standalone behavior is bounded to native or established local facilities.
Sickles and scythes use native axe/polearm skills, with the pinned extra scythe
damage die. Viperwhips reuse native whip application and retain their pinned
explicit-creation head count; rakuyo retains its non-dual-wield damage and
enchantment bonus; mirrorblade uses the better of its own and the defender's
wielded weapon damage. Broad lightsaber/form/technology systems remain
excluded, so the kamerel vajra and double lightsaber retain their audited base
weapon records without importing those frameworks. Faceless robes use the
existing imported armor DR mechanism (1/2/3), and the black and smoky variants
retain cold resistance. The universal key follows ordinary skeleton-key UI,
locking, pet, and monster-door behavior only. Both torches have persistent
fuel, can be lit/extinguished, shrink through radii 4/3/2, and are consumed at
zero fuel; no generalized darkness framework was added. Potion of amnesia uses
the existing bounded native memory-loss helper at blessed/uncursed/cursed
percentages 0/10/25. Space mead uses native healing, hunger, confusion, slow
digestion, and magical-breathing timeouts. The spellbook of secrets has valid
level-7 clerical metadata and a safe ordinary read result, without adding a
spell slot or Necronomicon behavior. Lifeless dolls and eyeballs retain their
origin species through the saved `corpsenm` field.

Explicit creations also preserve the pinned non-medium base sizes through the
existing saved per-instance size field: sickle, mirrorblade, double lightsaber,
and both torches are small; scythe, khakkhara, and lifeless doll are huge;
roundshield and spellbook of secrets are large; universal key and eyeball are
tiny. Kamerel vajra, viperwhip, rakuyo, witch hat, and faceless robes retain the
native medium default. Monster-specific overrides continue to take precedence.

The formerly base-blocked B2 paths are complete. Plumach uses the pinned
MACE/AXE/SICKLE/SCYTHE branch and optional shield order. Amm receives all four
physical/magical leader/driver loadouts, including levels, hit points, speed,
small sizing, enchantment, and copper/glass/silver material rolls. Ara receives
the +1 gold kamerel vajra and gold mirror. Apprentice witches, witches, coven
leaders, and Hmnyw-Pharaoh now receive their exact available hat, dress,
weapon, size, material, BUC, and familiar-dependent equipment. Illurien carries
the neutral-Beatitude spellbook base only. Shattered Ziggurat cultist, knight,
and wizard torch/faceless-robe loadouts are lit at birth with pinned fuel
ranges. Center now receives the reused bardiche, war hat, archaic gauntlets,
leather armor, robe, and boots. Living dolls drop a lifeless doll; parasitized
dolls drop 2d4 eyeballs plus a lifeless doll after life-saving has failed.

At the B3-1 checkpoint no artifact was declared. Center's First Key, Alhoon's
Second/Third Keys, Necronomicon behavior, Infinity's Mirrored Arc, Staff of
Twelve Mirrors, Sansara Mirror, Mirror Brand, and Soulmirror were then
explicitly deferred to B3-2 and later scoped B3 work. B4/B5, maps, Lua ports,
topology, scheduler, R'lyeh activation, and DL111 remain untouched by this
checkpoint.

The representative object save fixture round-trips ID, enchantment, material,
size, and property metadata through the production `struct obj` codec on x64
and Win32. It also confirms `EDITLEVEL=5`, rejection of prior/future epochs,
and unchanged native save routing. Focused B3-1 declaration/source gates,
Step 10B compatibility, B2-1/B2-2/B2-3/B2-4 declaration/runtime suites, witch
familiar paths, generated data, protected Step 7/8 source projections, and
the affected Step 9C foundation/armor-size/coating/defense/handedness/weapon-
damage/selection/spell suites pass on both architectures. The source-art
table appends 21 deliberately reused native tile drawings. Sequential
makedefs, tilemap, and tile2bmp generation reports 2,867 tiles, a complete
640x1574 8-bpp bitmap, and all 12,410 glyph indices within bounds.

Both final Release configurations completed with exit 0:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,983,232 bytes,
  `2026-09-11T05:16:36.2538224Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,845,568 bytes,
  `2026-09-11T05:17:20.8437578Z`.

STEP 10 MONSTER INVENTORY COMPLETE: YES
STEP 10B3-1 COMPLETE: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10B3-2: YES
READY FOR STEP 10C: NO

## Step 10B3-2 checkpoint: standard artifacts and Neutrality keys

Step 10B3-2 adds exactly the eight authorized standard artifacts, appended in
this order after the preserved historical artifact declarations:

1. First Key of Neutrality (`ART_FIRST_KEY_OF_NEUTRALITY`, ID 37)
2. Second Key of Neutrality (`ART_SECOND_KEY_OF_NEUTRALITY`, ID 38)
3. Third Key of Neutrality (`ART_THIRD_KEY_OF_NEUTRALITY`, ID 39)
4. Infinity's Mirrored Arc (`ART_INFINITYS_MIRRORED_ARC`, ID 40)
5. Staff of Twelve Mirrors (`ART_STAFF_OF_TWELVE_MIRRORS`, ID 41)
6. Sansara Mirror (`ART_SANSARA_MIRROR`, ID 42)
7. Mirror Brand (`ART_MIRROR_BRAND`, ID 43)
8. Soulmirror (`ART_SOULMIRROR`, ID 44)

The artifact sentinel is ID 45 and `NROFARTIFACTS` is 44. The append-only
prefix remains unchanged, the sentinel stays within the signed `oartifact`
storage field, and global random object generation continues to call the
existing artifact exclusion logic. Production values are `NUMMONS=503`,
`NUM_OBJECTS=547`, `MAX_GLYPH=12410`, and 2,867 generated tiles.

The three Neutrality Keys use the existing `UNIVERSAL_KEY` base, neutral
alignment, and `NOGEN|RESTR`, with distinct prices. Center's birth path gives
the First Key only when the artifact is not already present. Alhoon birth
selects the Second Key first, the Third Key second, and an ordinary universal
key thereafter; `exist_artifact` prevents duplicate artifact creation. The
local helper uses the existing serialized object `usecount` field and does not
import a donor `altmode` or counter framework.

The five standard artifacts use the following pinned local mappings and bounded
effects:

- Infinity's Mirrored Arc: native `DOUBLE_LIGHTSABER`, reflection, and a
  persisted alternate-mode toggle that enables a second beam path plus a
  bounded extra `3d3` artifact-hit effect.
- Staff of Twelve Mirrors: `KHAKKHARA`, reflection, and the local displacement
  flag, with bounded physical damage `5d6`.
- Sansara Mirror: `MIRRORBLADE`, gold material override, reflection, local
  half-spell-damage behavior, and bounded physical damage `8d8`.
- Mirror Brand: `LONG_SWORD`, silver material override, reflection,
  same-alignment damage, and bounded `2d10` stun damage. Native Magicbane's
  `rnd(4)` path is unchanged.
- Soulmirror: existing `PLATE_MAIL` with a local mithril material override,
  reflection, level-drain resistance, and a bounded +7 armor contribution.

The donor/local Soulmirror mapping is therefore `PLATE_MAIL -> MITHRIL
override`; no new base object was added. Donor-only frameworks such as
`SPFX3_NOCNT`, donor `altmode`, Necronomicon, and Silver Key remain excluded.

The packaged x64 runtime exercised all eight artifact names, discovery and
drop behavior, reflection/displacement/half-spell-damage/level-drain checks,
Infinity alternate mode, and save/restore with the alternate-mode state
preserved. A separate packaged runtime fixture verified Center's First Key and
four sequential Alhoon births (Second, Third, ordinary, ordinary). The native
bones source is unchanged; existing native save/restore and lifecycle codecs
also pass.

Source, focused C, generated-data, compatibility, historical B2/B3-1, Ogre,
and the affected Step 9C foundation/armor-size/coating/defense/handedness/
weapon-damage/selection/spell gates pass. Historical sentinel assertions in
the intentional B2-3, B2-4, and B3-1 fixtures were updated from the old
checkpoint sentinel 37 to the current append-only sentinel 45. x64 and Win32
Release builds both completed successfully:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,986,304 bytes,
  `2026-09-11T06:11:43.8681358Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,847,616 bytes,
  `2026-09-11T07:04:27.1890005Z`.

README, `dat/dungeon.lua`, maps, production topology, scheduler, fixed DL111,
Necronomicon, Silver Key, and new base objects were not added or modified.
The working tree remains intentionally dirty and uncommitted for the Step 10
checkpoint. B3-3 is the next permitted step; Step 10B and Step 10C remain
incomplete.

STEP 10B3-2 COMPLETE: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10B3-3: YES
READY FOR STEP 10C: NO

## Step 10B3-3 checkpoint: Necronomicon and Silver Key

Step 10B3-3 appends exactly two artifacts after the B3-2 prefix: ID 45 The
Necronomicon (`ART_NECRONOMICON`) and ID 46 The Silver Key
(`ART_SILVER_KEY`). The terminal sentinel is now 47 and `NROFARTIFACTS` is
46. IDs 1 through 44, all 503 monster IDs, and all 547 object IDs remain
stable. The signed `char obj.oartifact` field can hold the complete 1..46
range, and the existing compile-time storage assertion remains active without
a structure or save-epoch change (`EDITLEVEL=5`).

The pinned donor trace covered the declarations, artifact creation and
existence bookkeeping, carried-property query, read/invoke dispatch, the
complete normal Necronomicon menu and occupation callback, the generic portal
invocation, and native save/bones handling. The donor Necronomicon stores
discovered pages in `ovar1`, a study count in `spestudied`, a first-read event,
and global sanity/insight/madness, ward, seal, and spell/skill progression.
Those are dependencies rather than the identity of every operation. They were
removed from the retained paths; no substitute persistent occult state or
occupation was added.

### Complete pinned Necronomicon operation matrix

Every operation reachable through the donor's normal known-passage menu, plus
its unknown-page study path, was classified before implementation:

| # | Pinned operation | Class | Bounded result or reason |
|---:|---|---|---|
| 1 | Summon byakhee | ADAPT | One existing `PM_BYAKHEE` is created adjacent and tamed for 20 Pw. Donor variable count, temporary-vanish state, level inflation, and traitor logic are omitted. |
| 2 | Summon night-gaunt | ADAPT | One existing `PM_NIGHTGAUNT` is created adjacent and tamed for 10 Pw. Donor 1d4 count and loyal-pet extension are omitted. |
| 3 | Summon shoggoth | EXCLUDE | A plain `makemon(PM_SHOGGOTH)` adaptation was evaluated, but the pinned result is specifically an uncontrolled sleeping, crazed creature. Local has no donor crazed state; erasing that defining result would make this a different passage. |
| 4 | Summon ooze | EXCLUDE | The pinned result is a random donor ooze-table pet with level/HP inflation and possible betrayal. Flattening it to one ordinary local species would discard the passage's category-selection and risk behavior. |
| 5 | Summon demon | EXCLUDE | The pinned random demon table, enhanced pet state, and betrayal behavior are not local. A fixed tame demon was rejected as a materially different result. |
| 6 | Summon devil | EXCLUDE | The pinned random devil table, enhanced pet state, and betrayal behavior are not local. A fixed tame devil was rejected as a materially different result. |
| 7 | Learn protection | EXCLUDE | Permanent spell-repertoire/page progression; no page or learning subsystem is imported. |
| 8 | Learn turn undead | EXCLUDE | Permanent spell-repertoire/page progression; no page or learning subsystem is imported. |
| 9 | Learn force bolt | EXCLUDE | Permanent spell-repertoire/page progression; no page or learning subsystem is imported. |
| 10 | Learn drain life | EXCLUDE | Permanent spell-repertoire/page progression; no page or learning subsystem is imported. |
| 11 | Learn finger of death | EXCLUDE | Permanent spell-repertoire/page progression; no page or learning subsystem is imported. |
| 12 | Learn detect monsters | ADAPT | The existing level-1 `SPE_DETECT_MONSTERS` effect runs immediately for its native 5-Pw cost; no learned-page or spell-memory state is created. |
| 13 | Learn clairvoyance | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 14 | Learn detect unseen | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 15 | Learn identify | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 16 | Learn confuse monster | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 17 | Learn cause fear | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 18 | Learn levitation | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 19 | Learn stone to flesh | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 20 | Learn cancellation | EXCLUDE | Permanent spell-repertoire/page progression; it is not replaced by an unrelated one-shot effect. |
| 21 | Combat/weapon skill | EXCLUDE | Its core is permanent skill unrestriction, expert-cap mutation, and maximum-energy mutation, all donor progression. |
| 22 | Health and recovery | KEEP | Calls the existing `use_unicorn_horn` helper with the artifact itself, preserving native blessed/uncursed/cursed semantics and requiring no Pw. |
| 23 | Lost Carcosa/Yellow Sign | EXCLUDE | Its core is learning a prohibited ward. |
| 24 | Ancient wards | EXCLUDE | Its core is unlocking the donor ward set. |
| 25 | Elemental-lord wards | EXCLUDE | Its core is unlocking donor elemental wards. |
| 26 | Spirits/seals, first half | EXCLUDE | Its core is unlocking the first donor seal set. |
| 27 | Spirits/seals, second half | EXCLUDE | Its core is unlocking the second donor seal set. |
| 28 | Study unknown pages | EXCLUDE | Its core is randomized persistent page discovery driven by `ovar1`, `spestudied`, Intelligence, sanity, and insight. |

The resulting four-choice read menu is deliberately small. It contains two
artifact-local native summons, immediate monster detection, and native
health/recovery. Every choice is traced above and exercised through the real
packaged read/menu path. Cancellation returns without a turn or resource
change; insufficient Pw and confused reading consume the ordinary attempted
turn without resource loss; hallucination only changes the confusion message;
blind reading is rejected by the existing generic spellbook rule. A successful
read discovers and fully identifies the artifact. `#invoke` safely directs the
player to read it. The ordinary `SPE_SECRETS` path still produces the harmless
B3-1 hint and cannot enter this menu.

### Silver Key carrier, portal, and lifecycle architecture

The pinned Silver Key uses `UNIVERSAL_KEY`, a persistent silver material
override, and **carried** energy regeneration, teleport control, and polymorph
control. Local energy regeneration and teleport control already had artifact
bits and existing extrinsic/property plumbing. A narrow local `SPFX_PCTRL` bit
was added to the same carried-property path because local
`Polymorph_control` exists but had no artifact carrier mapping; no donor
`SPFX3` framework or global polymorph change was imported. Pickup, drop, and
restore therefore acquire/remove all three passives through
`set_artifact_intrinsic`, and the existing turn loop supplies energy
regeneration. Ordinary nonartifact `UNIVERSAL_KEY` objects retain their tiny
iron base material and ordinary key behavior.

Silver material is applied in the same `artifact_exists` creation path used by
the B3-2 material overrides, so every successful Silver Key creation receives
the saved `obranch_material=SILVER` metadata. Artifact existence/discovery
remain in the native artifact arrays. Object identity, name, material, and all
other object state continue through the complete native `struct obj` codec;
artifact passives are recomputed from carried inventory on restore. The native
`resetobjs(..., ONAME_BONES)` artifact lifecycle remains the bones owner. No
new saved field, occult state, object pointer, or save schema was added.

The portal safety core accepts an explicit candidate and an explicit descriptor
containing four distinct standard `d_level` identities: the caller-supplied
DoD Approach level, Neutral Quest dungeon, Lost Cities dungeon, and Dispensary
dungeon. It accepts only the exact Approach level, Neutral levels 1..7, Lost
Cities levels 1..13, or Dispensary level 1. Null, out-of-range, aliased,
unrelated, arbitrary-depth, and progression/endgame candidates are rejected;
selection clears its output on failure. There are no production dungeon
numbers, future globals, special-level records, `dungeon.lua` references, or
fixed DL111 values in this core.

Production invocation intentionally supplies no candidates or descriptor
until Step 10C exists. It consumes the local invocation command's normal turn,
reports that no door is available, and leaves artifact age, charges, dungeon
position, and progression unchanged. Step 10C must resolve the genuine four
domain identities, build only currently available/discovered candidates, pass
them with the descriptor to `silver_key_choose_destination`, and perform travel
only after that helper succeeds. That identity/candidate wiring and the actual
successful level transition are the entire deferred positive path.

### B3-3 verification and remaining scope

The focused C gate compiles and passes under x64 and Win32, covering IDs,
storage capacity, all four operation mappings/costs, accepted legal synthetic
subdomains, rejected malformed/unrelated/arbitrary-depth candidates, selection,
and fail-closed output. The source gate covers declarations, material creation,
carried properties, read isolation, lifecycle/save/bones ownership, and the
absence of future topology and prohibited state. The fresh packaged runtime
passes on both architectures: real artifact creation, menu cancellation, all
four choices and costs, both summons, ordinary-base isolation, all three
passives across pickup/drop/restore, energy-regeneration gain/loss, safe
no-destination invocation with unchanged age/location, confusion handling, and
artifact save/restore.

All Step 10B source gates, focused B1 through B3-3 gates, B2-4 runtime helpers,
B3-2 standard-artifact and Center/Alhoon packaged runtimes, and the materially
affected Step 9C foundation, armor-size, coating, defense, projectile, weapon,
selection, and spell regressions pass on x64 and Win32. Generated validation
reports `NUMMONS=503`, `NUM_OBJECTS=547`, `AFTER_LAST_ARTIFACT=47`,
`MAX_GLYPH=12410`, all 12,410 glyph indices in range, 2,867 tiles, and a
complete 640x1574 8-bpp bitmap.

Both final Release solutions completed successfully:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,989,888 bytes,
  `2026-09-11T10:34:30.4589884Z`.
- `E:/Codex/My_Nethack/binary/Release/Win32/NetHack.exe`: 4,850,688 bytes,
  `2026-09-11T10:35:41.5304995Z`.

B3-4 remains the next separately authorized B3 integration/closeout slice; no
artifact declaration remains after this checkpoint. Step 10B4 still owns the
previously recorded remaining branch support, Step 10B5 owns final Step 10B
closeout, and Step 10C owns maps, Lua ports, production dungeon/branch
registration, scheduler/activation, genuine portal identities, and any fixed
Approach placement. README and `dat/dungeon.lua` remain untouched.

STEP 10B3-1 COMPLETE: YES
STEP 10B3-2 COMPLETE: YES
STEP 10B3-3 COMPLETE: YES
ALL 10 STEP 10 ARTIFACTS DECLARED: YES
STEP 10B COMPLETE: NO
READY FOR STEP 10B3-4: YES
READY FOR STEP 10C: NO

## Step 10B5 / whole-Step-10B integration and closeout

Recorded 2026-09-11. This is the authoritative current Step 10B closeout.
Sections above remain the preserved Step 10A and B1/B2/B3/B4 historical
checkpoints. B5 added no gameplay content and found no production defect, so
it made zero production C/header/data changes. It added the unified x64 gate
`test/run_step10b5.py`, the small cross-phase source contract
`test/test_step10b5_source.py`, and corrected only two stale current-state test
expectations: `test/run_step9c_defense.py` now reverse-projects the intentional
B4 Outlands tree hook before its historical Step 9 byte comparison, and
`test/test_step10b_source.py` no longer prints that later Step 10B is
incomplete. The focused defense gate failed before that test-only correction
and passed afterward; production `src/dokick.c` was not changed in B5.

### Final identity and persistent-state contract

| quantity | final Step 10B value |
|---|---:|
| monster IDs | 0..502 |
| NUMMONS | 503 |
| object IDs | 0..546 |
| NUM_OBJECTS | 547 |
| legacy object class boundary | 526 |
| artifact IDs | 1..46 |
| AFTER_LAST_ARTIFACT / NROFARTIFACTS | 47 / 46 |
| EDITLEVEL | 5 |
| MAXDUNGEON / MAXLINFO | 18 / 3600 |
| MAX_GLYPH | 12410 |
| generated tiles | 2867 |

The compatibility gate compared all 993 declarations from the frozen Step 9
checkpoint and proved that every old monster, object, and artifact ID retains
its value. B5 added no monster, object, artifact, terrain, or trap ID and did
not bump `EDITLEVEL`.

Persistent Step 10B ownership is final: `u.uevent.sum_entered` is saved in the
native complete `struct you` codec; `svl.level.flags.lethe` is reset by
`clear_level_structures()` and saved/restored by the native level-flags codec;
Witch's Familiar relationships remain in the existing saved monster field and
bones ID remapper; B3 material, size, properties, origin species, artifact
identity, alternate-mode/usecount, existence, and discovery remain in native
object/artifact codecs; Silver Key passives are recomputed from carried
inventory; and ESHK/EPRI remain native mextra state. The B4
`step10b_level_context` enum is an argument-only transient value and does not
occur in player, level, save, or restore structures. No new persistent field or
save epoch was added in B5.

### B1-B4 ownership closure

- B1 is complete: `MAXDUNGEON=18`, `MAXLINFO=3600`, `EDITLEVEL=5`, the named
  Sum and Lethe saved bits, ogre mage, append-only compatibility, reset
  isolation, native codec, depth, recovery, and version rejection all pass.
- B2 is complete: all Step 10 species through Witch's Familiar ID 502,
  declarations, attacks, defenses, spells, equipment, selectors, Center and
  Alhoon behavior, R'lyeh primitive, mental adaptations, familiar lifecycle,
  soul/death/gas helpers, and saved/bones monster state pass their focused and
  integration gates.
- B3 is complete: objects 526..546 remain zero-probability extensions outside
  legacy random generation; class boundary 526 is fixed; base behavior and
  saved metadata pass; artifacts 37..46, all three Neutrality Keys, five mirror
  artifacts, Necronomicon, and Silver Key pass identity, uniqueness,
  discovery, lifecycle, carried-passive, save/restore, bones, and packaged
  runtime checks. B2 equipment/drop dependencies are connected.
- B4 is complete: the topology-independent context helpers, Lethe semantics,
  hero gradient, monster fumble modifiers, Outlands tree rules, mirror-shard
  pits, projectile materials, colors/descriptions, portal-seen reuse, Gate pet
  exception, and Plumach-shopkeeper/bridge-priest native role designators pass.
  Every production call site remains `STEP10B_CTX_NONE` until Step 10C supplies
  real level identity. No B4-owned dependency remains.

The whole-step audit found no ownership gap and no requirement improperly
deferred to Step 10C. Acquisition/removal/reset/save/restore/death/cleanup
symmetry is covered by the compatibility codec, B2-4 runtime helper, packaged
artifact/key runtimes, B4 real-structure state fixture, recovery fixture, and
native shop persistence runtime. Those gates cover familiar replacement/death
and bones remapping, artifact existence and passive removal, object metadata,
Lethe reset, deletion-safe container traversal, ESHK/EPRI validity and
idempotence, and transient-context nonpersistence.

### B5 integration and regression evidence

The final `test/run_step10b5.py` x64 master invocation passed all eleven
reported categories: identity/table invariants; B1; B2; B3; B4;
object/artifact/Lethe integration; monster/equipment/spell integration;
lifecycle/save/restore/recovery/bones; ordinary/non-Step-10 isolation; Step 10C
dormancy/boundary; and generated/glyph/tile consistency. Its composed evidence
includes every existing B1-B4 source/focused gate, B2-4 runtime helpers, ogre
packaged runtime, B3-2 and B3-3 packaged runtimes, B4 shop/portal runtime,
depth/save-layout, standalone recovery, and the affected Step 9C foundation,
armor-size, coatings, defense, weapon-damage, weapon-selection, handedness,
and spell regressions.

Observed cross-phase results include artifact and Book-of-the-Dead protection
through Lethe object rewriting; stable `POT_AMNESIA`; ordinary `SPE_SECRETS`
remaining separate from the Necronomicon; weight refresh with B3 object
metadata on Lethe transformations; B2 equipment with B4 spell context;
mirror-shard reuse of native silver hatred; valid native shopkeeper/priest
auxiliary state; and fail-closed Silver Key production invocation. Ordinary
random monster/object/shop, water, tree, trap, pet, priest, spell, rendering,
weapon, armor, projectile, and artifact behavior remains protected outside an
explicit Step 10 context.

The generated gate read the final Release enum dump and every generated row:
503 monsters, 547 objects, artifact sentinel 47, 12410 glyphs in exact index
order, every tile index below 2867, and a complete 640x1574 8-bpp bitmap. No
generated output was hand-edited.

The mandatory final command
`MSBuild.exe sys\\windows\\vs\\NetHack.sln /p:Configuration=Release
/p:Platform=x64 /v:m` returned exit 0. The rebuilt
`E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe` is 5,996,544 bytes with
timestamp `2026-09-11 19:01:49 +03:00` (`2026-09-11T16:01:49Z`) and SHA-256
`43F9DE1955DB254C7DFFA5842E772B3E15069B42F2F19531C72F03334B5F5AC1`.
The rebuilt `nhdat500` is 1,609,361 bytes with SHA-256
`178CB82B4DE9E4690886F7E65B133CC49DFDEA550554B97AD14D17062D00F65A`.

Fresh post-build packaged x64 runs passed: generated identity/tile/glyph
validation; ogre-mage monster-target casting and save/restore; the eight
standard artifacts and alternate-mode persistence; Center plus four Alhoon
births producing First, Second, Third, ordinary, ordinary keys; all four
Necronomicon operations; ordinary secrets-book isolation; Silver Key carried
passives, removal, dormant invocation, and restore; portal-guide discovery;
paid proof/acid/enchant/uncurse services; and shop item-state save/restore.
Dormant B4 map-identity behavior is covered by deterministic extraction/state
fixtures rather than temporary production topology. Win32 was not required and
no optional Win32 diagnostic was run because B5 made no serialization-, ABI-,
pointer-width-, integer-width-, or Windows-specific production fix.

### Step 10C handoff boundary

Only the following wiring/content work remains for Step 10C: the 26 concrete
Lua map bodies/resources and actual `neulev`; Neutral Quest, Lost Cities, and
Dispensary dungeon/branch definitions; persistent scheduler integration with a
randomized DoD parent in DL30..199; real Gate/out1/out2/out3/out4/Spire/Sum/
Lost Cities/R'lyeh/Dispensary identity resolution; map-set Lethe designation;
map calls to the Plumach shopkeeper and bridge-priest designators; actual
rooms, terrain, shops, portals, stairs, holes, and production topology; the
eleven Outlands functions `place_neutral_features`, `mkkamereltowers`,
`mkminorspire`, `mkfishingvillage`, `mkwell`, `mkpluhomestead`, `mkpluvillage`,
`mkferrutower`, `mkinvertzigg`, `mkneuriver`, and `neuliquify`, including
probability dispatch and placement; genuine Silver Key destination discovery
and travel; and the map-created cursed +12 `LONG_SWORD` with `OBP_DEEP` named
The Sword of the Deeps. Step 10C must wire the completed B4 primitives without
redesigning them.

B5 added no map, Lua resource, generator, dungeon record, branch record,
scheduler hook, real portal/stair/hole, fixed DL111 reservation, or production
Silver Key travel. `README.md`, `dat/dungeon.lua`, and `src/dungeon.c` remain
unchanged. No Step 10B-owned TODO remains. The final external safety record is
`E:/Codex/My_Nethack/_qa/step10b5-whole-step10b-integration.patch`,
`E:/Codex/My_Nethack/_qa/step10b5-untracked-intentional.txt`, and
`E:/Codex/My_Nethack/_qa/step10b5-untracked/manifest.txt`; the manifest records the final
status, complete and intentional untracked lists, copied paths, per-file
SHA-256 agreement, patch size, timestamp, and Win32 status. Nothing was staged,
committed, tagged, or pushed, and HEAD/upstream remain the frozen checkpoint.

STEP 10B1 COMPLETE: YES
STEP 10B2 COMPLETE: YES
STEP 10B3 COMPLETE: YES
STEP 10B4 COMPLETE: YES
STEP 10B5 COMPLETE: YES
STEP 10B COMPLETE: YES
READY FOR STEP 10C: YES

## Step 10C-A compatibility audit: static maps and resources

The 26 donor resources are audited proactively against the local monster and
object tables before packaged-runtime loading. The audit separates live
`des.monster` identity from statue-only `montype` identity. A donor class or
display-class character is treated as representation, not as a replacement
identity: fixed named species retain their local named species, while a true
random donor class would require an equivalent local pool with the donor
ordering and probabilities. No true random unsupported monster class occurs
in these 26 resources.

Category definitions used by the source gate:

1. exact local equivalent/renamed identity;
2. semantically equivalent local adaptation;
3. no valid local equivalent; no fallback is emitted.

The final substitution table is:

| Donor identity | Local identity | Category | Usage context | Semantic justification | Affected resources |
|---|---|---:|---|---|---|
| Monster class `"` with named Plumach/Ferrumach/Cuprilach/Argenach Rilmani | Existing local named Rilmani species | 1 | Live monsters; fixed named selections | Representation-only adaptation. Local named species, placement order, attributes, and occurrence order are preserved; no donor class collapse occurs. | `spire`, `sumall` |
| Monster class `{` with named `mirrored moonflower` | Existing local `mirrored moonflower` | 1 | Live monsters; fixed named selections with donor probabilities | Representation-only adaptation. The named species is local, so the unsupported donor display class is removed while the named identity and probabilities remain. | `lethe-b`, `nkai-a-1`, `nkai-a-2`, `nkai-b`, `nkai-c`, `nkai-z` |
| Other donor monster/display-class symbols and `random` class selections | Same local symbols/pools | 1 / representation-only | Live monster placements and true random selections | Full inventory across all 26 resources is checked against `defsym.h`; all other referenced symbols are valid locally. There are no unsupported true random class selections requiring a pool rewrite. | All 26 resources |
| `blessed +3 bone viperwhip` | `viperwhip` | 1 | Three statue contents in `lethe-f` | Existing local Step 10B object under a donor custom name: exact underlying identity, with donor name, blessing, and +3 preserved. | `lethe-f` |
| `trapped metal box` | `large box` | 1 | Trapped map object in `lethe-f` | Existing local Step 10B container under a donor custom name: preserve display name, trap state, curse, +5, and placement. | `lethe-f` |
| `gold amulet of magical breathing` | `amulet of magical breathing` | 1 | Statue content in `lethe-f` | Existing local Step 10B amulet identity; `gold` is donor descriptive wording rather than a distinct local object-table identity, and the magical-breathing effect is preserved. | `lethe-f` |
| `star sapphire` | `sapphire` | 2 | Two decorative Father Dagon statue contents in `lethe-f` | Donor-only gem variant is absent. Both are blue hard gems in the same object class; these contents have no later map-logic dependency, so the local sapphire preserves the relevant resource semantics and order. | `lethe-f` |
| `flying boots` | No local fallback | 3 | Live map object at `{73,12}` in `lethe-g` | Not approved as `levitation boots`: donor `FLYING` and local `LEVITATION` are distinct player movement properties. The object is omitted with a bounded marker rather than changing map gameplay silently. | `lethe-g` |
| Statue-only `dryad` `montype` | `oread` `montype` | 2 | Decorative historic statue | Both are existing nymph statue identities; no live-monster gameplay logic depends on this `montype` in the resource. | `lethe-f` |
| Statue-only `elder priest` `montype` | `high priest` `montype` | 2 | Decorative historic statue | Preserve the local priest role for a statue-only decorative use; no live-monster gameplay logic depends on the donor species here. | `lethe-g` |
| Statue-only `lethe elemental` `montype` | `water elemental` `montype` | 2 | Two decorative historic statues | Preserve the local river-elemental role; this is statue-only and no gameplay logic depends on the donor species here. | `lethe-g` |
| Statue-only `god` `montype` sentinel | No `montype`; named historic statue retained | 3 | Four decorative historic statues in `lethe-f` | Donor `god` is a non-species statue sentinel, not a monster identity. The correct adaptation is representation-level removal of `montype`, not substitution with a monster species. | `lethe-f` |
| Statue-only `gnoll` `montype` | No local equivalent; no fallback | 3 | One decorative historic statue at `{48,1}` in `lethe-g` | Local tables contain `gnoll ghoul`, not a live `gnoll`. The historic statue is retained without `montype`; no `kobold` substitution is made. This remains a bounded 10C-A compatibility decision. | `lethe-g` |
| `cursed +12 deep long sword named The Sword of the Deeps` | Existing local `long sword` base, deferred | 1 for the underlying base identity; `DEFERRED_10C_D` for the instance | Deferred map-owned named object | The base object is exact locally, but this particular 10C-D-owned instance is not emitted by 10C-A. Its donor coordinates, curse, +12, and name remain in the deferred contract marker. | `lethe-z` |

The source gate verifies the complete donor identity sets: no missing live
named monster identity; six missing object-table identities are explicitly
classified above; and five missing statue-only `montype` identities are
explicitly classified above. It also enumerates every donor monster class /
display-class symbol across the 26 resources and rejects any unsupported local
class that survives conversion. The final source gate additionally checks the
numeric `lit=0/1` form required by local `des.region` table syntax, the donor
`FLYING` versus local `LEVITATION` boot mechanics, and the decorative-only
Father Dagon sapphire scope. It passes.

The mandatory Release x64 rebuild completed successfully with exit 0. The
final packaged artifacts are:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,996,544 bytes,
  SHA-256 `B949436D817AF5961E3AABBD79336EE94496BFE33CAB61DD9C3F976C8480CB3C`.
- `E:/Codex/My_Nethack/binary/Release/x64/NetHackW.exe`: 7,316,992 bytes,
  SHA-256 `2DBC01041F6341CD319C2FFA6073FF57D74D2A38DF74309B8B7F521571D86E8C`.
- `E:/Codex/My_Nethack/binary/Release/x64/nhdat500`: 1,774,226 bytes,
  SHA-256 `87156D11F72FA90D4B97CFB7B8967DB191BBD2608278FA6CBBCCC4D866ABBA91`.

The fresh packaged-runtime sweep at
`E:/Codex/My_Nethack/_qa/step10c-a-tests/x64/runtime-20260912-final6` passed all 26
resources independently, including Lua loading, map generation for static
maps, deferred generator-only resources, resource registration, and the
identity-heavy Lethe files. No Lua error or paniclog remained. The runtime
test uses uppercase extended-command and filename input only as a narrow
WinPTY/TTY harness workaround; NetHack's extended-command matching and the
Windows package filesystem are case-insensitive.

Because `gnoll` has no established local equivalent, and `flying boots` cannot
be represented without changing `FLYING` to the materially different
`LEVITATION` property, 10C-A retains explicit bounded Category 3 decisions and
does not authorize a silent fallback. These are documented compatibility
decisions, not unresolved failures of the 10C-A completion contract; any later
donor-identity resolution remains separately scoped.

## Step 10C-A final status

The final 10C-A source/resource gate and the fresh packaged x64 runtime gate
pass. The established Step 10B5 master evidence remains green from the prior
whole-Step-10B checkpoint, and the current non-PTY B5 source, B1/ogre boundary,
and native compatibility/save-codec checks also pass. A newly attempted legacy
B3-3 WinPTY rerun produced no production diagnostic or failing invariant and
stalled in the known harness fixture; it is recorded as
`NOT COMPLETED — HARNESS STALL, NON-BLOCKING`. This is unrelated to the C-A
resource changes and is not a C-A completion blocker. No topology, scheduler,
generator, C-C, C-D, or later-step work was started.

STEP 10C-A STATIC MAPS/RESOURCES IMPLEMENTED: YES
STEP 10C-A COMPLETE: YES
STATIC RESOURCE INVENTORY COUNT: 26/26
FULLY CONVERTED RESOURCE COUNT: 26/26
TRUNCATED/STUBBED/PARTIAL MAP RESOURCES: NONE
LUA PARSE/LOAD GATE: PASS
CONTENT REFERENCE VALIDITY GATE: PASS
STATIC GEOMETRY/COORDINATE GATE: PASS
ALTERNATE MAP COMPLETENESS/COMPATIBILITY GATE: PASS
CONNECTOR CONTRACT TABLE COMPLETE: YES
DLB/PACKAGED RESOURCE GATE: PASS
RESOURCE REGISTRATION COMPLETE: YES
OUT1-OUT4 STATIC BASE MAP GATE: PASS
UNAVAILABLE 10C-C GENERATOR CALLS PRESENT: NO
FAKE/NO-OP GENERATOR STUBS ADDED: NO
STANDARDIZED DEFERRED C-C/C-D MARKERS COMPLETE: YES
EXISTING STEP 7-9 TEST/LOADER INFRASTRUCTURE REUSED: YES
NEW MONSTER IDS ADDED: 0
NEW OBJECT IDS ADDED: 0
NEW ARTIFACT IDS ADDED: 0
NEW TERRAIN IDS ADDED: 0
NEW TRAP IDS ADDED: 0
NEW PERSISTENT SAVE FIELDS ADDED: NO
EDITLEVEL: 5
NUMMONS: 503
NUM_OBJECTS: 547
ARTIFACT SENTINEL/COUNT: 47/46
MAX_GLYPH: 12410
TOTAL TILE COUNT: 2867
STEP 10B5 REGRESSION GATE: PASS (authoritative master plus current non-PTY evidence)
STEP 10B5 LEGACY B3-3 WINPTY RERUN: NOT COMPLETED — HARNESS STALL, NON-BLOCKING
10C-A X64 RESOURCE/PACKAGE GATE: PASS
10C-A GENERATED ID/TILE/GLYPH GATE: PASS (existing invariant evidence)
10C-A X64 RELEASE BUILD: PASS
FINAL X64 EXE SIZE: 5996544 bytes
FINAL X64 EXE SHA-256: B949436D817AF5961E3AABBD79336EE94496BFE33CAB61DD9C3F976C8480CB3C
WIN32 REQUIRED AS COMPLETION GATE: NO
WIN32 OPTIONAL DIAGNOSTIC RUN: NO
WIN32 OPTIONAL DIAGNOSTIC RESULT: NOT RUN
README MODIFIED: NO
STEP 10 PRODUCTION TOPOLOGY REGISTERED: NO
STEP 10 PRODUCTION SCHEDULER REGISTERED: NO
OUTLANDS PROCEDURAL GENERATOR IMPLEMENTED: NO
STEP 10 FIXED DL111 RESERVATION PRESENT: NO
REAL B4 STEP 10 IDENTITY ACTIVATION STARTED: NO
STEP 10C-B WORK STARTED: NO
STEP 10C-C WORK STARTED: NO
STEP 10C-D WORK STARTED: NO
STEP 10C-E WORK STARTED: NO
COMMITTED: NO
TAGGED: NO
PUSHED: NO
FINAL HEAD UNCHANGED (48fe150...): YES
WORKING TREE DIRTY WITH UNCOMMITTED STEP 10: YES
UNTRACKED ARTIFACTS CLEAN (NO BUILD LEAKS): YES
10C-A CHECKPOINT SAFETY BACKUP EXPORTED: YES
STEP 10B COMPLETE: YES
STEP 10C COMPLETE: NO
READY FOR STEP 10C-B: YES
READY FOR STEP 10C-C: NO
READY FOR STEP 10D: NO

## Step 10C-E — whole-10C closeout

The authoritative Step 10C-E closeout started from the frozen C-D
checkpoint `E:/Codex/My_Nethack/_qa/step10c-d-checkpoint-20260912-155807/`, on branch
`phase0/dod-length`, with `HEAD` and `origin/phase0/dod-length` both at
`48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`. The index remained empty, README
remained unchanged, all three submodules remained pinned, and no Step 10D or
playtest artifact was created. The external master runner is
`E:/Codex/My_Nethack/test/run_step10c_e.py`; all generated logs, saves,
transcripts, extracted packages, and fixture directories are under
`E:/Codex/My_Nethack/_qa/step10c-tests/x64/step10c-e-final2/`.

E1 passed the source/structure boundary audit, 10C-A/B/C/D source contracts,
the B5 cross-phase contract, and generated identity/glyph/tile invariants:
`NUMMONS=503`, `NUM_OBJECTS=547`, `AFTER_LAST_ARTIFACT=47`,
`MAX_GLYPH=12410`, `2867` tiles, and complete `640x1574` 8-bpp bitmap.

E2 passed the current Step 10 phase regression set. The executed non-PTY B5
components covered B1 compatibility/save codecs, B2 ogre/native spell,
B2-1..B2-4, B3-1..B3-4, B4 source and deterministic helpers, native B2-4,
packaged ogre, packaged B3-2 artifact naming/save/restore, depth/ledger/
recovery, all affected Step 9C suites, and the generated gate. It also passed
the 10C-A 26-resource package gate, 10C-B scheduler gate, 10C-C focused native
gate, and 10C-D native identity gate. The existing B3-3 interactive WinPTY
child was not used as an authoritative pass/fail signal after reproducing its
idle harness stall; its native core and packaged artifact gates passed.

E3 passed the exact 1,000-seed scheduler batch with deterministic idempotence,
zero illegal parents, zero collisions, and `169` observed parent depths:
DL30..DL199 except DL196. Dispensary parent frequencies were N2=`214`,
N3=`196`, N4=`208`, N5=`206`, and N6=`176`. The external distribution record
is `E:/Codex/My_Nethack/_qa/step10c-tests/x64/step10c-e-final2/e3/scheduler/frequency-distribution.json`.

E4 passed 1,000 native Outlands instances, 250 each of out1..out4, in four
in-process 250-case batches. Totals were Kamerel `36`, spire `62`, fishing
`56`, well `56`, river `141`, Plumach village `120`, inverted ziggurat `63`,
Ferrumach `153`, and homestead attempts/successes `1266/1266`. The external
record is `E:/Codex/My_Nethack/_qa/step10c-tests/x64/step10c-e-final2/e4/outlands/results.json`.

E5 passed the deterministic real whole-10C matrix. It loaded `neulev`, all
seven Neutral resources, Dispensary with every parent N2..N6 (depths 34..38
in this run), and all 17 Lost/R'lyeh resources, including both variants of
all four alternate groups. The matrix reported
`resources=26`, `dispensary_parents=5`, `alternate_groups=4`,
`lost_variants=8`, and topology hash `13891672719831820846`.

E6 passed the real production dungeon and special-chain save/restore codec
round trip using an external save file. It restored the topology hash
`8522382402598382183`, Dispensary parent N2, Lethe state, auxiliary ESHK/EPRI
state, and Silver Key destination validity, then revisited Lethe-E, Lethe-Z,
and Lbyrnth. The initial fixture-only failure used an output path whose parent
directory had not yet been created; `run_exe()` now creates that external
parent before launching. A first standalone fixture also used a stack
`NHFILE` with `close_nhfile()`; it was corrected to the existing heap-backed
allocator before rerun. No production code defect or production-state change
resulted.

E7 passed the final ordinary x64 Release build and established package target,
then loaded all 26 resources from the extracted ordinary package. The final
ordinary artifacts after the last no-hook rebuild are:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: `6020096` bytes,
  SHA-256 `CE2A42BDE23F7676FC5C5035D9C3242402705C1955CAF22394A59B3208FC108F`.
- `E:/Codex/My_Nethack/binary/Release/x64/NetHackW.exe`: `7340544` bytes,
  SHA-256 `89A2742F72A23B37F0727BA74CFE86968654AF890E9913B40F11234D479B7179`.
- `E:/Codex/My_Nethack/binary/Release/x64/nhdat500`: `1775240` bytes,
  SHA-256 `EC5448960207CBF9FBD8C0A0C95BF16DDCFC8DE23CC4DDDD088FBD9E03B3AE03`.
- `E:/Codex/My_Nethack/vspackage/nethack-500-win-x64.zip`: `5230714` bytes,
  SHA-256 `B62EB2A192496F9355C884134CC51A82A0C046FB17DF216E5D74F37FBC616C95`.

The package is produced by the existing MSBuild post-build target, whose
observed archive step is the standard `tar -a -cf ...nethack-500-win-x64.zip`
over `Guidebook.txt`, `license`, `NetHack.exe`, `NetHack.txt`, `NetHackW.exe`,
`opthelp`, `nhdat500`, `record`, and the templates. The final ordinary build
has no C-D/E test hooks; the separate diagnostic copy used for native topology
and matrix checks is external.

E8 passed the Step 7 scheduling/Lost Tomb suite, Step 8 Moria suite, Step 9
Sheol suite, Step 9 Dragon Caves suite, Step 9 Mithardir foundation suite,
eight native Castle/Medusa cross-phase topology samples, and eight packaged
topology samples. The packaged topology harness required a test-only startup
drain for the existing WinPTY welcome-page `--More--` race; after that narrow
harness correction, all eight samples passed with one Lost Tomb, Temple, and
Moria, Castle at DL200, distinct randomized parents, and collision-free
Moria/Sheol/Dragon/Mithardir topology.

The only observed interactive harness limitations are the existing
`#WIZLOADDES`/prompt race in the legacy Step 9C shop/portal fixture and the
separate existing B3-3 WinPTY interactive stall. Native, non-PTY, package,
and hardened topology evidence passed; these are not production defects.
Win32 optional diagnostics were not run because no ABI width, serialization
layout, persistent field, or Windows-specific production mechanism changed.
No stale production fixture corrections were required. No new monster,
object, artifact, terrain, trap, dungeon ID, or persistent field was added;
`EDITLEVEL` remains 5. The remaining work is the explicitly manual Step 10D
playtest ownership; Step 10D has not started.

The final Step 10C-E checkpoint is external and contains the binary tracked
patch, complete and intentional untracked lists, repository-relative copies,
manifest, and per-file SHA-256 verification. No file was staged, committed,
tagged, pushed, or written to README.

STEP 10B COMPLETE: YES
STEP 10C-A COMPLETE: YES
STEP 10C-B COMPLETE: YES
STEP 10C-C COMPLETE: YES
STEP 10C-D COMPLETE: YES
STEP 10C-E COMPLETE: YES
STEP 10C COMPLETE: YES
READY FOR STEP 10D: YES

## Step 10C-B production topology and persistent randomized scheduler

Step 10C-B is complete. Production topology now registers exactly two new
dungeon records, `Neutral Quest` and `The Lost Cities`, for an observed total
of **17/18** records. The Dispensary is `lbyrnth` at the eighth physical
Neutral slot with a persistent same-`dnum` `BR_STAIR` attachment; it is not a
third dungeon. The exact-count parser guard now accepts `n_dgns ==
MAXDUNGEON` and rejects only values above 18.

The DoD owns a DL30..199 portal branch to Gate Town and the special level
`neulev` at the same selected parent P. `neulev` retains its ordinary up and
down stairs, so P-1 -> P -> P+1 remains the main DoD route while its separate
branch portal enters Neutral. The normal downward route from P=199 therefore
continues to Castle at DoD DL200. Gate Town, Outlands 1-4, and Spire use the
audited bidirectional portal chain; Spire descends normally to Sum. Neutral
has seven logical floors N1..N7. The Dispensary parent is selected once from
N2..N6, and the same-dungeon branch stair returns through its saved
`stairway.tolev` endpoint.

Sum connects downward to Lost Cities entry level 2, `lethe-b`; LC1 is the
selected `leth-a` alternate above that entry. LC3 and LC4 select one concrete
`leth-c` and `leth-d` variant respectively, LC5..LC8 are `lethe-e`,
`lethe-f`, `lethe-g`, and `lethe-z`, LC9 selects one `nkai-a` variant, and
LC10..LC13 are `nkai-b`, `nkai-c`, `nkai-z`, and `rlyeh`. LC8's holes enter
LC9; LC9..LC12 retain upward return stairs and downward holes; R'lyeh retains
its upward endpoint. Inactive donor branch markers on LC8/LC9/LC11 remain
unowned residue and create no branch records. All 26 audited map resources
load from the package with their 10C-A connector contracts intact.

Display depth is derived from the persisted topology: Neutral N is `P+N-1`,
Lost Cities L is `P+L+5`, and a Dispensary attached to N is `P+N`. The
deterministic B1 fixture exercises P=30, P=100, and P=199 directly. At P=199,
LC13 displays at the maximum audited depth **217**, while Castle remains DoD
DL200 and Medusa's protected special-level structure is unchanged.

### Scheduler and persistence

`src/dungeon.c:step6b_schedule()` owns the production choice. Neutral is
classified by the generalized scheduled-branch/approach predicates and uses
the existing Steps 6-9 `used` array plus `step6b_depth_used()` collision
ledger. Every established Step 6-9 reservation keeps precedence; only after
Lost Tomb is reserved does Neutral claim one remaining ordinary DoD parent in
DL30..199. There is no fixed DL111 or other fallback. Four native `rn2(2)`
choices replace the provisional Lost Cities `-1` prototype names with their
concrete `-2` names when selected, and native `rn2(5)+2` chooses the
Dispensary parent. No new Lua scheduling RNG or persistent field was added.

`test/run_step10c_b_scheduler.py` extracts the final production function
bodies and ran exactly 1,000 consecutive deterministic seeds twice from
equivalent fresh state. Every seed passed the full shared-ledger, existing
Step 6 room-count, Step 9 parent, Neutral, alternate, Dispensary, Castle, and
depth assertions, and both runs produced identical saved-topology state
hashes. The final diagnostic file is
`E:/Codex/My_Nethack/_qa/step10b-tests/x64/step10c-b-1000-reviewed-final/frequency-distribution.json`.
It observed 169 legal parent depths, including P=30 eight times, P=100 seven
times, and P=199 five times. Concrete variant-2 counts were 749 (`leth-a`),
751 (`leth-c`), 251 (`leth-d`), and 249 (`nkai-a`). Dispensary N counts were
N2=214, N3=196, N4=208, N5=206, and N6=176. These are diagnostics only; no
uniformity threshold is a gate.

The compile-gated native topology target then ran 64 real
`init_dungeons()` initializations and observed 17 dungeons, seven logical
Neutral floors, thirteen Lost Cities floors, all four alternate groups, one
same-dnum Dispensary, distinct DL30..199 scheduled parents, Castle DL200, and
zero Step 9/Neutral collisions. Its final results at
`E:/Codex/My_Nethack/_qa/step10b-tests/x64/step10c-b-native-reviewed-final/results.json`
include actual P=30 and P=199 samples and actual maximum depth 217.

Topology persistence uses the existing saved `dungeon`, `branch`, and
`s_level` structures. The native compatibility fixture round-tripped P,
Neutral/Lost depth bases, the DoD-Neutral, Neutral-Lost, and same-dnum
Dispensary branches, `neulev`, and all four selected alternate names through
the production historical codecs, then verified the normal `save_dungeon`,
`restore_dungeon`, `savelevchn`, and `restlevchn` routing. Revisit cannot
rerun `step6b_schedule()` because it remains new-game initialization only.
The existing recovery-file suite also passed internal and standalone
recovery through ledger 3199 plus invalid-bound and version rejection. No
save width changed and `EDITLEVEL` remains 5.

### Regression, package, and closeout evidence

The post-review scheduler gate passed 1,000 seeds, and the affected Step 7
fixture passed 2,000 production scheduler samples with all established Step
6/9 counts and collision/depth semantics. The authoritative Step 10B5 x64
master passed all eleven categories after phase-stale topology projections
were narrowed to their enduring historical boundaries. Its one initial
random shop-position miss passed on the runner's existing retry; no production
change was made for it. The Step 10C-A source and packaged runtime sweep at
`E:/Codex/My_Nethack/_qa/step10c-a-tests/x64/runtime-step10c-b-final` loaded all 26 resources,
including both variants in every pair, R'lyeh, and `lbyrnth`.

Generated validation reports `NUMMONS=503`, `NUM_OBJECTS=547`, artifact
sentinel/count `47/46`, `MAX_GLYPH=12410`, and 2,867 tiles with a complete
640x1574 8-bpp bitmap. No monster, object, artifact, terrain, or trap ID was
added by 10C-B. The mandatory final command
`MSBuild.exe sys\windows\vs\NetHack.sln /p:Configuration=Release
/p:Platform=x64 /v:m` returned exit 0 after the compile-gated probe was
removed by an ordinary full rebuild. Final artifacts are:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 5,998,080 bytes,
  timestamp `2026-09-12T12:27:52.5944278+03:00`, SHA-256
  `892A895CC8E927B3656C724BAED4CEA9EA031E3D9E917C1ED220B0F68B9851B7`.
- `E:/Codex/My_Nethack/binary/Release/x64/NetHackW.exe`: 7,318,528 bytes,
  SHA-256 `88AB0BB2FE561C932E8BC7AA74F66F7BB4B78BA4097B42684D11AC2D6E98927B`.
- `E:/Codex/My_Nethack/binary/Release/x64/nhdat500`: 1,776,858 bytes,
  SHA-256 `2ABA77392FF7772ECBD40F7467E47B1EE3186040E7D827181F2879FA88830175`.
- `E:/Codex/My_Nethack/vspackage/nethack-500-win-x64.zip`: 5,216,635 bytes,
  SHA-256 `C40E91FC0503C88E8514ABD3F00F4D951A3802E9F4206B4DD02AAEC7292DF90F`.

Win32 was not run because 10C-B adds no serialized field or ABI-width change;
its optional diagnostic status is `NOT RUN`. README is unchanged. No 10C-C
Outlands generator, 10C-D map identity/mechanics activation, 10C-E work, or
Step 10D work was started. The fresh external safety record is
`E:/Codex/My_Nethack/_qa/step10c-b-checkpoint-20260912-123003/` and contains the binary
tracked patch, complete and intentional untracked lists, repository-relative
copies, manifest, and SHA-256 verification. Nothing is staged, committed,
tagged, or pushed; HEAD and origin remain `48fe150af4a087fd2f4ff576b43c1c96cc08c6fe`.

Remaining Step 10C-C ownership is limited to the eleven audited Outlands
procedural generator functions and their real dispatch. Step 10C-D continues
to own real B4 context/mechanics activation, final Lethe activation,
Plumach/bridge-priest designation, Silver Key positive travel, and the Sword
of the Deeps map instance. Those boundaries remain untouched.

STEP 10B COMPLETE: YES
STEP 10C-A COMPLETE: YES
STEP 10C-B COMPLETE: YES
STEP 10C COMPLETE: NO
READY FOR STEP 10C-C: YES
READY FOR STEP 10C-D: NO
READY FOR STEP 10D: NO

## Step 10C-C Outlands procedural generation subsystem

Recorded 2026-09-12. Step 10C-C is complete. The four production Outlands
resources (`out1` through `out4`) now receive the audited procedural feature
pass exactly once after their Lua special level has loaded. This checkpoint
does not alter the 10C-B topology or scheduler and does not activate any
10C-D identity-dependent mechanics.

### Donor audit and local ownership

The pinned donor is `E:/Codex/My_Nethack/_qa/dnethack-donor-pinned` at commit
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`. Symbol search located all eleven
audited definitions in `dnethack-3.4.3/src/mkroom.c`: `mkkamereltowers` at
line 1324, `mkminorspire` at 1746, `mkfishingvillage` at 1902, `mkwell` at
2081, `mkpluhomestead` at 2148, `mkpluvillage` at 2529, `mkferrutower` at
3283, `mkinvertzigg` at 3359, `place_neutral_features` at 3944,
`mkneuriver` at 5082, and `neuliquify` at 5215. The required private helper
`mkfishinghut` is at line 1971. The donor invokes the pass from
`dnethack-3.4.3/src/mkmaze.c` at line 591. `mkferrufort` remains deliberately
excluded because its donor dispatch is commented out.

Local production ownership follows that existing pattern. The eleven
functions and their minimum placement helpers live in the already-linked
`src/mkroom.c`; `include/extern.h` exposes only `place_neutral_features`.
`src/mkmaze.c::makemaz` identifies the resolved special-level filename after
successful `load_special()` and calls the dispatcher only for `out1.lua`,
`out2.lua`, `out3.lua`, or `out4.lua`, before the existing `dmonsfree()` and
return. This ENGINE-DRIVEN post-load bridge needs no Lua binding and cannot
duplicate the call. No new production compilation unit or build registration
was needed. The Visual Studio project changes are compile-gated test
registration only.

The local subsystem implements `place_neutral_features`, `mkkamereltowers`,
`mkminorspire`, `mkfishingvillage`, `mkwell`, `mkpluhomestead`,
`mkpluvillage`, `mkferrutower`, `mkinvertzigg`, `mkneuriver`, and
`neuliquify`. It retains the donor feature families, geometry and placement
roles: shallow-water Kamerel tower complexes, minor mirror spires, fishing
shore/huts/well, Plumach homesteads and village buildings, Ferrumach barracks,
the concentric inverted ziggurat, and meandering river liquification. Bounds,
terrain suitability, traps, rooms, existing monsters, and prior procedural
features participate in placement rejection or relocation. Failed bounded
searches leave the map unchanged instead of forcing unsafe placement.

### Dispatch, compatibility, and route safety

The production dispatcher preserves donor control flow and RNG-call order:
`!rn2(30)` Kamerel towers with its nested `!rn2(16)` fishing attempt; otherwise
`!rn2(16)` minor spire; otherwise `!rn2(16)` fishing village. It then evaluates
independent checks in this exact order: `!rn2(8)` river, `!rn2(8)` Plumach
village, `!rn2(16)` inverted ziggurat, `!rn2(8)` Ferrumach tower, followed by
the donor `!rn2(3)` homestead gate and exactly `rnd(4) + rn2(4)` attempts.
The mutually exclusive checks were not flattened and no equivalent-looking
RNG expression replaced a donor expression.

Current Step 10B identities are used directly. The Kamerel serpent-statue
pool retains every locally registered donor species; the absent donor Lillend
has no invented fallback and is omitted explicitly. The current object API
uses `obranch_size`/`obranch_material` for imported size/material metadata,
and current room APIs use `stock_room` and `fill_zoo` where the donor used its
older generic room fill path. A trap cell is rejected where the donor could
relocate it because this tree has no `rloc_trap` helper. Generic village shops
remain generic: no Plumach shopkeeper designation or other 10C-D context was
introduced.

Before feature placement, a local generation-only guard resolves both actual
magic-portal cells, protects every portal/stair/ladder cell, and computes a
four-way ordinary trail between the two portals. The mines-style Outlands
base can place portals in grass components separated by tree belts, so the
minimal route may convert tree/puddle cells along that trail to lit grass.
The exact resulting `struct rm` cells are snapshotted and restored after all
independent features. Every terrain helper also rejects protected cells and
traps. Thus river liquification and later wallification cannot overwrite a
connector or the required non-swimming route; this state is static generation
scratch only and adds no save field.

### Focused, stress, regression, and package evidence

`test/test_step10c_c_source.py` proves the eleven definitions, explicit
`mkferrufort` exclusion, four-name-only post-load bridge, helper call graph,
and the exact ordered dispatcher tokens. The compile-gated native fixture
`test/test_step10c_c_runtime.c` initializes real object, dungeon, artifact,
Lua, level, and vision state, calls the production `makemaz()` path, validates
terrain and monster/object IDs, resolves exactly two portals, and performs a
four-way BFS with current `ACCESSIBLE` semantics while rejecting pool, water,
and lava traversal. Its deterministic seed is installed before randomized
game-data initialization and before every generated level; no deterministic
scaffold is present in an ordinary build.

`test/run_step10c_c_focused.py` replayed isolated fixed seeds twice and passed
Kamerel, spire, fishing/well, river/neuliquify, Plumach village and homestead,
ziggurat, Ferrumach, no-feature, and multi-feature real-dispatch paths. The
post-review stress record at
`E:/Codex/My_Nethack/_qa/step10c-c-tests/runtime-reviewed/results.json` generated exactly
1,000 instances (250 each of out1/out2/out3/out4). Every instance completed,
retained two valid portals and their baseline BFS route, and contained only
valid terrain, object, and monster references. Diagnostic observations were:
Kamerel towers 37, minor spires 62, fishing villages 53, wells 53, rivers 141,
Plumach villages 117, inverted ziggurats 59, Ferrumach towers 147, and
homestead attempts/successes 1,275/1,275. Frequencies are diagnostic only;
the source and fixed-seed gates prove ordering.

The current 10C-B source and 1,000-seed scheduler/idempotence/persistence
gates pass. The corrected historical Step 7 fixture now supplies the four
production Lost Cities alternate records and passed 2,000 scheduler samples,
167 distinct Moria depths, all Step 6/9 collision/depth assertions, Castle at
DL200, and preserved Medusa structure. Native save compatibility, all eight
affected Step 9C suites, the 10C-A source gate, and generated identity/glyph/
tile validation pass. The final 10C-A packaged sweep at
`E:/Codex/My_Nethack/_qa/step10c-c-tests/step10c-a-package-final` loaded all 26 resources,
including generator-enabled out1 through out4, through the ordinary rebuilt
production executable. The attempted whole-Step-10B5 master passed all reached
source/native/package subgates through B3-2, then repeated the documented
legacy B3-3 WinPTY harness stall with no production diagnostic; the prior
authoritative master remains green and the current non-PTY components were
run directly.

The mandatory ordinary x64 Release solution build returned exit 0 and rebuilt
the production package. Final artifacts before documentation-only closeout
are:

- `E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`: 6,017,024 bytes,
  timestamp `2026-09-12T14:00:58.3192619+03:00`, SHA-256
  `BF85E931E2589E358F770D6E0A732B3998C1A7E572F7DAA6C5AD17128336C319`.
- `E:/Codex/My_Nethack/binary/Release/x64/NetHackW.exe`: 7,336,960 bytes,
  SHA-256 `8707E4DA621D33227A9A1654B5E740780D5FDBEE28DC8BDF19F5A56DDD96190E`.
- `E:/Codex/My_Nethack/binary/Release/x64/nhdat500`: 1,777,006 bytes,
  SHA-256 `55D0996F33CCA2DCCFB0ACB8C51B278BA05D001210E8204A723081F30911ED33`.
- `E:/Codex/My_Nethack/vspackage/nethack-500-win-x64.zip`: 5,229,015 bytes,
  SHA-256 `7F31D2AA87273AAA3654A0DA9AE64F656C175A4E7CD7505CA3154EA17D4EB2C6`.

Win32 remains optional and was not run; no ABI, serialization width, or new
persistent state was introduced. `EDITLEVEL` remains 5. No monster, object,
artifact, terrain, or trap ID was added. README is unchanged.

The final external recovery record is
`E:/Codex/My_Nethack/_qa/step10c-c-checkpoint-20260912-142452/`. It contains the binary
tracked patch, complete and intentional untracked lists, repository-relative
copies, manifest, and per-file SHA-256 verification. No file was staged to
create the checkpoint.

Step 10C-D still owns real B4 context/mechanics activation, final Lethe level
designation/wiring, Plumach shopkeeper designation, bridge-priest designation,
Silver Key positive travel, and the Sword of the Deeps final instance. None
of that work has started. Step 10C-E and Step 10D also remain untouched.

STEP 10B COMPLETE: YES
STEP 10C-A COMPLETE: YES
STEP 10C-B COMPLETE: YES
STEP 10C-C COMPLETE: YES
STEP 10C COMPLETE: NO
READY FOR STEP 10C-D: YES
READY FOR STEP 10C-E: NO
READY FOR STEP 10D: NO

## Step 10C-D — real identity and content wiring

Step 10C-D activates the completed B3/B4 mechanics using one authoritative
native resolver, `step10c_level_context()`. It obtains the current registered
`neulev` `d_level` from `find_level("neulev")` and resolves the Neutral Quest
and Lost Cities `dnum` values from their registered dungeon names. It does not
use display depth, ledger number, a fixed DL111, or a fixed numeric dungeon ID.
The verified relative mapping is Neutral 1 Gate, 2..5 out1..out4, 6 Spire,
7 Sum, and same-dnum floor 8 Dispensary; Lost Cities 1..12 use the Lost Cities
context and 13 uses R'lyeh. The scheduled DoD Approach is an exact registered
special-level identity, so the mapping follows every P in DL30..199 and the
native saved topology on revisit/restore. Ordinary levels return `NONE`.

All formerly dormant production B4 call sites now pass that resolver result:
hero and monster spell environments, Outlands tree kicking/cutting,
mirror-shard pits, projectile materials, terrain colors, and Gate pet
separation. The existing formulas and helper implementations are unchanged.
Focused native checks cover Gate, every Outlands floor, Spire, Sum, every Lost
Cities floor, R'lyeh, the Approach, and ordinary-level isolation.

The exact Lethe set is every Lost Cities logical floor (LC1..LC13), including
all scheduled concrete variants: `leth-a-1`, `leth-a-2`, `lethe-b`,
`leth-c-1`, `leth-c-2`, `leth-d-1`, `leth-d-2`, `lethe-e`, `lethe-f`,
`lethe-g`, `lethe-z`, `nkai-a-1`, `nkai-a-2`, `nkai-b`, `nkai-c`, `nkai-z`,
and `rlyeh`. `step10c_set_level_flags()` runs immediately after native level
state reset and sets only `svl.level.flags.lethe`; the existing level codec
owns persistence. Neutral, Approach, Dispensary, and ordinary water remain
non-Lethe. Existing B4 descriptions, immersion/amnesia, object rewriting,
artifact protection, teleport landing, and `spoteffects` remain the mechanics
owners.

Auxiliary content is wired at its audited creation points. Procedural
Outlands shops call the existing Plumach designator immediately after
`stock_room()`, after valid ESHK/shop ownership exists. The `lethe-e` native
temple priest is designated in the post-special-load hook after EPRI creation.
That same hook marks actual Step 10 `MAGIC_PORTAL` traps seen using their native
`tseen` field. Native generation verified Plumach+ESHK, the hostile
blasphemous-lurker bridge priest+EPRI, and seen portals on the Approach, Gate,
and two-portal Outlands maps; existing ESHK/EPRI/trap codecs own lifecycle.

Silver Key production travel now constructs four real candidates: registered
`neulev`, Neutral floor 1, the registered Lost Cities entry floor (currently
relative floor 2), and Neutral floor 8 Dispensary. A destination is offered
only when it exists, is not current, has native `VISITED` progression state,
and passes the existing B3 validator. Invocation/endgame state, carrying the
Amulet, lack of safe adjacency, unrelated domains, arbitrary depths, invalid
selection, and unavailable/unvisited targets fail safely. Selection uses the
native `NHW_MENU`/`start_menu`/`add_menu`/`select_menu` path, revalidates via
`silver_key_choose_destination()`, and travels with `goto_level()`; cancellation
keeps the original turn/no-Pw/no-cooldown behavior.

This integration reproduced one B3 descriptor defect: B3 modeled Dispensary as
a separate dungeon number and required all four domains to have distinct
`dnum` values, while 10C-B intentionally represents it as Neutral Quest floor
8. The narrow fix changes only that descriptor member to an exact `d_level`,
allows it to share Neutral's dnum, tests it before the Neutral 1..7 range, and
retains every other malformed-domain and arbitrary-depth rejection. No B3
mechanic or unrestricted teleport path was added.

`lethe-z` now owns exactly one `LONG_SWORD` at its audited map-relative
coordinate `{20,7}` with cursed BUC, +12 enchantment, exact name `The Sword of
the Deeps`, and `OBP_DEEP`. The existing Lua object parser gained only the
matching `deep=true` metadata input; native object naming/property/save codecs
own the instance. `leth-c-1` likewise uses the supported monster `appear_as`
field for the audited staircase-down giant mimic. All `DEFERRED_10C_D` resource
markers are resolved; no new ID, artifact, save field, or gameplay framework
was introduced.

Verification evidence: the focused source and compile-gated native C-D tests
pass; the native probe generates the real maps and checks all 13 Lethe
identities, real seen portals, actual ESHK/EPRI designations, and the exact
Sword state. Corrected post-phase B3/B4/B5 source expectations pass, the B3
validator and B4 deterministic state gates pass, the 1,000-seed C-B scheduler
passes with 169 observed parent depths, and the Step 6/9 production scheduler
gate passes 2,000 samples with 167 observed Moria depths. The complete C-A
source/package runtime loaded all 26 resources. The C-C focused dispatch and
1,000-instance stress gates passed with 250 generations of each Outlands
floor: Kamerel 36, minor spire 62, fishing 56, wells 56, rivers 141, Plumach
villages 120, ziggurats 63, Ferrumach towers 153, and 1266/1266 successful
homestead attempts.

The full non-PTY B1-B5 evidence passed: source boundaries, append-only
compatibility, B2/B3/B4 focused C tests, topology and object/level save codecs,
recovery, generated enums/glyphs/tiles, depth, and materially affected Step 9C
foundation, armor-size, coatings, defense, weapon damage/selection,
handedness, and spells. Packaged B3 artifact/Necronomicon/Silver passive and
save/restore fixtures also passed. The legacy WinPTY Step 9C shop/portal
fixture failed its initial `#wizloaddes` state assertion on both attempts and
left a paniclog, the documented prompt-race harness failure; PTY behavior is
explicitly not a 10C-D completion gate. The equivalent non-PTY native C-D map
probe validates real portal and ESHK/EPRI state.

The final full x64 Release solution and package build exited 0. The production
`binary/Release/x64/NetHack.exe` is 6,020,096 bytes, timestamp
`2026-09-12T12:56:16.8318794Z`, SHA-256
`83EBF209C9B2BEF15205F10D6CE08A6B539A909E642B07B0786840CCF15F22B3`.
`NetHackW.exe` is 7,340,544 bytes with SHA-256
`07CD721CE9CD21E77A9F8FFFE498F93FFADBF377FBC13B85192EBBAD88CFB69C`;
`nhdat500` is 1,775,240 bytes with SHA-256
`EC5448960207CBF9FBD8C0A0C95BF16DDCFC8DE23CC4DDDD088FBD9E03B3AE03`;
the 5,230,717-byte x64 zip has SHA-256
`DA66F6F243993D0E5F9F95BF83A920B92E92A569B55E9E8947499624923B1AC7`.
The final production package resource sweep also loaded all 26 resources.
Win32 was not run because C-D added no persistent field, ABI-width change, or
Windows-specific mechanism.

The external recovery record is
`E:/Codex/My_Nethack/_qa/step10c-d-checkpoint-20260912-155807/`, containing a binary tracked
patch, complete/intentional untracked lists, repository-relative copies,
manifest, and per-file SHA-256 verification. No file was staged for backup.

STEP 10B COMPLETE: YES
STEP 10C-A COMPLETE: YES
STEP 10C-B COMPLETE: YES
STEP 10C-C COMPLETE: YES
STEP 10C-D COMPLETE: YES
STEP 10C COMPLETE: NO
READY FOR STEP 10C-E: YES
READY FOR STEP 10D: NO

## Step 10C-E — final authoritative addendum

This addendum supersedes the provisional status blocks above. The complete
closeout evidence is external at
`E:/Codex/My_Nethack/_qa/step10c-tests/x64/step10c-e-final2/` and the runner is
`E:/Codex/My_Nethack/test/run_step10c_e.py`. E1 passed the source, structure,
cross-phase, and generated-data gates (`503/547/47`, `MAX_GLYPH=12410`, 2867
tiles, 640x1574 8-bpp bitmap). E2 passed the current Step 10 regression set:
non-PTY B1-B5 components, 10C-A package, 10C-B scheduler, 10C-C focused
native, and 10C-D native identity. The existing B3-3 interactive WinPTY child
was excluded after its reproduced idle stall; its core and packaged artifact
gates passed.

E3 passed 1,000 exact scheduler seeds, 169 parent depths (DL30..DL199 except
DL196), zero illegal parents, zero collisions, and Dispensary parents N2..N6
with frequencies 214/196/208/206/176. E4 passed 1,000 Outlands instances
(250 per floor), with totals Kamerel 36, spire 62, fishing 56, well 56, river
141, village 120, ziggurat 63, Ferrumach 153, and homestead
1266/1266. E5 passed all 26 real resources, all five Dispensary parents, both
variants of all four alternate groups, and the real identity matrix. E6
passed the real dungeon/special-chain codec round trip, restoring topology
hash `8522382402598382183`, Dispensary N2, Lethe, ESHK/EPRI auxiliaries, and
Silver Key validity. Its external save was 6044 bytes.

E7 passed the final ordinary no-hook x64 Release build/package and all 26
resource loads from the extracted package. Final artifacts are:

- `binary/Release/x64/NetHack.exe`: 6020096 bytes,
  `CE2A42BDE23F7676FC5C5035D9C3242402705C1955CAF22394A59B3208FC108F`.
- `binary/Release/x64/NetHackW.exe`: 7340544 bytes,
  `89A2742F72A23B37F0727BA74CFE86968654AF890E9913B40F11234D479B7179`.
- `binary/Release/x64/nhdat500`: 1775240 bytes,
  `EC5448960207CBF9FBD8C0A0C95BF16DDCFC8DE23CC4DDDD088FBD9E03B3AE03`.
- `vspackage/nethack-500-win-x64.zip`: 5230714 bytes,
  `B62EB2A192496F9355C884134CC51A82A0C046FB17DF216E5D74F37FBC616C95`.

E8 passed Step 6/7, Step 8 Moria, Step 9 Sheol, Dragon Caves, Mithardir,
eight native topology samples, and eight packaged topology samples. The
packaged topology harness required only a test-only startup `--More--` drain
for the existing WinPTY prompt race. No production defect or stale fixture
correction remained. Win32 optional diagnostics were not run; no ABI width,
serialization layout, persistent field, or Windows-specific production
mechanism changed. README, commit/tag/push state, and Step 10D remain untouched.

Final exact closeout lines:

STEP 10B COMPLETE: YES
STEP 10C-A COMPLETE: YES
STEP 10C-B COMPLETE: YES
STEP 10C-C COMPLETE: YES
STEP 10C-D COMPLETE: YES
STEP 10C-E COMPLETE: YES
STEP 10C COMPLETE: YES
READY FOR STEP 10D: YES

## Step 10D — manual-validation handoff preparation

Recorded 2026-09-12. The user-executable handoff is
`E:/Codex/My_Nethack/doc/step10-playtest.md`. It contains table-first pending
checklists for randomized topology, complete forward/reverse traversal, all
26 resources and all four alternate groups, Outlands visuals, live Lethe and
context mechanics, shops/temples/NPC roles, bosses/rewards, representative
save/reload/revisit, the maximum-parent case, TTY/tiles, defects, and user
sign-off. Every user-owned validation status remains `PENDING`; this addendum
does not close Step 10D or Step 10.

The focused pre-manual automated gate passed without a production rebuild or
production-code change. The frozen ordinary x64 executable remains 6,020,096
bytes with SHA-256
`CE2A42BDE23F7676FC5C5035D9C3242402705C1955CAF22394A59B3208FC108F`;
the x64 package remains 5,230,714 bytes with SHA-256
`B62EB2A192496F9355C884134CC51A82A0C046FB17DF216E5D74F37FBC616C95`.
Generated-data validation again reported `NUMMONS=503`, `NUM_OBJECTS=547`,
artifact sentinel 47, `MAX_GLYPH=12410`, and 2,867 tiles. The accepted native
non-PTY matrix freshly loaded all 26 real resources, every Dispensary parent,
and both members of all four alternate groups. Its fresh persistence smoke
restored topology, Lethe, auxiliary ESHK/EPRI state, and Silver Key validity.
The extracted scheduler harness again passed 1,000 deterministic inputs,
including representative P=199/depth-217 and Castle-DL200 assertions. Current
production `dat/dungeon.lua` and `src/dungeon.c` contain no fixed `111`.
Evidence is under
`E:/Codex/My_Nethack/_qa/step10d-tests/pre-manual-20260912-192819/`; the 6,081-byte native
persistence artifact has SHA-256
`C57706220F243BC7859B7EE5B845512A2859D07AA342B555E56857796566C1AD`
and is explicitly not a playable save fixture.

Wizard instructions were verified from the current `src/cmd.c` command table,
`src/wizcmds.c` implementations, `src/teleport.c` level-target parser, and
`src/wizard.c`. The supported commands documented in the handoff are
`#wizwhere`; `#wizlevelport`/Ctrl-V with number, name, or `?` menu;
`#wizgenesis`/Ctrl-G; `#wizwish`/Ctrl-W; `#wizmap`/Ctrl-F; `#wizmakemap`;
`#wizloaddes`; and `#wizidentify`/Ctrl-I. The verified Windows wizard launch
is `NetHack.exe -D -u wizard` (or `NetHackW.exe -D -u wizard` for tiles).
Production exposes no RNG seed CLI.

The live `--showpaths` probe reports the active save and level directory as
`C:/Users/yariv/AppData/Local/NetHack/5.0/`. Windows constructs the wizard save
as `wizard.NetHack-saved-game`; data is read from the executable directory.
The handoff includes copy-before-overwrite backup commands and a safe
playtest-save/archive/restore sequence. No external playable fixture was
created.

The existing scheduler batch identifies input seed `65` as its first P=199
case:
`SEED|65|P=199|A=1|C=1|D=0|N=0|DISP=5`. That fixture input drives the
harness LCG and cannot be supplied to the production ISAAC RNG. Because the
production executable has no seed injection and the existing E6 file is an
internal codec artifact rather than a complete player save, manufacturing a
playable P=199 save would require an unauthorized test-hook build or save
surgery. The guide therefore records the exact automated reproduction and
leaves the P=199 live rows pending until a naturally encountered or separately
authorized safe production fixture exists; production is not brute-forced or
modified.

Win32 remains optional and was not run. README is unchanged. The index remains
empty; nothing was staged, committed, tagged, or pushed. The external Step 10D
handoff checkpoint is
`E:/Codex/My_Nethack/_qa/step10d-handoff-checkpoint-20260912-194025/`.

STEP 10B COMPLETE: YES
STEP 10C COMPLETE: YES
STEP 10D HANDOFF PREPARED: YES
STEP 10D MANUAL VALIDATION: PENDING
STEP 10D COMPLETE: NO
READY FOR USER MANUAL PLAYTEST: YES
READY FOR FINAL STEP 10 CLOSEOUT: NO

## Step 10QA2 — remediation, regression hardening, and generated-content validation

Recorded 2026-09-13. QA1 and QA1-1 identified ten confirmed defects: eight
SR-01 special-room fill defects and two EP-01 explicit-payload conversion
defects. The QA1-1 classification is authoritative: 41 donor-normal
fill-capable concrete regions comprise exactly 36 defective `DEFAULT0`
regions, five explicit-payload temple cases, and eight intentional-unfilled
concrete regions (seven logical cases). The pinned donor was
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`.

QA2 repaired only the 36 required normal-fill regions: 11 in `neulev`, 9 in
`gatetwn`, 3 in `leth-d-1`, 4 in `leth-d-2`, 2 in `lethe-z`, and 7 in
`rlyeh`, using explicit local `filled=1` intent. The five explicit-payload
temples and all eight intentional-unfilled regions were preserved. No global
`des.region` behavior was changed. The two N'Kai-B create-monster scrolls now
use numeric `spe=0`; the five Lethe-Z corpse records now use the current
parser's `montype` field, with the neutral priest alias `aligned cleric` and
`spe=0`, proving actual corpse identity rather than a display name.

Fresh generated-state validation passed for every affected resource:

- Gate Town: 8/8 functioning shops, 8 keepers, native stock/billing state,
  and beehive population.
- `neulev`: 9/9 barracks, 2/2 shops, 8 fixed Deep Ones, shriekers, land
  mines, and the intentional unfilled throne.
- `leth-d-1`: zoo and both courts; `leth-d-2`: swamp and all three morgues.
- `lethe-z`: barracks, morgue, five actual corpse species, and the Sword of
  the Deeps.
- `rlyeh`: 7/7 temples, `has_temple=1`, exactly two required explicit hostile
  unknown priests, zero unintended auto-priests, and Dagon/Hydra/Cthulhu.

Permanent QA2 coverage is provided by
`test/run_step10qa2.py`, `test/test_step10qa2_source.py`, and
`test/test_step10_room_semantics.c`. The latter freshly generates and
semantically inspects all 26 resources, including rooms, monsters, objects,
traps, terrain, fixed identities, and corrected payload metadata. The full
master run passed under
`E:/Codex/My_Nethack/_qa/step10qa2-evidence-20260913-102800/`: Step 10B/C, the scheduler
1,000-seed regression, persistence/restore/revisit/recovery, the protected
Steps 5–9 regressions, all 26 generated resources, and normalized parity.
Parity accounted for 1,197/1,197 explicit monster directives and 488/488
explicit object/container directives, with zero missing and zero NOT PROVEN.

The Outlands regression retains the 1,000 native instances (250 each of
`out1`–`out4`) and now checks generated semantic payload for all eleven
finalized functions. The refined run passed with 27,857 semantic monsters,
17,502 semantic objects, 30,932 river cells, and positive required payloads
for Kamerel, Spire, fishing/well, Plumach, ziggurat, Ferrumach, water,
terrain, doors, shops, barracks, courts, and rewards. `mkferrufort` remains
excluded.

The current x64 Release build explicitly rebuilt `dlb.vcxproj`, then rebuilt
the package from the resulting current `dat` resources. The final ordinary
artifacts are: `NetHack.exe` 6,020,096 bytes,
SHA-256 `5b3aa15d13497fabfa7449d06d6fa29633ecc25cf372b767f164df3f67e4a849`;
`nhdat500` 1,775,581 bytes,
SHA-256 `b2e2162c26b3b49a0ebc4ecf8118504747778bc4279b63ccfa11e5934fc414b1`;
and `nethack-500-win-x64.zip` 5,230,759 bytes,
SHA-256 `ec12ccae895695654cbac8e3381a0143433b0647e47ff9618ad083359bbda3b3`.
The final extracted package semantic gate freshly regenerated all 26
resources and passed the affected-room assertions.

No new serialized IDs were added. `EDITLEVEL` remains 5, `NUMMONS=503`,
`NUM_OBJECTS=547`, artifact sentinel/count is 47/46, `MAX_GLYPH=12410`,
total tiles are 2,867, `MAXDUNGEON=18`, actual dungeon records are 17, and
`MAXLINFO=3600`. The user save was not modified; no save existed at the
configured path during this isolated run. README remains unchanged, the
index is empty, and no commit, tag, or push was made.

EVERY FUTURE CONTENT ADDITION / DONOR PORT MUST RECEIVE ACTUAL GENERATED CONTENT VALIDATION, NOT MERELY BUILD/LOAD/TOPOLOGY VALIDATION.

Step 10D remains a separate manual phase. The corrected fresh-generation
state is ready for a new user playtest; Step 10D and final Step 10 closeout
remain pending.

## Step 10 portal-arrival hotfix

Recorded 2026-09-13. Root-cause tracing showed that `goto_level()` selected
the first destination-level magic portal for every portal arrival, while the
existing Mithardir-only exception matched a return destination. The pinned
donor at commit `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` confirms the
source-level return match before the first-portal fallback. The smallest
generic fix now applies that full `d_level` match to every portal arrival;
Lua portal destinations, coordinates, scheduler behavior, topology, IDs,
`EDITLEVEL`, and README are unchanged.

Permanent coverage is `test/run_step10_portal_arrival.py`, wired into
`test/run_step10qa2.py`. It traverses DoD `neulev` ↔ Gate Town and the full
Gate/out1/out2/out3/out4/Spire chain forward and reverse, asserts each
arrival point is the portal back to the source level, checks no immediate
retrigger, and performs a save/reload round trip. The pre-fix RED run failed
at Gate → out1 by returning to the DoD; the post-fix packaged run passed.

## Step 10QA3 combat-semantic audit and remediation

Recorded 2026-09-14. Step 10D manual playtesting reported this combat
sequence:

```text
The lurking one's tentacles suck your brain!
Gaze attack 6?
Program in disorder! (Saving and reloading may fix this problem.)
```

Source tracing proved that the failing entry was the lurking one's sixth
attack, `AT_GAZE + AD_ELEC + 4d8`. `include/monattk.h` defines `AT_GAZE` as
15 and `AD_ELEC` as 6. The local `src/mhitu.c:gazemu()` dispatch reached its
intentional default `impossible("Gaze attack %d?")` because it had no
`AD_ELEC` gaze branch. The pinned donor at revision
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` defines the same attack and
handles it in `dnethack-3.4.3/src/xhity.c:xgazey()`.

The bounded fix adds only the missing electric branch to the existing local
gaze architecture. It preserves the donor-relevant eye-contact and visibility
gate, 4/5 firing chance, cancellation reaction, declared `4d8` damage,
shock resistance, electrical inventory side effect, and local electrical
resistance/golem helpers. The preceding fifth attack remains
`AT_TENT + AD_PHYS + 4d8`; the existing `hitmsg()` text and
`mhitm_ad_phys()` path remain unchanged. The `impossible()` default remains
in place for genuinely unsupported gaze types.

The complete Step 10 inventory contains 73 monsters, 209 explicit attack
entries, and 68 distinct attack/damage combinations. Sixty-seven combinations
are supported; one (`AT_NONE + AD_UNKN`) is intentionally inert under the
finalized contract; unsupported and NOT PROVEN counts are both zero. Each
record includes the six-slot attack position, local source, pinned-donor
definition source where available, hero handler, monster handler, and
classification in `step10qa3-attack-inventory.json`. The audited flashing-lake
`AT_MAGC + AD_ELEC` path is supported by the existing ranged `buzzmu()` and
monster-target `mith_castmm()`/`mhitm_adtyping()` architecture; no additional
defect was found and no speculative cast branch was added.

Permanent coverage is `test/run_step10qa3.py`. Its isolated x64 runtime
fixtures passed for the non-resistant and shock-resistant lurking one, plus
non-resistant and resistant flashing-lake electrical combat. The corrected
fixtures contain the shocking gaze/electrical resistance messages and no
`Gaze attack ` or `Program in disorder!` diagnostics. The source contract
also proves that the preceding brain-sucking tentacle slot still resolves to
the existing physical-hit path. Evidence is retained under
`E:/Codex/step10qa3-evidence-20260914-qa3-final2/`, including the root-cause
trace, source contract, machine-readable inventory, and four runtime
transcripts.

The subsequent clean Step 10QA2 master run also passed all protected,
cross-phase, generated-data, package, runtime, persistence, topology, and
portal-arrival gates. Its checkpoint is
`E:/Codex/My_Nethack/_qa/step10qa2-evidence-20260914-qa3-final2/`. The final
x64 artifacts produced by that run are `NetHack.exe`
(`780FEB06463F87F28AB2AAEA1AF7F8A2F124D9082D4C62C696E0826C6913AD2D`) and
`nethack-500-win-x64.zip`
(`235A281BC7B3273200C058296B73B26C6FBBDB7F2F42B4226190846DCE9EB8A4`).

QA3 does not approve Step 10D. The user must perform a fresh focused lurking-
one combat retest before final Step 10 closeout.

## Step 10QA3-1 attack-handler audit hardening

Recorded 2026-09-15. The narrow follow-up replaced the former broad
`aatyp`-based classification with an explicit fail-closed matrix for every
current Step 10 `(aatyp, adtyp)` pair. Independent regeneration found 73
monsters, 209 explicit attack entries, and exactly 68 distinct pairs. The
matrix contains 68 unique entries, matches the inventory pair set, and
records separate hero-target and applicable monster-target dispatch,
handler, exact damage-type branch, default/unsupported-path review, and
runtime requirement/result. The result is 67 supported pairs, one explicitly
inert `AT_NONE + AD_UNKN` marker, zero unsupported pairs, and zero NOT PROVEN
pairs.

The audit found one additional unsupported pair: voice in the dark's
`AT_GAZE + AD_DRLI + 4d4` fell into `gazemu()`'s `Gaze attack %d?`
diagnostic. The pinned donor's `xhity.c:xgazey()` confirms the intended
life-drain gaze semantic. The bounded fix routes only this exact gaze case
through the existing `mhitm_ad_drli()` helper with gaze-specific life-force
messaging. A focused isolated runtime passed, and the existing lurking-one
non-resistant/resistant sequence plus flashing-lake control regressions also
passed unchanged.

Permanent coverage remains `test/run_step10qa3.py`. The machine-readable
matrix for the passing run is
`E:/Codex/My_Nethack/_qa/step10qa3-1-evidence-20260915-final2/step10qa3-attack-handler-matrix.json`;
the companion inventory, source contract, default-path audit, root-cause
trace, and isolated transcripts are in the same directory. README was not
modified.

## Final Step 10 closeout (authoritative)

Recorded 2026-09-15. This section is the authoritative current state for the
milestone; historical partial and pending checkpoint sections above are
retained as provenance.

Step 10D manual validation is **PASS**, based on the user's approval record in
`doc/step10-playtest.md`, section 16. This is a user-supplied approval record;
no row-level observations or unperformed manual actions are invented here.

The final Step 10QA2 master remediation and regression gate is **PASS** at
`E:/Codex/My_Nethack/_qa/step10qa2-evidence-20260915-final-closeout-r11/`.
All ten entering defects are resolved, all 26 Lua resources have fresh
generated and packaged runtime/content coverage, protected and cross-phase
regressions pass, and the packaged Neutral portal chain passes forward,
save/reload, and reverse traversal. The external evidence mirror is
`E:/Codex/step10-closeout-evidence-20260915-final/`.

The final x64 Release executable is
`E:/Codex/My_Nethack/binary/Release/x64/NetHack.exe`, SHA-256
`588bde86aec4d7e983b90dcaa303dbda436d8e531724ba97c21c73fee468633d`. The
final x64 package is
`E:/Codex/My_Nethack/vspackage/nethack-500-win-x64.zip`, SHA-256
`d02f70a31f90b6383035b1626fb7bcf0a3585562f46e80f833dc9940ad483b1d`.

Compatibility and serialized-content invariants remain
`NUMMONS=503`, `NUM_OBJECTS=547`, artifact sentinel/count `47/46`,
`MAXDUNGEON=18` with 17 dungeon records, `MAXLINFO=3600`,
`MAX_GLYPH=12410`, 2867 total tiles, and `EDITLEVEL=5`. No unexpected
serialized IDs were found. The QA3 combat regression and QA3-1 attack-handler
audit pass for 73 monsters, 209 explicit attack entries, and 68 distinct
attack/damage combinations: 67 supported, one intentionally inert,
zero unsupported, and zero NOT PROVEN.

The milestone publication is on `phase0/dod-length` under the annotated tag
`step10-neutral-quest-lost-cities`; the exact commit hash and remote
verification are recorded in the external final checkpoint. Publication used
no force push. The user's save was absent before and after validation, and
generated/cache evidence remains outside the milestone commit.
