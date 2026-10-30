"""Run hardware-backed bit-index cases against the selected CCPU fixture."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
parser.add_argument('--worker-build', default='build/.ninja/msvc-x86-release/worker')
args = parser.parse_args()
root = (repo / args.build_root).resolve()
if not root.is_relative_to((repo / 'build').resolve()) or root.exists():
    raise SystemExit('a fresh repository build directory is required')
root.mkdir(parents=True)
tree = (repo / args.worker_build).resolve()
exe = tree / 'ccpu-bit-matrix-test.exe'
mapfile = tree / 'ccpu-bit-matrix-test.exe.map'
native = tree / 'ccpu-bit-native-oracle.exe'
maptext = mapfile.read_text(encoding='utf-8', errors='replace')
if not re.search(r'c_cpu_simulate.*original-ccpu386:c_main', maptext, re.IGNORECASE):
    raise SystemExit('link map did not establish selected CCPU decoder owner')
if not re.search(r'host_exint_hook.*ccpu_bit_matrix_fixture\.obj', maptext, re.IGNORECASE):
    raise SystemExit('link map did not establish the fixture exception hook')
results = []
oracle = subprocess.run([str(native)], cwd=tree, capture_output=True, text=True, errors='replace', timeout=60)
(root / 'native.stdout.txt').write_text(oracle.stdout, encoding='utf-8')
(root / 'native.stderr.txt').write_text(oracle.stderr, encoding='utf-8')
if oracle.returncode or ('BITMEM NEGATIVE BTS/BTR PASS' not in oracle.stdout or
        'BITMEM WORD BOUNDARIES PASS count=9' not in oracle.stdout or
        'BITMEM DWORD BOUNDARIES PASS count=9' not in oracle.stdout or
        'BITMEM WORD FAMILY PASS cases=36' not in oracle.stdout or
        'BITMEM DWORD FAMILY PASS cases=36' not in oracle.stdout):
    raise SystemExit('native bit oracle failed: ' + str(oracle.returncode) + ' ' + oracle.stdout + oracle.stderr)
for case in (0, 2, 3, 4, 6, 7, 8, 9, 10, 11, *range(12, 120)):
    env = os.environ.copy()
    env['CCPU_BIT_CASE'] = str(case)
    run = subprocess.run([str(exe)], cwd=tree, env=env, capture_output=True, text=True, errors='replace', timeout=60)
    (root / ('case-%d.stdout.txt' % case)).write_text(run.stdout, encoding='utf-8')
    (root / ('case-%d.stderr.txt' % case)).write_text(run.stderr, encoding='utf-8')
    if run.returncode:
        raise SystemExit('case %d failed: %d %s%s' % (case, run.returncode, run.stdout, run.stderr))
    results.append({'case': case, 'exit': run.returncode, 'output': run.stderr.strip()})
digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
(root / 'result.json').write_text(json.dumps({'result': 'pass', 'scope': '118 selected-object cases including BT/BTS/BTR/BTC signed boundaries and 16/32-bit addressing; guest coverage remains separate',
    'fixture_sha256': digest(exe), 'map_sha256': digest(mapfile), 'object_sha256': digest(tree / 'obj/ccpu/c_main.obj'),
    'native_oracle_sha256': digest(native),
    'cases': results}, indent=2), encoding='utf-8')
print('PASS selected CCPU bit-index control cases')
