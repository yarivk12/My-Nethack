# Public main source modification and provenance record

My-NetHack documentation created 2026-10-15. Original copyright, attribution,
license, modification and warranty notices remain applicable.

This record applies specifically to the older public `main` tree at
`3529330468d3b638a5397d51840e92a184766b17`, compared with upstream
`1b752f0e114e6f8de1e6d3569cbd1a760fad553d`. It does not import the later
Step 22 implementation or its save format. `EDITLEVEL` remains 5.

The audit covered 1,540 tracked entries: 1,537 regular files and three
dependency gitlinks. The 385 current project-added or modified files lacked
complete dated project notices. Each now carries a prominent notice identifying
local integration/modifications and the actual Git author dates for that path.
The notice itself is dated 2026-10-15. Git metadata records commit dates, not
unrecorded editing times. The complete file list, dates, commit IDs, timestamps,
original and annotated hashes, exact insertions and donor-map origins are in
[public-main-modifications.json](public-main-modifications.json).

Every original file body is retained. The 1,152 other regular files remain
unchanged. Notices use C block comments (158), hash comments (140), Lua comments
(71), Markdown prose (13), plain prose (one), or XML comments after the
declaration (two). Python shebangs and encoding declarations keep their required
positions. No modified binary or comment-incompatible format was identified;
there are no modified JSON files in this older tree. No separate changelog is
being used as a substitute for a per-file notice.

The older tree already contains all 26 upstream artist-attribution blocks
missing from the later Step 22 tile source. They are preserved, with no
duplicate restoration. Existing Guest41 Moria credits, NerfHack conversion
credits and other upstream/donor notices are preserved.

Missing donor notices are restored in 37 Lua maps. All 37 original map bodies
are identical to the previously audited pre-compliance Step 22 Git objects,
and their origins are separately recorded in the manifest. Eight Chaos maps
derive from dNetHack `chaos2.des`, 25 Neutral Quest maps from its
`neutrality.des`, and four Dragon Caves maps from UnNetHack `dragons.des`.
The latter restore the Jean-Christophe Collet (1989) and M. Stephenson (1991)
copyright lines, as well as the redistribution notice. Only the donor comment
marker is converted to valid Lua syntax. These notices do not claim local
authorship of donor work.

The applicable pinned donor licenses were checked in the preceding repository
audit: NerfHack `0cb8781b0929b4617590a3b9fe78f972ef42c25f`, UnNetHack
`439b8d63d3d1ca78fb08588dd43f61874114b21a`, and dNetHack
`17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0` use the same NetHack General
Public License. Their normalized license SHA-256 is
`93a3ae2cb8dee482daddfaebe53bcffe5b114b603def19b4dca21621cbc5a747`.
Earlier milestone records in `doc/step6a.md` through `doc/step10.md` retain
the detailed import and adaptation boundaries. Later milestone donors are
not attributed to this older tree merely because they appear in Step 22.

`LICENSE` is a verbatim copy of the unchanged `dat/license`. This modified
NetHack source is distributed at no charge under that license, including its
warranty provisions. Paragraph 1 requires retention of notices and delivery
of the license. Paragraph 2(a) requires the prominent modification/date
notices supplied here. Paragraph 2(b) requires the derivative work to retain
the same no-charge license terms without extra recipient restrictions.

The three original gitlinks are unchanged: Lua
`6e22fedb74cf0c9b6656e9fce8b7331db847c605`, PDCurses
`09cf16db29753305e4241d4ae609aac997fa730d`, and PDCursesMod
`6cd9c16900fef82754923c718fab7fe85f761bb6`. Their separate component
terms remain applicable. Retain Lua's MIT-style copyright/permission and
warranty notice. PDCurses has public-domain core code and separately licensed
exceptions; PDCursesMod also has component-specific terms, including GPL
configuration scripts. Do not describe every dependency file as public domain
or relicense these components as NetHack code.

A complete buildable source checkout must initialize the pinned submodules.
A plain superproject source ZIP omits their contents. Any future executable
distribution must also satisfy NetHack paragraph 3, using matching complete
machine-readable source or, for noncommercial distribution, full instructions
for obtaining it from an appropriate archive. This change publishes no binary
and makes no claim that an isolated binary ZIP supplies complete source.

## Validation and limits

The audit reconstructed all 385 original bodies by removing only the recorded
insertions and compared them to immutable `352933046` Git objects. All 1,537
regular files and three pins pass the full-tree integrity comparison. All 133
changed Python ASTs and two changed XML element trees are identical before and
after annotation. Original map text, tile pixels, code and license text remain
unchanged. No gameplay, balance, persistence, build logic or save epoch changes
are included.

All 770 notice/body mutation probes are rejected by the frozen integrity
hashes. The complete diff passes whitespace review with CR-at-end-of-line
recognized for the existing CRLF XML project format.

Twenty-one existing source/package checks were exercised. Thirteen pass on
the annotated files. Two additional historical checks (`test_step10b3_4_source.py`
and `test_step8a_source.py`) pass when exact verified notice insertions are
projected out for their historical byte/header comparisons. No historical
assertion, stored hash or gameplay baseline was changed.

The remaining attempted checks do not establish a new regression:

| Checker | Result using original-tip body projection |
| --- | --- |
| `test_step10b2_2_source.py` | Existing protected-file equality assertion fails against its older milestone baseline |
| `test_step10b_source.py` | Existing historical README equality assertion fails |
| `test_step7_source.py` | Existing `src/mkroom.c` equality assertion fails after later room-generation milestones |
| `test_step10b5_source.py` | Historical untracked-file allowlist does not recognize the new compliance manifest and LICENSE |
| `test_step9a_source.py`, `test_step9b_source.py` | Require a donor checkout argument, unavailable in this isolated worktree invocation |

Generated-resource and donor-argument tests needing additional build/donor
inputs were not represented as passing. Native runtime and Release builds
were not rerun for this older tree: its dependency worktrees are unpopulated.
The exact-body and parser proof establishes that these notice changes preserve
the existing implementation, independently of those unavailable build inputs.
The separate Step 22 branch has its own current native/Release evidence.

Local reproducibility evidence is retained outside this worktree under
`_qa/publication/main-audit/`: `prepare.py`, `integrity-result.json`,
`verify.py`, `final-integrity.json`, `check_sources.py`, `source-results.json`, the individual checker logs and
`source-summary.log`. The published manifest supplies the original object IDs,
normalized hashes and exact insertions needed to reproduce the body comparison.

This record does not resolve or reclassify the unconfirmed screenshot crash
reported during Step 22 work. That crash's exact cause remains unconfirmed.
No claim of a production crash fix is made by this license-only change.
