from pathlib import Path
import re
import sys

base = Path(sys.argv[1] if len(sys.argv) > 1 else 'build/.ninja/t475-handle-compare-p')
cases = {
    'worker-x86/nthook32.dll.map': ('process_hook_payload_write', 'process_hook_payload_take', 'process_hook_startup_attach'),
    'hook64/nthook64.dll.map': ('process_hook_payload_write', 'process_hook_payload_take', 'process_hook_startup_attach'),
    'worker-x86/ntvdm.exe.map': ('process_hook_payload_write',),
    'worker-native/ntvwm.exe.map': ('process_hook_payload_write',),
    'launcher/run16.exe.map': ('process_hook_payload_take',),
}
def check(text, required):
    if re.search(r'detour', text, re.I):
        raise ValueError('Detours-linked symbol or object')
    missing = [name for name in required if name not in text]
    if missing:
        raise ValueError('missing product symbols: ' + ', '.join(missing))
for path, required in cases.items():
    text = (base / path).read_text(encoding='utf-8', errors='replace')
    check(text, required)
    try:
        check(text + '\nDetourTransactionBegin\n', required)
    except ValueError:
        pass
    else:
        raise SystemExit('negative control was not rejected')
    print('PASS ' + path)
print('PASS product link maps use the new payload/hook providers and reject Detours')
