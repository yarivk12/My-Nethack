# Phase 1 Step 16A: offensive affixes

The existing enhancement catalogue now includes Vampiric I-IV, Acid I-III,
Anarchic I-II, Axiomatic I-II and Stoning I-IV. Acquisition uses the existing
uniform tier pools and tier pricing. Primordial remains fire/cold/shock only.
No reserve Slayer, Concordant, Sliming or defensive entries were added.

Ordinary and socket Acid/alignment entries use the same descriptors and combat
functions. Vampiric and Stoning remain ordinary only. Alignment is one mutually
exclusive family across ordinary and socket layers. Vampiric and Stoning each
permit one ordinary tier. Forge inheritance keeps the strongest eligible tier,
then applies the normal two-property limit and catalogue tie-breaking.

Ammo can carry ordinary Acid and Vampiric. Alignment and Stoning exclude ammo;
Stoning also excludes stacked recipients. Existing socket recipient restrictions
remain in effect. Stoning objects never merge, even when all fields match.

## Combat and knowledge

Acid uses the existing direct elemental path and normal acid resistance without
item damage or statuses. Alignment uses current `u.ualign.type` or
`mon_aligntyp()`, preserves A_NONE, and enters the physical channel before
mitigation. Native contact immunities still apply.

Vampiric I/II heal 10/20 percent of final physical damage; III/IV heal 25/30
percent of total immediate direct damage. Positive bases heal at least one,
round down and cap at maximum HP. The basis is not capped to target HP.
`nonliving()` determines target eligibility. Matched launcher/ammunition
sources resolve once using the strongest tier. The native damage handlers
report immediate poison and early pudding-split damage separately, and the
existing branch-weapon effect function reports its physical contribution.

Stoning invokes `do_stone_u()`, `munstone()` and `minstapetrify()`. Native
resistance, stone-golem changes, lifesaving and cures remain in control.
Successful consequences, including consuming a native cure, spend recharge.
Cooldowns are 75/50/25/10 normal turns. No random activation roll is used.

Visible damage, healing and petrification do not establish exact tiers and
therefore do not identify these affixes or elemental affixes. Explicit
identification remains. Existing worn-property observation still requires one
unambiguous property, tier and source. Inspection shows cooldown only for a
known Stoning property and does not advance time.

## Object format and lifecycle

`struct obj` adds `o_stoning_remaining` and `o_stoning_turn`. A single normal-turn
hook visits hero inventory, floor, buried objects and active monster inventories,
recursing through containers. It excludes inactive and migrating chains. The
activation turn is skipped; no elapsed-time catch-up is applied on restore.
Bone imports clear the foreign activation-turn marker while preserving remaining
cooldown. Ordinary saves/checkpoints preserve both fields. Same-type polymorph
preserves identity state; type changes and artifact conversion discard it. Fresh
forge results start Ready.

On Windows x64 the object grows from 144 to 152 bytes. EDITLEVEL advances from
10 to 11, rejecting older saves and bones through the native version gate.
Existing property IDs are unchanged; the fifteen new IDs follow the previous
socket-only entries. New ordinary bits occupy bits 33 through 47.

## Validation

Use the established runners from the repository root:

```text
python test/run_step13.py
python test/run_step15.py
python test/test_step16a_source.py
python test/test_step13_source.py
python test/test_step14_source.py
python test/test_step15_source.py
python test/test_step15b_source.py
python test/test_step15c_source.py
python test/test_step15d_source.py
python test/test_step15_overview_source.py
python test/run_step13_save.py _qa/step13-diagnostic/bin <fresh-output-directory>
```

The linked fixtures cover the new catalogue, family rules, acid dice/resistance,
hero/monster alignment, canonical physical mitigation, Vampiric on nine native
combat paths, real monster-fired launcher ownership, pudding splitting,
life-saved engulfers absorbing and merging projectiles,
Stoning native consequences, cooldown progression, inspection, forge, every
remaining cooldown value, level/bones lifecycle and full checkpoint recovery.
The existing million-object generation corpus and 5,345,280-state codec corpus
remain enabled. `step16a_historical_changes.json` preserves exact reviewed
production deltas for earlier source identity and mutation-rejection gates.

Build the Windows Release x64 `sys/windows/vs/NetHack.sln` target for both native
frontends, utilities, data and packages.
