# Step 9A–9C deterministic branch integration

Baseline: `phase0/dod-length` at
`5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`, initially clean.

This is the intermediate deterministic checkpoint, **Add Sheol, Dragon Caves
and Mithardir**. The user confirmed successful manual validation of all three
branches and authorized this checkpoint commit and branch publication. README
now includes the cumulative Step9A–9C state. Final randomization and the final
Step9 milestone tag remain deferred to a later, separately authorized closeout.

| Phase | Branch | Temporary DoD parent | Status |
| --- | --- | --- | --- |
| [9A](step9a.md) | Sheol | 108 | Implemented; automated and user manual validation PASS |
| [9B](step9b.md) | Dragon Caves | 109 | Implemented; automated and user manual validation PASS |
| [9C](step9c.md) | Mithardir | 110 | Implemented; automated and user manual validation PASS |

All existing Step5–8 rules and the frozen 200-level DoD foundation remain
protected. The user closed the implementation scope at Steps9A–9C and canceled
Step9D after its preliminary read-only donor audit. No Neutral Quest maps,
monsters, objects or runtime integration were added. The unfinished audit is
retained in [step9d.md](step9d.md) as historical notes, not planned work.
Findings and fixes are recorded in the validation report. These fixed depths
preserve the known-good manual-test state; they are not final production
placement.

The user chose to retain fixed DL108–110 entrances for manual testing. The
unused DoD111 reservation was removed after Step9D was canceled. DL111 is
again eligible for the existing randomized Step6–8 scheduler. This does not
change Mithardir's internal logical depths. Closeout validation passed on
both Release architectures. No gameplay changes accompany publication.

Step9B validation also corrected the post-release corpse dispatcher for Moria
and Sheol imports. See [the ongoing findings report](step9-playtest.md).

## Manual-test handoff

Start a **new wizard game** with `NetHack.exe -D` (TTY) or `NetHackW.exe -D`
from the architecture-specific Release directory. The combined Step9 save
epoch is EDITLEVEL4; pre-Step9 saves are not supported.

| Branch | Ctrl-V parent | Entrance / route | Detailed checklist |
| --- | --- | --- | --- |
| Sheol | 108 | Branch stair down; 6–8 floors, frozen fillers and palace/Executioner | [Step9A](step9a.md) |
| Dragon Caves | 109 | Branch stair down; four floors and chromatic cave dragon | [Step9B](step9b.md) |
| Mithardir | 110 | Central approach portal; Elshava, Wastes, Last Spire and generated Catacombs | [Step9C manual route](step9c.md#manual-validation-route-and-checklist) |

Executable directories are `binary/Release/x64` and `binary/Release/Win32`.
The per-phase documents record monsters/items, compatibility adaptations,
save/reload checks, known reused artwork and automated evidence. All three
phase gates passed. The subsequent DL111 reservation cleanup also passed
the final build/topology refresh described below.

No Neutral Quest entrance exists. Its canceled preliminary audit remains
historical documentation only. The route above is retained for reproducing
the now-passed manual validation.

## Final Steps9A–9C closeout results

| Check | Result / evidence in external `../step9-audit` directory |
| --- | --- |
| x64 and Win32 full Release solutions | PASS, `build-abc-close-{x64,Win32}-1.log`; both TTY and tiled executables rebuilt |
| Scheduler and prior milestone regressions | PASS both, `abc-close-regressions-{x64,Win32}-1.log`; 2,000 production scheduler samples verify DL108–110 exclusion and DL111 reuse |
| Depth, ledger, recovery and allocator contracts | PASS both in the cumulative regression logs, including invalid/old-format rejection |
| Final fresh-game topologies | PASS four per architecture, `abc-close-topology-{x64,Win32}-1.log`; all three fixed entrances, prior randomized branches/counts, Castle200 |
| Sheol/Dragon Caves donor/source/package checks | PASS both, `abc-close-source-{x64,Win32}-1.log` |
| Mithardir DLB bytes | PASS both, `abc-close-packages-1.log`; all eight resources plus dungeon/quest bytes |
| TTY/tile indices and reused art | PASS, `abc-close-tiles-1.log`; 2,554 tiles fit the640×1390 generated bitmap; artwork substitutions remain documented for manual inspection |
| Full branch traversal/encounters/save-reload | All three phase gates PASS; see per-phase reports. Latest Mithardir ten-floor round trip and boss/equipment/service checks passed on both architectures before the reservation-only closeout |
| Scope and whitespace | `git diff --check` PASS; 226 source/test/document files, no generated build/save/log artifacts; full inventory `abc-close-files.txt` |

The table records the completed pre-publication validation from baseline
`5ce8b8193e4c581dd293ccac2bd0cafb4da89e96`. Earlier per-phase notes about pending
work or withholding publication describe their historical implementation gates;
this checkpoint status supersedes them.

## Checkpoint publication validation

User manual validation: **PASS for 9A, 9B and 9C**. The checkpoint retains fixed
DL108/109/110 and EDITLEVEL4, with Step9D absent and DL111 unreserved. The only
publication edits are the cumulative README, current-status documentation,
source-test publication guards and a scoped Git whitespace rule. The obsolete
README-equality guard now preserves the Step5–8 sections while allowing the
authorized cumulative summary.

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

Publication is limited to one checkpoint commit on `phase0/dod-length` and a
push to that origin branch. No tag or final randomization belongs to this
checkpoint. The cumulative README is included in the same commit.
