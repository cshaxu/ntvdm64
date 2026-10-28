"""Bind each selected host header to one dependent released function body."""
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/worker')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    family = A / 'shard-0/softpc-host-headers-observations.json'
    rows = json.loads(family.read_text(encoding='utf-8-sig'))['files']
    symbols = h['symbols']((B / 'ntvdm.exe.map').read_text())
    image_path = Path('assets/release/ntvdm.exe')
    assert sha(image_path) == sha('O:/winnt/system32/ntvdm.exe')
    image = h['Image'](image_path)
    result = []
    for row in rows:
        if not row['valid_worker_objects']:
            continue
        assert row['reference_hashes'] == [row['sha256'], row['sha256']]
        chosen = None
        for path in row['valid_worker_objects']:
            obj_path = B / path
            if not obj_path.is_file():
                continue
            candidates = [s for s in symbols if s['function'] and s['owner'].lower().endswith(':' + obj_path.name.lower())]
            if not candidates:
                continue
            obj = h['Coff'](obj_path)
            for symbol in candidates:
                try:
                    binding = h['bind'](image, obj, symbol)
                except (AssertionError, StopIteration, KeyError, ValueError, IndexError):
                    continue
                chosen = dict(name=row['name'], dependency=path, object_sha256=sha(obj_path), binding=binding)
                break
            if chosen:
                break
        if chosen:
            result.append(chosen)
    return dict(result='pass', family_sha256=sha(family), selected=result, release_sha256=sha(image_path),
                map_sha256=sha(B / 'ntvdm.exe.map'),
                scope='One dependent body per header, relocation-masked; conditional declarations and runtime behavior not proved')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
