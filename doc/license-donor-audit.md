# My-NetHack donor license and provenance audit

My-NetHack documentation created 2026-10-15. Distributed under the NetHack
General Public License; see `dat/license`. This record identifies donor
contributions and does not claim My-NetHack authorship of their original work.

## Scope and evidence

This audit examines the proposed source distribution through validated Step 22
commit `cbef52d28b24e177c875e816397b6ed3e4cb834a`, including donor imports from
earlier milestones. The encompassing repository audit counted 1,696 tracked
entries: 1,693 regular files and three unchanged submodule gitlinks. That total
does not mean 1,693 donor files were imported or individually relicensed.

Milestone source mappings are retained in `doc/step6a.md`, `doc/step7.md`,
`doc/step8a.md`, `doc/step9a.md`, `doc/step9b.md`, `doc/step9c.md`,
`doc/step10.md`, `doc/step11.md`, `doc/step19.md`, `doc/step20.md`, and
`doc/step21.md`. Those records distinguish imports, native adaptations,
approved overrides, and excluded donor systems. Step 22 reuses the local
native implementation and does not assert a new external source import.

The immutable donor revisions below were checked on 2026-10-15. Their
`dat/license` files all contain the same NetHack General Public License as
the project. After converting CRLF to LF, each is 4,875 bytes with SHA-256:

`93a3ae2cb8dee482daddfaebe53bcffe5b114b603def19b4dca21621cbc5a747`

| Donor and exact revision | Applicable milestone provenance | License verification |
|---|---|---|
| [NerfHack `0cb8781b0929b4617590a3b9fe78f972ef42c25f`](https://github.com/elunna/NerfHack/tree/0cb8781b0929b4617590a3b9fe78f972ef42c25f) | Steps 6 and 7 room and Tomb imports | Official pinned `dat/license`, retrieved by HTTPS |
| [UnNetHack `439b8d63d3d1ca78fb08588dd43f61874114b21a`](https://github.com/UnNetHack/UnNetHack/tree/439b8d63d3d1ca78fb08588dd43f61874114b21a) | Moria, Sheol, Dragon Caves, selected later monsters | Pinned local Git object `dat/license` |
| [dNetHack `17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0`](https://github.com/Chris-plus-alphanumericgibberish/dnethack/tree/17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0) | Mithardir, Neutral Quest and associated scoped content | Pinned local Git object `dnethack-3.4.3/dat/license` |
| [xNetHack `6eef39403f16f65e13f5d57242ee8d036307687a`](https://github.com/copperwater/xNetHack/tree/6eef39403f16f65e13f5d57242ee8d036307687a) | Step 11 selected theme rooms | Official pinned `dat/license`, retrieved by HTTPS |
| [SLASH'EM `fbd743a6081f4a447b9fc2dc545f20c3bc603330`](https://github.com/blitz/slashem/tree/fbd743a6081f4a447b9fc2dc545f20c3bc603330) | Steps 19 through 21 selected definitions | Pinned local checkout `dat/license` |
| [EvilHack `c444f6a3ab1e9f16d0676961dba86f628e91c6ba`](https://github.com/k21971/EvilHack/tree/c444f6a3ab1e9f16d0676961dba86f628e91c6ba) | Steps 19 and 20 selected monsters and artifacts | Pinned local checkout `dat/license` |
| [Hack'EM `cebe2f36e1908e82665c01a30a9eef2f108d07d9`](https://github.com/elunna/hackem/tree/cebe2f36e1908e82665c01a30a9eef2f108d07d9) | Steps 19 through 21 selected definitions | Pinned local checkout `dat/license` |
| [SpliceHack `8d70ade6f015c7894c171af691393327983173fe`](https://github.com/NullCGT/SpliceHack/tree/8d70ade6f015c7894c171af691393327983173fe) | Steps 19 and 20 dragons and selected monsters | Pinned local checkout `dat/license` |
| [Original SpliceHack `b3a1427a43fd6ecc6088129e72ac594a1d3e531f`](https://github.com/NullCGT/SpliceHack/tree/b3a1427a43fd6ecc6088129e72ac594a1d3e531f) | Step 21 Void Lily reference and adaptations | Official pinned `dat/license`, retrieved by HTTPS |
| [dNAO `d512426550448e944f381469e498423b656f2e74`](https://github.com/Chris-plus-alphanumericgibberish/dNAO/tree/d512426550448e944f381469e498423b656f2e74) | Step 21 artifact definitions and bounded behavior adaptations | Official pinned `dat/license`, retrieved by HTTPS |
| [notdnethack `7a18812f35d77109923fe6022576073c16ebcc67`](https://github.com/demogorgon22/notdnethack/tree/7a18812f35d77109923fe6022576073c16ebcc67) | Step 21 Buer reference and adaptations | Official pinned `dat/license`, retrieved by HTTPS |

Local audit evidence is retained under ignored `_qa/license-compliance/`:
the five HTTPS license responses and retrieval metadata in `donor-licenses/`,
`donor-local-licenses.json`, `donor-map-notices.json`, and
`donor-code-header-candidates.json`. These diagnostic files are not part of
the source distribution or a replacement for the retained license.

## Restored donor notices

The following 37 Lua conversions omitted notices carried by their pinned
donor map source. Restoration copies the applicable original notice wording,
changing only the `.des` comment marker `#` to the Lua marker `--`.
The separate My-NetHack modification notice identifies local adaptation and
its dates, without replacing donor copyright, attribution, or license text.

| Donor source | Files needing notice restoration | Restored notice |
|---|---|---|
| dNetHack `dnethack-3.4.3/dat/chaos2.des` at the pin above | `dat/cat1.lua`, `dat/cat2.lua`, `dat/cat3.lua`, `dat/chalv2.lua`, `dat/mith1.lua`, `dat/mith2.lua`, `dat/mith3.lua`, `dat/ossa1.lua` (8) | Original NetHack redistribution and license notice |
| dNetHack `dnethack-3.4.3/dat/neutrality.des` at the pin above | `dat/gatetwn.lua`, `dat/leth-a-1.lua`, `dat/leth-a-2.lua`, `dat/leth-c-1.lua`, `dat/leth-c-2.lua`, `dat/leth-d-1.lua`, `dat/leth-d-2.lua`, `dat/lethe-b.lua`, `dat/lethe-e.lua`, `dat/lethe-f.lua`, `dat/lethe-g.lua`, `dat/lethe-z.lua`, `dat/neulev.lua`, `dat/nkai-a-1.lua`, `dat/nkai-a-2.lua`, `dat/nkai-b.lua`, `dat/nkai-c.lua`, `dat/nkai-z.lua`, `dat/out1.lua`, `dat/out2.lua`, `dat/out3.lua`, `dat/out4.lua`, `dat/rlyeh.lua`, `dat/spire.lua`, `dat/sumall.lua` (25) | Original NetHack redistribution and license notice |
| UnNetHack `dat/dragons.des` at the pin above | `dat/drgnA.lua`, `dat/drgnB.lua`, `dat/drgnC.lua`, `dat/drgnD.lua` (4) | Original copyrights for Jean-Christophe Collet (1989) and M. Stephenson (1991), plus NetHack redistribution and license notice |

The ten Moria conversions already credit Guest41, matching the source's
author credit. Existing NerfHack conversion credits, including
hackemslashem and the documented SLASH'EM/SlashTHEM history, remain intact.
The Step 11 theme-room source retains Pasi Kallinen's copyright and its
NetHack license notice. No new original-author attribution is invented.

Leading copyright, license, and warranty notices were compared against the
locally modified `src/` and `include/` paths available in six complete pinned
donor repositories, and against the cached Step 21 donor reference files.
After ignoring whitespace differences, those comparisons found no additional
missing leading notices. This statement concerns those inspected headers;
it is not a claim that every line in every donor repository was imported.

Step 19 and 20 resource mappings reuse existing native tile artwork, as
documented in `doc/step20.md` and checked by `test/test_step20_tiles.py`.
No new donor artwork is asserted by those mappings.

## Separate submodule terms and source distribution

These existing gitlinks remain unchanged. Their own terms and notices remain
applicable; the NetHack license does not replace the licenses of independent
third-party components.

| Component and recorded Git commit | Local license evidence and distribution implications |
|---|---|
| Lua `6e22fedb74cf0c9b6656e9fce8b7331db847c605` | `submodules/lua/lua.h`, final copyright notice, grants MIT-style permission and requires retaining the Lua.org, PUC-Rio copyright and permission notice in copies or substantial portions. Its warranty and liability exclusions remain intact. |
| PDCurses `09cf16db29753305e4241d4ae609aac997fa730d` | `submodules/pdcurses/README.md` describes the core and most ports as public domain, with copyright exceptions in `demos/` and `x11/`. Keep those directory and file-specific terms and its AS IS/no-warranty notice. Do not describe every file in the submodule as public domain. |
| PDCursesMod `6cd9c16900fef82754923c718fab7fe85f761bb6` | `submodules/pdcursesmod/README.md` describes a public-domain core with component exceptions. Its root distribution section identifies `config.guess` and `config.sub` as GPL and `configure` as governed by its embedded free license. The `pdcurses/` and `wingui/` README distribution sections identify their files as public domain. Retain all applicable per-directory and per-file terms, acknowledgements and warranty notices. |

A source archive made only from the superproject does not necessarily contain
the three submodules. Supply their exact recorded contents when providing
complete buildable source, including the applicable notices. For an executable
distribution, satisfy NetHack license paragraph 3 by accompanying it with
complete machine-readable source, or, for noncommercial distribution, full
information identifying an appropriate archive site for that complete source.
Neither an unpopulated gitlink nor this provenance record supplies missing
component source or replaces required component license notices.

## Boundaries and remaining obligations

No incompatible donor license was identified among the eleven inspected
revisions. This is a source and provenance evidence record, not a legal
opinion or a claim to have established every historical contributor's title.
It preserves the distinction between My-NetHack changes and third-party
authorship. Original license wording, copyrights, attribution, warranty
notices and existing modification records must remain intact.

Formats without safe comments need a format-specific mechanism that carries
the notice with the modified file. A separate changelog alone is not assumed
to satisfy that requirement. The encompassing modification audit records the
mechanisms selected for such files and their validation.

This documentation adds no gameplay, balance, persistence or engine behavior.
It does not resolve or reclassify the unconfirmed screenshot crash described
in `doc/step22_implementation.md`.
