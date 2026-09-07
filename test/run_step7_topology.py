"""Fresh packaged wizard-game topology smoke test (requires pywinpty, pyte).

python test/run_step7_topology.py RELEASE_DIRECTORY NEW_OUTPUT_DIRECTORY [COUNT]
Each game runs in its own copied data directory; no real player save is used.
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

release, output = map(lambda s: Path(s).resolve(), sys.argv[1:3])
count = int(sys.argv[3]) if len(sys.argv) > 3 else 5
output.mkdir(parents=True, exist_ok=True)
results = []
for sample in range(count):
    game = output / str(sample)
    game.mkdir()  # refuse to overwrite an existing fixture
    for f in release.iterdir():
        if f.is_file():
            shutil.copy2(f, game / f.name)
    (game / "sysconf").write_text("WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n")
    env = dict(os.environ, NETHACKOPTIONS="windowtype:tty,name:wizard,role:Wizard,"
               "race:human,gender:male,align:neutral,!news,!legacy,!tutorial,!tips")
    (game / ".nethackrc").write_text("OPTIONS=" + env["NETHACKOPTIONS"] + "\n")
    p = PtyProcess.spawn([str(game / "NetHack.exe"), "-D", "-u", "wizard"],
                         cwd=str(game), env=env, dimensions=(80,100))
    screen = pyte.Screen(100,80)
    stream = pyte.Stream(screen)
    raw = []
    def reader():
        try:
            while True:
                chunk = p.read(65536)
                raw.append(chunk)
                stream.feed(chunk)
        except EOFError:
            pass
    thread = threading.Thread(target=reader, daemon=True)
    thread.start()
    def wait_for(needle):
        deadline = time.monotonic()+30
        while time.monotonic()<deadline:
            text = "\n".join(screen.display)
            if needle in text:
                return text
            time.sleep(0.05)
        raise AssertionError("Missing " + needle + "\n" + text)
    try:
        wait_for("Dlvl:1")
        p.write("#wizwhere\n")
        text = wait_for("Floating branches")
        # Complete rendering of the final page before reading it.
        time.sleep(0.3)
        text = "\n".join(screen.display)
        (game / "topology.txt").write_text(text)
        tombs = re.findall(r"Stair to The Lost Tomb: (\d+)", text)
        temples = re.findall(r"Stair to The Temple of Moloch: (\d+)", text)
        morias = re.findall(r"Stair to The Ruins of Moria: (\d+)", text)
        big = [int(n) for n in re.findall(r"bigrm: (\d+)", text)]
        assert len(tombs)==len(temples)==len(morias)==1
        tomb, temple, moria = (int(tombs[0]), int(temples[0]), int(morias[0]))
        assert all(30<=d<=199 for d in (tomb, temple, moria))
        assert len({tomb, temple, moria}.intersection(big)) == 0
        assert len({tomb, temple, moria}) == 3
        assert 3<=len(big)<=5 and len(big)==len(set(big))
        assert "castle: 200" in text and "tomb-2" not in text
        assert re.search(r"The Lost Tomb: (?:level|depth) " + str(tomb+1)+r"\s", text)
        assert 'The Ruins of Moria: levels' in text
        maps = re.findall(r'moria([1-6])-([1-4]): (\d+)', text)
        assert len(maps) == 6 and len({n for n, _, _ in maps}) == 6
        # The DoD branch stair is one level below the first Moria map;
        # moria1 is entrance-1 and moria6 is entrance-6.
        expected = {n: moria - int(n) for n, _, _ in maps}
        for n, variant, depth in maps:
            assert int(depth) == expected[n]
            assert int(variant) <= ({'4': 4, '6': 2}.get(n, 1))
        if (game / "paniclog").exists():
            assert not (game / "paniclog").read_text().strip()
        results.append(dict(tomb=tomb, temple=temple, moria=moria, bigrooms=big))
        print("PASS fresh game", sample, results[-1], flush=True)
    finally:
        p.terminate(force=True)
        thread.join(timeout=3)
        (game / "terminal.txt").write_text("".join(raw), encoding="utf-8")
assert len({r["tomb"] for r in results})>1
assert len({r["moria"] for r in results})>1
(output / "results.json").write_text(json.dumps(results, indent=2))
print("PASS: fresh packaged topology varies; one Tomb, Temple and Moria, 3-5 Big Rooms, Castle200")
