"""Bind original VDD source identities and selected bodies to the current image."""
import json
import re
import runpy
import shutil
import struct
import subprocess
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD = H['A'], H['BUILD']
rows = runpy.run_path(str(A / 'disk-image-observer.py'))['rows']


def inspect():
    image_path = Path('assets/release/ntvdm.exe')
    image, obj = Image(image_path), Coff(BUILD / 'obj/host/nt_vdd.obj')
    mapped = (BUILD / 'ntvdm.exe.map').read_text()
    targets = {}
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+f\s+(\S+)\s*', line)
        if match:
            targets[match[1]] = (int(match[2], 16), match[3])
    required = {'_io_connect_port', '_io_disconnect_port', '_dmaGetAdaptor', '_host_ica_lock', '_host_ica_unlock'}
    calls = []
    functions = []
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+f\s+original-softpc-host-roots:nt_vdd.obj\s*', line)
        if not match:
            continue
        name, va = match[1], int(match[2], 16)
        body, relocs = obj.function(name)
        assert body and all(kind in (6, 20) for _, _, kind in relocs)
        ignored = {i for p, _, _ in relocs for i in range(p, p + 4)}
        actual = data_at(image, va, len(body))
        assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), name
        for offset, target, kind in relocs:
            if target not in required:
                continue
            assert kind == 20 and body[offset - 1] in (0xe8, 0xe9)
            target_va, owner = targets[target]
            displacement = struct.unpack('<i', data_at(image, va + offset, 4))[0]
            assert va + offset + 4 + displacement == target_va + struct.unpack_from('<i', body, offset)[0]
            calls.append(dict(caller=name, target=target, owner=owner, target_va=target_va))
        functions.append(dict(name=name, va=va, bytes=len(body), relocation_words=len(relocs)))
    assert len(functions) == 9
    assert {r['target'] for r in calls} == required
    graph = (BUILD / 'build.ninja').read_text().replace('$:', ':')
    edge = next(line for line in graph.splitlines() if line.startswith('build obj/host/nt_vdd.obj:'))
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-host-roots.lib:'))
    assert 'obj/host/nt_vdd.obj' in archive.split()
    deps = subprocess.check_output([shutil.which('ninja'), '-C', str(BUILD), '-t', 'deps', 'obj/host/nt_vdd.obj'], text=True)
    assert '(VALID)' in deps and '(STALE)' not in deps
    assert 'nt_vddp.h' in deps and 'nt_vdd.h' in deps
    scope = json.loads(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/seam-scope.json').read_text())
    wow = {(r['edition'], r['path']): r for r in rows(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv'))}
    dos = {r['path']: r for r in rows(Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv'))}
    sources = []
    for relative in ('softpc.new/host/src/nt_vdd.c', 'softpc.new/host/inc/nt_vddp.h'):
        path = Path('src/mvdm') / relative
        original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
        assert path.read_bytes() == original.read_bytes()
        catalog = []
        for edition in scope['editions']:
            name = edition['mvdm'] + '/' + relative
            row = wow[(edition['edition'], name)]
            assert row['source_version'] == edition['commit']
            catalog.append(dict(id=row['id'], edition=edition['edition'], path=name,
                                sha256=sha((Path(edition['root']) / name).read_bytes())))
        dos_row = dos['base/mvdm/' + relative]
        assert dos_row['sha256'].upper() == sha(path.read_bytes())
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog))
    # Public declarations are outside the 4684-file MVDM census and remain a
    # separately recorded external boundary, not a third reconciled MVDM row.
    public = Path('src/opennt-abi/source/public/sdk/inc/nt_vdd.h')
    recipe = Path('src/mvdm/softpc.new/host/src/sources')
    assert re.search(r'\bnt_vdd\.c\b', recipe.read_text())
    assert recipe.read_bytes() == Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/sources').read_bytes()
    text = Path('src/mvdm/softpc.new/host/src/nt_vdd.c').read_text()
    assert text.count('iChannel > DMA_CONTROLLER_CHANNELS*DMA_ADAPTOR_CONTROLLERS') == 3
    assert 'return w;' in text and 'wAdapter = FreeVddAdapter(hVdd);' in text
    inputs = [image_path, BUILD / 'ntvdm.exe.map', BUILD / 'obj/host/nt_vdd.obj', BUILD / 'build.ninja', public, recipe]
    return dict(sources=sources, functions=functions, calls=calls, compile_edge=edge, dependencies=deps,
                public_declaration=dict(path=str(public), sha256=sha(public.read_bytes())),
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Original bytes/catalogs and nine image-bound functions; not complete VDD lifecycle acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
