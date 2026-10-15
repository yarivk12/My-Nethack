# Public phase0 source compliance record

My-NetHack notice and provenance documentation created 2026-10-15.
Existing upstream and donor authors retain their copyright, attribution,
license and warranty notices. This is a modified NetHack distribution.

## Snapshot and scope

This record audits the existing public `phase0/dod-length` snapshot
`7fa6dd11a544470a4c1bda3cf9c0628bbb73da29` (Step 11), relative to upstream
`1b752f0e114e6f8de1e6d3569cbd1a760fad553d`. It does not replace that snapshot
with later milestones. The audit covers 1,543 regular tracked files and three
unchanged dependency pins. All 393 paths added or modified by My-NetHack receive
prominent notices identifying local integration/modifications and actual Git
author dates. These dates identify recorded changes, not an inferred time of
unrecorded work. Notices were added on 2026-10-15.

[The complete path manifest](public-source-modifications.json) lists every
corrected filename, dates, exact project commits, original and annotated
SHA-256 hashes, exact insertion positions/text, and hashes for unchanged files.
It preserves author and committer timestamps and subjects for relevant history.
The original body of every file is unchanged, including all existing notices.
`EDITLEVEL` remains 6. No gameplay, balance, persistence or engine behavior is
changed. Historical snapshots retain their original content.

## License, donor provenance and distribution

[LICENSE](../LICENSE) is a verbatim copy of [dat/license](../dat/license).
Paragraph 1 requires preservation of copyright, license and warranty notices.
Paragraph 2(a) requires prominent modification-and-date notices in changed
files. Paragraph 2(b) retains the same no-charge NetHack General Public License
terms for the derivative, without additional restrictions. This distribution
retains those terms and does not claim My-NetHack authorship of donor work.

This older tree's existing milestone records remain the source mappings:
`doc/step6a.md`, `doc/step7.md`, `doc/step8a.md`, `doc/step9a.md`,
`doc/step9b.md`, `doc/step9c.md`, `doc/step10.md` and `doc/step11.md`.
The applicable pinned donor licenses were checked during the encompassing
2026-10-15 compliance audit, and these branch-specific files were matched to
the same documented conversions. Each donor's `dat/license` is the NetHack
General Public License (LF-normalized SHA-256
`93a3ae2cb8dee482daddfaebe53bcffe5b114b603def19b4dca21621cbc5a747`).

| Donor | Exact revision | Relevant provenance |
| --- | --- | --- |
| NerfHack | `0cb8781b0929b4617590a3b9fe78f972ef42c25f` | Rooms and Tomb |
| UnNetHack | `439b8d63d3d1ca78fb08588dd43f61874114b21a` | Moria, Sheol, Dragon Caves |
| dNetHack | `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` | Mithardir and Neutral Quest |
| xNetHack | `6eef39403f16f65e13f5d57242ee8d036307687a` | Selected Step 11 theme rooms |

Missing original redistribution notices are restored to the eight `chaos2.des`
conversions and 25 `neutrality.des` conversions from dNetHack, using the exact
original notice text with Lua comment markers. The four UnNetHack Dragon Caves
conversions also regain the original Jean-Christophe Collet (1989) and
M. Stephenson (1991) copyright notices. Exact filenames, donor pins, source
paths and restored wording are recorded in the manifest and file headers.
Guest41's Moria credit and existing NerfHack and theme-room notices remain.
Unlike the later Step 22 tree, this snapshot already retains all 26 examined
upstream artist-attribution blocks before their matching unchanged monster
tiles. No replacement artwork or additional authorship is asserted.

The three dependency pins are unchanged. Lua retains its MIT-style copyright,
permission and warranty notice. PDCurses and PDCursesMod retain their public
domain core notices and their component-specific exceptions and licenses.
Their original per-file terms must accompany dependency source; they are not
relicensed by this record. No incompatible donor license or unresolved file
format mechanism was identified in this source snapshot. This records source
evidence, not a legal opinion about every contributor's title.

Paragraph 3 imposes additional obligations on executable distribution:
provide complete matching machine-readable source, or the allowed full source
archive information for noncommercial distribution. No executable or release
asset is approved or published by this notice-only correction. A GitHub source
ZIP alone omits submodule contents. A complete-source checkout must initialize
the exact pinned submodules and preserve their own notices. A future binary
distribution must identify its exact corrected source commit and include all
required component source and notices. A pre-compliance historical tag does
not identify this corrected tree.

## Formats and validation

C and header files use closed C block comments, Lua uses line comments,
Python/PowerShell/build and tile files use their existing hash-comment syntax,
and XML comments follow the declaration. Prose files receive prose notices.
Existing shebangs and mixed line endings are preserved. No JSON or binary file
needs a notice mechanism in this branch's changed-file set. No map strings,
tile pixels, code tokens or other functional content is changed.

Reproduce integrity validation with:

```text
python test/verify_public_source_compliance.py
python test/test_step10b3_3_source.py
python test/test_step10b4_source.py
python test/test_step10c_d_source.py
python test/test_step9c_package.py
python test/verify_public_source_compliance.py test/test_step10b5_source.py
```

The first check verifies the complete original snapshot, unchanged dependency
pins, exact approved insertions, safe comment syntax, 393 negative mutation
probes, 136 unchanged Python ASTs and two unchanged XML trees. Historical
source projection is allowed only after validating every actual file, and
removes exactly those recorded insertions. No expected gameplay hash or old
assertion is revised. Direct portal, lifecycle/B4, scheduler/D and resource
manifest checks pass on the actual annotated files. The B5 checker also
requires a clean committed tree; run it after committing these notices.

The archived Step 7 checker fails at its old `src/mklev.c` whole-file equality
assertion even when supplied with the verified original Step 11 snapshot
bytes. The archived Step 10B2-2 check likewise rejects the original Step 11
README against its earlier milestone snapshot. These inherited
incompatibilities are not notice regressions. They are not
repaired by changing historical expectations. Local evidence is retained in
the ignored parent `_qa/publication/phase0-audit/` directory.

Native runtime and Release builds were not rerun for this historical branch:
its isolated checkout lacks initialized build dependencies and prior outputs.
The targeted proof is exact original-body identity plus parser, source and
resource checks. Later Step 22 runtime/build results are not claimed for this
older branch. This notice-only work does not establish or claim a cause or fix
for the previously reported, unconfirmed screenshot crash.
