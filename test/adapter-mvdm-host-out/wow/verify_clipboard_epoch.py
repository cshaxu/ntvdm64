"""Exercise actual clipboard open/close callers and generation adapter.

Only native API boundaries and guest frame/handle decoding are substituted.
No user's clipboard is opened. Run as an owned background process.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-root', required=True, type=Path)
parser.add_argument('--architecture', choices=('x86', 'x64'), default='x86')
args = parser.parse_args()
repo = Path(__file__).resolve().parents[3]
root = args.build_root.resolve()
if root.exists() or not root.is_relative_to(repo / 'build'):
    parser.error('require a fresh build-owned directory')
root.mkdir(parents=True)
source = repo / 'src/mvdm-host/wow32/wuclip.c'
adapter = repo / 'src/compat/wow_clipboard_epoch.c'
fixture = Path(__file__).with_name('wow_clipboard_epoch_fixture.c')
record = {'inputs': {str(p.relative_to(repo)): hashlib.sha256(p.read_bytes()).hexdigest()
                    for p in (source, adapter, fixture)},
          'scope': 'actual callers/adapter with controlled native boundary; not guest acceptance'}
try:
    with (root/'compile.log').open('w') as log:
        result = subprocess.run(['cmd.exe','/d','/c',str(repo/'tools/build-fixtures.cmd'), args.architecture, 'clipboard-epoch'], cwd=repo,
                                stdout=log, stderr=subprocess.STDOUT, timeout=300, creationflags=subprocess.CREATE_NO_WINDOW)
    record['compile_exit'] = result.returncode
    result.check_returncode()
    result = subprocess.run([str(repo/'build/.ninja'/('fixtures-'+args.architecture)/'clipboard-epoch.exe')], cwd=repo, capture_output=True,
                            text=True, timeout=30, creationflags=subprocess.CREATE_NO_WINDOW)
    (root/'result.txt').write_text(result.stdout + result.stderr)
    record['test_exit'] = result.returncode
    record['result'] = result.stdout.strip()
    result.check_returncode()
    if 'failures=0 clears=6' not in result.stdout:
        raise RuntimeError('missing assertion receipt')
finally:
    (root/'receipt.json').write_text(json.dumps(record, indent=2)+'\n')
