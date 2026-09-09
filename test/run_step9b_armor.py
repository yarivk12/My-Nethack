"""Actual wizard wearing, enchantment, cancellation, light and save/restore."""
from pathlib import Path
import re
import sys
import time
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    def give(name):
        text=game.lua('local o=obj.new("'+name+'");u.giveobj(o);nh.pline("ITEM_LETTER "..o:totable().invlet)')
        return re.findall(r'ITEM_LETTER (.)',text)[-1]
    def attributes(label):
        game.send('#attributes\n',1)
        game.wait("Wizard's attributes:",more=False)
        deadline=time.monotonic()+40
        while time.monotonic()<deadline:
            text=game.text()
            if re.search(r'\((?:end|\d+ of \d+)\)',text):break
            time.sleep(.1)
        else:raise AssertionError('Incomplete attributes menu\n'+text)
        pages=[text]
        page=re.search(r'\((\d+) of (\d+)\)',text)
        if page:
            for index in range(int(page[1])+1,int(page[2])+1):
                game.send(' ',1)
                pages.append(game.wait('(%d of %s)'%(index,page[2]),more=False))
        text='\n'.join(pages)
        (game.path/(label+'.txt')).write_text(text,encoding='utf8')
        game.send('\x1b');game.settle()
        return text
    def check(name,lit=None):
        game.lua('''local o=u.inventory;local found=false
while not o:isnull() do local t=o:totable()
 if t.otyp_name=="NAME" then
  assert(t.owornmask~=0,"armor not worn");found=true
  LIGHT
 end
 o=o:next()
end
assert(found,"missing armor NAME")'''.replace('NAME',name).replace('LIGHT',
        '' if lit is None else 'assert(t.lamplit==%d,"wrong worn light")'%int(lit)))
    def remove(letter):
        game.send('T',1);game.settle()
        # NetHack auto-selects the sole worn armor item. Do not queue its
        # inventory letter as a new command after that automatic removal.
        if 'What do you want to take off?' in game.text():
            game.send(letter,2);game.settle()
    powers=['magic-protected','fire resistant','cold resistant','sleep resistant',
            'disintegration resistant','shock resistant','poison resistant',
            'acid resistant','petrification resistant','reflection']
    armor=give('uncursed +0 chromatic dragon scales')
    game.send('W'+armor,2);game.settle();check('chromatic dragon scales')
    text=attributes('chromatic-scales')
    assert all(p in text for p in powers),text
    enchant=give('uncursed scroll of enchant armor')
    game.send('r'+enchant,2);game.settle();check('chromatic dragon scale mail')
    text=attributes('chromatic-mail')
    assert all(p in text for p in powers),text
    wand=give('wand of cancellation (0:10)')
    game.send('z'+wand);game.wait('In what direction?');game.send('.');game.settle()
    check('chromatic dragon scales')
    text=attributes('chromatic-cancelled')
    assert all(p in text for p in powers),text
    game.save();game.close();game=Game(release,output,restore=True)
    check('chromatic dragon scales');text=attributes('chromatic-restored')
    assert all(p in text for p in powers),text
    remove(armor)
    text=attributes('chromatic-removed')
    assert not any(p in text for p in powers),text
    print('PASS actual chromatic ten powers: wear, scales-to-mail, worn cancellation, save/restore, removal',flush=True)
    armor=give('uncursed +0 glowing dragon scales')
    game.send('W'+armor,2);game.settle();check('glowing dragon scales',True)
    assert 'petrification resistant' in attributes('glowing-scales')
    enchant=give('uncursed scroll of enchant armor')
    game.send('r'+enchant,2);game.settle();check('glowing dragon scale mail',True)
    wand=give('wand of cancellation (0:10)')
    game.send('z'+wand);game.wait('In what direction?');game.send('.');game.settle()
    check('glowing dragon scales',True)
    game.save();game.close();game=Game(release,output,restore=True)
    check('glowing dragon scales',True)
    remove(armor)
    assert 'petrification resistant' not in attributes('glowing-removed')
    game.lua('''local o=u.inventory;while not o:isnull() do local t=o:totable()
if t.otyp_name=="glowing dragon scales" then assert(t.lamplit==0 and t.owornmask==0) end
o=o:next() end''')
    print('PASS actual glowing stone resistance/light, both conversions, save/restore and removal',flush=True)
finally:
    game.close()
