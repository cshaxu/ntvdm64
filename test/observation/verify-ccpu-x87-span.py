"""Check selected CCPU x87 exact/short segment-limit cases."""
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
fixture = args.fixture.resolve()
output = args.output.resolve()
if not fixture.is_relative_to(build) or not output.is_relative_to(build) or output.exists():
    raise SystemExit('Require build-owned fixture and fresh output')
mapping = Path(str(fixture) + '.map')
text = mapping.read_text(errors='replace').lower()
if 'original-ccpu386:zfrsrvd.obj' not in text or 'ccpu_x87_span_fixture.obj' not in text:
    raise SystemExit('Selected decoder or fixture owner missing from link map')
rows = []
for case in range(8):
    result = subprocess.run([str(fixture), str(case)], capture_output=True,
                            text=True, errors='replace', timeout=30)
    expected = 'vector=13 error=0 ip=8002' if case & 4 else 'vector=-1 error=0'
    if result.returncode or expected not in result.stderr:
        raise SystemExit(f'case {case} failed: {result.returncode} {result.stderr}')
    rows.append({'case': case, 'exit': result.returncode, 'stderr': result.stderr.strip()})
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(json.dumps({'fixture_sha256': hashlib.sha256(fixture.read_bytes()).hexdigest(),
                              'map_sha256': hashlib.sha256(mapping.read_bytes()).hexdigest(),
                              'cases': rows}, indent=2), encoding='utf-8')
print('PASS selected CCPU x87 segment span cases')
