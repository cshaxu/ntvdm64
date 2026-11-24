"""Exercise NIDDB allocation rollback using an intercepted cached COFF object."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
args = parser.parse_args()
build = (root / 'build').resolve()
out = (root / args.build_root).resolve()
original = root / 'build/.ninja/msvc-x86-release/worker/obj/bios/virtual.obj'
if not out.is_relative_to(build) or out == build or not original.is_relative_to(build):
    raise SystemExit('output and diagnostic object must be within the repository build directory')
out.mkdir(parents=True, exist_ok=True)
fixture = root / 'test/mvdm-host/niddb_no_retry_fixture.c'
native = root / 'build/.ninja/fixtures-x86'
intercepted = native / 'niddb-intercept.obj'
exe = native / 'niddb-no-retry.exe'
with (out / 'build.log').open('w') as log:
    subprocess.run(['cmd.exe', '/d', '/c', str(root / 'tools/build-fixtures.cmd'),
                    'x86', 'niddb-no-retry'], cwd=root, stdout=log,
                   stderr=subprocess.STDOUT, timeout=300, check=True)
symbols = subprocess.run(['objdump', '-t', str(intercepted)], check=True,
                         capture_output=True, text=True).stdout
for name in ('_niddb_test_malloc', '_niddb_test_calloc', '_niddb_test_free'):
    if name not in symbols:
        raise SystemExit(f'missing intercepted symbol: {name}')
result = subprocess.run([str(exe)], cwd=out, capture_output=True, text=True, timeout=15)
(out / 'stdout.txt').write_text(result.stdout)
(out / 'stderr.txt').write_text(result.stderr)
markers = [f'NIDDB_NO_RETRY mode={mode} ordinal={ordinal} '
           'active_after_failure=1 active_after_exit=0 live=0'
           for mode in range(2) for ordinal in range(5)]
passed = result.returncode == 0 and all(m in result.stdout for m in markers)
paths = (fixture, original, intercepted, exe)
receipt = {'result': 'pass' if passed else 'fail', 'exit': result.returncode,
           'scope': 'intercepted object and controlled VxD boundaries, not guest execution',
           'inputs': [{'path': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()}
                      for p in paths]}
(out / 'result.json').write_text(json.dumps(receipt, indent=2) + '\n')
print(json.dumps(receipt, indent=2))
if not passed:
    raise SystemExit('NIDDB no-retry rollback contract failed')
