# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-15.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Run the established isolated live conflict/save fixture for the ogre mage.

py -3 -B test/run_step10b_ogre_runtime.py RELEASE_DIRECTORY NEW_EXTERNAL_DIR
"""
from pathlib import Path
import runpy
import sys

assert len(sys.argv) == 3
repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[2]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
sys.argv.append("ogre mage")
runpy.run_path(str(repo / "test/run_step9c_elder_mm_runtime.py"), run_name="__main__")
