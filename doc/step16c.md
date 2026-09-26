# Step 16C implementation record

Baseline HEAD: `3dd0a7aba5080cdbe401968559559d35ae50e5ae`.

The mandatory preflight examined the actual working tree, including its Git
status (no tracked changes), the enhancement catalogue, object and artifact
definitions, and source/data terminology. All 23 approved prefix/suffix pairs
were searched case-insensitively. No material collision was found. Matches
were unrelated prose, comments, or local variables. No approved name changes.

Implementation and validation plan:

1. Add portable second actual/known words and authoritative property mapping.
   Verify stable old identities, exact new coordinates, counts, legality,
   knowledge subsets, and deterministic normalization.
2. Close generation, forging, naming, inspection, transformations, merging,
   and native serialization. Verify recipient/quality/socket boundaries,
   round trips and lamp preservation.
3. Integrate hero Utility runtime at native turn, damage, knowledge and digging
   boundaries. Verify source activation separately from target scope.
4. Integrate shop settlement, canonical content weight, and Oilskin water
   protection. Verify accounting, integer rounding and nested composition.
5. Run focused Step 16C and relevant Step 13-16B regressions, source assumption
   audit, and the normal Release x64 solution build. Record actual results.

## Implementation

All 23 approved Utility properties are appended to the shared 82-entry ordinary
catalogue. Persistent identities are 94 through 116, in the approved order.
`enhancement_mask_property()` is the authoritative identity-to-coordinate map:
the descriptor's `bit` selects word 0 and `bit2` selects word 1. Utility entries
have word 0 zero and word 1 bits 0 through 22. Existing identities and word-0
assignments are unchanged, including reserved positions. No native `__int128`
or property-ID shift is used.

The portable `enhancement_mask` has two `uint64` words. Object storage keeps the
original `o_enh_props`/`o_enh_known` word-0 members and adds
`o_enh_props2`/`o_enh_known2`. Generic assignment, normalization, generation,
forge inheritance, pricing, naming, inspection and knowledge use both words.
The explicit word-0 adapters remain for existing combat clients. Knowledge is
intersected with actual state on removal and normalization. The maximum is two
properties, with all eleven Utility families enforcing tier exclusivity.

Tool legality uses explicit object-origin metadata: ordinary vanilla tools
are eligible by default after the explicit exclusions and artifact/unique
checks. Custom and unknown origins are denied; no custom tools are approved.
Crystal Pick is explicitly excluded. Pick-axe alone can receive Excavating.
Tools always have Standard Quality and remain socket-ineligible. Inspection
shows `Quality: Standard`, hides unidentified affixes and timers, and omits
socket information for currently ineligible recipients.

`EDITLEVEL` is 13. The native whole-object and context codecs persist both
actual/known words and dedicated Purification runtime state. Windows x64
`struct obj` is 176 bytes. `o_purification_remaining` stores the countdown;
`o_purification_sampled` records a pending sampled turn. The saved
`context.purification_snapshot_turn` preserves the sampling boundary even when
there were no sources. A saved-away object completes at most its one pending
decrement when restored; there is no off-level catch-up. Bones discard the
previous game's pending sample. Native tool charge, fuel, age and special
state remain separate.

## Native integration

- `src/utility.c` and `src/allmain.c`: sample source activity once per normal
  turn, debit at the native turn boundary, and resolve Ready sources after
  native effects. Fast actions and zero-time commands retain the correct
  sample. Occupations process each elapsed turn. Native `uncurse()` performs
  each randomly selected legal transition; source order follows inventory.
- `src/mkobj.c`, `src/invent.c`, `src/trap.c`, and `src/zap.c`: exact shared
  equipped-target predicate for complementary curse protection; immediate
  beatitude knowledge; erosion, combustible destruction, acid, fire and lava
  item-damage protection. Independently timed sources cannot merge.
- `src/dig.c`: double the finalized native effort contribution from the actual
  Pick-axe. Identify only when the added contribution crosses the native
  completion threshold.
- `src/shk.c`: apply Commerce once at `dopayobj()` settlement, charge through
  native `pay()`, and discharge the full original obligation through native
  bill updates. Immediate quotes use the same rounding boundary. Stored debt,
  sale values, usage fees and damage charges retain native accounting.
- `src/mkobj.c` and `src/pickup.c`: Storage adjusts each complete direct-content
  contribution in canonical `weight()` before the Bag of Holding aggregate
  formula. Existing container weight propagation handles insert/remove,
  split/merge, nested containers and enhancement changes.
- `src/trap.c`: Watertight enters the existing waterproof-container branch,
  including the native cursed leak roll and native content damage.
- `src/enhance.c`, `src/apply.c`, and `src/zap.c`: legal generation and forge
  inheritance, fresh forge timers, transformation validation, artifact
  stripping, and preserved Magic Lamp to Oil Lamp state. Same-type polymorph
  transfers a pending Purification sample to the replacement object.

## Validation record, 2026-09-26

- Full native Step 13-16B regression plus initial Step 16C integration passed:
  `_qa/step16c-full.log`. This includes the million-object generation corpus
  and 5,345,280 historical object-codec combinations.
- Latest Step 16C integration and persistence passed with
  `python test/run_step13.py --step16c-only --out _qa/step16c-loop-final`.
  Coverage includes the actual game loop, fast/zero-time actions, occupations,
  native digging, Commerce settlement, nested Storage, native water leakage,
  all 23 identities, every Purification countdown value, pending-turn restore,
  and actual level/bones codecs.
- Latest Step 13-16B combat and knowledge regression passed using the same
  binary with `--combat-only --no-build`.
- Step 15 runtime, Utility forge/inspection checks and the 1,000-case world
  corpus passed: `_qa/step16c-forge/runtime.log` and `corpus.log`.
- All relevant source gates passed: Steps 13, 14, 15, 15B, 15C, 15D, forge
  overview, 16A, 16B, 16C and the Phase 1 audit. The new exact-delta projection
  covers 21 production files and rejects 120 mutations while retaining earlier
  baseline hashes and mutation checks.
- The source audit found no generic raw-property-ID shifts, `< 64` identity
  assumptions, native 128-bit dependency, or omitted second-word comparisons.
  Remaining word-0 queries are explicitly limited to pre-existing combat or
  socket properties. `git diff --check` passed.
- Normal `sys/windows/vs/NetHack.sln` Release x64 build passed, including both
  frontends, `recover.exe`, generated resources and the Windows package:
  `_qa/step16c-release.log`. Existing `mkmap.c` uninitialized-local warnings
  remain; no new production compile error was present.

- Full-game save/restore and native checkpoint recovery passed with
  `python test/run_step13_save.py _qa/step16c-loop-final/bin _qa/step16c-save-verified`:
  `_qa/step16c-save-verified.log`. Both restore passes independently verify all
  23 Utility identities, alternating knowledge, paused timers and all existing
  ownership paths. The first recovery attempt used the old epoch-12 recovery
  utility; rebuilding the normal solution resolved that version mismatch. The
  expanded fixture then exposed an obsolete generic success-line count in the
  runner; assertions now independently require both original and Utility
  checks on both restore passes. The final complete command passed.

The original closeout reported all required gates passed. The initial
recipient-model review found the blocker below; the authorized provenance
implementation in the final section resolves it.
Changes are left in the working tree; baseline HEAD and the pre-existing
untracked `.codegraph/` directory are unchanged.

## Initial recipient-model review, 2026-09-26 (superseded below)

The original implementation used a positive eligibility list in
`src/enhance.c:utility_recipient()`. Its default switch result was false.
Although the listed recipients cover the current approved vanilla roster,
this is not the required vanilla allow-by-default model. The documentation's
word "allowlist" was accurate, and passing current-roster tests did not prove
the required default rule.

Production paths consistently reach that helper:

- Generation: `enhancement_generate()` checks `enhancement_eligible()` and
  validates candidates with `enhancement_mask_allowed()`.
- Forge: `forge_inherit()` uses the same eligibility/candidate validation and
  `enhancement_set_mask()`.
- Assignment, including callers from development/test code: the word-0 setter
  delegates to `enhancement_set_mask()`, which checks the shared mask validator.
  No separate wizard Utility assignment path was found in `src/wizcmds.c`.
- Normalization and retained transformations: `socket_normalize()` validates
  each retained property through `enhancement_mask_allowed()`; the normalizer
  and same-type polymorph use this path. Type-changing paths otherwise retain
  their existing stripping semantics, with the documented lamp exception.
- Restore: `restobj()` calls `socket_normalize()` immediately after `Sfi_obj()`.
- Utility property validation reaches `utility_property_allowed()` and then
  `utility_recipient()`. Family restrictions follow general recipient legality.

At this review, the repository did not provide a complete object-origin classifier:
`struct objclass` has class/unique/magic and other gameplay fields, but no
vanilla/custom provenance. `step10b_extension_otyp()` identifies only the
bounded Step 10 extension. `is_mith_mask`, `is_mith_syllable` and `is_mith_slab`
identify specific Mithardir families, not all project-added tools. Git history
confirms earlier additions interleaved with original tool declarations:
Crystal Pick in `ee2c894bd`, Magic Candle in `a3b0ec624`, and Iron Safe in
`5ce8b8193`. Thus a tool-block cutoff is not an authoritative origin marker.
A blacklist of known imports would also fail to establish default denial for
unclassified project additions.

Per the follow-up's explicit instruction to stop when a reliable distinction
requires a material classification decision, gameplay code was not changed.
The outstanding decision is how object definitions should identify vanilla,
project-added, and unknown origins. Explicit definition-level provenance with
unknown origins denied would support both requested defaults, but it is not
an existing repository mechanism and has not been silently introduced.

Rechecked successfully:

- `python test/test_step16c_source.py`, including 120 mutation checks.
- `python test/run_step13.py --step16c-only --no-build --out _qa/step16c-loop-final`,
  using the previously built matching binary; output is recorded in
  `_qa/step16c-recipient-review.log`.

These checks validate the current roster and integration, not an allow-by-
default implementation. No production code, save format, or build input was
changed, so no new production build was necessary. Nothing was committed.
At this point, Step 16C was not closed with respect to the recipient model.

## Authorized object provenance implementation, 2026-09-26

The user authorized the missing origin classification. `enum object_origin`
and `object_origin(int otyp)` now provide read-only object-type metadata beside
the existing object catalog in `src/objects.c`, declared in `include/objclass.h`.
The values are UNKNOWN (zero), VANILLA, and CUSTOM. The lookup explicitly
classifies the entire existing catalog, including non-tools and excluded
vanilla objects. It contains no Utility permissions. Unlisted types, invalid
IDs, and generic display placeholders return UNKNOWN. There are no numeric
origin ranges, object-instance fields, mutable origin bits, or save-layout
changes. Description shuffling and saved object-class data cannot overwrite
provenance.

The vanilla reference is the repository's pre-import `include/objects.h` at
`9191de7079624e94acdde19d07814a0f03ce9450`. Its 463 enabled identities include
the optional mail scroll, which retains its `MAIL_STRUCTURES` guard. Four
deferred `#if 0` definitions are excluded. The 66 project-added identities are
classified CUSTOM. `test/test_object_provenance.py` independently checks both
sets against that historical catalog and current definitions. Adding a new
object without classifying it leaves its origin UNKNOWN and fails closed.

`utility_recipient()` now rejects artifacts, unique objects, invalid/display
types and the complete Step 16C exclusion list, then accepts VANILLA origin
by default. No custom or unknown-origin types have been explicitly approved.
Future approvals belong in Utility policy, not in origin metadata. Crystal
Pick is explicitly excluded in addition to its CUSTOM origin; the separate
property-family check still restricts Excavating to Pick-axe alone. Generation,
forge inheritance, shared assignment (including development callers),
normalization, transformations and restore retain their shared validation
paths through this recipient rule. No separate wizard Utility setter exists.

Follow-up files changed:

- `include/objclass.h`, `src/objects.c`, `src/enhance.c`: provenance contract,
  catalog classification and negative-exclusion recipient policy.
- `test/test_object_provenance.py`, `test/test_step16c_source.py`: origin audit,
  exact exclusions and shared validation-path checks.
- `test/test_step16c.c`, `test/test_step13_runtime.c`, `test/test_step15c.c`:
  recipient, generation, assignment, normalization, native restore and forge
  regression fixtures.
- `test/step16c_historical_changes.json`, `test/step16c_baseline_hashes.json`:
  reviewed production deltas; historical preflight hashes remain anchored to
  the original baseline, with the two newly affected files added.
- `doc/step16c.md`: corrected current behavior and this implementation record.

Validation:

- The new vanilla-as-tool regression failed against the former allowlist,
  then passed after the provenance-based change. A vanilla Arrow is
  temporarily classified as a tool by the native fixture, proving that it
  needs no per-type Utility approval. The fixture restores its native class.
- `python test/run_step13.py --step16c-only --out _qa/step16c-provenance`
  passed. The full existing tool roster is checked independently, including
  all exclusions, imports, unknown generic tools and sole Pick-axe Excavating.
  Native save/restore accepts the newly classified vanilla tool and removes
  injected illegal properties, knowledge and timers from rejected recipients.
  Source audit covers the transformation and shared setter call paths.
- `python test/run_step15.py --out _qa/step16c-provenance-forge` passed,
  including the new default-eligibility and excluded/custom/unknown forge
  cases, all existing forge fixtures and the 1,000-case world corpus.
- `python test/test_step16c_source.py` passed, including the origin audit,
  all shared recipient paths and 124 mutation checks over 23 production files.
- Step 13 through Step 16B source gates, including 15B/15C/15D and forge
  overview, passed. The native combat/knowledge regression passed using
  `--combat-only --no-build --out _qa/step16c-provenance`.
- The affected full `sys/windows/vs/NetHack.sln` Release x64 build passed,
  including console/GUI executables, utilities and package creation. Output:
  `_qa/step16c-provenance-release.log`; package:
  `vspackage/nethack-500-win-x64.zip`. Existing `mkmap.c` uninitialized-local
  warnings remain; no new provenance warnings were reported.
- Independent read-only review found no actionable issues in the provenance
  lookup, recipient policy or new tests. `git diff --check` passed.

The Step 16C recipient-model issue is fully closed. No commits were made;
baseline HEAD remains `3dd0a7aba5080cdbe401968559559d35ae50e5ae`.
