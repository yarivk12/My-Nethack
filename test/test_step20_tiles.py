"""Check catalogue tile order, exact reused art, and generated glyph mappings.

Run after the native tilemap/tile2bmp Release build, like test_step9a_tiles.py.
The pinned pre-validation catalogue preserves every existing named tile body.
"""
from collections import Counter
from pathlib import Path
import re
import struct
import subprocess
import unittest

ROOT = Path(__file__).resolve().parents[1]
BASE = '2f067f4094eba86e520dd07b7e35a6eff1fcd6ad'
REUSE = {
    'hound of Tindalos': 'hell hound', 'elder brain': 'master mind flayer',
    'juggernaut': 'mumak', 'priestess of Ghaunadaur': 'high cleric',
    'drider': 'giant spider', 'Nightmare': 'black unicorn',
    'neothelid': 'purple worm', 'Solar': 'Archon', 'Planetar': 'Archon',
    'astral deva': 'Angel', 'deep dragon': 'black dragon',
    'razor dragon': 'white dragon', 'filth dragon': 'green dragon',
    'shadow dragon': 'black dragon', 'celestial dragon': 'gold dragon',
    'void dragon': 'black dragon', 'Vecna': 'arch-lich',
    'death knight': 'barrow wight', 'giant shoggoth': 'shoggoth',
    'ruby golem': 'stone golem', 'diamond golem': 'glass golem',
    'sapphire golem': 'stone golem', 'crystal golem': 'glass golem',
    # Existing conditional tiles are retained; active species need another pair.
    'beholder': 'beholder', 'baby shimmering dragon': 'baby shimmering dragon',
    'shimmering dragon': 'shimmering dragon',
    'vorpal jabberwock': 'vorpal jabberwock', 'vampire mage': 'vampire mage',
}
RESERVED = {
    'HELL_HOUND': ['Cerberus'], 'SHOCKING_SPHERE': ['beholder'],
    'BABY_SILVER_DRAGON': ['baby shimmering dragon'],
    'SILVER_DRAGON': ['shimmering dragon'], 'JABBERWOCK': ['vorpal jabberwock'],
    'VAMPIRE_LEADER': ['vampire mage'], 'CROESUS': ['Charon'],
    'SHAMAN_KARNOV': ['Earendil', 'Elwing'],
    'CHROMATIC_DRAGON': ['Goblin King'], 'NEANDERTHAL': ['High-elf'],
}
OBJECT_REUSE = {
    'knife / sacrificial knife': 'knife',
    'mummified hand': 'old gloves / leather gloves',
    'lapis lazuli / gain intelligence': 'ruby / conflict',
    'quartz / gain wisdom': 'diamond / warning',
    'malachite / carrying': 'pearl / poison resistance',
    'spiral / amulet of power': 'circular / amulet of ESP',
    'slate / passwall': 'dusty / magic mapping',
    'stitched / repair armor': 'plain / blank paper',
}
for new, reused in [('shimmering', 'shimmering'), ('deep', 'black'),
                    ('razor', 'white'), ('filth', 'green'),
                    ('shadow', 'black'), ('celestial', 'gold')]:
    for suffix in ['dragon scales', 'dragon scale mail']:
        OBJECT_REUSE[new + ' ' + suffix] = reused + ' ' + suffix


def tiles(text):
    return [(int(index), label, ''.join(body.split())) for index, label, body in
            re.findall(r'# tile (\d+) \(([^\n]+)\)\s*'
                       r'(?:#_[^\n]*\s*)?\{([^}]+)\}', text)]


def tile_header(text):
    return text[:re.search(r'^# tile \d+ \(', text, re.M).start()]


def object_refs(source):
    rows = [(int(tile), int(index), name, int(onum)) for tile, index, name, onum in
            re.findall(r'NO_CUSTOMCOLOR, NO_CUSTOMCOLOR,\s+(\d+), 0 \},\s*/\* \[\d+\] '
                       r'objects.txt:(\d+) (.*?) \(onum=(\d+)\)', source)
            if not name.startswith('piletop ')]
    offset = next(tile - index for tile, index, _, onum in rows if onum == 0)
    # Native corpse comments use CORPSE (onum), rather than the file index.
    # Follow the actual glyph tile number, which selects the bitmap pixels.
    return {onum: (tile - offset, name) for tile, _, name, onum in rows}


def catalogue_names():
    source = (ROOT / 'include/monsters.h').read_text()
    names = {}
    for match in re.finditer(r'\bMON\((NAM(?:S)?\([^\n]+?\))', source):
        depth, quoted, escaped = 1, False, False
        for end in range(match.start() + 4, len(source)):
            char = source[end]
            if char == '"' and not escaped:
                quoted = not quoted
            if not quoted:
                depth += (char == '(') - (char == ')')
            if not depth:
                break
            escaped = char == '\\' and not escaped
        key = re.search(r'([A-Z0-9_]+)\s*$', source[match.start() + 4:end])[1]
        names[key] = re.findall(r'"([^"]*)"', match[1])[-1]
    dump = subprocess.check_output(
        [str(ROOT / 'binary/Release/x64/NetHack.exe'), '--dumpenums'], cwd=ROOT,
        text=True)
    enums = re.findall(r'\bPM_([A-Z0-9_]+)\s*=\s*(\d+)', dump.split('NUMMONS')[0])
    assert len(enums) == 531, 'Recheck catalogue and reserved slots after adding monsters'
    assert 'MAIL_DAEMON' in dict(enums) and 'CHARON' not in dict(enums)
    return [(key, int(pm), names[key]) for key, pm in enums]


class Step20Tiles(unittest.TestCase):
    def test_bitmap_contains_exact_monster_and_object_pixels(self):
        bitmap = (ROOT / 'win/win32/tiles.bmp').read_bytes()
        offset = struct.unpack_from('<I', bitmap, 10)[0]
        width, height = struct.unpack_from('<ii', bitmap, 18)
        self.assertEqual(struct.unpack_from('<H', bitmap, 28)[0], 8)
        palette = [tuple(bitmap[i:i + 3][::-1]) for i in range(54, offset, 4)]
        stride = ((width * 8 + 31) // 32) * 4
        tile_offset = 0
        for filename in ['monsters.txt', 'objects.txt']:
            text = (ROOT / 'win/share' / filename).read_text()
            colors = {symbol: tuple(map(int, rgb)) for symbol, *rgb in
                      re.findall(r'^([^\s])\s*=\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)', text, re.M)}
            blocks = tiles(text)
            for index, name, pixels in blocks:
                tile = tile_offset + index
                x0, y0 = (tile % (width // 16)) * 16, (tile // (width // 16)) * 16
                actual = []
                for y in range(16):
                    row = abs(height) - 1 - (y0 + y) if height > 0 else y0 + y
                    start = offset + row * stride + x0
                    actual.extend(palette[value] for value in bitmap[start:start + 16])
                self.assertEqual(actual, [colors[symbol] for symbol in pixels],
                                 (filename, index, name, 'bitmap not regenerated from tile text'))
            tile_offset += len(blocks)

    def test_native_generated_bounds_and_bitmap(self):
        # The Windows GUI executable does not expose dumpenums through stdout.
        dump = subprocess.check_output(
            [str(ROOT / 'binary/Release/x64/NetHack.exe'), '--dumpenums'],
            text=True, cwd=ROOT)
        max_glyph = int(re.search(r'\bMAX_GLYPH\s*=\s*(\d+)', dump)[1])
        source = (ROOT / 'src/tile.c').read_text()
        count = int(re.search(r'int total_tiles_used = (\d+)', source)[1])
        table = source.split('glyph_map glyphmap[MAX_GLYPH] = {', 1)[1].split('};', 1)[0]
        rows = [(int(tile), int(glyph)) for tile, glyph in
                re.findall(r'NO_CUSTOMCOLOR, NO_CUSTOMCOLOR,\s+(\d+), 0 \},\s*/\* \[(\d+)\]', table)]
        self.assertEqual([glyph for _, glyph in rows], list(range(max_glyph)))
        self.assertTrue(all(0 <= tile < count for tile, _ in rows))
        bitmap = (ROOT / 'win/win32/tiles.bmp').read_bytes()
        self.assertEqual(bitmap[:2], b'BM')
        offset = struct.unpack_from('<I', bitmap, 10)[0]
        width, height = struct.unpack_from('<ii', bitmap, 18)
        bits = struct.unpack_from('<H', bitmap, 28)[0]
        stride = ((width * bits + 31) // 32) * 4
        self.assertGreaterEqual(len(bitmap), offset + stride * abs(height))
        self.assertEqual(width % 16, 0)
        self.assertLessEqual(((count + width // 16 - 1) // (width // 16)) * 16, abs(height))

    def test_existing_object_art_and_exact_step19_substitutions(self):
        current_text = (ROOT / 'win/share/objects.txt').read_text()
        old_text = subprocess.check_output(
            ['git', 'show', BASE + ':win/share/objects.txt'], cwd=ROOT, text=True)
        self.assertEqual(tile_header(current_text), tile_header(old_text))
        old, current = tiles(old_text), tiles(current_text)
        self.assertEqual([i for i, _, _ in current], list(range(570)))
        self.assertTrue(all(len(pixels) == 256 for _, _, pixels in current))
        expected = Counter((label, pixels) for _, label, pixels in old)
        art = {label: pixels for _, label, pixels in old}
        for added, reused in OBJECT_REUSE.items():
            expected[(added, art[reused])] += 1
        self.assertEqual(Counter((label, pixels) for _, label, pixels in current), expected)

    def test_generated_object_references_and_step19_armor(self):
        source = (ROOT / 'src/tile.c').read_text()
        labels = {index: label for index, label, _ in
                  tiles((ROOT / 'win/share/objects.txt').read_text())}
        refs = object_refs(source)
        self.assertEqual(set(refs), set(range(568)))
        for onum, (index, name) in refs.items():
            self.assertEqual(labels[index], name, (onum, index, 'wrong object tile'))
        dump = subprocess.check_output(
            [str(ROOT / 'binary/Release/x64/NetHack.exe'), '--dumpenums'], cwd=ROOT,
            text=True)
        armor = re.findall(r'\b([A-Z_]+_DRAGON_(?:SCALES|SCALE_MAIL))\s*=\s*(\d+)', dump)
        self.assertEqual(len(armor), 36)
        for enum, onum in armor:
            index, name = refs[int(onum)]
            self.assertEqual(name, enum.lower().replace('_', ' '))
            self.assertEqual(labels[index], name)
        # Native legacy reserve slots remain distinct from active shimmering armor.
        values = dict(re.findall(r'\b([A-Z_]+)\s*=\s*(\d+)', dump))
        for enum, name in [('SILVER_DRAGON_SCALE_MAIL', 'shimmering dragon scale mail'),
                           ('SILVER_DRAGON_SCALES', 'shimmering dragon scales')]:
            silver_index, _ = refs[int(values[enum])]
            self.assertEqual(labels[silver_index + 1], name)
            self.assertNotEqual(refs[int(values['SHIMMERING_' + enum[7:]])][0],
                                silver_index + 1)

    def test_existing_art_and_exact_substitutions(self):
        current_text = (ROOT / 'win/share/monsters.txt').read_text()
        old_text = subprocess.check_output(
            ['git', 'show', BASE + ':win/share/monsters.txt'], cwd=ROOT, text=True)
        self.assertEqual(tile_header(current_text), tile_header(old_text), 'palette/header changed')
        old, current = tiles(old_text), tiles(current_text)
        self.assertEqual([i for i, _, _ in current], list(range(1085)))
        self.assertTrue(all(len(pixels) == 256 for _, _, pixels in current))
        expected = Counter((label, pixels) for _, label, pixels in old)
        art = {label: pixels for _, label, pixels in old}
        for added, reused in REUSE.items():
            for sex in ('male', 'female'):
                expected[(added + ',' + sex, art[reused + ',' + sex])] += 1
        self.assertEqual(Counter((label, pixels) for _, label, pixels in current),
                         expected, 'existing art changed or unapproved pixels added')

    def test_native_catalogue_and_conditional_order(self):
        expected = []
        for key, _, name in catalogue_names():
            for entry in [name] + RESERVED.get(key, []):
                expected.extend(entry + ',' + sex for sex in ('male', 'female'))
        expected.append('invisible monster, nogender')
        self.assertEqual([label for _, label, _ in
                          tiles((ROOT / 'win/share/monsters.txt').read_text())], expected)

    def test_generated_male_and_female_references(self):
        source = (ROOT / 'src/tile.c').read_text()
        labels = {index: label for index, label, _ in
                  tiles((ROOT / 'win/share/monsters.txt').read_text())}
        refs = {(int(pm), sex): (int(index), name) for index, sex, name, pm in
                re.findall(r'monsters.txt:(\d+) (male|female) (.*?) \(mnum=(\d+)\)', source)}
        for _, pm, name in catalogue_names():
            for sex in ('male', 'female'):
                self.assertIn((pm, sex), refs)
                index, generated_name = refs[(pm, sex)]
                # Generated comments can append a gendered display-name alias.
                # Native active Beholder is capitalized; the retained tile
                # label (and deferred donor definition) uses lowercase.
                self.assertTrue(generated_name.casefold() == name.casefold() or
                                generated_name.casefold().startswith(name.casefold() + ' {'),
                                (pm, sex, name, generated_name, 'stale glyph map'))
                self.assertEqual(labels[index], name + ',' + sex,
                                 (pm, sex, index, 'glyph references wrong tile'))


if __name__ == '__main__':
    unittest.main()
