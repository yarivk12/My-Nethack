# Phase 1 Step 18A: Weapon and Armor Forge Progression Chains

## Purpose

Step 18A expands the existing Forge equipment catalogue with ordinary weapon
and armor progression formulas. It uses the existing Forge menu,
recipe representation, ingredient selection, confirmation, transaction, and
output construction paths.

## Final weapon and armor catalogue

The weapon and armor categories contain exactly 23 formulas: 13 weapons and 10
armor recipes. Other Forge categories, if present, are outside this count.

### Weapons (13)

1. `2 Long Swords -> Katana`
2. `Long Sword + Broadsword -> Two-Handed Sword`
3. `Katana + Two-Handed Sword -> Tsurugi`
4. `2 Axes -> Battle-Axe`
5. `Pick-Axe + Dwarvish Short Sword -> Dwarvish Mattock`
6. `Scimitar + Spear -> Trident`
7. `Dagger + Stiletto -> Athame`
8. `Broadsword + Dagger -> Runesword`
9. `2 Knives -> Dagger`
10. `2 Daggers -> Short Sword`
11. `2 Short Swords -> Long Sword`
12. `Spear + Axe -> Halberd`
13. `2 Maces -> Morning Star`

The Battle-Axe formula replaces the obsolete `Axe + Broadsword -> Battle-Axe`
formula. Only the two-Axe formula remains.

### Armor (10)

1. `2 Ring Mails -> Chain Mail`
2. `2 Leather Armors -> Studded Leather Armor`
3. `Studded Leather Armor + Ring Mail -> Scale Mail`
4. `Scale Mail + Chain Mail -> Splint Mail`
5. `Ring Mail + Chain Mail -> Banded Mail`
6. `Splint Mail + Chain Mail -> Plate Mail`
7. `Banded Mail + Chain Mail -> Plate Mail`
8. `2 Small Shields -> Large Shield`
9. `Large Shield + Amulet of Reflection -> Shield of Reflection`
10. `Elven Dagger + Small Shield -> Elven Shield`

The two Plate Mail recipes use distinct exact ingredient formulas and coexist
in the catalogue.

## Progression

The blade chain is `Knife -> Dagger -> Short Sword -> Long Sword`. Long Swords
feed the existing Katana and Two-Handed Sword recipes, and those outputs feed
the existing Tsurugi recipe. Athame and Runesword remain side branches.

Axes combine into a Battle-Axe. The spear branches make a Trident or Halberd,
and two Maces make a Morning Star.

Body armor progresses from Leather Armor through Studded Leather Armor and
Scale Mail. Ring Mail can form Chain Mail or Banded Mail. Scale and Chain Mail
form Splint Mail; either Splint Mail or Banded Mail can then combine with Chain
Mail to form Plate Mail.

Two Small Shields form a Large Shield. The existing Elven Shield side branch
remains available.

## Approved magical-equipment exception

`Large Shield + Amulet of Reflection -> Shield of Reflection` is the sole
Step 18A magical-equipment exception. An Amulet of Reflection is allowed only
as an ingredient in this exact recipe. General amulet crafting and other
magical or miscellaneous recipe expansion remain outside Step 18A.

## Preserved Forge behavior

Recipes still have two exact base-type requirements and make one output.
Ingredient order remains irrelevant, and same-type quantities use the current
two-requirement representation. Forged outputs remain normal ingredients for
later recipes.

Eligibility, menu explanations, output preflight, atomic consumption, turn
cost, sockets, affixes, Essence, and Forge persistence keep their existing
systems. Worn objects, including the Amulet of Reflection, remain excluded by
the generic equipped-item rule.

Step 15C inheritance continues to use its existing reductions for quality,
legal properties, signed `spe`, beatitude, erosion, and erosion-proofing.
Step 18A also makes source-field support explicit in that shared path: an
ineligible source cannot contribute enhancement state, a source whose class
does not support `spe` cannot donate its value, and non-erosion objects cannot
donate erosion state. Output capability rules remain unchanged, and fields
with no supported donor keep the constructor's canonical defaults. Recipe 23
therefore inherits normally from its Large Shield, while the Amulet still
contributes valid beatitude state.

No recipe tree, progression metadata, Forge provenance, new objects, or later
Step 18 recipes were added.

## Validation

Validation passed:

- `py -3 -B test\run_step15.py --out _qa\step18a-catalogue`: exact catalogue
  formula and category checks, all 23 native crafting transactions, both
  forged-output progression chains, inheritance and eligibility regressions,
  socket destruction, existing Step 15C and Step 17 Forge regressions, and the
  1,000-case Phase 1 world corpus.
- `PowerShell: $env:STEP17_ONLY='1'; py -3 -B test\run_step13.py --out _qa\step18a-step17`:
  focused Step 17 native fixtures for affixes, Essence, wishes, persistence,
  menus, and Phase 1 codecs.
- `py -3 -B test\run_step17_source.py`: Step 17 source-scope checks, 336
  mutation rejections, and historical Steps 13 through 16C source checks.
- Release x64 build of `sys\windows\vs\NetHack\NetHack.vcxproj` with isolated
  output directories: passed and produced `NetHack.exe`.

The full multi-project Visual Studio solution build was also attempted, but its
bundled library projects were denied writes to their `.tlog` files. The
affected Release x64 NetHack target built successfully on its own.
