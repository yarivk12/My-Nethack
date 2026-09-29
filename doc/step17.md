# Step 17 implementation record

Baseline: `phase1/equipment-enhancement`,
`9a89a7e341eb5caa3552b85c5de15b8dbafe8f05` (completed Step 16C).
No tracked changes existed. The pre-existing `.codegraph/` and user-supplied
`doc/Phase1_Step17_Forge_Affix_System_Design_FINAL.md` are preserved.

## Baseline validation

- All Step 13-16C source gates and the Phase 1 audit passed.
- `test/run_step13.py --out _qa/step17-baseline13` passed, including the
  million-object corpus and 5,345,280 historical codec combinations.
- `test/run_step15.py --out _qa/step17-baseline15` passed, including the
  1,000-case world corpus.
- MSBuild requires a deduplicated child environment in this session. Native
  runs use `PORTABLE_DEVICE_PATHS=1` in their isolated fixture sysconf.

## Implementation

Applying a war hammer while standing on a Forge opens the existing Forge
menu. Affix Crafting contains Add Random, Reroll, Extract, Imprint and the
stored Affix Essence ledger. Salvage is a top-level operation. An artifact
war hammer retains its activation exception but cannot become a participant.

Ordinary affixes now have two canonical inline slots. Each slot stores its
property ID, immutable tier and saturated history. Tier zero means unused;
property zero with a nonzero tier means Open. Active masks, numeric values,
knowledge and property timers remain synchronized by the enhancement APIs.
Removing a property leaves an Open slot and clears its runtime state. Native
generation and recipe inheritance construct slots in acquisition/donor order.
Recipe outputs discard Open slots and start inherited histories at zero.

The four affix operations require known ordinary state and all information
needed for legality. An explicit known-absence flag prevents hidden empty
positions from becoming a menu oracle. Open positions remain visible through
identification loss. Salvage accepts unidentified equipment, uses the full
stack, and conceals the return whenever relevant state is unknown. Containers
must be known empty. Precommit cancellation is free; successful commands
return the existing single-turn dispatcher result. Paid Reroll Keep and Escape
retain payment, history and the committed turn.

Generic Essence is a zero-probability mineral tool with a tile, unit weight,
permanent neutral beatitude, and automatic identification. It has no shop,
affix or socket participation. Only direct inventory pays its cost. Exact
wish limits and a narrowly scoped 1% ordinary floor/container replacement
hook implement its acquisition. Affix Essence uses canonical ordinary IDs
in a per-game uint64 ledger, with tier/alphabetical menu presentation.

Affixed, Open and socketed items cannot merge or split. Ammunition cannot
receive affixes or sockets. Constructors that finalize quantities after
enhancement generation clear enhancements on generated stacks. Existing
recipes and socketing retain their established transaction behavior, with
quantity-one socket targets and artifact-participant exclusions.

## Persistence

The native object/context codecs serialize their complete structures. Inline
slot storage therefore follows their existing ownership and lifetime model.
Shared persistent definitions must precede both obj.h and context.h. Active
property values, knowledge and timers remain centrally synchronized caches.
Restore validates corrupt slot state before normalization. `struct obj` grows
from 176 to 184 bytes in the x64 build. The context contains the canonical
uint64 ledger. `EDITLEVEL` changes exactly once, from 13 to 14; old development
data is rejected by the native version gate. No sidecar or cross-run storage
is introduced. Bones preserve physical slots and do not import a ledger.

## Validation

| Design area | Evidence |
| --- | --- |
| Slots and runtime | Native tier/order/capacity/Open/history tests, invalid-state rejection, Stoning and Purification reset, lifecycle and codec tests |
| Add and Reroll | Exact pool sampling and RNG-tail replay, retained-family legality, single-candidate behavior, history pricing, paid Keep/Escape and deferred numeric rolls |
| Extract and Imprint | All-tier costs, history-10 transitions, exact ledger counts, fresh magnitude replay, unused-slot rejection and same-property reacquisition |
| Salvage | All quality/tier/Open combinations, whole-stack gold replay, direct ownership, known-empty containers, exclusions and hidden preview comparisons |
| Essence | 20,000 seeded floor draws, 1-2 quantities, ordinary container positives, explicit/class-constrained exclusions, wish table, native BUC, merge, weight and shop APIs |
| Knowledge and menus | Paired hidden-state menu/inspect/Salvage snapshots, Open visibility, ledger ordering, canonical names and generic Trueflight description |
| Transactions and turns | Each operation through `forge_interact`, zero-time cancellations, no side effects/RNG before commitment, single `ECMD_TIME`, artifact-hammer activation |
| Persistence | Mixed Open/occupied slots, history 0/10, magnitude, sockets and Essence codec; actual save/restore and recover.exe; level/bones round trips; 64-bit ledger and old-version rejection |
| Prior behavior | Cumulative Steps 13-16C native tests, million-object corpus, 5,345,280 codec exercises, all twelve Forge recipes, 480 socket outcomes and 1,000-world corpus |

All completed gates pass:

- `python test/run_step17_source.py`: current contracts plus unchanged
  historical Steps 13-16C source gates and Phase 1 audit. Exact reviewed
  production hunks are frozen in `step17_historical_changes.json`; 332
  mutation probes prevent the projection from hiding new changes.
- `STEP17_ONLY=1 python test/run_step13.py --out _qa/step17`: focused native
  Step 17 fixtures, including menu callbacks and real level/bones codecs.
  In PowerShell set `$env:STEP17_ONLY='1'` for this invocation, then remove it
  before running the cumulative suite.
- `python test/run_step13.py --out _qa/step17`: cumulative native suite.
- `python test/run_step15.py --out _qa/step17-forge`: Forge suite and corpus.
- `python test/run_step13_save.py _qa/step17/bin _qa/step17-save-verified`:
  live save/restore plus native recovery. Use a new output directory per run.
- Normal `sys/windows/vs/NetHack.sln`, Release x64: console, Windows GUI,
  tiles/resources, recover.exe and packaged distribution.
- `git diff --check`: clean.

Historical fixture setup uses `phase1_fixture.h` to construct complete legal
initial configurations. It does not replace the production setter in Step 17
tests. Obsolete ammunition/affixed-stack expectations were updated to the
explicit new invariants. Review retained whole-object failure assertions and
the existing source mutation gates.

Logs are under `_qa/step17`, `_qa/step17-forge`,
`_qa/step17-save-verified`, `_qa/step17-source.log`, and
`_qa/step17-release-final.log`. Windows environment duplication is handled
by explicit child environments in diagnostic runners. A fresh symbols
directory avoided a locked PDB during the normal solution build. Save tests
package only game assets so old diagnostic panic logs cannot contaminate a
new run. No unresolved implementation or environment blocker remains.

## Follow-on integration

The later post-Step-17 integration that gives successfully wished eligible
single items a normal enhancement-generation opportunity is documented
separately in [Phase 1 Step 17.5](step17.5.md). It is a follow-on to this
implementation record, not part of the original Step 17 implementation.
