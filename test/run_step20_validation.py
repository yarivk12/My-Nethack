"""Build a disposable, instrumented copy for controlled Step 20 validation.

Production sources and previous evidence are never patched. Resource inputs
ignored by Git (notably nethack.ico) are copied explicitly with the generated
headers and dependency libraries needed by the existing MSVC project.
"""
from pathlib import Path
import argparse, hashlib, json, os, shutil, subprocess, ctypes

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('--out', type=Path, required=True)
p.add_argument('--driver', type=Path, required=True)
p.add_argument('--hooks', type=Path, required=True)
p.add_argument('--no-build', action='store_true')
p.add_argument('--rebuild', action='store_true', help='Rebuild an existing disposable source tree')
p.add_argument('--extra-driver', type=Path)
p.add_argument('--additional-driver', type=Path)
p.add_argument('--additional-hooks', type=Path)
p.add_argument('--include-driver', type=Path, action='append', default=[])
p.add_argument('--label', default='', help='Distinct evidence suffix for a repeated build/run')
a = p.parse_args()
# Child diagnostic failures must be logged, never display a desktop fault box.
if os.name == 'nt':
    ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
out = a.out.resolve()
copy = out / 'source'
out.mkdir(parents=True, exist_ok=True)

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def record_run(name, command, cwd, env=None):
    # Windows environment keys are case-insensitive; inherited PATH/Path
    # duplicates otherwise make MSBuild's CL task fail before compilation.
    env = {key.upper(): value for key, value in (env or os.environ).items()}
    name += a.label
    if (out / (name + '.log')).exists():
        raise SystemExit('Refusing to overwrite evidence: ' + name)
    with (out / (name + '.log')).open('w', encoding='utf8') as log:
        run = subprocess.run(command, cwd=cwd, env=env, stdout=log,
                             stderr=subprocess.STDOUT, timeout=900)
    (out / (name + '-command.json')).write_text(json.dumps(
        {'command': command, 'cwd': str(cwd), 'exit_code': run.returncode},
        indent=2), encoding='utf8')
    print(name, 'exit', run.returncode, flush=True)
    if run.returncode:
        print((out / (name + '.log')).read_text(encoding='utf8')[-4000:])
        raise SystemExit(run.returncode)

if not a.no_build and not a.rebuild:
    if copy.exists():
        raise SystemExit('Use a fresh evidence directory or --no-build.')
    paths = subprocess.check_output(['git', 'ls-files'], cwd=ROOT,
                                    text=True).splitlines()
    for name in paths:
        source = ROOT / name
        if source.is_file():
            target = copy / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
    for name in ['lib', 'include', 'tools/Release/x64', 'binary/Release/x64']:
        shutil.copytree(ROOT / name, copy / name, dirs_exist_ok=True)
    resources = ['sys/windows/nethack.ico', 'win/win32/record']
    for name in resources:
        shutil.copy2(ROOT / name, copy / name)
    (out / 'resource-inputs.json').write_text(json.dumps(
        {name: digest(ROOT / name) for name in resources}, indent=2))
    hooks = json.loads(a.hooks.read_text(encoding='utf8'))
    if a.additional_hooks:
        for change in json.loads(a.additional_hooks.read_text(encoding='utf8')):
            name=change['path']
            if 'append' in change:
                with (copy / name).open('a', encoding='utf8') as f:
                    f.write('\n'+change['append']+'\n')
            else:
                hooks.setdefault(name,[]).append(change)
    for name, changes in hooks.items():
        target = copy / name
        source = target.read_text(encoding='utf8')
        for change in changes:
            if 'old' in change:
                if change['old'] not in source:
                    raise SystemExit('Instrumentation anchor absent: ' + name)
                source = source.replace(change['old'], change['new'], -1 if change.get('replace_all') else 1)
            elif 'entry_hook' in change:
                start = source.index('\n' + change['entry_hook'] + '(')
                brace = source.index('\n{', start) + 2
                source = source[:brace] + '\n' + change['code'] + source[brace:]
        target.write_text(source, encoding='utf8')
    wrappers = {
        'src/makemon.c': '\nvoid qa_initweap(struct monst *m) { m_initweap(m); }\n',
        'src/mhitu.c': '\nint qa_hitmu(struct monst *m,int slot) { return hitmu(m,&m->data->mattk[slot]); }\n',
        'src/mhitm.c': '\nint qa_damage(struct monst *a,struct monst *b,int slot) { return mdamagem(a,b,&a->data->mattk[slot],MON_WEP(a),1); }\n',
        'src/eat.c': '\nvoid qa_cpostfx(int pm,struct obj *food) { cpostfx(pm,food); }\n',
        'test/test_step15b.c': '\n#include "qa20.c"\n',
    }
    for name, addition in wrappers.items():
        with (copy / name).open('a', encoding='utf8') as f:
            f.write(addition)
if not a.no_build:
    shutil.copy2(a.driver, copy / 'test/qa20.c')
    if a.extra_driver:
        shutil.copy2(a.extra_driver, copy / 'test/qa20_species.c')
    if a.additional_driver:
        shutil.copy2(a.additional_driver, copy / 'test/qa20_reachweb.c')
    for driver in a.include_driver:
        shutil.copy2(driver, copy / 'test' / driver.name)
    locator = Path(os.environ['ProgramFiles(x86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
    vs = Path(subprocess.check_output([str(locator), '-latest', '-products', '*',
                                      '-property', 'installationPath'], text=True).strip())
    cmd = [str(vs / 'MSBuild/Current/Bin/MSBuild.exe'),
           str(copy / 'sys/windows/vs/NetHack/NetHack.vcxproj'),
           '/p:Configuration=Release', '/p:Platform=x64', '/p:STEP15_TEST=true',
           '/v:minimal', '/nologo']
    for key, name in [('BinDir', 'bin'), ('ObjDir', 'obj'), ('SymbolsDir', 'symbols')]:
        cmd.append(f'/p:{key}={(out / name).as_posix()}/')
    record_run('build', cmd, copy)

bin_dir = out / 'bin'
for name in ['nhdat500', 'symbols.template', 'sysconf.template',
             'nethackrc.template', 'Guidebook.txt', 'opthelp', 'license']:
    shutil.copy2(ROOT / 'binary/Release/x64' / name, bin_dir / name)
(bin_dir / 'sysconf').write_text('WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n')
env = {k: v for k, v in os.environ.items()
       if not k.startswith(('NETHACK_STEP', 'STEP11_', 'STEP13_', 'STEP15_', 'CUSTOMROOM', 'SHOPTYPE'))}
env.update(NETHACK_STEP15_TEST='1', QA20_EXTRA='1',
           NETHACKOPTIONS='!news,!legacy,!tutorial,!tips')
input_paths = ([a.driver, a.hooks]
               + [v for v in [a.extra_driver, a.additional_driver,
                               a.additional_hooks] if v]
               + a.include_driver)
source_inputs = {}
for directory, pattern in [('src', '*.c'), ('include', '*.h'),
                           ('test', '*.c'), ('sys/windows/vs', '*.vcxproj')]:
    for source in sorted((copy / directory).rglob(pattern)):
        source_inputs[source.relative_to(copy).as_posix()] = digest(source)
(out / ('source-inputs' + a.label + '.json')).write_text(
    json.dumps(source_inputs, indent=2))
(out / ('build-identity' + a.label + '.json')).write_text(json.dumps({
    'head': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
    'exe_sha256': digest(bin_dir / 'NetHack.exe'),
    'driver_sha256': digest(a.driver), 'hooks_sha256': digest(a.hooks),
    'validation_inputs': {str(v): digest(v) for v in input_paths},
    'source_inputs_sha256': digest(out / ('source-inputs' + a.label + '.json')),
}, indent=2))
record_run('runtime', [str(bin_dir / 'NetHack.exe')], bin_dir, env)
result = (out / ('runtime' + a.label + '.log')).read_text(encoding='utf8')
if 'QA20 COMPLETE failures=0' not in result or '\nFAIL ' in result:
    raise SystemExit('Controlled fixture assertions failed; inspect the runtime log.')
