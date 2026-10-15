# My-NetHack modification and source-distribution record

My-NetHack compliance documentation created 2026-10-15. Existing upstream
and donor authors retain their copyright, attribution and warranty notices.
This project is a modified NetHack distribution, not an upstream release.

## Scope, dates and corrections

The audit starts at upstream commit
`1b752f0e114e6f8de1e6d3569cbd1a760fad553d`, immediately before the first
My-NetHack project commit `64db689a1d457a58a6180701a840612db8bb36a5`.
It covers the complete proposed source tree through validated Step 22 commit
`cbef52d28b24e177c875e816397b6ed3e4cb834a`, not only unpublished commits.

The audit examined 1,696 tracked entries: 1,693 regular files and three
unchanged dependency submodule pins. Of the regular files, 134 differ from
existing upstream files and 427 were added by this project. All 561 carry
new prominent notices identifying local project integration/modifications
and the dates recorded for those paths in Git history. The three earlier
partial local notices are retained, along with all other existing text.

Dates in these notices are actual Git author dates, not estimates of when an
unrecorded edit occurred. The [machine-readable provenance record](my-nethack-modifications.json)
retains exact commit IDs, author timestamps, committer timestamps and subjects.
Each path links to its relevant commits, recorded dates, original Step 22
content hash, exact notice insertions and resulting hash. The notices were
added on 2026-10-15. They do not assign original authorship to My-NetHack or
change the authorship of donor adaptations.

The audit also restores 26 verbatim upstream artist-attribution blocks in
`win/share/monsters.txt`. Each block is placed before its matching original
tile, identified by both name and unchanged pixel content, including repeated
werewolf names. It restores missing notices in 37 donor-derived Lua maps:
33 redistribution/license notices and four sets also carrying the original
Jean-Christophe Collet and M. Stephenson copyright lines. Existing Guest41
credits and all original file bodies are retained. See the
[donor and dependency audit](license-donor-audit.md).

## License and source distribution

[LICENSE](../LICENSE) is a verbatim copy of [dat/license](../dat/license).
The original license text has not been edited. My-NetHack as a NetHack
derivative is distributed at no charge under that NetHack General Public
License, including its warranty provisions. Separately licensed, unmodified
dependencies retain their own terms and notices; they are not relicensed by
this statement.

The provisions relevant to this source distribution are:

- Paragraph 1: preserve copyright, license and absence-of-warranty notices,
  and distribute the license with the source. Existing notices are preserved;
  removed donor/artist credits identified above are restored.
- Paragraph 2(a): modified files carry prominent local modification and date
  notices. This correction is in each affected file, not only this document.
- Paragraph 2(b): publish the NetHack-derived work on the same no-charge
  license terms, without additional restrictions on recipients.
- Paragraph 3: executable distribution must accompany complete machine-readable
  source, or, for noncommercial distribution, full information for obtaining
  that source from an appropriate archive. Publishing a binary ZIP alone is
  not established as compliant by this source audit.

For a reproducible complete-source checkout, clone the published repository
at the exact distribution commit and initialize its pinned submodules:

```text
git clone --recurse-submodules https://github.com/yarivk12/My-Nethack.git
git checkout <published-distribution-commit>
git submodule update --init --recursive
```

A source archive accompanying binaries must contain that exact source tree,
build files, license/provenance records and required dependency source with
its own notices. A plain GitHub source ZIP omits submodule contents. Do not
substitute the earlier pre-compliance Step 22 commit as the source of a
distribution carrying these corrected notices. Ignored build outputs are
validation artifacts, not an approved public binary release. No binary or
remote publication is performed by this compliance change.

## Safe notice formats

C/header/include modules use block comments; Lua uses line comments; Python,
PowerShell, makefiles, Git configuration files and tile sources use their
supported hash comments. Python shebangs and encoding declarations remain
in their required positions. XML comments follow the XML declaration.
Markdown and the plain README contain visible prose notices.

All 17 test-data JSON files remain strict JSON. They carry an inline
`_modification_notice` string in an ignored metadata position: the first hunk
object for flat hunk-list manifests, the first record for flat record maps,
or the top-level metadata area for structured manifests and lookup-only hash
maps. Existing semantic data is identical after removing that single metadata
field. The old consumers and assertions are unchanged. No JSON comments,
invalid syntax, changed expected hashes or presumed sidecar exemption are used.
The license itself remains verbatim because its text must not be changed.
No unsupported comment format remains unresolved in the current source tree.

## Integrity and validation

`test/run_license_compliance.py` verifies every original tracked regular
file against the immutable Step 22 Git objects, checks all three dependency
pins, and permits only the recorded exact notice insertions. The original
1,132 unchanged regular files must remain identical after line-ending
normalization. All original tokens, strings, maps, pixel data, JSON semantic
data, copyright, license and warranty text remain unchanged. Gameplay,
balance, saved fields and `EDITLEVEL 18` are unchanged.

For historical source tests only, the runner removes those exact, independently
validated insertions and presents the original Step 22 text to the unchanged
source/mutation checkers. It validates full annotated bytes first and rejects
body or notice mutations. This avoids refreezing gameplay baselines around
comment changes. Historical expectations and their stored hashes are not
updated. Native runtime tests and Release builds always use the real annotated
files, never projected source.

```powershell
python test/run_license_compliance.py
python test/run_license_compliance.py --verify-only
python test/run_step22_native.py --label license-compliance
python test/run_step22_release.py
python test/run_license_compliance.py --historical-script test_step20_tiles.py --historical-arg=--step21
python test/run_step9c_elder_mm.py _qa/license-compliance/elder-native
python test/run_step10b3_3.py _qa/license-compliance/portal-current --step21
```

Validation completed against the actual annotated files on 2026-10-15:

| Check | Result | Local evidence |
| --- | --- | --- |
| Exact notice-only integrity | PASS: 1,693 regular files, three unchanged dependency pins, 561 corrections and 1,122 mutation rejections | `_qa/license-compliance/source-accepted.log` |
| Step 22 and inherited historical source/mutation gates | PASS with unchanged expected hashes; 49 Step 22 gameplay deltas and 247 historical mutation probes still accepted | Same log, ending in the historical completion marker |
| Independent parser review | PASS: 181 original Python ASTs, three XML trees and 17 JSON semantic payloads unchanged | Independent audit and the exact insertion record |
| Historical native regression plus all Step 22 property/T1-T8 fixtures | PASS, including native object/hero/level reload cases | `_qa/step22/license-compliance-runtime.log` |
| Tile/resource checks | PASS: all seven, with exact notice projection for historical comment/header comparisons | `_qa/license-compliance/resources-accepted.log` |
| Elder native spells; Silver Key/Necronomicon | PASS against actual annotated source and current catalogue | `_qa/license-compliance/elder-accepted.log`, `portal-current.log` |
| Release x64 console, GUI, utilities, data and ZIP package | PASS | `_qa/license-compliance/release.log`, `_qa/step22/release-package-build-6.log` |

Initial diagnostic attempts are retained. The unprojected resource checker
reported its two expected old-comment-header comparisons; the exact-notice
reconciled run passes all seven tests. A first portal invocation omitted its
required `--step21` current-catalogue flag; the correct invocation passes.
The first byte-level source projection normalized mixed working-file line
endings too early; the validator now preserves those original bytes while
still requiring normalized content equality with the immutable Git objects.
No gameplay baseline or historical expectation was changed to resolve these.

An optional archived Step 7 source checker was also tried. Its old reverse
projection fails on later Mithardir room-generation changes even with every
file supplied directly from the original `cbef52d28` Git objects. This
pre-existing archived-checker incompatibility is recorded in
`_qa/license-compliance/archived-step7-baseline.log`; it is not a notice
regression and was not repaired by changing unrelated historical code.
The supported current Step 22 source/mutation and native regression gates
above pass. No unresolved source-tree license/provenance issue was identified.

Existing Step 22 property, T1-T8, production save/reload and resource evidence
remains documented in [step22_implementation.md](step22_implementation.md).

## History and unpublished tag strategy

The compliance work is a new descendant commit with message
`Add NetHack license modification notices`. It does not amend or rewrite
Step 22 or earlier history. The existing annotated tag
`step22-exotic-artifact-engine-complete` is left unchanged and unpublished;
it still resolves to `cbef52d28b24e177c875e816397b6ed3e4cb834a`, which lacks
these corrections.

Recommended strategy: preserve that tag as the historical engine-validation
marker and create a distinct annotated distribution tag on the compliance
commit, after separate approval, for example
`step22-exotic-artifact-engine-distribution-ready`. Publish the corrected
branch and the new distribution tag only when authorized. Publishing the old
tag alone would identify a pre-compliance source snapshot. Neither tag
creation nor publication is performed here. Historical Git objects retain
their original content; this audit corrects the current distribution tree,
not every immutable historical snapshot.

The screenshot crash remains unconfirmed. A corrected diagnostic-fixture
initialization fault does not establish its cause. No controlled production
reproduction proving that screenshot's cause is claimed. This notice-only
work does not change that limitation or claim a crash fix.
