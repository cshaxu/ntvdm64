"""Capture the six frozen Base client consumers in the selected MSVC environment."""
import gzip, json, re, subprocess, sys, hashlib
from pathlib import Path
r = Path(__file__).resolve().parents[2]
arch, phase = sys.argv[1:]
if arch not in ('x86', 'x64') or phase not in ('before', 'after', 'final'):
    raise ValueError('expected x86|x64 before|after|final')
out = r / 'build/t480-s3' / ('vdm-' + phase + '-' + arch)
out.mkdir(exist_ok=False)
e = json.loads(gzip.decompress((r / 'artifacts/mvdm-audit/t480-s1-build-evidence.json.gz').read_bytes()))
f = next((f for f in e['files'] if f['path'] == 'src/non-mvdm/base/win32/client/vdm.c'))
results = []
for i, c in enumerate(f['consumers']):
    row = e['commands'][c['object']]
    if not row['graph'].startswith('msvc-' + arch):
        continue
    dst = out / (str(i) + '.obj')
    cmd = re.sub('/Fo(?:"[^"]+"|\\S+)', '/Fo"' + dst.as_posix() + '"', row['command'])
    run = subprocess.run(cmd, shell=True, cwd=row['directory'], capture_output=True, timeout=180)
    dst.with_suffix('.log').write_bytes(run.stdout + run.stderr)
    if run.returncode:
        raise RuntimeError(c['object'])
    results.append({'object': c['object'], 'command': row['command'], 'file': dst.name, 'source_sha256': hashlib.sha256((r / f['path']).read_bytes()).hexdigest(), 'exit_code': 0})
(out / 'result.json').write_text(json.dumps(results, indent=2))
print('PASS', arch, phase, len(results))
