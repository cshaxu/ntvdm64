"""Selected CCPU port instructions, controlled device and privilege boundary."""
import argparse
import hashlib
import itertools
import json
from pathlib import Path
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
args = parser.parse_args()
out = (repo / args.build_root).resolve()
build = (repo / 'build').resolve()
if not out.is_relative_to(build) or out == build or out.exists():
    raise SystemExit('fresh repository build-owned output required')
out.mkdir(parents=True)
worker = build / '.ninja/msvc-x86-release/worker'
source = repo / 'test/mvdm-host/ccpu_port_privilege_fixture.c'
obj = worker / 'obj/tests/ccpu-port-privilege.obj'
exe = worker / 'ccpu-port-privilege.exe'
mapping = worker / 'ccpu-port-privilege.exe.map'
with (out / 'build.log').open('w') as log:
    result = subprocess.run(['cmd', '/d', '/c', str(repo / 'tools/build-fixtures.cmd'),
                             'x86', 'ccpu-port-privilege'], cwd=repo,
                            stdout=log, stderr=subprocess.STDOUT,
                            creationflags=subprocess.CREATE_NO_WINDOW, timeout=300, check=False)
if result.returncode:
    raise SystemExit(f'fixture build failed: {result.returncode}')
text = mapping.read_text(encoding='utf-8', errors='replace')
for symbol, owner in (('_c_cpu_simulate', 'original-ccpu386:c_main.obj'),
                      ('_IN8', 'original-ccpu386:in.obj'),
                      ('_OUT8', 'original-ccpu386:out.obj'),
                      ('_io_connect_port', 'original-softpc-support:ios.obj'),
                      ('_host_exint_hook', 'ccpu-port-privilege.obj')):
    if not any(symbol in line and owner in line for line in text.splitlines()):
        raise SystemExit(f'unexpected link owner: {symbol}')
cases = []
for scenario, width, form, repeat in itertools.product(range(7), (1, 2, 4), (0, 1, 2, 3), (0, 1, 2)):
    if (form == 2 and repeat == 1) or (form != 2 and repeat != 1):
        continue
    marker = (f'vector=-1 error=0 ip=0000 reads={width * repeat} writes={width * repeat} '
              f'first={"5a" if repeat else "00"} result=1' if scenario in (0, 2) else
              f'vector=13 error=0 ip={"800c" if form == 2 else "8009"} reads=0 writes=0 first=00 result=1')
    result = subprocess.run([str(exe), str(scenario), str(width), str(form), str(repeat)], cwd=repo,
                            capture_output=True, text=True, creationflags=subprocess.CREATE_NO_WINDOW, timeout=60)
    if result.returncode or marker not in result.stderr:
        raise SystemExit(f'case {scenario} width {width} form {form} repeat {repeat}: {result.returncode}: {result.stderr}')
    cases.append({'case': scenario, 'width': width, 'form': form, 'repeat': repeat,
                  'exit': result.returncode, 'output': result.stderr.strip()})
for scenario, width in itertools.product(range(7, 11), (1, 2, 4)):
    result = subprocess.run([str(exe), str(scenario), str(width), '2', '2'], cwd=repo,
                            capture_output=True, text=True, creationflags=subprocess.CREATE_NO_WINDOW, timeout=60)
    if result.returncode or 'result=1 ' not in result.stderr:
        raise SystemExit(f'memory case {scenario} width {width}: {result.returncode}: {result.stderr}')
    cases.append({'case': scenario, 'width': width, 'form': 2, 'repeat': 2,
                  'exit': result.returncode, 'output': result.stderr.strip()})
files = [source, obj, exe, mapping, worker / 'original-ccpu386.lib',
         worker / 'original-softpc-support.lib']
(out / 'result.json').write_text(json.dumps({
    'result': 'pass', 'cases': cases,
    'scope': 'selected scalar/string decoder, IOS, controlled byte device/TSS and page faults; not guest dispatch or complete TSS bitmap',
    'limitations': ['fixture link uses /force:multiple and duplicate-symbol warnings'],
    'inputs': [{'path': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()}
               for p in files]}, indent=2) + '\n')
print('PASS selected CCPU scalar/string I/O privilege and memory controls')
