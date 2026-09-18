"""Packaged wizard traversal tests; pywinpty and pyte required.

Usage: python test/run_step8a_runtime.py RELEASE_DIRECTORY NEW_OUTPUT_DIRECTORY
Every game/save is isolated outside the repository. No production test hooks.
"""
from pathlib import Path
import json
import os
import re
import shutil
import sys
import threading
import time
import pyte
from winpty import PtyProcess

def tty_transcript(raw):
    """Retain printed messages and spaces skipped by the Windows console."""
    out=[]
    x=y=0
    for token in re.findall(r'\x1b\[[0-?]*[ -/]*[@-~]|[^\x1b]+',raw):
        if token.startswith('\x1b['):
            if token[-1] in 'Hf':
                values=token[2:-1].split(';')
                ny=int(values[0] or '1')-1
                nx=int(values[1] or '1')-1 if len(values)>1 else 0
                if ny==y and nx>x:
                    out.append(' '*(nx-x))
                elif ny!=y or nx<x:
                    out.append('\n')
                x,y=nx,ny
            elif token[-1]=='C':
                n=int(token[2:-1] or '1');out.append(' '*n);x+=n
        else:
            out.append(token)
            for c in token:
                if c=='\n':y+=1;x=0
                elif c=='\r':x=0
                elif c=='\b':x=max(0,x-1)
                else:x+=1
    return ''.join(out)

class Game:
    def __init__(self, release, output, restore=False):
        self.path = Path(output).resolve()
        if not restore:
            self.path.mkdir(parents=True)  # refuse existing fixtures
            for f in Path(release).resolve().iterdir():
                if f.is_file():
                    shutil.copy2(f, self.path / f.name)
            (self.path / 'sysconf').write_text('WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n')
        options = ('windowtype:tty,name:wizard,role:Wizard,race:human,gender:male,'
                   'align:neutral,!news,!legacy,!tutorial,!tips,!autopickup,number_pad:0')
        (self.path / '.nethackrc').write_text('OPTIONS=' + options + '\n')
        self.raw = []
        self.screen = pyte.Screen(100,80)
        self.stream = pyte.Stream(self.screen)
        self.p = PtyProcess.spawn([str(self.path/'NetHack.exe'), '-D', '-u', 'wizard'],
            cwd=str(self.path), env=dict(os.environ, NETHACKOPTIONS=options),
            dimensions=(80,100))
        self.thread = threading.Thread(target=self.reader, daemon=True)
        self.thread.start()
        try:
            self.wait('Dlvl:')
        except Exception:
            # Retain startup diagnostics too (for example a Lua dungeon
            # error), rather than losing the transcript before construction.
            self.close()
            raise
        if restore:
            self.wait('keep the save file')
            self.send('N')
            self.settle()
        self.seq = 0

    def reader(self):
        try:
            while True:
                chunk = self.p.read(65536)
                self.raw.append(chunk)
                self.stream.feed(chunk)
                self.last_output = time.monotonic()
        except EOFError:
            pass

    def text(self):
        return '\n'.join(self.screen.display)

    def wait(self, needle, more=True, timeout=40):
        deadline = time.monotonic()+timeout
        while time.monotonic()<deadline:
            text = self.text()
            if needle in text:
                time.sleep(1)
                return self.text()
            if 'Oops...' in text and 'Hit <Enter>' in text:
                # panic() waits here before printing its actual diagnostic.
                # Advance only this error screen so the failure is captured.
                self.send('\n',1)
            if more and '--More--' in text:
                self.p.write(' ')
                # Windows redraws this prompt character by character. An
                # extra queued space can answer the following save question
                # before its intended 'n', which then becomes a movement key.
                time.sleep(1)
            time.sleep(.05)
        raise AssertionError('Missing '+needle+'\n'+text)

    def send(self, keys, delay=.3):
        self.p.write(keys)
        time.sleep(delay)

    def settle(self):
        for _ in range(100):
            if re.search(r'Call (?:a|an) [^\n]+:',self.text()):
                # Observing a monster use an unknown scroll/potion can ask
                # for a name. Decline naming in these isolated fixtures.
                self.send('\x1b',1)
            elif '--More--' in self.text():
                self.send(' ',1)
            else:
                time.sleep(.3)
                if ('--More--' not in self.text()
                        and time.monotonic()-getattr(self,'last_output',0)>1):
                    return
        raise AssertionError('Unending messages\n'+self.text())

    def lua(self, code):
        self.settle()
        raw_start = len(self.raw)
        self.seq += 1
        marker = 'LUA_DONE_%d' % self.seq
        (self.path/'probe.lua').write_text(code+'\nnh.pline("'+marker+'");\n')
        for attempt in range(3):
            # See the Step 10C-A packaged tty gate: WinPTY currently drops
            # lowercase alphabetic input in this line editor.  Extended
            # command matching is case-insensitive.
            self.send('#WIZLOADLUA\n')
            try:
                self.wait('Load which lua file?',timeout=12)
                break
            except AssertionError:
                if attempt==2: raise
                # A slow console redraw can consume the first command as a
                # --More-- dismissal. The file has not been submitted yet.
                self.send('\x1b')
                self.settle()
        self.send('PROBE.LUA\n')
        self.wait(marker)
        assert 'Lua error' not in self.text(), self.text()
        # Lua can emit several messages on the same TTY row. The completion
        # marker can overwrite the data we need before the screen is sampled.
        # Read this command's transcript, preserving those earlier messages.
        text = tty_transcript(''.join(self.raw[raw_start:]))+'\n'+self.text()
        assert 'Lua error' not in text, text
        self.settle()
        return text

    def state(self):
        text = self.lua('nh.pline(string.format("POS %d %d %d %d",u.dnum,u.dlevel,u.ux,u.uy));')
        # Message history can scroll earlier lines out of the current screen;
        # refresh via Ctrl-P if the completion message occupied its own line.
        found = re.findall(r'POS (\d+) (\d+) (\d+) (\d+)', text)
        if not found:
            self.send('\x10')
            found = re.findall(r'POS (\d+) (\d+) (\d+) (\d+)', self.text())
        assert found, self.text()
        return tuple(map(int, found[-1]))

    def branch_depth(self, name):
        """Return a fresh game's persistent DoD entrance for a named branch."""
        # Preserve the terminal model: Windows can omit unchanged cells.
        # Wait for this command's redraw before looking for the parent.
        self.send('#wizwhere\n',1)
        text = self.wait(' to ' + name + ':', more=False)
        matches = re.findall(r'(?:Stair|Portal) to ' + re.escape(name) + r': (\d+)', text)
        assert len(matches) == 1, (name, text)
        depth = int(matches[0])
        self.send('\x1b')
        self.settle()
        return depth

    def dungeon_number(self, name):
        marker = 'DNUM_QUERY'
        lua_name = name.replace('\\', '\\\\').replace('"', '\\"')
        text = self.lua('''for i=0,31 do if nh.dnum_name(i)=="%s" then nh.pline(string.format("%s %%d",i)) end end'''
                    % (lua_name, marker))
        matches = list(dict.fromkeys(re.findall(marker + r' (\d+)', text)))
        assert len(matches) == 1, (name, text)
        return int(matches[0])

    def branch_stair(self, name, direction, depth_hint):
        """Find a named branch stair near a scheduler-reported DoD depth."""
        lua_name = name.replace('\\', '\\\\').replace('"', '\\"')
        for target_depth in range(max(1, depth_hint - 2), depth_hint + 3):
            self.send('\x16')
            self.wait('To what level')
            self.send(str(target_depth) + '\n', 1)
            self.settle()
            text = self.lua('''for _,s in ipairs(nh.stairways()) do
 if s.dnum ~= u.dnum and nh.dnum_name(s.dnum)=="%s" then
  nh.pline("BRANCH_TARGET %%d %%d",s.dnum,s.up and 1 or 0)
 end
end''' % lua_name)
            found = re.findall(r'BRANCH_TARGET (\d+) (\d+)', text)
            if found:
                dnum, up = found[0]
                return self.stair('up' if up == '1' else 'down', True, int(dnum))
        raise AssertionError('missing branch stair '+name)

    def stair(self, direction, branch=False, target_dnum=None):
        # Ctrl-T's native teleport cost consumes 100 nutrition even when
        # debug hunger is disabled. Feed through the normal eating command
        # before long round trips can faint and lose control of the hero.
        text = self.lua('''if u.uhunger < 350 then
 local o=obj.new("uncursed food ration");u.giveobj(o)
 nh.pline("TRAVERSAL_FOOD "..o:totable().invlet)
end''')
        food = re.findall(r'TRAVERSAL_FOOD (.)',text)
        if food:
            self.send('e'+food[-1],2)
            self.settle()
        text = self.lua('''
local target
for _,s in ipairs(nh.stairways()) do
 if s.up == UP and ((s.dnum ~= u.dnum) == BRANCH)
    and (TARGET < 0 or s.dnum == TARGET) then
  assert(not target, "duplicate requested stairs"); target=s
 end
end
assert(target,"missing requested stairs")
nh.pline(string.format("MOVE %d %d %d %d %d %d",u.ux,u.uy,target.x,target.y,target.dnum,target.dlevel))
'''.replace('UP',str(direction=='up').lower()).replace('BRANCH',str(branch).lower())
          .replace('TARGET',str(-1 if target_dnum is None else target_dnum)))
        match = re.findall(r'MOVE (\d+) (\d+) (\d+) (\d+) (\d+) (\d+)',text)
        assert match, text
        x,y,tx,ty,dnum,dlevel = map(int,match[-1])
        if (x,y)!=(tx,ty):
            keys=('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)
            # A monster on the stair makes controlled teleport reject the
            # destination and choose a random square. Clear that blocker in
            # this wizard traversal fixture before testing the connection.
            self.send('#wizkill\n')
            self.wait('Pick first monster to slay')
            self.settle()
            self.send(keys+'.')
            self.settle()
            self.send('\x1b')
            self.settle()
            self.send('\x14')
            self.wait('Where do you want to be teleported?')
            self.settle()
            self.send(keys+'.')
            self.settle()
        self.send('<' if direction=='up' else '>', 1)
        self.settle()
        actual = self.state()
        assert actual[:2] == (dnum,dlevel), (actual,dnum,dlevel,self.text())
        print('PASS stair', actual, flush=True)
        if actual[0] == 11:
            self.lua(Path(__file__).with_name('test_step8a.lua').read_text())
        return actual

    def close(self):
        if self.p.isalive():
            self.p.terminate(force=True)
        self.thread.join(timeout=3)
        (self.path/'terminal.txt').write_text(''.join(self.raw), encoding='utf8')
        (self.path/'screen.txt').write_text(self.text(),encoding='utf8')
        panic = self.path/'paniclog'
        assert not panic.exists() or not panic.read_text().strip(), panic

    def save(self):
        self.settle()
        self.send('S')
        self.wait('Really save?')
        # The Windows console's active keyboard layout can remap lowercase
        # letters; native yes/no handling accepts uppercase as well.
        self.send('Y',1)
        deadline=time.monotonic()+40
        while self.p.isalive() and time.monotonic()<deadline:
            if '--More--' in self.text() or 'press a key' in self.text().lower():
                self.send(' ',1)
            time.sleep(.2)
        assert not self.p.isalive(), self.text()
        self.thread.join(timeout=3)
        (self.path/'before-restore-terminal.txt').write_text(''.join(self.raw),encoding='utf8')

def main():
    release, output = sys.argv[1:3]
    game = Game(release, output)
    try:
        game.send('#levelchange\n')
        game.wait('To what experience level')
        game.send('30\n')
        game.settle()
        game.lua('nh.debug_flags({hunger=false})')
        moria_depth = game.branch_depth('The Ruins of Moria')
        assert 30 <= moria_depth <= 199
        game.send('\x16')
        game.wait('To what level')
        game.send(str(moria_depth)+'\n',1)
        game.settle()
        assert game.state()[:2] == (0,moria_depth)
        game.stair('up', True, game.dungeon_number('The Ruins of Moria'))
        for _ in range(5):
            game.stair('up')
        for _ in range(5):
            game.stair('down')
        assert game.stair('down',True, 0)[:2] == (0,moria_depth)
        print('PASS complete six-level randomized Moria traversal and return to DoD%d' % moria_depth,flush=True)
    finally:
        game.close()

if __name__ == '__main__':
    main()
