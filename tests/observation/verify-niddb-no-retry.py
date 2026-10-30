"""Exercise NIDDB allocation rollback using an intercepted cached COFF object."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
parser.add_argument('--object', default='build/.ninja/msvc-x86-release/worker/obj/bios/virtual.obj')
parser.add_argument('--expect-baseline', action='store_true')
args = parser.parse_args()
build = (root / 'build').resolve()
out = (root / args.build_root).resolve()
original = (root / args.object).resolve()
if not out.is_relative_to(build) or out == build or not original.is_relative_to(build):
    raise SystemExit('output and diagnostic object must be within the repository build directory')
out.mkdir(parents=True, exist_ok=True)
fixture = root / 'tests/mvdm-host/niddb_no_retry_fixture.c'
driver = root / 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
intercepted = out / 'virtual-intercept.obj'
exe = out / 'fixture.exe'
subprocess.run(['objcopy', '--redefine-sym', '_malloc=_niddb_test_malloc',
                '--redefine-sym', '_calloc=_niddb_test_calloc',
                '--redefine-sym', '_free=_niddb_test_free',
                str(original), str(intercepted)], check=True, cwd=root)
symbols = subprocess.run(['objdump', '-t', str(intercepted)], check=True,
                         capture_output=True, text=True).stdout
for name in ('_niddb_test_malloc', '_niddb_test_calloc', '_niddb_test_free'):
    if name not in symbols:
        raise SystemExit(f'missing intercepted symbol: {name}')
script = out / 'link.cmd'
script.write_text(f'@echo off\ncall "{driver}"\n'
                  f'cl.exe /nologo /MT /Od "{fixture}" "{intercepted}" '
                  f'/Fo:"{out / "fixture.obj"}" /Fe:"{exe}" /link kernel32.lib\n')
linked = subprocess.run(['cmd', '/c', str(script)], cwd=out, capture_output=True, text=True)
(out / 'link.log').write_text(linked.stdout + linked.stderr)
if linked.returncode:
    raise SystemExit(f'link failed: {linked.returncode}')
result = subprocess.run([str(exe)], cwd=out, capture_output=True, text=True)
(out / 'stdout.txt').write_text(result.stdout)
(out / 'stderr.txt').write_text(result.stderr)
expected = ('NIDDB_NO_RETRY_FAIL mode=0 ordinal=1 calls=1 live=1 errors=1 carries=1'
            if args.expect_baseline else None)
markers = [f'NIDDB_NO_RETRY mode={mode} ordinal={ordinal} '
           'active_after_failure=1 active_after_exit=0 live=0'
           for mode in range(2) for ordinal in range(5)]
passed = (result.returncode == 3 and expected in result.stdout if args.expect_baseline
          else result.returncode == 0 and all(m in result.stdout for m in markers))
paths = (fixture, original, intercepted, exe)
receipt = {'result': 'pass' if passed else 'fail', 'exit': result.returncode,
           'scope': 'intercepted object and controlled VxD boundaries, not guest execution',
           'baseline_control': args.expect_baseline,
           'inputs': [{'path': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()}
                      for p in paths]}
(out / 'result.json').write_text(json.dumps(receipt, indent=2) + '\n')
print(json.dumps(receipt, indent=2))
if not passed:
    raise SystemExit('NIDDB no-retry rollback contract failed')
