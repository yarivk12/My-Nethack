# My-Nethack

## Personal NetHack 5.0 Expansion

This repository contains a private NetHack 5.0 expansion project.

### Current milestone: Expanded Dungeons of Doom

The structural dungeon expansion milestone is complete.

Current implementation:

- Dungeons of Doom expanded to 200 levels
- Castle placed at Dungeons of Doom level 200
- Existing vanilla dungeon topology and special-level placement preserved
- Physical dungeon depth widened beyond the original signed 8-bit assumptions
- Save/restore and recovery level identifiers widened to signed 16-bit values
- Ledger handling hardened beyond the previous 255-level runtime limitation
- Current audited runtime ledger capacity: 3199
- Deep-level save/restore validated
- Castle, Gehennom, and Vlad's Tower transitions validated
- Windows x86 and x64 builds validated
- No new gameplay content has been added yet

Baseline commit:

`64db689a1` - `Expand dungeon depth and harden ledger capacity`

Baseline tag:

`expanded-dod-200-baseline`

### Development strategy

Development is phased and conservative.

The project uses vanilla NetHack 5.0 as the base and prefers adapting existing code and content from established NetHack variants rather than creating new systems from scratch.

The completed first milestone focused exclusively on safely expanding the existing Dungeons of Doom before introducing new content.
