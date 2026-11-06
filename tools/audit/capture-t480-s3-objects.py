"""Capture the three frozen S3 expression consumers in the x86 MSVC environment."""
import gzip, json, hashlib, re, subprocess, sys
from pathlib import Path
root = Path(__file__).resolve().parents[2]
phase = sys.argv[1]
if not re.fullmatch('[a-zA-Z0-9][a-zA-Z0-9_-]*', phase):
    raise ValueError('phase must be one directory name')
out = root / 'build/t480-s3' / phase
out.mkdir(exist_ok=False)
e = json.loads(gzip.decompress((root / 'artifacts/mvdm-audit/t480-s1-build-evidence.json.gz').read_bytes()))
paths = ['src/mvdm-host/dos/command/cmdexec.c', 'src/mvdm-host/dpmi32/dpmiint.c', 'src/mvdm-host/softpc.new/base/ccpu386/fpu.c']
result = []
for f in e['files']:
    if f['path'] not in paths:
        continue
    for consumer in f['consumers']:
        key = consumer['object']
        row = e['commands'][key]
        assert row['graph'].startswith('msvc-x86')
        target = out / (Path(f['path']).stem + '.obj')
        cmd = re.sub('/Fo(?:"[^"]+"|\\S+)', '/Fo"' + target.as_posix() + '"', row['command'])
        run = subprocess.run(cmd, shell=True, cwd=row['directory'], capture_output=True, timeout=180)
        (out / (target.stem + '.log')).write_bytes(run.stdout + run.stderr)
        if run.returncode:
            raise RuntimeError(key)
        data = bytearray(target.read_bytes())
        data[4:8] = b'\x00' * 4
        result.append({'source': f['path'], 'source_sha256': hashlib.sha256((root / f['path']).read_bytes()).hexdigest(), 'object': key, 'command': row['command'], 'exit_code': run.returncode, 'coff_without_timestamp_sha256': hashlib.sha256(data).hexdigest()})
(out / 'result.json').write_text(json.dumps(result, indent=2))
print('PASS', phase, len(result), 'actual object compilations')
