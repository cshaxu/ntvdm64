"""Exercise selected CCPU x87 operands across non-contiguous and faulting pages."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('--fixture', required=True, type=Path)
parser.add_argument('--output', required=True, type=Path)
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
build = (repo / 'build').resolve()
fixture, output = args.fixture.resolve(), args.output.resolve()
if not fixture.is_relative_to(build) or not output.is_relative_to(build) or output.exists():
    raise SystemExit('Require build-owned fixture and fresh output')
mapping = Path(str(fixture) + '.map')
maptext = mapping.read_text(errors='replace').lower()
if 'original-ccpu386:zfrsrvd.obj' not in maptext or 'ccpu_x87_page_fixture.obj' not in maptext:
    raise SystemExit('Selected decoder or fixture owner missing from map')
rows = []
for case in range(128):
    if (case & 32 and case & 4) or (case & 64 and case & (32 | 4)):
        continue
    run = subprocess.run([str(fixture), str(case)], capture_output=True,
                         text=True, errors='replace', timeout=30)
    if run.returncode or f'case={case} ' not in run.stderr:
        raise SystemExit(f'case {case} failed: {run.returncode} {run.stderr}')
    rows.append({'case': case, 'exit': run.returncode, 'stderr': run.stderr.strip()})
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps({'fixture_sha256': hashlib.sha256(fixture.read_bytes()).hexdigest(),
                              'map_sha256': hashlib.sha256(mapping.read_bytes()).hexdigest(),
                              'cases': rows}, indent=2), encoding='utf-8')
print(f'PASS {len(rows)} selected CCPU x87 cross-page cases')
