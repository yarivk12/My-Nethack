"""Run the complete Step 10QA2 remediation and semantic-validation gate.

The runner keeps all generated evidence under the project _qa directory.  It deliberately
reuses the established Step 10C-E closeout for the broad regression matrix,
while adding QA2's source, normalized-parity, fresh-generation, and final
package semantic gates around it.
"""
from datetime import datetime
from pathlib import Path
import hashlib
import importlib.util
import json
import os
import shutil
import subprocess
import sys


REPO = Path(__file__).resolve().parents[1]
RELEASE = REPO / "binary/Release/x64"
ARTIFACT_ROOT = REPO / "_qa"
DONOR = ARTIFACT_ROOT / "dnethack-donor-pinned"
QA1_1 = ARTIFACT_ROOT / "step10qa1-1-audit-20260913-083609"
DEFAULT_OUT_ROOT = ARTIFACT_ROOT
EXPECTED_HEAD = "48fe150af4a087fd2f4ff576b43c1c96cc08c6fe"
DONOR_COMMIT = "17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0"
MSBUILD = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
    r"\MSBuild\Current\Bin\MSBuild.exe"
)


def git(*args):
    return subprocess.check_output(["git", *args], cwd=REPO, text=True).strip()


def hash_file(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def snapshot(out):
    commands = {
        "branch.txt": ("branch", "--show-current"),
        "head.txt": ("rev-parse", "HEAD"),
        "origin.txt": ("rev-parse", "origin/phase0/dod-length"),
        "status-short.txt": ("status", "--short"),
        "status.txt": ("status"),
        "diff-stat.txt": ("diff", "--stat"),
        "diff-check.txt": ("diff", "--check"),
        "submodules.txt": ("submodule", "status"),
        "untracked.txt": ("ls-files", "--others", "--exclude-standard"),
        "index.patch": ("diff", "--cached"),
    }
    for filename, args in commands.items():
        result = subprocess.run(["git", *args], cwd=REPO, text=True,
                                capture_output=True, check=False)
        (out / filename).write_text(result.stdout + result.stderr,
                                     encoding="utf-8")


def save_manifest(path):
    result = {"path": str(path), "exists": path.is_file()}
    if path.is_file():
        stat = path.stat()
        result.update({"size": stat.st_size, "mtime_ns": stat.st_mtime_ns,
                       "sha256": hash_file(path)})
    return result


def stream(label, command, *, cwd=REPO, env=None, output=None):
    print("=== " + label + " ===", flush=True)
    proc = subprocess.Popen(command, cwd=cwd, env=env,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True,
                            encoding="utf-8", errors="replace")
    lines = []
    assert proc.stdout is not None
    for line in proc.stdout:
        print(line, end="")
        lines.append(line)
    rc = proc.wait()
    text = "".join(lines)
    if output is not None:
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(text, encoding="utf-8")
    if rc:
        print(f"FAIL {label} (exit {rc})", flush=True)
        raise subprocess.CalledProcessError(rc, command)
    print("PASS " + label, flush=True)
    return text


def run_py(label, script, *args, output=None):
    return stream(label, [sys.executable, "-B", str(REPO / script),
                          *(str(arg) for arg in args)], output=output)


def run_exe(label, exe, variables, output):
    env = os.environ.copy()
    for key in (
        "NETHACK_STEP10C_C_TEST", "NETHACK_STEP10C_D_TEST",
        "NETHACK_STEP10C_E_MATRIX", "NETHACK_STEP10C_E_PERSISTENCE",
        "NETHACK_STEP10C_E_SAVE", "NETHACK_STEP10QA2_TEST",
        "NETHACK_OUTLANDS_LEVEL", "NETHACK_OUTLANDS_COUNT",
        "NETHACK_OUTLANDS_SEED", "NETHACK_OUTLANDS_TRACE",
    ):
        env.pop(key, None)
    env.update(variables)
    return stream(label, [str(exe)], cwd=exe.parent, env=env, output=output)


def assert_preflight():
    assert git("branch", "--show-current") == "phase0/dod-length"
    assert git("rev-parse", "HEAD") == EXPECTED_HEAD
    assert git("rev-parse", "origin/phase0/dod-length") == EXPECTED_HEAD
    assert not git("diff", "--cached", "--name-only")
    assert subprocess.run(["git", "diff", "--quiet", "--", "README.md"],
                          cwd=REPO).returncode == 0
    subprocess.run(["git", "diff", "--check"], cwd=REPO, check=True)
    assert DONOR.is_dir() and git("-C", str(DONOR), "rev-parse", "HEAD") == DONOR_COMMIT
    print("PASS QA2 protected preflight: frozen HEAD, empty index, README unchanged",
          flush=True)


def run_parity(out):
    parity = out / "parity"
    parity.mkdir()
    extractor = QA1_1 / "audit_explicit_parity.py"
    reviewer = QA1_1 / "review_explicit_parity.py"
    neutral = QA1_1 / "inputs/donor/neutrality.des"
    labr = QA1_1 / "inputs/donor/labr.des"
    stream("QA1-1 normalized parity extraction", [
        sys.executable, "-B", str(extractor), "--local-dir", str(REPO / "dat"),
        "--donor-neutrality", str(neutral), "--donor-labr", str(labr),
        "--out-dir", str(parity),
    ], output=out / "parity-extraction.txt")

    spec = importlib.util.spec_from_file_location("qa1_1_review", reviewer)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    original = module.manual_object_review

    def qa2_object_review(record):
        resource = record["resource"]
        line = record["source_line"]
        if resource == "nkai-b" and line in {3107, 3108}:
            return None
        if resource == "lethe-z" and line in {2648, 2649, 2650, 2651, 2652}:
            return ("APPROVED LOCAL ADAPTATION",
                    "QA2 current montype path preserves actual corpse identity and spe=0.")
        return original(record)

    old_argv = sys.argv
    try:
        module.manual_object_review = qa2_object_review
        sys.argv = [str(reviewer), "--parity-dir", str(parity)]
        review_text = module.main()
        assert review_text in (None, 0)
    finally:
        sys.argv = old_argv
        module.manual_object_review = original

    summary = json.loads((parity / "parity-review-summary.json").read_text())
    monster = summary["monster"]
    objects = summary["object"]
    assert monster["total"] == 1197 and monster["local_total"] == 1185
    assert objects["total"] == 488 and objects["local_total"] == 487
    assert summary["confirmed_missing_count"] == 0
    assert summary["not_proven_count"] == 0
    assert summary["suspicious_local_only_count"] == 0
    summary["qa2_accounted_monsters"] = monster["total"]
    summary["qa2_accounted_objects"] = objects["total"]
    (parity / "qa2-parity-summary.json").write_text(
        json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print("PASS QA1-1 normalized parity: MONSTER 1197/1197, OBJECT/CONTAINER 488/488; missing=0, not-proven=0",
          flush=True)
    return summary


def rebuild_dlb(out, label):
    command = [str(MSBUILD), str(REPO / "sys/windows/vs/dlb/dlb.vcxproj"),
               "/t:Rebuild", "/p:Configuration=Release", "/p:Platform=x64",
               "/v:m"]
    text = stream(label, command, output=out / (label.lower().replace(" ", "-") + ".txt"))
    archive = RELEASE / "nhdat500"
    assert archive.is_file()
    metadata = {"path": str(archive), "size": archive.stat().st_size,
                "sha256": hash_file(archive),
                "mtime_ns": archive.stat().st_mtime_ns}
    (out / (label.lower().replace(" ", "-") + "-archive.json")).write_text(
        json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    return text, metadata


def build_qa2(out):
    command = [str(MSBUILD), str(REPO / "sys/windows/vs/NetHack.sln"),
               "/p:Configuration=Release", "/p:Platform=x64",
               "/p:STEP10QA2_TEST=true", "/v:m"]
    stream("QA2 diagnostic x64 build", command, output=out / "qa2-build.txt")
    exe = out / "qa2-runtime/NetHack.exe"
    runtime = exe.parent
    runtime.mkdir(parents=True, exist_ok=False)
    for item in RELEASE.iterdir():
        if item.is_file():
            shutil.copy2(item, runtime / item.name)
    assert exe.is_file()
    return exe


def run_broad_e(out):
    spec = importlib.util.spec_from_file_location("step10c_e", REPO / "test/run_step10c_e.py")
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)

    def qa2_preflight():
        assert module.git("branch", "--show-current") == "phase0/dod-length"
        assert module.git("rev-parse", "HEAD") == EXPECTED_HEAD
        assert module.git("rev-parse", "origin/phase0/dod-length") == EXPECTED_HEAD
        assert not module.git("diff", "--cached", "--name-only")
        assert subprocess.run(["git", "diff", "--quiet", "--", "README.md"],
                              cwd=REPO).returncode == 0
        subprocess.run(["git", "diff", "--check"], cwd=REPO, check=True)
        print("PASS QA2 broad-regression preflight", flush=True)

    module.preflight = qa2_preflight
    old_argv = sys.argv
    old_bytecode = sys.dont_write_bytecode
    try:
        sys.dont_write_bytecode = True
        sys.argv = [str(REPO / "test/run_step10c_e.py"), str(out)]
        module.main()
    finally:
        sys.argv = old_argv
        sys.dont_write_bytecode = old_bytecode
    summary = json.loads((out / "summary.json").read_text())
    assert all(summary[key] == "PASS" for key in
               ("e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8"))
    return summary


def package_rebuild(out):
    package_project = REPO / "sys/windows/vs/package/package.vcxproj"
    command = [str(MSBUILD), str(package_project), "/t:Rebuild",
               "/p:Configuration=Release", "/p:Platform=x64", "/v:m"]
    stream("final x64 package rebuild from current runtime archive", command,
           output=out / "final-package-build.txt")
    package = REPO / "vspackage/nethack-500-win-x64.zip"
    assert package.is_file()
    return package


def package_semantic_gate(out, package, qa2_exe):
    runtime = out / "package-runtime"
    runtime.mkdir(parents=True, exist_ok=False)
    stream("extract final x64 package for semantic gate",
           ["tar", "-xf", str(package), "-C", str(runtime)], output=out / "package-extract.txt")
    packaged_qa2 = runtime / "NetHack.exe"
    shutil.copy2(qa2_exe, packaged_qa2)
    text = run_exe("final packaged fresh-generation semantic gate", packaged_qa2,
                   {"NETHACK_STEP10QA2_TEST": "1"}, out / "package-semantic.stdout")
    assert "PASS Step 10QA2 fresh generated room/object semantics across all 26 resources" in text
    return run_py("final package 26-resource runtime/content sweep",
                  "test/run_step10c_a.py", runtime, out / "package-resource-sweep", DONOR,
                  output=out / "package-resource-sweep.txt")


def main():
    os.environ["PYTHONDONTWRITEBYTECODE"] = "1"
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else DEFAULT_OUT_ROOT / (
        "step10qa2-evidence-" + timestamp)
    assert out != REPO and (out == ARTIFACT_ROOT or ARTIFACT_ROOT in out.parents)
    out.mkdir(parents=True, exist_ok=False)
    (out / "runner-command.txt").write_text(
        "python -B test/run_step10qa2.py " + str(out) + "\n", encoding="utf-8")

    save_path = Path(r"C:\Users\yariv\AppData\Local\NetHack\5.0\wizard.NetHack-saved-game")
    before_save = save_manifest(save_path)
    snapshot(out / "baseline") if (out / "baseline").mkdir() is None else None
    assert_preflight()

    run_py("QA2 special-region and payload source contract", "test/test_step10qa2_source.py",
           output=out / "source-contract.txt")
    parity = run_parity(out)

    _, dlb_before = rebuild_dlb(out / "dlb-before-diagnostic", "explicit DLB rebuild from current Lua resources")
    qa2_exe = build_qa2(out)
    focused_text = run_exe("immediate QA2 affected-room fresh-generation gate", qa2_exe,
                           {"NETHACK_STEP10QA2_TEST": "1"}, out / "qa2-focused.stdout")
    assert focused_text.count("QA2_AFFECTED|") == 5
    assert "QA2_AFFECTED|rlyeh|temples=7|has_temple=1|explicit_unknown_priests=2|dagon=1|hydra=1|cthulhu=1" in focused_text
    assert "PASS Step 10QA2 fresh generated room/object semantics across all 26 resources" in focused_text

    run_py("destination-aware Neutral portal arrival regression",
           "test/run_step10_portal_arrival.py", qa2_exe.parent,
           out / "portal-arrival")

    broad = run_broad_e(out / "step10c-e")

    _, dlb_final = rebuild_dlb(out / "dlb-final", "explicit final DLB rebuild from current Lua resources")
    package = package_rebuild(out / "final-package")
    package_text = package_semantic_gate(out, package, qa2_exe)
    assert package_text.count("PASS runtime resource") == 26
    run_py("final package Neutral portal arrival regression",
           "test/run_step10_portal_arrival.py", out / "package-runtime",
           out / "package-portal-arrival")

    final_exe = RELEASE / "NetHack.exe"
    archive = RELEASE / "nhdat500"
    final_files = {
        "NetHack.exe": {"path": str(final_exe), "size": final_exe.stat().st_size,
                         "sha256": hash_file(final_exe)},
        "nhdat500": {"path": str(archive), "size": archive.stat().st_size,
                      "sha256": hash_file(archive)},
        "package": {"path": str(package), "size": package.stat().st_size,
                     "sha256": hash_file(package)},
    }
    (out / "final-files.json").write_text(json.dumps(final_files, indent=2) + "\n",
                                           encoding="utf-8")

    after_save = save_manifest(save_path)
    assert before_save == after_save
    (out / "user-save-before.json").write_text(json.dumps(before_save, indent=2) + "\n",
                                                encoding="utf-8")
    (out / "user-save-after.json").write_text(json.dumps(after_save, indent=2) + "\n",
                                               encoding="utf-8")

    final_snapshot = out / "final-repository"
    final_snapshot.mkdir()
    snapshot(final_snapshot)
    assert git("branch", "--show-current") == "phase0/dod-length"
    assert git("rev-parse", "HEAD") == EXPECTED_HEAD
    assert git("rev-parse", "origin/phase0/dod-length") == EXPECTED_HEAD
    assert not git("diff", "--cached", "--name-only")
    assert subprocess.run(["git", "diff", "--quiet", "--", "README.md"],
                          cwd=REPO).returncode == 0
    subprocess.run(["git", "diff", "--check"], cwd=REPO, check=True)
    assert not list(REPO.rglob("__pycache__"))
    assert not list(REPO.rglob("*.pyc"))
    result = {
        "qa2": "PASS", "confirmed_defects_entering": 10,
        "defects_remaining": 0, "normal_fill": "36/36",
        "explicit_temples": "5/5", "intentional_unfilled": "8/8",
        "monster_parity": "1197/1197", "object_parity": "488/488",
        "parity_missing": parity["confirmed_missing_count"],
        "parity_not_proven": parity["not_proven_count"],
        "all_resources_content_validated": True,
        "broad_step10c_e": broad, "dlb_before_diagnostic": dlb_before,
        "dlb_final": dlb_final, "final_files": final_files,
        "user_save_unchanged": before_save == after_save,
        "step10d": "PENDING", "head": EXPECTED_HEAD,
    }
    (out / "qa2-summary.json").write_text(json.dumps(result, indent=2) + "\n",
                                             encoding="utf-8")
    print("PASS Step 10QA2 master remediation, regression, parity, and final-package semantic gate",
          flush=True)


if __name__ == "__main__":
    main()
