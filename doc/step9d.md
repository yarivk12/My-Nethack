# Step9D: Neutral Quest / Lost Cities — canceled; historical audit only

The user canceled Step9D and closed the implementation scope at Steps9A–9C.
This unfinished read-only audit is retained for provenance. Its proposed
actions are not approved follow-on work. No9D production integration began.

Step9C's automated gate passed before this audit began. No Step9D production
integration was made. The initial DoD111 placeholder reservation was removed
during the requested Steps9A–9C closeout. At cancellation the local branch was
`phase0/dod-length` at `5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`, with
uncommitted Steps9A–9C preserved and a gate checkpoint archived outside the
repository. Those three branches subsequently passed user manual validation
and received checkpoint publication authorization; see [step9.md](step9.md).
The proposals and publication restrictions below remain historical audit notes
and confer no authorization to implement Step9D. DL111 remains unreserved.

Use exactly the dNetHack revision pinned before Step9C:
**17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0**, from the requested
`Chris-plus-alphanumericgibberish/dnethack` repository's `master` branch.
Use `git show` at that pin or the immutable export under
`../_qa/step9-audit/dnethack-pinned/dnethack-3.4.3`; do not follow the repository's
relocation notice or mix revisions. The clone is outside the project.

The complete dependency closure was not finished before cancellation.
The verified topology below supersedes approximate secondary counts. Preserve the
existing local endgame and role Quest. No Law/Chaos quest import, new ascension
gating, README update, random Step9 placement, commit, tag or push.

| Donor element | Donor file(s) | Local equivalent | Required action | Compatibility adaptation |
| --- | --- | --- | --- | --- |
| Seven Outlands levels and parent approach | `dat/dngnch{1,2,3}.def`, `dat/neutrality.des` | Lua dungeon and persistent Step9 parent reservations | 2/3: import approach and seven resources | Exactly one optional portal branch at DoD111; native depth semantics, no alignment-quest selector. |
| Thirteen Lost Cities levels, entry at level2 | Same dungeon definitions; `neutrality.des` | Native subordinate dungeon/entry level | 2/3: preserve complete route and four two-map variant slots | Enter `lethe-b` from Sum of All; headwaters remain reachable above entry. Do not flatten to an approximate 19-floor branch. |
| Shrouded Dispensary | `dat/dngnch2.def`, `dat/labr.des` | Native one-level subbranch and Lua maze | 3: include as normal Outlands subbranch | Entrance randomly selected among Outlands levels2–6, while root remains fixed DoD111. No extra DoD reservation. |
| Outlands forests and inclusions | `src/mkmaze.c`, `src/mkroom.c:place_neutral_features` and its callees | Native cellular map generation; scoped Mithardir reuse | 2/3: audit and port all active feature generators and populations | Preserve conditional probabilities; do not import commented-out fort or unrelated Law/Chaos feature generators. |
| Lethe water | `src/trap.c:water_damage/drown`, callers, level flags | Native water, object erosion/forgetting, saved level state | 2/3: import forgetting and scoped item effects | Preserve actual active code, not commented-out magic-lamp/whistle/stone conversions. Native magic lamps and wishing remain unchanged. |
| N'Kai descent | `neutrality.des:nkai-*` | Native holes, up stairs and falling | 1/2: preserve four hole-driven downward regions and no-hardfloor behavior | No invented down stairs to replace the donor holes. Audit branch placeholders and landing eligibility. |
| Outlands spell suppression | `src/spell.c`, `src/mcastu.c` | Native hero and monster spell paths; Naen from9C | 2: radial-by-level spell penalties and Spire prohibition; Sum of All bonus | Branch-local only, including order relative to Naen. Keep native global spell and skill systems. |
| Branch monster generation | `src/questpgr.c:neutral_montype`, `src/makemon.c:rndmonst` | Native generation plus existing scoped9C pool | 2/3: exact weighted pools, populations and R'lyeh stair-side groups | Gate Town/Sum/R'lyeh generation differs from ordinary levels; donor-wide Center of All spawning requires a documented branch-local adaptation. |
| Center of All | `src/makemon.c`, `src/monmove.c`, `src/wizard.c`, `src/mcastu.c`, monster/artifact tables | Native uniqueness/artifacts/monster movement | 3: audit full encounter, key and persistence | Do not add the donor's unconditional worldwide1/5000 random spawn to ordinary local dungeon generation. Exact scoped trigger remains under audit. |
| Dispensary encounters | `labr.des`, `src/monst.c`, `src/mcastu.c`, `src/makemon.c`, `src/mon.c`, `src/wizard.c` | Native minotaur plus imported spell/unique handling | 2/3: minotaur priestess and Illurien, shuffled placements, six spellbooks and thirteen rust traps | Audit Illurien's death/reappearance and learning attacks without importing unrelated global seals/roles. |

## Verified topology

All three pinned `dngnch*.def` variants agree on the Neutral topology.
There are **21 branch floors**: seven Outlands, thirteen Lost Cities and the
one-level Dispensary, plus the ordinary DoD111 approach (not an extra branch
floor). There are **26 Lua resources to port**, including that approach and
four extra map variants. The Lost Cities `ENTRY: 2` is significant.

| Region | Logical levels | Pinned resources / connection |
| --- | --- | --- |
| DoD111 approach | Existing parent | `neulev`, portal to Gate Town |
| Outlands | 1 | `gatetwn`, return portal to parent, onward portal to `out1` |
| Outlands forests | 2–5 | `out1`, `out2`, `out3`, `out4`, portal chain |
| Spire | 6 | `spire`, portal back to `out4`, down stair to Sum of All |
| Sum of All | 7 | `sumall`, up stair and branch stair to Lost Cities level2 |
| Lethe headwaters | Lost Cities1 | `leth-a-1` or `leth-a-2`, equal variant weights; above entrance |
| Lethe entrance | Lost Cities2 | `lethe-b`, branch return to Sum of All, ordinary up/down stairs |
| Lethe Waterway | Lost Cities3–4 | `leth-c-1/2`, `leth-d-1/2`, independent equal variants |
| Lower Lethe | Lost Cities5–8 | `lethe-e`, `lethe-f`, `lethe-g`, `lethe-z` |
| Gulf of N'Kai | Lost Cities9 | `nkai-a-1/2`, equal variants; up stair, four downward holes |
| Gulf of N'Kai | Lost Cities10–12 | `nkai-b`, `nkai-c`, `nkai-z`; hole descent and return stairs |
| R'lyeh | Lost Cities13 | `rlyeh`, hardfloor/noteleport/nommap/lethe, up stair |
| Shrouded Dispensary | Separate level1 | `lbyrnth`; native branch selected at Outlands2–6, no teleport/mapping/digging/passwall |

The donor map directives and source-use indexes are recorded outside the
repository under `../_qa/step9-audit/neutral-{map-inventory.json,map-directives.txt,
topology-uses.txt,dispensary-uses.txt}`. The current direct named-monster union
has109 names; this is an audit input, not the final number of imported species.
Random class entries, procedural features, equipment, artifact containers,
summons, growth forms and death mechanics still need dependency closure.

### Exact source observations to preserve during the remaining audit

- `neutral_montype` has donor tests for Ferrumach strength which return
  **Cuprilach**, in both the Outlands and Sum of All lists. This must be
  explicitly preserved or documented as a correction, not silently rewritten.
- Outlands features actively call `mkkamereltowers`, `mkminorspire`,
  `mkfishingvillage`, `mkneuriver`, `mkpluvillage`, `mkinvertzigg`,
  `mkferrutower`, and `mkpluhomestead`. `mkferrufort` is commented out.
- R'lyeh suppresses nineteen out of twenty ordinary pool invocations and
  places the selected encounter group around the up stair. Inclusive donor
  countdown loops affect the number of creatures and must be tested.
- Lethe's active water code clears BUC, can rewrite scrolls as amnesia,
  changes water into amnesia potion, blanks nonartifact spellbooks, drains
  positive equipment charges/enchantment and magic-marker charges, and
  retains ordinary rust handling. Several advertised tool downgrades and
  proof-removal effects are commented out in this pin and must stay absent.
- The separate Dispensary is structurally part of the requested adventure;
  its two new named encounters are not optional omissions.

**Status:** canceled; the audit was incomplete, and no9D integration or
gate/build success is claimed. Its unused parent reservation was released.
No continuation is planned. No README update, commit, tag, push or final
random Step9 placement was performed.
