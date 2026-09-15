#include "hack.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
{
    assert(STEP10B_CTX_NONE == 0);
    assert(step10b_is_outlands_context(STEP10B_CTX_GATE));
    assert(step10b_is_outlands_context(STEP10B_CTX_OUTLANDS_4));
    assert(step10b_is_outlands_context(STEP10B_CTX_SPIRE));
    assert(step10b_is_outlands_context(STEP10B_CTX_SUM));
    assert(!step10b_is_outlands_context(STEP10B_CTX_NONE));
    assert(!step10b_is_outlands_context(STEP10B_CTX_LOST_CITIES));
    assert(!step10b_is_outlands_context(STEP10B_CTX_RLYEH));

    assert(step10b_hero_spell_chance(STEP10B_CTX_NONE, 73, 3) == 73);
    assert(step10b_hero_spell_chance(STEP10B_CTX_GATE, 100, 2) == 80);
    assert(step10b_hero_spell_chance(STEP10B_CTX_OUTLANDS_1, 100, 2) == 60);
    assert(step10b_hero_spell_chance(STEP10B_CTX_OUTLANDS_2, 100, 2) == 40);
    assert(step10b_hero_spell_chance(STEP10B_CTX_OUTLANDS_3, 100, 2) == 20);
    assert(step10b_hero_spell_chance(STEP10B_CTX_OUTLANDS_4, 100, 2) == 0);
    assert(step10b_hero_spell_chance(STEP10B_CTX_SPIRE, 73, 3) == 73);
    assert(step10b_hero_spell_chance(STEP10B_CTX_SUM, 20, 3) == 90);

    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_NONE, 2) == 2);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_GATE, 2) == 4);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_OUTLANDS_1, 2) == 6);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_OUTLANDS_2, 2) == 8);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_OUTLANDS_3, 2) == 10);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_OUTLANDS_4, 2) == 12);
    assert(step10b_mon_spell_fumble_threshold(STEP10B_CTX_SUM, 2) == 1);
    assert(step10b_mon_spell_always_fumbles(STEP10B_CTX_SPIRE));
    assert(!step10b_mon_spell_always_fumbles(STEP10B_CTX_OUTLANDS_4));

    assert(step10b_tree_kick_has_loot(STEP10B_CTX_NONE));
    assert(!step10b_tree_kick_has_loot(STEP10B_CTX_GATE));
    assert(step10b_tree_cut_sticks(STEP10B_CTX_OUTLANDS_2, 5) == 4);
    assert(step10b_tree_cut_sticks(STEP10B_CTX_NONE, 5) == 0);

    assert(step10b_mirror_pit_damage(STEP10B_CTX_NONE, TRUE, 7, 13) == 7);
    assert(step10b_mirror_pit_damage(STEP10B_CTX_GATE, FALSE, 7, 13) == 7);
    assert(step10b_mirror_pit_damage(STEP10B_CTX_SPIRE, TRUE, 7, 13) == 20);
    assert(step10b_trap_projectile_material(STEP10B_CTX_NONE,
                                            0, 0, 0, 0, IRON) == IRON);
    assert(step10b_trap_projectile_material(STEP10B_CTX_GATE,
                                            1, 0, 0, 0, IRON) == METAL);
    assert(step10b_trap_projectile_material(STEP10B_CTX_GATE,
                                            0, 1, 0, 0, METAL) == IRON);
    assert(step10b_trap_projectile_material(STEP10B_CTX_GATE,
                                            0, 0, 1, 0, METAL) == COPPER);
    assert(step10b_trap_projectile_material(STEP10B_CTX_GATE,
                                            0, 0, 0, 1, METAL) == SILVER);
    assert(step10b_trap_projectile_material(STEP10B_CTX_GATE,
                                            0, 0, 0, 0, METAL) == GOLD);

    assert(step10b_terrain_color(STEP10B_CTX_NONE, S_vwall, CLR_RED)
           == CLR_RED);
    assert(step10b_terrain_color(STEP10B_CTX_GATE, S_vwall, CLR_GRAY)
           == CLR_BROWN);
    assert(step10b_terrain_color(STEP10B_CTX_SUM, S_room, CLR_GRAY)
           == CLR_BROWN);
    assert(step10b_terrain_color(STEP10B_CTX_SUM, S_darkroom, CLR_GRAY)
           == CLR_BLACK);
    assert(step10b_terrain_color(STEP10B_CTX_LOST_CITIES, S_hwall, CLR_GRAY)
           == CLR_BLACK);
    assert(step10b_terrain_color(STEP10B_CTX_LOST_CITIES, S_room, CLR_RED)
           == CLR_GRAY);
    assert(step10b_terrain_color(STEP10B_CTX_RLYEH, S_tlwall, CLR_GRAY)
           == CLR_BRIGHT_BLUE);
    assert(step10b_terrain_color(STEP10B_CTX_RLYEH, S_darkroom, CLR_GRAY)
           == CLR_BLUE);
    assert(step10b_terrain_color(STEP10B_CTX_RLYEH, S_tree, CLR_GREEN)
           == CLR_GREEN);
    assert(step10b_terrain_color(STEP10B_CTX_GATE, S_room, NO_COLOR)
           == NO_COLOR);

    assert(step10b_lethe_otyp(SCROLL_CLASS, SCR_IDENTIFY, FALSE, 0)
           == SCR_AMNESIA);
    assert(step10b_lethe_otyp(SCROLL_CLASS, SCR_IDENTIFY, FALSE, 1)
           == SCR_BLANK_PAPER);
    assert(step10b_lethe_otyp(SCROLL_CLASS, STEP10B_SCR_RESISTANCE,
                              FALSE, 0) == STEP10B_SCR_RESISTANCE);
    assert(step10b_lethe_otyp(SCROLL_CLASS, SCR_IDENTIFY, TRUE, 0)
           == SCR_IDENTIFY);
    assert(step10b_lethe_otyp(SPBOOK_CLASS, SPE_FORCE_BOLT, FALSE, 0)
           == SPE_BLANK_PAPER);
    assert(step10b_lethe_otyp(SPBOOK_CLASS, SPE_FORCE_BOLT, TRUE, 0)
           == SPE_FORCE_BOLT);
    assert(step10b_lethe_otyp(POTION_CLASS, POT_WATER, FALSE, 0)
           == POT_AMNESIA);
    assert(step10b_lethe_otyp(POTION_CLASS, POT_AMNESIA, FALSE, 0)
           == POT_AMNESIA);
    assert(step10b_lethe_otyp(POTION_CLASS, POT_HEALING, FALSE, 0)
           == POT_WATER);
    assert(step10b_lethe_otyp(POTION_CLASS, POT_ACID, FALSE, 0)
           == STEP10B_LETHE_DESTROYED);
    assert(step10b_lethe_marker_spe(20, 0) == 17);
    assert(step10b_lethe_marker_spe(5, 9) == 0);
    assert(step10b_lethe_drain_spe(WEAPON_CLASS, FALSE, 3) == 2);
    assert(step10b_lethe_drain_spe(TOOL_CLASS, TRUE, 1) == 0);
    assert(step10b_lethe_drain_spe(TOOL_CLASS, FALSE, 3) == 3);

    puts("PASS Step 10B4 deterministic environment helpers");
    return 0;
}
