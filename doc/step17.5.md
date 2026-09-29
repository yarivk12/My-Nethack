# Phase 1 Step 17.5: Wished Item Enhancement Generation

## Purpose

Step 17.5 connects successful physical wishes to the existing natural
enhancement generator while preserving NetHack wish semantics. Quality and
ordinary affixes are not wishable properties: a player cannot explicitly
request them through wish syntax. An eligible single-item wish receives one
normal natural enhancement-generation opportunity. That opportunity can
produce no enhancement, depending on the canonical generator's rules.

## Integration

The hook is in the real wish path, `makewish()` in `src/zap.c`. It runs after
`readobjnam()` has produced the physical object and the parser's requested
object state has been applied, including type, quantity, `+N`, beatitude,
proofing, and naming. Artifact bookkeeping is also finalized before the
generation call. The call is:

```c
enhancement_generate(otmp, depth(&u.uz));
```

It runs before `doname()` builds the wish-result presentation and log entry,
and before the later inventory or floor delivery. The non-object sentinel
returns for `nothing` and `hands_obj` occur before the hook.

`readobjnam()` remains an unchanged generic object parser. It does not perform
wish-only enhancement work, and its non-wish callers are unaffected.

## Canonical ownership

Wish code does not duplicate enhancement eligibility, probability, or
candidate-selection rules. The existing `enhancement_generate()` path remains
authoritative for depth bands, Quality, affix presence, slot count, tiers,
candidate legality, magnitude, and knowledge state. Its existing eligibility
and generation rules also govern stacks, ammunition, artifacts, unique items,
and other exclusions. Step 17.5 supplies the wished object and current dungeon
depth, then leaves those decisions to the canonical generator.

## Wish behavior

Existing `+N`, quantity, blessed/uncursed/cursed state, proofing, artifact,
naming, and other wish behavior remains governed by the normal parser and wish
path. Wish-requested properties and Phase 1 Quality/affixes are independent
layers: natural enhancement generation does not convert them into explicit
wish options or replace the requested properties.

The wish path invokes the generator once for a physical object that reaches
normal wish delivery. A wished stack is not split and does not receive
per-item rolls. The canonical generator rejects quantities other than one
before its generation roll, so stacked wishes retain their existing stack
semantics and receive no enhancement roll.

## Deterministic tests

`test/test_step17.c` adds fixed-seed integration coverage through the actual
`makewish()` path. It checks a blessed `+3` long sword and an eligible
pick-axe against replayed canonical generation, including use of
`depth(&u.uz)` and preservation of unknown enhancement knowledge. It also
covers an Essence stack, an eligible dagger stack, ammunition, Excalibur, the
Amulet of Yendor, and the `nothing` and `hands_obj` early returns. Parser
checks confirm that Quality and ordinary affixes cannot be explicitly
requested. The assertions verify the hook count and that excluded or stacked
objects gain no enhancement state.

`test/run_step17_source.py` enforces the source contract: one canonical call in
`makewish()`, after the `hands_obj` return and before wish naming and delivery,
with no duplicated eligibility/context logic in the wish path and no
generation call in `readobjnam()`.

## Validation

The following completed checks passed:

| Check | Result |
| --- | --- |
| PowerShell: `$env:STEP17_ONLY='1'; py -3 -B test\run_step13.py --out _qa\step175-focused` (focused Step 17 native x64 fixtures) | PASS |
| PowerShell: `Remove-Item Env:STEP17_ONLY -ErrorAction SilentlyContinue; py -3 -B test\run_step13.py --out _qa\step175-cumulative` (cumulative Step 13 native runtime suite, seeded generation corpus, and persistence coverage) | PASS |
| `py -3 -B test\run_step17_source.py` (Step 13 through Step 17.5 source contracts and historical mutation checks) | PASS |

The final focused x64 Step 17 run included the completed wished-item fixtures.
The source checks also verify the production call's ordering and scope.
No tests were rerun for this documentation-only correction. The PASS results
record the earlier implementation validation; the corrected command strings
above are provided for reproducibility and do not claim new runs.

## Status

Implementation and validation are complete. At the time of this documentation
update, the implementation and test changes remain uncommitted.
