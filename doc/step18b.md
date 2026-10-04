# Phase 1 Step 18B: Magical Equipment and Tools Forge Recipes

## Catalogue

Step 18B adds exactly 24 formulas with 24 distinct outputs to the 23 unchanged
Step 18A formulas. Every formula consumes one of each ingredient and produces
one ordinary object. Ingredient order is irrelevant.

### Helmets

1. `Helmet + Potion of Gain Ability -> Helm of Brilliance`
2. `Helmet + Ring of Warning -> Helm of Caution`
3. `Helmet + Amulet of ESP -> Helm of Telepathy`

### Cloaks

4. `Leather Cloak + Amulet of Guarding -> Cloak of Protection`
5. `Leather Cloak + Ring of Invisibility -> Cloak of Invisibility`
6. `Leather Cloak + Amulet of Unchanging -> Cloak of Magic Resistance`
7. `Leather Cloak + Ring of Teleportation -> Cloak of Displacement`
8. `Robe + Amulet versus Poison -> Alchemy Smock`

### Gloves

9. `Leather Gloves + Ring of Gain Strength -> Gauntlets of Power`
10. `Leather Gloves + Ring of Increase Accuracy -> Gauntlets of Dexterity`
11. `Leather Gloves + Potion of Confusion -> Gauntlets of Fumbling`

### Boots

12. `Low Boots + Ring of Stealth -> Elven Boots`
13. `Elven Boots + Spellbook of Haste Self -> Speed Boots`
14. `High Boots + Amulet of Magical Breathing -> Water Walking Boots`
15. `High Boots + Spellbook of Jumping -> Jumping Boots`
16. `Iron Shoes + Ring of Increase Damage -> Kicking Boots`
17. `High Boots + Potion of Confusion -> Fumble Boots`
18. `Low Boots + Ring of Levitation -> Levitation Boots`

### Shields

19. `Large Shield + Spellbook of Drain Life -> Shield of Drain Resistance`
20. `Large Shield + Ring of Shock Resistance -> Shield of Shock Resistance`

### Magical tools

21. `Sack + Ring of Levitation -> Bag of Holding`
22. `Tin Whistle + Scroll of Taming -> Magic Whistle`
23. `Wooden Flute + Scroll of Taming -> Magic Flute`
24. `Wooden Harp + Scroll of Taming -> Magic Harp`

## State and generation

Recipe output types are deterministic. Enchantments inherit the existing
highest signed value from weapons, armor, weapon-tools, and charged rings
whose `spe` is a stat modifier. Wands and charged non-weapon tools never donate
their consumable charges. Recharge history is not inherited.

Magic Flute and Magic Harp receive fresh native randomized `4..8` charges.
The native magical-instrument charge initializer is shared with ordinary object
generation and called by target type from Forge construction. Magic Whistle
has no charges. Other unrelated native generation rules are unchanged; adding
future charged output types requires sharing their native rule in this seam.

Forge retains canonical `mksobj(..., FALSE, FALSE)` construction, followed by
charge initialization and the existing supported-state inheritance. It does
not randomly initialize enchantment, BUC, erosion, poison, container contents,
locks, or traps. Quality, legal ordinary affixes, BUC precedence, erosion, and
proofing retain their existing capability checks and reductions.

Only eligible top-level carried objects can be consumed. Equipment must be
removed normally first. Identification, artifacts/protected objects, shop
ownership, and relinquishment checks remain unchanged. Partial stacks retain
all remaining state except quantity and weight. Consumed sockets are destroyed.

A Sack must be empty. Rejection leaves its contents untouched; the forged Bag
of Holding is a new empty container. Forged outputs may feed later exact-type
recipes, including `Low Boots -> Elven Boots -> Speed Boots`.

No save/bones fields, recipe provenance, object IDs, or EDITLEVEL change.

## Validation

Validation passed:

- `py -3 -B test/run_step15.py --out _qa/step18b-step15`: all 47 native
  crafting transactions, the 24 new formulas in both orders, distinct outputs,
  partial stacks, worn jewelry, container rejection without mutation, the
  boots chain, socket destruction, signed enchantments, and high-charge
  exclusion. Seeded RNG checks compare exact native charge draws and the next
  RNG value, including direct coverage of the shared initializer. Existing
  Forge inheritance, eligibility, preflight, affix, and socket fixtures pass.
- The same runner's fresh-engine Phase 1 world corpus: 1,000 cases, 129
  selections, 127 placements, 2 without candidates, 0 invalid placements,
  and 0 multiple placements.
- `$env:STEP17_ONLY='1'; py -3 -B test/run_step13.py --out _qa/step18b-step17`:
  focused native Step 17/17.5 fixtures, Forge operations, wishes, Essence,
  affix history, menus, and real save/bones codecs pass.
- `py -3 -B test/run_step17_source.py`: Step 18B catalogue/native contracts
  and 9 exact-delta mutation rejections, all 336 Step 17 mutation rejections,
  and the historical Steps 13 through 16C source gates pass. Exact reviewed
  Step 18B deltas are projected out only for the historical gates; native
  fixtures execute the current sources.
- Release x64 `sys/windows/vs/NetHack/NetHack.vcxproj` with isolated
  `BinDir`, `ObjDir`, and `SymbolsDir` under `_qa/step18b-release` produced
  `bin/NetHack.exe`. The build used `env=dict(os.environ)`, matching the native
  test runners, to normalize duplicate Windows `PATH`/`Path` entries.
- `git diff --check`, new-file whitespace checks, and final scope review pass.

The new catalogue fixture first failed against the 23-recipe baseline, then
passed with the implementation. No unresolved validation blocker remains.
