"""Production executable: all artifact identities, native inspection and full save/reload."""
from pathlib import Path
import json
import re
import sys
import argparse
from run_step8a_runtime import Game

parser = argparse.ArgumentParser()
parser.add_argument('release')
parser.add_argument('output')
parser.add_argument('--state-only', action='store_true', help='Skip the 23-item UI tour; still check all identities and full save state')
args = parser.parse_args()
release, output = args.release, args.output
names = [
    "The Silver Key", "The Brass Lantern of Zork", "The Book of Infinite Spells",
    "The Lodestone of Gravity", "The Hellrider's Saddle", "The Mirrored Mask",
    "The Tie-Dye Shirt of Shambhala", "The Beastmaster's Duster",
    "The Cloak of the Unheld One", "The Iron Spoon of Liberation",
    "The Candle of Eternal Flame", "The Ring of P'hul", "Nenya", "Vilya",
    "Ehecailacocozcatl", "The Talisman of Buer", "The Holographic Void Lily",
    "The Mask of Tlaloc", "The Hand-Mirror of Cthylla", "The Earth Crystal",
    "The Glitterstone", "The Storm Whistle", "The Lyre of Orpheus",
]

def inventory(g):
    text = g.lua('local names={' + ','.join(json.dumps(n) for n in names) + '''}
local letters=""
for _,name in ipairs(names) do
 local o=u.inventory;local found=0
 while not o:isnull() do local t=o:totable()
  if t.oname==name then
   assert(t.quan==1,name.." quantity");found=found+1;letters=letters..t.invlet
  end;o=o:next()
 end
 assert(found==1,name.." identity count "..found)
end
nh.pline("ART21LETTERS "..letters)''')
    letters = re.findall(r'ART21LETTERS ([a-zA-Z]{23})', text)
    assert letters, text
    return {name: (letter,) for name, letter in zip(names, letters[-1])}

def snapshot(g, verify=False):
    g.lua('local verify=' + ('true' if verify else 'false') + '''
local fields={"o_id","quan","age","lamplit","owornmask","where"}
local stored=verify and nh.variable("step21_saved_inventory") or {}
local o=u.inventory;local count=0
while not o:isnull() do local t=o:totable()
 if t.has_oname~=0 then
  count=count+1
  if verify then
   for _,k in ipairs(fields) do assert(stored[t.oname][k]==t[k],t.oname.." "..k) end
  else
   local row={};for _,k in ipairs(fields) do row[k]=t[k] end;stored[t.oname]=row
  end
 end;o=o:next()
end
assert(count==23)
if not verify then nh.variable("step21_saved_inventory",stored) end
for _,name in ipairs({"The Brass Lantern of Zork","The Candle of Eternal Flame","The Glitterstone"}) do
 assert(stored[name].lamplit==1,name.." light") end''')

def inspect(g, letter):
    g.send('#INSPECT\n'); g.wait('inspect?'); g.send(letter)
    text = g.wait('Quality:')
    g.send('\x1b'); g.settle()
    return text

g = Game(release, output)
try:
    g.settle()
    g.send('#LEVELCHANGE\n'); g.wait('To what experience level'); g.send('30\n'); g.settle()
    g.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    g.lua('local names={' + ','.join(json.dumps(n) for n in names) + '''}
for _,n in ipairs(names) do
 local o=obj.new("uncursed "..n);assert(not o:isnull(),n)
 assert(o:totable().oname==n,"missing identity "..n.." got "..tostring(o:totable().oname).." type "..o:totable().otyp_name)
 u.giveobj(o)
end''')
    items = inventory(g)
    g.send('#WIZIDENTIFY\n'); g.wait('permanently identify')
    g.send('_\n'); g.settle()
    # Full identification uses the production wizard command, not a debug setter.
    before_pages = {}
    for name in ([] if args.state_only else names):
        text = inspect(g, items[name][0])
        assert any(s in text for s in ('Carried:', 'Worn:', 'Worn on face:', 'Permanent', 'Read:', 'Mounted:', 'Invoke:')), (name, text)
        before_pages[name] = text
        print('PASS inspect ' + name, flush=True)
    book = items['The Book of Infinite Spells'][0]
    g.send('#INVOKE\n'); g.wait('invoke?'); g.send(book)
    g.wait('Use the Book'); g.send('t'); g.settle()
    text = inspect(g, book)
    page = re.findall(r'Stored spell: ([^\n]+)', text)[-1].strip()
    assert 'turns remaining' in text, text
    saddle = items["The Hellrider's Saddle"][0]
    g.send('#INVOKE\n'); g.wait('invoke?'); g.send(saddle); g.settle()
    print('Saddle invoked', flush=True)
    assert 'Invisibility toggle: active' in inspect(g, saddle)
    saved = inventory(g)
    snapshot(g)
    (g.path/'step21-before.json').write_text(json.dumps({'items': saved, 'page': page}, indent=2))
    g.save()
finally:
    g.close()

g = Game(release, output, restore=True)
try:
    restored = inventory(g)
    assert saved == restored, (saved, restored)
    snapshot(g, verify=True)
    text = inspect(g, restored['The Book of Infinite Spells'][0])
    assert re.findall(r'Stored spell: ([^\n]+)', text)[-1].strip() == page, text
    assert 'Invisibility toggle: active' in inspect(g, restored["The Hellrider's Saddle"][0])
    (g.path/'step21-after.json').write_text(json.dumps(restored, indent=2))
    print('PASS production all23 ' + ('identities' if args.state_only else 'identified contracts')
          + ', Book page/cooldown, Saddle toggle, lights and full native save/reload', flush=True)
finally:
    g.close()
