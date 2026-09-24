"""Production socketing, replacement failure, inspection, effects and native recovery."""
from pathlib import Path
import hashlib, os, re, shutil, subprocess, sys
from run_step8a_runtime import Game

release, diagnostic, output = sys.argv[1:4]
os.environ.update(STEP13_GAME_FIXTURE='1', STEP15D_GAME_SEED='1')
g = Game(diagnostic, output)
try:
    g.save()
finally:
    g.close()
    os.environ.pop('STEP13_GAME_FIXTURE')
    os.environ.pop('STEP15D_GAME_SEED')
shutil.copy2(Path(release)/'NetHack.exe', Path(output)/'NetHack.exe')
assert hashlib.sha256((Path(output)/'NetHack.exe').read_bytes()).digest() == hashlib.sha256((Path(release)/'NetHack.exe').read_bytes()).digest()
g = Game(release, output, restore=True)

def letters():
    g.send('i');text=g.wait('15d-hammer')
    items={}
    for letter, description, name in re.findall(r'([a-zA-Z]) - (.*?) named (15d-[a-z]+)',text):
        quantity=re.match(r'(\d+) ',description)
        items[name,int(quantity.group(1)) if quantity else 1]=letter
    g.send('\x1b');g.settle()
    assert len(items)>=4,text
    return items

def inspect(letter):
    g.lua('nh.variable("inspect_turn",u.moves)')
    g.send('#INSPECT\n');g.wait('inspect?');g.send(letter)
    text = g.wait('Sockets:')
    g.send('\x1b');g.settle()
    g.lua('assert(u.moves==nh.variable("inspect_turn"),"inspection is free")')
    return text

def affix(target, material, replacement):
    g.lua('nh.variable("affix_turn",u.moves)')
    g.send('a'+hammer);g.wait('Socket gemstone');g.send('s')
    g.wait('Choose equipment to socket');g.send(target)
    g.wait('Choose a gemstone');g.send(material)
    if replacement:
        g.wait('Choose a socket to replace');g.send('a')
    text=g.wait('Confirm socketing?')
    assert 'Success chance: unknown' in text, text
    g.send('y');g.settle()
    if any(prompt in g.text() for prompt in
           ('Try again?', 'Socket another gem?', 'Replace a gem?')):
        g.send('n');g.settle()
    g.lua('assert(u.moves==nh.variable("affix_turn")+1,"affixing spends one action")')

try:
    g.lua('nh.debug_flags({hunger=false,mongen=false})')
    (g.path/'forge.lua').write_text('''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[-------
|.f...|
|.....|
-------]])
des.region(selection.area(1,1,5,2),"lit")
des.stair("up",1,1);des.stair("down",5,2)
local x,y=nh.abscoord(2,1);nh.variable("forge_pos",{x=x,y=y})''')
    g.send('#WIZLOADDES\n');g.wait('Load which des lua file?');g.send('FORGE.LUA\n',2);g.settle()
    text=g.lua('local p=nh.variable("forge_pos");nh.pline(string.format("TARGET %d %d %d %d",u.ux,u.uy,p.x,p.y))')
    a,b,x,y=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
    if (a,b)!=(x,y):
        g.send('\x14');g.wait('Where do you want to be teleported?');g.settle()
        g.send(('l' if x>a else 'h')*abs(x-a)+('j' if y>b else 'k')*abs(y-b)+'.');g.settle()
    items=letters();hammer=items['15d-hammer',1];stack=items['15d-stack',3]
    assert 'Strength III +4' in inspect(stack)
    g.send('w'+stack);g.wait('St:20');g.settle()
    g.send('w-');g.wait('St:16');g.settle()
    affix(stack,items['15d-material',2],True)
    items=letters();empty=items['15d-stack',1]
    assert 'Socket 1: empty' in inspect(empty)
    assert 'Strength III +4' in inspect(items['15d-stack',2])
    for attempt in range(30):
        items=letters()
        material=next(v for (name,q),v in items.items() if name=='15d-gems')
        affix(empty,material,False)
        text=inspect(empty)
        if 'Socket 1: empty' not in text:
            break
    else:
        raise AssertionError('No successful affixing in 30 production attempts')
    result=re.findall(r'Socket 1: ([^\n]+)',text)[-1].strip()
    assert result != 'unknown'
    for stage in ('after-affix','after-level'):
        if stage=='after-level':
            for level in (2,1):
                g.send('\x16');g.wait('To what level');g.send(str(level)+'\n',1);g.settle()
        assert result in inspect(letters()['15d-stack',1])
        g.save();shutil.copytree(output,output+'-'+stage)
        g=Game(release,output,restore=True)
    g.close()
    proc=subprocess.run([str(Path(release).resolve()/'recover.exe'),'-d',str(Path(output).resolve()),'wizard'],capture_output=True,text=True)
    (Path(output)/'recovery.log').write_text(proc.stdout+proc.stderr)
    assert proc.returncode==0,proc.stderr
    shutil.copytree(output,output+'-after-recovery')
    g=Game(release,output,restore=True)
    assert result in inspect(letters()['15d-stack',1])
finally:
    g.close()

os.environ.update(STEP13_GAME_FIXTURE='1',STEP15D_GAME_CHECK='1')
try:
    for stage in ('after-affix','after-level','after-recovery'):
        probe=output+'-'+stage
        shutil.copy2(Path(diagnostic)/'NetHack.exe',Path(probe)/'NetHack.exe')
        g=Game(diagnostic,probe,restore=True)
        try:
            assert 'PASS production affixing' in (Path(probe)/'step15d-game-results.txt').read_text()
        finally:
            g.close()
finally:
    os.environ.pop('STEP13_GAME_FIXTURE');os.environ.pop('STEP15D_GAME_CHECK')
print('PASS production activation, STR 16+4=20, replacement failure on one weapon, successful affixing, free inspection, level/save/recovery and diagnostic actual-state checks')
