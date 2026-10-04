# Phase 1 Step 17.6: Forge Menu Terminology

## Purpose

Remove the confusing `Forge -> Forge an item` naming. The `Forge an item`
choice could read as a description of the whole Forge rather than the specific
equipment-crafting action. Clarify the visible menu choices without changing
how any operation works.

## Integration

The top-level Forge menu uses these labels in the existing order:

| Previous label | Current label | Menu ID and shortcut |
| --- | --- | --- |
| `Forge an item` | `Craft Equipment` | 1, `f` |
| `Socket gemstone` | `Socket Gems` | 2, `s` |
| `Affix Crafting` | `Manage Affixes` | 3, `a` |
| `Salvage item` | `Salvage Equipment` | 4, `v` |
| `Leave Forge` | `Leave Forge` | 5, `l` |

The separate affix-management submenu title was also renamed from `Affix
Crafting` to `Manage Affixes`. Its action labels remain `Add Random Affix`,
`Reroll Affix`, `Extract Affix`, `Imprint Affix`, `View Stored Affix Essence`,
and `Back`. The distinct `Socket gemstone` salvage-confirmation text remains
unchanged.

The menu IDs, shortcuts, and dispatch paths are unchanged. This step changes
visible labels only; it does not change gameplay, recipes, eligibility, costs,
randomness, persistence, turn use, or EDITLEVEL.

## Validation

Regression coverage asserts the exact top-level label order and menu IDs, keeps
the existing action dispatch checks, and verifies the affix submenu heading
and action order. The Step 15B menu fixture now recognizes `Socket Gems` as the
top-level shortcut while continuing to check its separate confirmation label.
The Step 17 historical source projection records these label-only changes so
the existing historical source checks remain applicable.

Validation passed:

- Focused Step 17 x64 native fixtures: `$env:STEP17_ONLY='1'; py -3 -B test\run_step13.py --out _qa\step176-focused`.
- Step 15 native runtime fixtures and 1,000-case corpus: `py -3 -B test\run_step15.py --out _qa\step176-step15`.
- `py -3 -B test\run_step17_source.py` passed the Step 17 integration contracts, including the wished-item hook and Forge menu checks, and the Step 13 through Step 16C historical source and mutation gates against the Step 17 source projection.
- `git diff --check`.

## Status

Implementation, regression coverage, source contracts, and documentation are
complete. No gameplay behavior changed.
