"""Intercept only CRT allocation references in an actual worker COFF object."""
from pathlib import Path
import argparse
import hashlib
import subprocess

root = Path(__file__).resolve().parents[3]
parser = argparse.ArgumentParser()
parser.add_argument('--object', default='build/.ninja/msvc-x86-release/worker/obj/bios/virtual.obj')
parser.add_argument('--expect-baseline', action='store_true')
args = parser.parse_args()
out = root / 'build/t473-s10/niddb-object-rollback'
out.mkdir(parents=True, exist_ok=True)
original = (root / args.object).resolve()
if not original.is_relative_to((root / 'build').resolve()):
    raise SystemExit('diagnostic object must be under build')
instrumented = out / 'virtual-intercept.obj'
fixture = root / 'artifacts/mvdm-audit/host-ch-scope/niddb-object-rollback-fixture.c'
driver = root / 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
exe = out / 'niddb-object-rollback.exe'
objcopy = ['objcopy', '--redefine-sym', '_malloc=_niddb_test_malloc',
           '--redefine-sym', '_calloc=_niddb_test_calloc',
           '--redefine-sym', '_free=_niddb_test_free', str(original), str(instrumented)]
subprocess.run(objcopy, check=True, cwd=root)
symbols = subprocess.run(['objdump', '-t', str(instrumented)], check=True,
                         capture_output=True, text=True).stdout
for name in ('_niddb_test_malloc', '_niddb_test_calloc', '_niddb_test_free'):
    if name not in symbols:
        raise SystemExit(f'missing intercepted symbol: {name}')
script = out / 'link.cmd'
script.write_text(f'@echo off\ncall "{driver}"\n'
                  f'cl.exe /nologo /MT /Od "{fixture}" "{instrumented}" '
                  f'/Fe:"{exe}" /link kernel32.lib\n')
build = subprocess.run(['cmd', '/c', str(script)], cwd=out, capture_output=True, text=True)
(out / 'link.log').write_text(build.stdout + build.stderr)
if build.returncode: raise SystemExit(f'link failed: {build.returncode}')
result = subprocess.run([str(exe)], cwd=out, capture_output=True, text=True)
print('original_object_sha256', hashlib.sha256(original.read_bytes()).hexdigest())
print('intercepted_object_sha256', hashlib.sha256(instrumented.read_bytes()).hexdigest())
print('exit', result.returncode, result.stdout.strip())
expected = ('OBJECT_FAIL ordinal=1 calls=1 live=1 errors=2 carries=2'
            if args.expect_baseline else 'OBJECT_PASS ordinals=3')
expected_exit = 1 if args.expect_baseline else 0
if result.returncode != expected_exit or expected not in result.stdout:
    raise SystemExit('selected-object allocation diagnostic failed')
