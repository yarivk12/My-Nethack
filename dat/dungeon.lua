-- NetHack 3.6	dungeon dungeon.lua	$NHDT-Date: 1781994881 2026/06/20 22:34:41 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.11 $
-- Copyright (c) 1990-95 by M. Stephenson
-- NetHack may be freely redistributed.  See license for details.
--
-- Step 6B modified 2026-09-06: randomized imported room/branch placement.
-- The dungeon description file.
dungeon = {
   {
      name = "The Dungeons of Doom",
      bonetag = "D",
      base = 200,
      alignment = "unaligned",
      themerooms = "themerms.lua",
      branches = {
         { name="Sheol", base=30, range=170, direction="down" },
         { name="The Dragon Caves", base=30, range=170, direction="down" },
         { name="Mithardir", base=30, range=170, branchtype="portal" },
         { name="Neutral Quest", base=30, range=170, branchtype="portal" },
         {
            name = "The Gnomish Mines",
            base = 2,
            range = 3
         },
         {
            name = "Sokoban",
            chainlevel = "oracle",
            base = 1,
            direction = "up"
         },
         {
            name = "The Quest",
            chainlevel = "oracle",
            base = 6,
            range = 2,
            branchtype = "portal"
         },
         {
            name = "Fort Ludios",
            base = 18,
            range = 4,
            branchtype = "portal"
         },
         {
            name = "Gehennom",
            chainlevel = "castle",
            base = 0,
            branchtype = "no_down"
         },
         {
            name = "The Elemental Planes",
            base = 1,
            branchtype = "no_down",
            direction = "up"
         },
         {
            name = "The Lost Tomb",
            base = 30,
            range = 170,
            direction = "down"
         },
         {
            name = "The Temple of Moloch",
            base = 30,
            range = 170,
            direction = "down"
         },
         {
            name = "The Ruins of Moria",
            base = 30,
            range = 170,
            direction = "up"
         }
      },
      levels = {
         { name = "chalv2", base = 110 },
         {
            name = "rogue",
            bonetag = "R",
            base = 15,
            range = 4,
            flags = "roguelike",
         },
         {
            name = "oracle",
            bonetag = "O",
            base = 5,
            range = 5,
            alignment = "neutral"
         },
         {
            name = "bigrm",
            bonetag = "B",
            base = 10,
            range = 3,
            chance = 40,
            nlevels = 14
         },
         {
            name = "medusa",
            base = -5,
            range = 4,
            nlevels = 4,
            alignment = "chaotic"
         },
         {
            name = "castle",
            base = -1
         },
         {
            name = "neulev",
            base = 30,
            range = 170
         },
      }
   },
   {
      name = "Gehennom",
      bonetag = "G",
      base = 20,
      range = 5,
      flags = { "mazelike", "hellish" },
      lvlfill = "hellfill",
      alignment = "noalign",
      branches = {
         {
            name = "Vlad's Tower",
            base = 9,
            range = 5,
            direction = "up"
         }
      },
      levels = {
         {
            name = "valley",
            bonetag = "V",
            base = 1
         },
         {
            name = "sanctum",
            base = -1
         },
         {
            name = "juiblex",
            bonetag = "J",
            base = 4,
            range = 4
         },
         {
            name = "baalz",
            bonetag = "B",
            base = 6,
            range = 4
         },
         {
            name = "asmodeus",
            bonetag = "A",
            base = 2,
            range = 6
         },
         {
            name = "wizard1",
            base = 11,
            range = 6
         },
         {
            name = "wizard2",
            bonetag = "X",
            chainlevel = "wizard1",
            base = 1
         },
         {
            name = "wizard3",
            bonetag = "Y",
            chainlevel = "wizard1",
            base = 2
         },
         {
            name = "orcus",
            bonetag = "O",
            base = 10,
            range = 6
         },
         {
            name = "fakewiz1",
            bonetag = "F",
            base = -6,
            range = 4
         },
         {
            name = "fakewiz2",
            bonetag = "G",
            base = -6,
            range = 4
         },
      }
   },
   {
      name = "The Gnomish Mines",
      bonetag = "M",
      base = 8,
      range = 2,
      alignment = "lawful",
      flags = { "mazelike" },
      lvlfill = "minefill",
      levels = {
         {
            name = "minetn",
            bonetag = "T",
            base = 3,
            range = 2,
            nlevels = 7,
            flags = "town"
         },
         {
            name = "minend",
--          5.0.0: minend changed to no-bones to simplify achievement tracking
--          bonetag = "E"
            base = -1,
            nlevels = 3
         },
      }
   },
   {
      name = "The Quest",
      bonetag = "Q",
      base = 5,
      range = 2,
      levels = {
         {
            name = "x-strt",
            base = 1,
            range = 1
         },
         {
            name = "x-loca",
            bonetag = "L",
            base = 3,
            range = 1
         },
         {
            name = "x-goal",
            base = -1
         },
      }
   },
   {
      name = "Sokoban",
      base = 4,
      alignment = "neutral",
      flags = { "mazelike" },
      entry = -1,
      levels = {
         {
            name = "soko1",
            base = 1,
            nlevels = 2
         },
         {
            name = "soko2",
            base = 2,
            nlevels = 2
         },
         {
            name = "soko3",
            base = 3,
            nlevels = 2
         },
         {
            name = "soko4",
            base = 4,
            nlevels = 2
         },
      }
   },
   {
      name = "Fort Ludios",
      base = 1,
      bonetag = "K",
      flags = { "mazelike" },
      alignment = "unaligned",
      levels = {
         {
            name = "knox",
            bonetag = "K",
            base = -1
         }
      }
   },
   {
      name = "Vlad's Tower",
      base = 3,
      bonetag = "T",
      protofile = "tower",
      alignment = "chaotic",
      flags = { "mazelike" },
      entry = -1,
      levels = {
         {
            name = "tower1",
            base = 1
         },
         {
            name = "tower2",
            base = 2
         },
         {
            name = "tower3",
            base = 3
         },
      }
   },
   {
      name = "The Elemental Planes",
      bonetag = "E",
      base = 6,
      alignment = "unaligned",
      flags = { "mazelike" },
      entry = -2,
      levels = {
         {
            name = "astral",
            base = 1
         },
         {
            name = "water",
            base = 2
         },
         {
            name = "fire",
            base = 3
         },
         {
            name = "air",
            base = 4
         },
         {
            name = "earth",
            base = 5
         },
         {
            name = "dummy",
            base = 6
         },
      }
   },
   {
      name = "The Tutorial",
      base = 2,
      flags = { "mazelike", "unconnected" },
      levels = {
         {
            name = "tut-1",
            base = 1,
         },
         {
            name = "tut-2",
            base = 2,
         },
      }
   },
   -- Enrichment branches follow vanilla dungeons. Step 7 uses a new save epoch.
   {
      name = "The Lost Tomb",
      base = 1,
      bonetag = "Z",
      flags = { "mazelike" },
      alignment = "chaotic",
      levels = { { name = "tomb-1", bonetag = "Z", base = 1 } }
   },
   {
      name = "The Temple of Moloch",
      base = 1,
      flags = { "mazelike" },
      alignment = "chaotic",
      levels = { { name = "moloch", base = 1 } }
   },
   {
      name = "The Ruins of Moria",
      base = 6,
      entry = -1,
      flags = { "mazelike" },
      alignment = "unaligned",
      -- Choose equal-weight variants once; exact names survive in sp_levchn.
      -- This also identifies each variant's runtime monster-generation rule.
      levels = {
         { name = "moria6-" .. math.random(1,2), base = 1 },
         { name = "moria5-1", bonetag = "5", base = 2 },
         { name = "moria4-" .. math.random(1,4), bonetag = "4", base = 3 },
         { name = "moria3-1", bonetag = "3", base = 4 },
         { name = "moria2-1", bonetag = "2", base = 5 },
         { name = "moria1-1", bonetag = "1", base = 6 }
      }
   },
   {
      name = "Sheol", bonetag = "S", base = 6, range = 2,
      alignment = "unaligned", lvlfill = "sheolfil",
      levels = {
         { name = "sheolmid", bonetag = "H", base = 2 },
         { name = "palace_f", bonetag = "P", base = -2 },
         { name = "palace_e", bonetag = "U", base = -1 }
      }
   },
   {
      name = "The Dragon Caves", bonetag = "D", base = 4,
      flags = { "mazelike" }, alignment = "chaotic",
      levels = {
         { name = "drgnA", bonetag = "D", base = 1 },
         { name = "drgnB", bonetag = "D", base = 2, flags = { "town" } },
         { name = "drgnC", bonetag = "D", base = 3 },
         { name = "drgnD", bonetag = "D", base = 4 }
      }
   },
   {
      name = "Mithardir", bonetag = "M", base = 10,
      alignment = "chaotic",
      levels = {
         { name = "ossa1", base = 1, flags = { "town" } },
         { name = "mith1", base = 2 },
         { name = "mith2", base = 3 },
         { name = "mith3", base = 4 },
         { name = "cat1", base = 5 },
         { name = "cat2", base = 6 },
         { name = "cat3", base = 7 }
      }
   },
   {
      -- Seven logical Neutral floors plus the same-dnum Dispensary slot.
      name = "Neutral Quest", bonetag = "N", base = 8,
      flags = { "mazelike" }, alignment = "neutral",
      branches = {
         { name="The Lost Cities", chainlevel="sumall", base=0, direction="down" }
      },
      levels = {
         { name = "gatetwn", base = 1 },
         { name = "out1", bonetag = "A", base = 2 },
         { name = "out2", bonetag = "B", base = 3 },
         { name = "out3", bonetag = "C", base = 4 },
         { name = "out4", bonetag = "D", base = 5 },
         { name = "spire", bonetag = "E", base = 6 },
         { name = "sumall", base = 7 },
         { name = "lbyrnth", base = 8 }
      }
   },
   {
      name = "The Lost Cities", bonetag = "R", base = 13,
      entry = 2, flags = { "mazelike" }, alignment = "neutral",
      levels = {
         -- The native scheduler chooses and persists each equal alternate.
         { name = "leth-a-1", bonetag = "F", base = 1 },
         { name = "lethe-b", bonetag = "G", base = 2 },
         { name = "leth-c-1", bonetag = "H", base = 3 },
         { name = "leth-d-1", bonetag = "I", base = 4 },
         { name = "lethe-e", bonetag = "J", base = 5 },
         { name = "lethe-f", bonetag = "K", base = 6 },
         { name = "lethe-g", bonetag = "L", base = 7 },
         { name = "lethe-z", bonetag = "M", base = 8 },
         { name = "nkai-a-1", bonetag = "N", base = 9 },
         { name = "nkai-b", bonetag = "O", base = 10 },
         { name = "nkai-c", bonetag = "P", base = 11 },
         { name = "nkai-z", bonetag = "Q", base = 12 },
         { name = "rlyeh", base = 13 }
      }
   },

}
