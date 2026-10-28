"""Bind shared MVDM header dependents to their actual released component."""
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    family_path = A / 'shard-0/mvdm-inc-header-family-observations.json'
    family = json.loads(family_path.read_text(encoding='utf-8-sig'))
    specs = {'worker': ('ntvdm.exe', B / 'worker/ntvdm.exe.map'),
             'redir': ('VDMREDIR.DLL', A / 'shard-3/redir-private-map.txt'),
             'wow32': ('WOW32.DLL', B / 'wow32/wow32.dll.map')}
    images, symbols = {}, {}
    for key, (name, map_path) in specs.items():
        path = Path('assets/release') / name
        assert sha(path) == sha(Path('O:/winnt/system32') / name)
        images[key] = h['Image'](path)
        symbols[key] = h['symbols'](map_path.read_text())
    selected = []
    for row in family['files']:
        assert len(set(row['hashes'])) == 1
        chosen = None
        for dep in row['valid_objects']:
            graph, relative = dep.split('/', 1)
            key = 'redir' if graph == 'worker' and relative.startswith('obj/redir/') else graph
            obj_path = B / graph / relative
            if not obj_path.is_file():
                continue
            candidates = [s for s in symbols[key] if s['function'] and
                          (s['owner'].lower() == obj_path.name.lower() or s['owner'].lower().endswith(':' + obj_path.name.lower()))]
            if not candidates:
                continue
            obj = h['Coff'](obj_path)
            for symbol in candidates:
                try:
                    binding = h['bind'](images[key], obj, symbol)
                except (AssertionError, StopIteration, KeyError, ValueError, IndexError):
                    continue
                chosen = dict(path=row['path'], dependency=dep, object_sha256=sha(obj_path),
                              component=key, binding=binding)
                break
            if chosen:
                break
        if chosen:
            selected.append(chosen)
    return dict(result='pass', family_sha256=sha(family_path), selected=selected,
                release_sha256={key: sha(Path('assets/release') / name) for key, (name, _) in specs.items()},
                map_sha256={key: sha(path) for key, (_, path) in specs.items()},
                scope='One dependent relocation-masked body per header; no proof of individual declarations or runtime behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
