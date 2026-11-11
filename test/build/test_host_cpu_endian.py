"""Check actual register byte layout and rejection of the unused alternative."""
import argparse
from pathlib import Path
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--scratch', required=True)
args = parser.parse_args()
scratch = (repo / args.scratch).resolve()
if not scratch.is_relative_to((repo / 'build').resolve()):
    raise SystemExit('scratch must be under repository build')
scratch.mkdir(parents=True, exist_ok=True)
source = repo / 'test/build/host_cpu_endian_fixture.c'
exe = scratch / 'host-cpu-endian.exe'
obj = scratch / 'host-cpu-endian.obj'
base = ['cl.exe', '/nologo', '/TC', '/MT', str(source), '/Fo' + str(obj), '/Fe' + str(exe)]
good = subprocess.run(base + ['/DLITTLEND'], cwd=scratch, capture_output=True, text=True, errors='replace')
if good.returncode:
    raise SystemExit('little-endian fixture failed to compile: ' + good.stdout + good.stderr)
run = subprocess.run([str(exe)], cwd=scratch, capture_output=True)
if run.returncode:
    raise SystemExit('actual register low/high byte layout is incorrect')
bad = subprocess.run(base, cwd=scratch, capture_output=True, text=True, errors='replace')
if not bad.returncode or 'unsupported non-little-endian host register layout' not in bad.stdout + bad.stderr:
    raise SystemExit('unsupported big-endian alternative did not reject')
print('PASS actual register byte layout and non-little-endian rejection')
