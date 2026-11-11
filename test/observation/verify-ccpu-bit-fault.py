"""Verify selected CCPU paging faults and its original split-page policy."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
parser.add_argument('--fixture', default='build/.ninja/msvc-x86-release/worker/ccpu-bit-fault-test.exe')
args = parser.parse_args()
root = (repo / args.build_root).resolve()
build = (repo / 'build').resolve()
fixture = (repo / args.fixture).resolve()
if not root.is_relative_to(build) or root.exists() or not fixture.is_relative_to(build):
    raise SystemExit('fresh evidence directory and build-owned fixture required')
root.mkdir(parents=True)
mapfile = Path(str(fixture) + '.map')
mapping = mapfile.read_text(encoding='utf-8', errors='replace')
if not re.search(r'c_cpu_simulate.*original-ccpu386:c_main', mapping, re.IGNORECASE):
    raise SystemExit('selected CCPU decoder owner not established')
if not re.search(r'host_exint_hook.*ccpu_bit_fault_fixture\.obj', mapping, re.IGNORECASE):
    raise SystemExit('fixture exception hook owner not established')
cases = []
for case, expected in ((0, 'vector=-1 error=0 cf=1 cr2=00000000 result=1'),
                       (1, 'vector=14 error=7 cf=1 cr2=7ffe0ffc result=1'),
                       (2, 'vector=14 error=6 cf=1 cr2=7ffe0ffc result=1'),
                       (3, 'vector=14 error=7 cf=1 cr2=7ffe0ffc result=1'),
                       (4, 'vector=14 error=7 cf=1 cr2=7ffe0ffc result=1'),
                       (5, 'vector=-1 error=0 cf=0 cr2=00000000 result=1'),
                       (6, 'vector=14 error=7 cf=1 cr2=7ffe0ffe result=1'),
                       (7, 'vector=14 error=7 cf=1 cr2=7ffe1001 result=1'),
                       (8, 'vector=14 error=6 cf=1 cr2=7ffe1001 result=1'),
                       (9, 'vector=14 error=7 cf=1 cr2=7ffe1000 result=1'),
                       (10, 'vector=14 error=6 cf=1 cr2=7ffe1000 result=1'),
                       (11, 'vector=14 error=7 cf=1 cr2=7ffe1002 result=1'),
                       (12, 'vector=14 error=6 cf=1 cr2=7ffe1002 result=1')):
    run = subprocess.run([str(fixture), str(case)], cwd=repo, capture_output=True,
                         text=True, errors='replace', timeout=30)
    (root / f'{case}.stdout.txt').write_text(run.stdout, encoding='utf-8')
    (root / f'{case}.stderr.txt').write_text(run.stderr, encoding='utf-8')
    if run.returncode or expected not in run.stderr:
        raise SystemExit(f'case {case} failed: {run.returncode} {run.stderr}')
    cases.append({'case': case, 'exit': run.returncode, 'output': run.stderr.strip()})
digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
(root / 'result.json').write_text(json.dumps({
    'result': 'pass', 'fixture_sha256': digest(fixture), 'map_sha256': digest(mapfile),
    'scope': 'controlled selected-CCPU paging cases; guest exception delivery remains separate',
    'cases': cases}, indent=2), encoding='utf-8')
print('PASS selected CCPU bit-operation page-fault cases')
