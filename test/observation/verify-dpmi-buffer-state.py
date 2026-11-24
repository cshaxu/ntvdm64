"""Current original DPMI buffer/state contracts; not guest acceptance."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
args = parser.parse_args()
out = (repo / args.build_root).resolve()
if not out.is_relative_to(repo / 'build') or out.exists():
    raise SystemExit('A fresh repository build directory is required')
out.mkdir(parents=True)
with (out / 'build.txt').open('w', encoding='utf-8') as log:
    subprocess.run(['cmd.exe', '/d', '/c', str(repo / 'tools/build-fixtures.cmd'),
                    'x86', 'dpmi-buffer-state'], cwd=repo, stdout=log,
                   stderr=subprocess.STDOUT, timeout=300, check=True)
exe = repo / 'build/.ninja/fixtures-x86/dpmi-buffer-state.exe'
result = subprocess.run([str(exe)], capture_output=True, text=True, timeout=15)
(out / 'result.txt').write_text(result.stdout + result.stderr, encoding='utf-8')
result.check_returncode()
if 'controlled original buffer/state checks; exhaustion sentinel retained' not in result.stdout:
    raise SystemExit('Missing DPMI contract witness')
paths = [repo / 'src/mvdm-host/dpmi32' / name for name in ('buffer.c', 'savestat.c')]
paths += [repo / 'test/mvdm-host/dpmi_buffer_state_fixture.c', exe]
(out / 'identity.json').write_text(json.dumps([
    dict(path=str(p), sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in paths
], indent=2) + '\n', encoding='utf-8')
print(result.stdout, end='')
