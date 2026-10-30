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
driver = build / '.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
source = repo / 'tests/mvdm-host/ccpu_port_privilege_fixture.c'
commands = subprocess.check_output(
    ['ninja', '-C', str(worker), '-t', 'commands', 'ccpu-bit-fault-test.exe'],
    text=True).splitlines()
compile_cmd = next(s for s in commands if 'ccpu_bit_fault_fixture.c' in s)
link_cmd = next(s for s in commands if s.startswith('link.exe ') and
                '/out:ccpu-bit-fault-test.exe ' in s)
obj, exe = out / 'fixture.obj', out / 'fixture.exe'
mapping = out / 'fixture.exe.map'
compile_cmd = compile_cmd.replace('obj/tests/ccpu_bit_fault_fixture.obj', obj.as_posix())
compile_cmd = compile_cmd.replace('tests/mvdm-host/ccpu_bit_fault_fixture.c',
                                  'tests/mvdm-host/ccpu_port_privilege_fixture.c')
link_cmd = link_cmd.replace('obj/tests/ccpu_bit_fault_fixture.obj', obj.as_posix())
link_cmd = link_cmd.replace('/out:ccpu-bit-fault-test.exe', '/out:' + exe.as_posix())
link_cmd = link_cmd.replace('/map:ccpu-bit-fault-test.exe.map', '/map:' + mapping.as_posix())
batch = out / 'build.cmd'
batch.write_text('@echo off\ncall "' + str(driver) + '"\n' + compile_cmd +
                 '\nif errorlevel 1 exit /b 1\n' + link_cmd + '\nexit /b %errorlevel%\n',
                 encoding='ascii')
with (out / 'build.log').open('w') as log:
    result = subprocess.run(['cmd', '/d', '/c', str(batch)], cwd=worker,
                            stdout=log, stderr=subprocess.STDOUT,
                            creationflags=subprocess.CREATE_NO_WINDOW, check=False)
if result.returncode:
    raise SystemExit(f'fixture build failed: {result.returncode}')
text = mapping.read_text(encoding='utf-8', errors='replace')
for symbol, owner in (('_c_cpu_simulate', 'original-ccpu386:c_main.obj'),
                      ('_IN8', 'original-ccpu386:in.obj'),
                      ('_OUT8', 'original-ccpu386:out.obj'),
                      ('_io_connect_port', 'original-softpc-support:ios.obj'),
                      ('_host_exint_hook', 'fixture.obj')):
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
                            capture_output=True, text=True, creationflags=subprocess.CREATE_NO_WINDOW)
    if result.returncode or marker not in result.stderr:
        raise SystemExit(f'case {scenario} width {width} form {form} repeat {repeat}: {result.returncode}: {result.stderr}')
    cases.append({'case': scenario, 'width': width, 'form': form, 'repeat': repeat,
                  'exit': result.returncode, 'output': result.stderr.strip()})
for scenario, width in itertools.product(range(7, 11), (1, 2, 4)):
    result = subprocess.run([str(exe), str(scenario), str(width), '2', '2'], cwd=repo,
                            capture_output=True, text=True, creationflags=subprocess.CREATE_NO_WINDOW)
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
