# Step 9A–9C final production parent closeout

Baseline: `phase0/dod-length` at
`5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`, initially clean.

The approved Step 9A–9C implementations remain unchanged internally. This
working tree removes their temporary parent forcing and routes Sheol, Dragon
Caves and Mithardir (including Mithardir's DoD approach map) through the shared
persistent scheduler. Final x64 and Win32 Release, native-topology, focused
regression, package and non-PTY Mithardir gates have passed. Step 9D is
canceled and absent from production.

| Phase | Branch | Historical manual parent | Production parent |
| --- | --- | --- | --- |
| [9A](step9a.md) | Sheol | 108 | Persistent randomized DL30–199 parent |
| [9B](step9b.md) | Dragon Caves | 109 | Persistent randomized DL30–199 parent |
| [9C](step9c.md) | Mithardir | 110 | Persistent randomized DL30–199 parent and rebased `chalv2` approach |

All existing Step5–8 rules and the frozen 200-level DoD foundation remain
protected. The user closed the implementation scope at Steps9A–9C and canceled
Step9D after its preliminary read-only donor audit. No Neutral Quest maps,
monsters, objects or runtime integration were added. The unfinished audit is
retained in [step9d.md](step9d.md) as historical notes, not planned work.
Findings and fixes are recorded in the validation report. The fixed depths in
the table are historical manual-validation locations only; production parents
are selected once from DL30–199, are distinct, and are collision-safe with
the existing scheduler. DL111 is not reserved.

The user-approved manual state used fixed DL108–110 entrances. Production no
longer does. The unused DoD111 reservation remains absent and DL111 is again
eligible for the ordinary scheduler. This does not change Mithardir's internal
logical depths. Final validation used the real `init_dungeons()` path in a
compile-gated native fixture, with eight fresh samples per architecture.

Step9B validation also corrected the post-release corpse dispatcher for Moria
and Sheol imports. See [the ongoing findings report](step9-playtest.md).

## Manual-test handoff

Start a **new wizard game** with `NetHack.exe -D` (TTY) or `NetHackW.exe -D`
from the architecture-specific Release directory. The combined Step9 save
epoch is EDITLEVEL4; pre-Step9 saves are not supported.

| Branch | Ctrl-V parent | Entrance / route | Detailed checklist |
| --- | --- | --- | --- |
| Sheol | randomized DL30–199 | Branch stair down; 6–8 floors, frozen fillers and palace/Executioner | [Step9A](step9a.md) |
| Dragon Caves | randomized DL30–199 | Branch stair down; four floors and chromatic cave dragon | [Step9B](step9b.md) |
| Mithardir | randomized DL30–199 | Central approach portal; Elshava, Wastes, Last Spire and generated Catacombs | [Step9C manual route](step9c.md#manual-validation-route-and-checklist) |

Executable directories are `binary/Release/x64` and `binary/Release/Win32`.
The per-phase documents record monsters/items, compatibility adaptations,
save/reload checks, known reused artwork and historical automated evidence.
The current compiled closeout is complete. Historical fixed-parent playtest
notes remain below as provenance; they do not describe the final randomized
production placement.

No Neutral Quest entrance exists. Its canceled preliminary audit remains
historical documentation only. The route above is retained for reproducing
the now-passed manual validation.

## Closeout validation status

| Check | Result / evidence in external `../step9-audit` directory |
| --- | --- |
| Shared randomized-parent contract | PASS, `test/test_step9_scheduler_contract.py`; legal DL30–199 entries, no fixed DL108–110 loop, DL111 not reserved |
| Pinned donor/source boundaries | PASS, Step 9A and 9B source checks against the pinned UnNetHack revision; prior Step 7/8 source checks PASS |
| Python test syntax and diff hygiene | PASS, focused `py_compile` and `git diff --check` |
| x64 and Win32 full Release solutions | PASS, final Release rebuilds; artifacts and logs are outside the repository |
| x64/Win32 scheduler regressions and fresh topologies | PASS, fresh real `init_dungeons()` topology samples (8 per architecture), plus focused scheduler/Step 9A/9B suites |
| Branch traversal, save/reload, DLB/package and tile/glyph gates | PASS, prior recorded wizard/runtime evidence plus final DLB, source/content, package and tile checks |
| Step 9C native/comparison coverage | PASS, complete non-PTY Mithardir suite on x64 and Win32 |
| Commit, annotated tag and push | FINAL publication step after this staged-delta review |

The status records checks observed for the final working tree. Earlier
per-phase notes about fixed parents describe historical manual validation; they
do not override the production scheduler. External binaries, saves and logs
remain in `../step9-audit` and are not committed.

## Checkpoint publication validation

User manual validation: **PASS for 9A, 9B and 9C** at the historical fixed
parents. The production code now uses randomized DL30–199 parents, retains
EDITLEVEL4, leaves Step9D absent and leaves DL111 unreserved. The final native
and package validation matrix passes on x64 and Win32.

The first staged whitespace check found significant trailing spaces inside
nine new ASCII map literals, which the earlier unstaged check had not examined.
Their map/package bytes are preserved exactly. Path-specific `.gitattributes`
rules permit end-of-line map cells; `test_step9a_source.py` still rejects
trailing spaces outside those map literals. The rerun staged check passes.

Quick pinned Sheol/Dragon Caves source checks, all 16 Step9 map bytes in both
DLBs, Mithardir content/shop/statue checks, tile indices and whitespace checks
pass. The prior full build/runtime/regression evidence above remains applicable;
no gameplay source was changed or long suite relabelled as a new run. External
audit data, binaries, saves and logs are excluded from the checkpoint.

Those earlier package/tile/build statements are historical checkpoint
evidence. The current working tree contains the randomized-parent
implementation and documentation; the final Release rebuild and publication
are the authoritative closeout actions for this revision.
