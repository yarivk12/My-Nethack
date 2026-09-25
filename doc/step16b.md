# Phase 1 Step 16B: Defensive / Armor enhancements

Step 16B extends the ordinary enhancement catalogue with twelve armor entries.
Existing capacity, tier probabilities, generation, naming, inspection, explicit
identification and forge inheritance remain shared with Steps 13-16A. These
entries have no new socket/gem mappings or recipes. Utility erosion immunity
and carried-inventory curse protection are not included.

| Family | Tier | Prefix | Suffix |
|---|---:|---|---|
| Warding | 3 | Sanctified | of Warding |
| Casting | 2 | Mystic | of Casting |
| Casting | 4 | Sorcerous | of the Archmage |
| Lightness | 1 | Lightweight | of Lightness |
| Lightness | 2 | Feathered | of Featherweight |
| Lightness | 3 | Weightless | of Weightlessness |
| Reflection | 1 | Barbed | of Thorns |
| Reflection | 2 | Spiked | of Retribution |
| Damage Reduction | 1 | Hardened | of Resilience |
| Damage Reduction | 2 | Stalwart | of Preservation |
| Damage Reduction | 3 | Aegis-Bound | of Aegis |
| Damage Reduction | 4 | Bulwarked | of Safeguarding |

Different tiers of a family cannot coexist on one item. Different families
can coexist within the existing two-property limit. Ordinary artifacts remain
excluded by the existing enhancement eligibility rule.

## Equipment and object behavior

Sanctified blocks the native curse transition for actively equipped objects,
including its own armor, weapons, active offhand, jewelry and equipped tools.
It neither cures existing curses nor protects loose inventory or quivered
ammunition. Sources are binary. A blocked attempt does not choose a replacement
victim and does not identify the property.

Casting eligibility follows the actual penalty-bearing armor categories and
material rules, including nonmetal shields. Mystic halves each item's penalty,
rounding the remaining penalty down; Sorcerous removes it. The shield's separate
heavy-shield chance penalty is reduced independently too. Robe adjustments,
Knight clerical exemptions, staff bonuses, role, skills, stats and level effects
retain their existing behavior. Casting properties identify on hero wear.

Lightness eligibility uses underlying base weight at least 100. The canonical
weight function reduces the otherwise calculated whole object's weight by
30/60/90 percent with floor rounding. Material, size and quantity therefore
participate before Lightness. Recalculation starts from base weight, and
property mutations refresh object and enclosing-container caches. Wear, pickup
and carrying do not identify Lightness.

## Damage boundary

`enhancement_reduce` composes worn DR using integer rational products, capped
at 70 percent, then floors the event's total reduction once. T1/T2 apply only
to physical numerical HP damage; T3/T4 apply to all numerical HP damage.
Mixed events retain physical and nonphysical components. Native aggregate
artifact mitigation keeps its original total; its elemental component is
carried proportionally through that mitigation for Step 16B classification.

The shared helpers are integrated at these existing HP-loss boundaries:

| Native area | Integration |
|---|---|
| Hero ordinary losses | losehp_damage, with explicit physical or nonphysical call sites |
| Monster hits on hero | mdamageu_damage, typed melee, engulf, gaze, spell and passive calls |
| Hero and monster melee | Final HP subtraction after native mitigation; poison and affix components remain distinct |
| Thrown weapons and projectiles | Actual thrower/launcher attribution, direct physical and elemental channels |
| Zaps and spell effects | Existing buzzer or explicit caster, after native resistance |
| Explosions | explode_by preserves an explicit source where available; environmental wrapper has none |
| Potion impacts | potionhit_by carries the actual thrower through physical impact and acid/holy damage |
| Traps, terrain, falling and environmental effects | Explicit physical/nonphysical numerical-loss calls for both hero and monsters |

Maximum-HP changes, level and attribute loss, status effects, item destruction,
clone HP redistribution and instant death remain separate. Explicit transient
fatal markers preserve native numeric death sentinels for beheading, completed
digestion, drowning, deadly poison and draining past level zero. Death rays and
disintegration retain their existing native path. These effects do not receive
percentage DR or produce thorns retaliation.

Barbed/Spiked apply to body armor, shields, helmets, gloves and boots. Reflection
uses positive actual HP loss after DR and adjacency at damage resolution,
regardless of melee/ranged/magic category. Hero percentages add without a cap;
monsters use the strongest source. One floor operation follows percentage
combination. Unattributed environmental events do not acquire a guessed actor.

Reflected damage is direct physical damage, with native physical mitigation
and target DR, without a to-hit roll or contact/passive attack. A scoped guard
suppresses reflection recursion alone. Native death/lifesaving processing is
retained and hero-generated reflection uses native player kill/XP attribution.
Hero lethal damage resolves before reflection can award XP. Dead attackers
cannot continue melee, dereference destroyed launchers or catch returning
weapons. Offensive Vampiric bases include the new post-mitigation DR layer.

Only Casting auto-identifies. All other properties remain unknown until the
existing standard identification path reveals them.

## Current persistent format

The twelve new IDs append after Step 16A; existing IDs are unchanged. Bits
48 through 59 occupy unused space in the existing uint64 property/knowledge
fields. No new persisted object fields are needed and Windows x64 struct obj
remains 152 bytes. EDITLEVEL advances from 11 to 12. Older development saves
and bones are rejected by the normal gate, with no compatibility shim.

## Regression and build commands

```
python test/run_step13.py --out _qa/step16b-diagnostic
python test/run_step15.py
python test/test_step16b_source.py
python test/test_step16a_source.py
python test/test_step13_source.py
python test/test_step14_source.py
python test/test_step15_source.py
python test/test_step15b_source.py
python test/test_step15c_source.py
python test/test_step15d_source.py
python test/test_step15_overview_source.py
python test/run_step13.py --out _qa/step16b-focused --combat-only
python test/run_step13_save.py _qa/step16b-focused/bin _qa/step16b-save-2
```

The native harness adds catalogue/eligibility/weight/cache/curse/casting,
exact DR, mixed mitigation, adjacency, reflection stacking/recursion, player
kill credit, explicit fatal markers, all-property codec, level/bones and full
save/checkpoint cases. Existing Step 16A combat tests, generation RNG traces,
the million-object generation corpus and 5,345,280-state codec corpus remain.
Frozen Step 16B deltas compose with earlier source identity projections and
retain their mutation-rejection checks.

The primary build is `sys/windows/vs/NetHack.sln`, Release, x64, including native
frontends, utilities, generated resources and packages.

Validation completed on 2026-09-25: the full Step 13 runtime suite, Step 15
runtime/generation suite, and all source/projection gates above passed. After
the final oil-source attribution and caster-death propagation changes, the
focused combat suite passed again. Full game save/restore and native
recover.exe checkpoint restore passed after correcting the test container's
cached weight during fixture setup. The final Windows Release x64 solution
build passed and produced both NetHack.exe and NetHackW.exe plus the package.
Logs are in `_qa/step16b-full.log`, `_qa/step16b-step15.log`,
`_qa/step16b-focused.log`, `_qa/step16b-save.log`, and
`_qa/step16b-release.log`.
