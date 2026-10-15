# Step 22 ability catalogue and native dispatch audit

The persistent key is one delivery method plus one special damage/effect ID.
`AD_SPEL`, `AD_CLRC`, and `AD_PSON` normalize to their full `AT_MAGC`
channel. Equivalent repeated slots are deduplicated before uniform selection.
Only one key is saved on a monster, alongside species and an absolute world-turn
deadline. Reapplication keeps the later of the existing deadline and a new 2d3
deadline. No global species descriptor or cancellation flag is changed.

The catalogue follows native `SEDUCE=0` substitutions (`c_sa_no` for the
succubus/incubus list, and individual `AD_SSEX` to `AD_DRLI` replacements).
Consecutive disease/hunger attacks that can become stun after a previous hit
also expose their concrete stun delivery/effect capability. Retained native
shapechanger identity (`cham`) exposes transformation even while the current
form is an ordinary wolf, bat or fog cloud. The current species remains the
saved mute identity, so an actual species change clears that mute.

This is a source map. Runtime evidence belongs to the Step 22 verification
matrix and build logs; inclusion in this table does not itself establish PASS.

| Capability | Catalogue identity | Native execution boundaries |
| --- | --- | --- |
| Mixed melee special riders | Actual attack delivery/effect, excluding physical-only slots | `getmattk`, `mhitm_adtyping`, hero and monster hit dispatch in `mhitu.c` and `mhitm.c`; replacement retains physical dice |
| Imported Mithardir drain, desiccation, disintegration, ice, web and status riders | Their actual delivery/effect | `mhitm_adtyping` dispatch before `mith_drain_attack`, `mith_desiccate`, `mith_disintegrate`, and Step 20 handlers; separately dispatched Ghaunadaur web uses innate web key |
| Coure weapon sleep | `AT_WEAP` + `AD_SLEE` | `mhitm_adtyping`; `mith_coure_sleep` also checks the key at its deferred post-damage execution boundary |
| Gaze, including imported wisdom drain, paralysis, mist and blast | `AT_GAZE` + actual effect | `gazemu`, `gazemm`; imported `AT_WDGZ` normalizes to the concrete gaze delivery |
| Black-Goat composite attacks | Distinct possible concrete delivery/effect outputs | `getmattk` resolves `AT_BKGT` and `AT_BKG2`; catalogue explicitly expands the legal special outputs, then deduplicates overlaps |
| Breath and spit | Actual delivery/effect | `breamm` and `spitmm`, shared by hero-targeted and monster-targeted wrappers |
| Internal silver/loadstone projectiles | `AT_ARRW` + actual effect | `mith_internal_projectile` before creating or launching an object |
| Active proximity explosion | `AT_EXPL` + effect | `explmu`, `explmm`; death-triggered `AT_BOOM` is intentionally excluded |
| Native and imported spells | One complete `AD_SPEL`, `AD_CLRC`, or `AD_PSON` channel | `castmu`, `buzzmu`, `mith_castmm`; elemental magical attacks retain their own effect key |
| Alabaster dynamically substituted spell attack | Full `AD_SPEL` channel when that individual's syllable/identity actually grants it | `getmattk` substitution, native cast entrypoints and `mith_castmm` |
| Contact passives, including living cockatrice | `AT_NONE` + effect | `passive`, `passivemm`, plus the pre-damage living-contact stoning check in `mdamagem` |
| Innate teleport, including covetous teleport tactics | `EX_INNATE_TELEPORT` | fleeing and Tengu paths in `monmove.c`, liquid escape in `mon.c`, `tactics` in `wizard.c`; external wand/trap teleport remains native |
| Demon/were innate summons and witch familiar creation | `EX_INNATE_SUMMON` | `summonmu`, familiar creation gate in `dochug`; spell-channel summoning remains part of its spell key |
| Innate transformation, lycanthropy and imported fey shifts | `EX_INNATE_TRANSFORM` | `m_calcdistress`, `decide_to_shapeshift`, `were_change`, `new_were`, `can_fog`, and `vamp_shift` |
| Fern spore release | Existing `AT_GAZE` + `AD_SPOR` | `gazemu` before `fern_release`; it is not duplicated as an innate summon candidate |
| Shrieker and Erinys vocal alert | `EX_INNATE_SHRIEK` | `m_respond` before shrieking or Erinys aggravation |
| Imported group coordination | `EX_INNATE_GROUP` | `dochug` before `sheol_chillbug_turn` |
| Drider/Ghaunadaur standalone web | `EX_INNATE_WEB` | ranged hero and monster attack dispatch; Ghaunadaur's separate web in `mhitm_adtyping` |
| First Wraithworm wind | `EX_INNATE_WIND` | `m_calcdistress` before `mith_worm_wind` |
| Hezrou stench and steam-vortex vapor | `EX_INNATE_STENCH` | `m_postmove_effect` before native cloud creation |
| Imported Shadow darkening | `EX_INNATE_DARKEN` | `m_postmove_effect` before changing the tile's illumination |
| Mind-flayer and imported Cthulhu psychic blast | `EX_INNATE_PSYCHIC` | `mind_blast` and the Cthulhu dispatch gate in `dochug`; this is distinct from an `AD_PSON` spell channel |

For a hero's triggering contact, the original species and pre-existing mute are
captured before damage. Native passive processing consumes this snapshot, so a
new mute or polymorph does not cancel the original contact's consequences.
Living-monster muting does not affect dangerous corpses or other inventory
objects. Genuine transformation clears mute while retaining anti-regeneration.
Protected progression actors are filtered before mute selection.

Automatic monster HP regeneration is suppressed independently of mute, both in
`mon_regen` and in native off-level catch-up. Active healing, drain healing,
lifesaving, transformations and resurrection keep their native behavior.
