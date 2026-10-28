import hashlib
import json
import pathlib
import subprocess
import sys

root = pathlib.Path(__file__).resolve().parents[3]
out = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else root / 'build/t473-s10/winmm-user-gdi-object'
if not out.is_absolute():
    out = root / out
cases = {
    'missing-user16to32': 'result=6 out16=aaaa out32=cccccccc gdi_to=0 gdi_from=0',
    'missing-gdi16to32': 'result=6 out16=aaaa out32=cccccccc gdi_to=0 gdi_from=0',
    'user16to32': 'result=1 out16=aaaa out32=00001234 gdi_to=0 gdi_from=0',
    'user32to16': 'result=1 out16=2222 out32=cccccccc gdi_to=0 gdi_from=0',
    'gdi16to32': 'result=1 out16=aaaa out32=55667788 gdi_to=1 gdi_from=0',
    'gdi32to16': 'result=1 out16=6789 out32=cccccccc gdi_to=0 gdi_from=1',
    'gdi-zero16to32': 'result=0 out16=aaaa out32=00000000 gdi_to=1 gdi_from=0',
    'gdi-zero32to16': 'result=0 out16=0000 out32=cccccccc gdi_to=0 gdi_from=1',
}
paths = [root / 'src/mvdm/wow32/wmmedia.c', root / 'build/.ninja/msvc-x86-release/wow32/obj/wmmedia.obj', pathlib.Path(__file__), pathlib.Path(__file__).with_name('wow-winmm-user-gdi-object-fixture.c'), pathlib.Path(__file__).with_name('wow-winmm-user-gdi-object-build.cmd'), out / 'wmmedia-seam.obj', out / 'fixture.obj', out / 'fixture.exe', out / 'fixture.map']
results = []
for case, expected in cases.items():
    p = subprocess.run([str(out / 'fixture.exe'), case], capture_output=True, text=True, check=False)
    result = p.stdout.strip()
    if p.returncode != 0 or result != case + ' ' + expected:
        raise RuntimeError(f'{case}: exit={p.returncode} stdout={result!r} stderr={p.stderr!r}')
    results.append({'case': case, 'exit': p.returncode, 'stdout': result, 'stderr': p.stderr})
receipt = {'sha256': {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}, 'results': results}
print(json.dumps(receipt, indent=2))
