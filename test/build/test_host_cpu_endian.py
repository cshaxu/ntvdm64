"""Check fixed register byte layout and rejection of excluded CPU30."""
import argparse
from pathlib import Path
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--scratch', required=True)
parser.add_argument('--architecture', choices=('x86', 'x64'), default='x86')
args = parser.parse_args()
scratch = (repo / args.scratch).resolve()
if not scratch.is_relative_to((repo / 'build').resolve()):
    raise SystemExit('scratch must be under repository build')
scratch.mkdir(parents=True, exist_ok=True)
exe = repo / 'build/.ninja' / ('fixtures-' + args.architecture) / 'host-cpu-endian.exe'
base = ['cmd.exe', '/d', '/c', str(repo / 'tools/build-fixtures.cmd'), args.architecture]

def build(target):
    result = subprocess.run(base + [target], cwd=repo, capture_output=True,
                            text=True, errors='replace', timeout=300,
                            creationflags=subprocess.CREATE_NO_WINDOW)
    (scratch / (target + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
    return result

good = build('host-cpu-endian')
if good.returncode:
    raise SystemExit('little-endian fixture failed to compile: ' + good.stdout + good.stderr)
run = subprocess.run([str(exe)], cwd=scratch, capture_output=True, timeout=30,
                     creationflags=subprocess.CREATE_NO_WINDOW)
if run.returncode:
    raise SystemExit('actual register low/high byte layout is incorrect')
plain = build('host-cpu-endian-default')
plain.check_returncode()
subprocess.run([str(exe.with_name('host-cpu-endian-default.exe'))], cwd=scratch,
               timeout=30, check=True, creationflags=subprocess.CREATE_NO_WINDOW)
bad = build('host-cpu-endian-reject')
if not bad.returncode or 'Excluded original conditional branch' not in bad.stdout + bad.stderr:
    raise SystemExit('excluded CPU30 alternative did not reject')
print('PASS fixed register byte layout with/without legacy macro and CPU30 rejection')
