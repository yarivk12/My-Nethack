/* NetHack 5.0	objects.c	$NHDT-Date: 1781973059 2026/06/20 16:30:59 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.76 $ */
/* Copyright (c) Mike Threepoint, 1989.                           */
/* NetHack may be freely redistributed.  See license for details. */

#include "config.h"
#include "weight.h"
#include "obj.h"

#include "prop.h"
#include "skills.h"
#include "color.h"
#include "objclass.h"

static struct objdescr obj_descr_init[NUM_OBJECTS + 1] = {
#define OBJECTS_DESCR_INIT
#include "objects.h"
#undef OBJECTS_DESCR_INIT
};

static struct objclass obj_init[NUM_OBJECTS + 1] = {
#define OBJECTS_INIT
#include "objects.h"
#undef OBJECTS_INIT
};

void objects_globals_init(void); /* in hack.h but we're using config.h */

struct objdescr obj_descr[SIZE(obj_descr_init)];
struct objclass objects[SIZE(obj_init)];

void
objects_globals_init(void)
{
    memcpy(obj_descr, obj_descr_init, sizeof(obj_descr));
    memcpy(objects, obj_init, sizeof(objects));
}

/* Origin is object-type metadata, independent of class and gameplay policy.
 * Vanilla identities come from the pre-import objects.h at
 * 9191de7079624e94acdde19d07814a0f03ce9450. Custom identities were added by
 * Lost Tomb (a3b0ec624), Moria (5ce8b8193), Sheol/Mithardir (ee2c894bd),
 * and Step 10 (fdc824dfd). Keep new identities explicit here; unclassified
 * types and generic display placeholders remain UNKNOWN. This read-only
 * metadata is never shuffled with descriptions or loaded from save files.
 */
enum object_origin
object_origin(int otyp)
{
    switch (otyp) {
    case ARROW: case ELVEN_ARROW: case ORCISH_ARROW: case SILVER_ARROW:
    case YA: case CROSSBOW_BOLT: case DART: case SHURIKEN: case BOOMERANG:
    case SPEAR: case ELVEN_SPEAR: case ORCISH_SPEAR: case DWARVISH_SPEAR:
    case SILVER_SPEAR: case JAVELIN: case TRIDENT: case DAGGER:
    case ELVEN_DAGGER: case ORCISH_DAGGER: case SILVER_DAGGER: case ATHAME:
    case SCALPEL: case KNIFE: case STILETTO: case WORM_TOOTH: case CRYSKNIFE:
    case AXE: case BATTLE_AXE: case SHORT_SWORD: case ELVEN_SHORT_SWORD:
    case ORCISH_SHORT_SWORD: case DWARVISH_SHORT_SWORD: case SCIMITAR:
    case SILVER_SABER: case BROADSWORD: case ELVEN_BROADSWORD:
    case LONG_SWORD: case TWO_HANDED_SWORD: case KATANA: case TSURUGI:
    case RUNESWORD: case PARTISAN: case RANSEUR: case SPETUM: case GLAIVE:
    case HALBERD: case BARDICHE: case VOULGE: case FAUCHARD: case GUISARME:
    case BILL_GUISARME: case LUCERN_HAMMER: case BEC_DE_CORBIN:
    case DWARVISH_MATTOCK: case LANCE: case MACE: case SILVER_MACE:
    case MORNING_STAR: case WAR_HAMMER: case CLUB: case RUBBER_HOSE:
    case QUARTERSTAFF: case AKLYS: case FLAIL: case BULLWHIP: case BOW:
    case ELVEN_BOW: case ORCISH_BOW: case YUMI: case SLING: case CROSSBOW:
    case ELVEN_LEATHER_HELM: case ORCISH_HELM: case DWARVISH_IRON_HELM:
    case FEDORA: case CORNUTHAUM: case DUNCE_CAP: case DENTED_POT:
    case HELM_OF_BRILLIANCE: case HELMET: case HELM_OF_CAUTION:
    case HELM_OF_OPPOSITE_ALIGNMENT: case HELM_OF_TELEPATHY:
    case GRAY_DRAGON_SCALE_MAIL: case GOLD_DRAGON_SCALE_MAIL:
    case SILVER_DRAGON_SCALE_MAIL:
    case RED_DRAGON_SCALE_MAIL: case WHITE_DRAGON_SCALE_MAIL:
    case ORANGE_DRAGON_SCALE_MAIL: case BLACK_DRAGON_SCALE_MAIL:
    case BLUE_DRAGON_SCALE_MAIL: case GREEN_DRAGON_SCALE_MAIL:
    case YELLOW_DRAGON_SCALE_MAIL: case GRAY_DRAGON_SCALES:
    case GOLD_DRAGON_SCALES: case SILVER_DRAGON_SCALES:
    case RED_DRAGON_SCALES:
    case WHITE_DRAGON_SCALES: case ORANGE_DRAGON_SCALES:
    case BLACK_DRAGON_SCALES: case BLUE_DRAGON_SCALES:
    case GREEN_DRAGON_SCALES: case YELLOW_DRAGON_SCALES: case PLATE_MAIL:
    case CRYSTAL_PLATE_MAIL: case BRONZE_PLATE_MAIL: case SPLINT_MAIL:
    case BANDED_MAIL: case DWARVISH_MITHRIL_COAT: case ELVEN_MITHRIL_COAT:
    case CHAIN_MAIL: case ORCISH_CHAIN_MAIL: case SCALE_MAIL:
    case STUDDED_LEATHER_ARMOR: case RING_MAIL: case ORCISH_RING_MAIL:
    case LEATHER_ARMOR: case LEATHER_JACKET: case HAWAIIAN_SHIRT:
    case T_SHIRT: case MUMMY_WRAPPING: case ELVEN_CLOAK: case ORCISH_CLOAK:
    case DWARVISH_CLOAK: case OILSKIN_CLOAK: case ROBE: case ALCHEMY_SMOCK:
    case LEATHER_CLOAK: case CLOAK_OF_PROTECTION: case CLOAK_OF_INVISIBILITY:
    case CLOAK_OF_MAGIC_RESISTANCE: case CLOAK_OF_DISPLACEMENT:
    case SMALL_SHIELD: case SHIELD_OF_DRAIN_RESISTANCE:
    case SHIELD_OF_SHOCK_RESISTANCE: case ELVEN_SHIELD: case URUK_HAI_SHIELD:
    case ORCISH_SHIELD: case LARGE_SHIELD: case DWARVISH_ROUNDSHIELD:
    case SHIELD_OF_REFLECTION: case LEATHER_GLOVES:
    case GAUNTLETS_OF_FUMBLING: case GAUNTLETS_OF_POWER:
    case GAUNTLETS_OF_DEXTERITY: case LOW_BOOTS: case IRON_SHOES:
    case HIGH_BOOTS: case SPEED_BOOTS: case WATER_WALKING_BOOTS:
    case JUMPING_BOOTS: case ELVEN_BOOTS: case KICKING_BOOTS:
    case FUMBLE_BOOTS: case LEVITATION_BOOTS: case RIN_ADORNMENT:
    case RIN_GAIN_STRENGTH: case RIN_GAIN_CONSTITUTION:
    case RIN_INCREASE_ACCURACY: case RIN_INCREASE_DAMAGE: case RIN_PROTECTION:
    case RIN_REGENERATION: case RIN_SEARCHING: case RIN_STEALTH:
    case RIN_SUSTAIN_ABILITY: case RIN_LEVITATION: case RIN_HUNGER:
    case RIN_AGGRAVATE_MONSTER: case RIN_CONFLICT: case RIN_WARNING:
    case RIN_POISON_RESISTANCE: case RIN_FIRE_RESISTANCE:
    case RIN_COLD_RESISTANCE: case RIN_SHOCK_RESISTANCE: case RIN_FREE_ACTION:
    case RIN_SLOW_DIGESTION: case RIN_TELEPORTATION:
    case RIN_TELEPORT_CONTROL: case RIN_POLYMORPH: case RIN_POLYMORPH_CONTROL:
    case RIN_INVISIBILITY: case RIN_SEE_INVISIBLE:
    case RIN_PROTECTION_FROM_SHAPE_CHAN: case AMULET_OF_ESP:
    case AMULET_OF_LIFE_SAVING: case AMULET_OF_STRANGULATION:
    case AMULET_OF_RESTFUL_SLEEP: case AMULET_VERSUS_POISON:
    case AMULET_OF_CHANGE: case AMULET_OF_UNCHANGING:
    case AMULET_OF_REFLECTION: case AMULET_OF_MAGICAL_BREATHING:
    case AMULET_OF_GUARDING: case AMULET_OF_FLYING:
    case FAKE_AMULET_OF_YENDOR: case AMULET_OF_YENDOR: case LARGE_BOX:
    case CHEST: case ICE_BOX: case SACK: case OILSKIN_SACK:
    case BAG_OF_HOLDING: case BAG_OF_TRICKS: case SKELETON_KEY:
    case LOCK_PICK: case CREDIT_CARD: case TALLOW_CANDLE: case WAX_CANDLE:
    case BRASS_LANTERN: case OIL_LAMP: case MAGIC_LAMP: case EXPENSIVE_CAMERA:
    case MIRROR: case CRYSTAL_BALL: case LENSES: case BLINDFOLD: case TOWEL:
    case SADDLE: case LEASH: case STETHOSCOPE: case TINNING_KIT:
    case TIN_OPENER: case CAN_OF_GREASE: case FIGURINE: case MAGIC_MARKER:
    case LAND_MINE: case BEARTRAP: case TIN_WHISTLE: case MAGIC_WHISTLE:
    case WOODEN_FLUTE: case MAGIC_FLUTE: case TOOLED_HORN: case FROST_HORN:
    case FIRE_HORN: case HORN_OF_PLENTY: case WOODEN_HARP: case MAGIC_HARP:
    case BELL: case BUGLE: case LEATHER_DRUM: case DRUM_OF_EARTHQUAKE:
    case PICK_AXE: case GRAPPLING_HOOK: case UNICORN_HORN:
    case CANDELABRUM_OF_INVOCATION: case BELL_OF_OPENING: case TRIPE_RATION:
    case CORPSE: case EGG: case MEATBALL: case MEAT_STICK:
    case ENORMOUS_MEATBALL: case MEAT_RING: case GLOB_OF_GRAY_OOZE:
    case GLOB_OF_BROWN_PUDDING: case GLOB_OF_GREEN_SLIME:
    case GLOB_OF_BLACK_PUDDING: case KELP_FROND: case EUCALYPTUS_LEAF:
    case APPLE: case ORANGE: case PEAR: case MELON: case BANANA: case CARROT:
    case SPRIG_OF_WOLFSBANE: case CLOVE_OF_GARLIC: case SLIME_MOLD:
    case LUMP_OF_ROYAL_JELLY: case CREAM_PIE: case CANDY_BAR:
    case FORTUNE_COOKIE: case PANCAKE: case LEMBAS_WAFER: case CRAM_RATION:
    case FOOD_RATION: case K_RATION: case C_RATION: case TIN:
    case POT_GAIN_ABILITY: case POT_RESTORE_ABILITY: case POT_CONFUSION:
    case POT_BLINDNESS: case POT_PARALYSIS: case POT_SPEED:
    case POT_LEVITATION: case POT_HALLUCINATION: case POT_INVISIBILITY:
    case POT_SEE_INVISIBLE: case POT_HEALING: case POT_EXTRA_HEALING:
    case POT_GAIN_LEVEL: case POT_ENLIGHTENMENT: case POT_MONSTER_DETECTION:
    case POT_OBJECT_DETECTION: case POT_GAIN_ENERGY: case POT_SLEEPING:
    case POT_FULL_HEALING: case POT_POLYMORPH: case POT_BOOZE:
    case POT_SICKNESS: case POT_FRUIT_JUICE: case POT_ACID: case POT_OIL:
    case POT_WATER: case SCR_ENCHANT_ARMOR: case SCR_DESTROY_ARMOR:
    case SCR_CONFUSE_MONSTER: case SCR_SCARE_MONSTER: case SCR_REMOVE_CURSE:
    case SCR_ENCHANT_WEAPON: case SCR_CREATE_MONSTER: case SCR_TAMING:
    case SCR_GENOCIDE: case SCR_LIGHT: case SCR_TELEPORTATION:
    case SCR_GOLD_DETECTION: case SCR_FOOD_DETECTION: case SCR_IDENTIFY:
    case SCR_MAGIC_MAPPING: case SCR_AMNESIA: case SCR_FIRE: case SCR_EARTH:
    case SCR_PUNISHMENT: case SCR_CHARGING: case SCR_STINKING_CLOUD:
    case SC01: case SC02: case SC03: case SC04: case SC05: case SC06:
    case SC07: case SC08: case SC09: case SC10: case SC11: case SC12:
    case SC13: case SC14: case SC15: case SC16: case SC17: case SC18:
    case SC19: case SC20:
#ifdef MAIL_STRUCTURES
    case SCR_MAIL:
#endif
    case SCR_BLANK_PAPER: case SPE_DIG:
    case SPE_MAGIC_MISSILE: case SPE_FIREBALL: case SPE_CONE_OF_COLD:
    case SPE_SLEEP: case SPE_FINGER_OF_DEATH: case SPE_LIGHT:
    case SPE_DETECT_MONSTERS: case SPE_HEALING: case SPE_KNOCK:
    case SPE_FORCE_BOLT: case SPE_CONFUSE_MONSTER: case SPE_CURE_BLINDNESS:
    case SPE_DRAIN_LIFE: case SPE_SLOW_MONSTER: case SPE_WIZARD_LOCK:
    case SPE_CREATE_MONSTER: case SPE_DETECT_FOOD: case SPE_CAUSE_FEAR:
    case SPE_CLAIRVOYANCE: case SPE_CURE_SICKNESS: case SPE_CHARM_MONSTER:
    case SPE_HASTE_SELF: case SPE_DETECT_UNSEEN: case SPE_LEVITATION:
    case SPE_EXTRA_HEALING: case SPE_RESTORE_ABILITY: case SPE_INVISIBILITY:
    case SPE_DETECT_TREASURE: case SPE_REMOVE_CURSE: case SPE_MAGIC_MAPPING:
    case SPE_IDENTIFY: case SPE_TURN_UNDEAD: case SPE_POLYMORPH:
    case SPE_TELEPORT_AWAY: case SPE_CREATE_FAMILIAR: case SPE_CANCELLATION:
    case SPE_PROTECTION: case SPE_JUMPING: case SPE_STONE_TO_FLESH:
    case SPE_CHAIN_LIGHTNING:
    case SPE_BLANK_PAPER: case SPE_NOVEL: case SPE_BOOK_OF_THE_DEAD:
    case WAN_LIGHT: case WAN_SECRET_DOOR_DETECTION: case WAN_ENLIGHTENMENT:
    case WAN_CREATE_MONSTER: case WAN_WISHING: case WAN_STASIS:
    case WAN_NOTHING: case WAN_STRIKING: case WAN_MAKE_INVISIBLE:
    case WAN_SLOW_MONSTER: case WAN_SPEED_MONSTER: case WAN_UNDEAD_TURNING:
    case WAN_POLYMORPH: case WAN_CANCELLATION: case WAN_TELEPORTATION:
    case WAN_OPENING: case WAN_LOCKING: case WAN_PROBING: case WAN_DIGGING:
    case WAN_MAGIC_MISSILE: case WAN_FIRE: case WAN_COLD: case WAN_SLEEP:
    case WAN_DEATH: case WAN_LIGHTNING: case WAN1: case WAN2: case WAN3:
    case GOLD_PIECE: case DILITHIUM_CRYSTAL: case DIAMOND: case RUBY:
    case JACINTH: case SAPPHIRE: case BLACK_OPAL: case EMERALD:
    case TURQUOISE: case CITRINE: case AQUAMARINE: case AMBER: case TOPAZ:
    case JET: case OPAL: case CHRYSOBERYL: case GARNET: case AMETHYST:
    case JASPER: case FLUORITE: case OBSIDIAN: case AGATE: case JADE:
    case WORTHLESS_WHITE_GLASS: case WORTHLESS_BLUE_GLASS:
    case WORTHLESS_RED_GLASS: case WORTHLESS_YELLOWBROWN_GLASS:
    case WORTHLESS_ORANGE_GLASS: case WORTHLESS_YELLOW_GLASS:
    case WORTHLESS_BLACK_GLASS: case WORTHLESS_GREEN_GLASS:
    case WORTHLESS_VIOLET_GLASS: case LUCKSTONE: case LOADSTONE:
    case TOUCHSTONE: case FLINT: case ROCK: case BOULDER: case STATUE:
    case HEAVY_IRON_BALL: case IRON_CHAIN: case BLINDING_VENOM:
    case ACID_VENOM:
        return OBJ_ORIGIN_VANILLA;
    case SPIKE: case RAPIER: case CRYSTAL_SWORD: case HIGH_ELVEN_WARSWORD:
    case ELVEN_SICKLE: case MOON_AXE: case GLOWING_DRAGON_SCALE_MAIL:
    case CHROMATIC_DRAGON_SCALE_MAIL: case GLOWING_DRAGON_SCALES:
    case CHROMATIC_DRAGON_SCALES: case LIVING_ARMOR: case BARNACLE_ARMOR:
    case ELVEN_TOGA: case WAR_HAT: case KITE_SHIELD: case HIGH_ELVEN_HELM:
    case ARCHAIC_HELM: case ARCHAIC_PLATE_MAIL: case HIGH_ELVEN_PLATE:
    case BUCKLER: case ARCHAIC_GAUNTLETS: case HIGH_ELVEN_GAUNTLETS:
    case ARCHAIC_BOOTS: case GENTLEMAN_S_SUIT: case GENTLEWOMAN_S_DRESS:
    case JACKET: case STILETTOS: case VICTORIAN_UNDERWEAR: case BLACK_DRESS:
    case IRON_SAFE: case MAGIC_CANDLE: case CRYSTAL_PICK: case LIVING_MASK:
    case MASK: case SYLLABLE_OF_STRENGTH__AESH: case SYLLABLE_OF_POWER__KRAU:
    case SYLLABLE_OF_LIFE__HOON: case SYLLABLE_OF_GRACE__UUR:
    case SYLLABLE_OF_THOUGHT__NAEN: case SYLLABLE_OF_SPIRIT__VAUL:
    case FIRST_WORD: case DIVIDING_WORD: case NURTURING_WORD:
    case UNREFINED_MITHRIL: case FREEZING_ICE: case SICKLE: case SCYTHE:
    case MIRRORBLADE: case KAMEREL_VAJRA: case VIPERWHIP: case RAKUYO:
    case KHAKKHARA: case ROUNDSHIELD: case WITCH_HAT:
    case WHITE_FACELESS_ROBE: case BLACK_FACELESS_ROBE:
    case SMOKY_VIOLET_FACELESS_ROBE: case UNIVERSAL_KEY: case TORCH:
    case SHADOWLANDER_S_TORCH: case DOUBLE_LIGHTSABER: case EYEBALL:
    case POT_AMNESIA: case POT_SPACE_MEAD: case SPE_SECRETS:
    case LIFELESS_DOLL:
        return OBJ_ORIGIN_CUSTOM;
    default:
        return OBJ_ORIGIN_UNKNOWN;
    }
}

/*objects.c*/
