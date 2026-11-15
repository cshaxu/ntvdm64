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
parser.add_argument('--compiler-env', required=True, type=Path)
args = parser.parse_args()
repo = Path(__file__).resolve().parents[3]
root = args.build_root.resolve()
if root.exists() or not root.is_relative_to(repo / 'build'):
    parser.error('require a fresh build-owned directory')
root.mkdir(parents=True)
source = repo / 'src/mvdm-host/wow32/wuclip.c'
adapter = repo / 'src/wow32-dll/source/wow_clipboard_epoch.c'
fixture = Path(__file__).with_name('wow_clipboard_epoch_fixture.c')
raw = source.read_text()
def body(name):
    start = raw.index('ULONG FASTCALL ' + name + '(')
    end = raw.index('{', start) + 1
    depth = 1
    while depth:
        depth += (raw[end] == '{') - (raw[end] == '}')
        end += 1
    return raw[start:end]
(root / 'selected-clipboard-open-close.inc').write_text(
    body('WU32OpenClipboard') + '\n' + body('WU32CloseClipboard') + '\n')
(root / 'precomp.h').write_text('''#include <windows.h>
DWORD WINAPI epoch_sequence(void);
HWND WINAPI epoch_owner(void);
DWORD WINAPI epoch_owner_pid(HWND, LPDWORD);
DWORD WINAPI epoch_current_pid(void);
void W32EmptyClipboard(void);
#define GetClipboardSequenceNumber epoch_sequence
#define GetClipboardOwner epoch_owner
#define GetWindowThreadProcessId epoch_owner_pid
#define GetCurrentProcessId epoch_current_pid
''')
command = root / 'compile.cmd'
command.write_text('@echo off\ncall "' + str(args.compiler_env.resolve()) +
    '" -arch=x86 -host_arch=x64 >nul\nif errorlevel 1 exit /b %errorlevel%\n' +
    'cl.exe /nologo /W4 /WX /I"' + str(root) + '" "' + str(fixture) +
    '" "' + str(adapter) + '" /Fo"' + root.as_posix() + '/" /Fe"' +
    str(root/'fixture.exe') + '"\nexit /b %errorlevel%\n')
record = {'inputs': {str(p.relative_to(repo)): hashlib.sha256(p.read_bytes()).hexdigest()
                    for p in (source, adapter, fixture)},
          'scope': 'actual callers/adapter with controlled native boundary; not guest acceptance'}
try:
    with (root/'compile.log').open('w') as log:
        result = subprocess.run(['cmd.exe','/d','/c',str(command)], cwd=repo,
                                stdout=log, stderr=subprocess.STDOUT, timeout=60)
    record['compile_exit'] = result.returncode
    result.check_returncode()
    result = subprocess.run([str(root/'fixture.exe')], cwd=repo, capture_output=True,
                            text=True, timeout=30)
    (root/'result.txt').write_text(result.stdout + result.stderr)
    record['test_exit'] = result.returncode
    record['result'] = result.stdout.strip()
    result.check_returncode()
    if 'failures=0 clears=6' not in result.stdout:
        raise RuntimeError('missing assertion receipt')
finally:
    (root/'receipt.json').write_text(json.dumps(record, indent=2)+'\n')
