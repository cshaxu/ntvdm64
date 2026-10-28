"""Original CPU40 UMB, host-SAS alternative and physical-video owner audit."""
import json
import re
import runpy
import struct
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
rows = runpy.run_path(str(A / 'disk-image-observer.py'))['rows']


def inspect():
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    root = Path('src/mvdm/softpc.new/host/src')
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    flags = next(line for line in graph.splitlines() if line.startswith('host_cflags = '))
    assert '/DCPU_40_STYLE' in flags and '/DCCPU' in flags and '/DC_VID' in flags
    assert not any('/D' + name in flags for name in ('MONITOR', 'X86GFX', 'HOST_SAS', 'SUN_VA'))
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    deps = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    recipe = root / 'sources'
    original_recipe = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/sources')
    assert recipe.read_bytes() == original_recipe.read_bytes()
    dos_path = Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv')
    wow_path = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv')
    scope_path = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/seam-scope.json')
    editions = {r['edition']: r for r in json.loads(scope_path.read_text())['editions']}
    dos, wow = {r['path']: r for r in rows(dos_path)}, {(r['edition'], r['path']): r for r in rows(wow_path)}
    frozen_path = A / 'frozen-file-reconciliation.csv'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-host-roots.lib:'))
    symbols = []
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if match:
            symbols.append(dict(name=match[1], va=int(match[2], 16), function=bool(match[3]), owner=match[4]))

    def bind(symbol, obj):
        body, relocs = obj.function(symbol['name'])
        assert body and all(kind in (6, 20) for _, _, kind in relocs)
        ignored = {i for p, _, _ in relocs for i in range(p, p + 4)}
        actual = data_at(image, symbol['va'], len(body))
        assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), symbol
        return dict(symbol=symbol['name'], va=symbol['va'], bytes=len(body), relocation_words=len(relocs))

    sources = []
    for stem in ('nt_umb', 'nt_sas', 'nt_sec'):
        path, original = root / (stem + '.c'), original_recipe.parent / (stem + '.c')
        assert path.read_bytes() == original.read_bytes()
        relative = path.relative_to('src/mvdm').as_posix()
        dos_row = dos['base/mvdm/' + relative]
        assert dos_row['sha256'].upper() == sha(path.read_bytes())
        catalog = []
        for edition, metadata in editions.items():
            name = metadata['mvdm'] + '/' + relative
            row = wow[(edition, name)]
            original_hash = sha((Path(metadata['root']) / name).read_bytes())
            assert original_hash == frozen[relative]['opennt_sha256' if edition == 'OpenNT' else 'opennt45_sha256']
            assert row['source_version'] == metadata['commit']
            catalog.append(dict(id=row['id'], edition=edition, path=name, original_sha256=original_hash, catalog_git_blob=row['git_blob']))
        target = 'obj/host/' + stem + '.obj'
        assert re.search(r'\b' + re.escape(path.name) + r'\b', recipe.read_text())
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert path.resolve().as_posix() in edge and target in archive.split()
        assert '(VALID)' in deps[target] and '(STALE)' not in deps[target]
        obj = Coff(BUILD / target)
        functions = [bind(s, obj) for s in symbols if s['function'] and s['owner'] == 'original-softpc-host-roots:' + stem + '.obj']
        assert len(functions) == (7 if stem == 'nt_umb' else 0)
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, compile_edge=edge, dependency_block=deps[target],
                            object_sha256=sha(obj.data), functions=functions,
                            disposition='partial-candidate' if stem == 'nt_umb' else 'alternative-profile-file'))
    assert not any(s['name'] in ('_host_sas_funcs', '_LoseRegenMemory', '_RegainRegenMemory', '_GetROMsMapped') for s in symbols)
    sas_obj = Coff(BUILD / 'obj/ccpu/ccpusas4.obj')
    providers = []
    for name in ('_c_sas_connect_memory', '_sas_init', '_sas_term'):
        found = [s for s in symbols if s['name'] == name and s['owner'] == 'original-ccpu386:ccpusas4.obj']
        assert len(found) == 1
        providers.append(bind(found[0], sas_obj))
    callers = []
    msscs_obj = Coff(BUILD / 'obj/host/nt_msscs.obj')
    for name, target in (('_VDDAllocMem', '_VDDCommitUMB'), ('_VDDFreeMem', '_VDDDeCommitUMB')):
        found = [s for s in symbols if s['name'] == name and s['owner'] == 'original-softpc-host-roots:nt_msscs.obj']
        destination = [s for s in symbols if s['name'] == target and s['owner'] == 'original-softpc-host-roots:nt_umb.obj']
        assert len(found) == len(destination) == 1
        bound = bind(found[0], msscs_obj)
        body, relocs = msscs_obj.function(name)
        matches = [(p, k) for p, n, k in relocs if n == target]
        assert len(matches) == 1
        p, kind = matches[0]
        assert kind == 20 and body[p - 1] in (0xe8, 0xe9)
        displacement = struct.unpack('<i', data_at(image, found[0]['va'] + p, 4))[0]
        assert found[0]['va'] + p + 4 + displacement == destination[0]['va'] + struct.unpack_from('<i', body, p)[0]
        callers.append(dict(**bound, target=target))
    public = (root / 'nt_msscs.c').read_text()
    assert public.count('pmh->StartAddr <= dwStart &&') == 2
    assert public.count('pmh->StartAddr + pmh->Count >= dwStart + count)') == 2
    assert '#ifdef HOST_SAS' in (root / 'nt_sas.c').read_text()
    assert '#ifdef SUN_VA' in (root / 'nt_sas.c').read_text()
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-host-roots.lib', BUILD / 'obj/host/nt_msscs.obj',
              BUILD / 'obj/ccpu/ccpusas4.obj', graph_path, map_path, deps_path, recipe, original_recipe,
              dos_path, wow_path, scope_path, frozen_path, root / 'nt_msscs.c', root / 'nt_fulsc.c',
              Path('src/mvdm/softpc.new/obj.vdm/CDEFINE.INC'), Path('src/mvdm/softpc.new/base/ccpu386/ccpusas4.c'),
              A / 'bios-image-observer.py', A / 'disk-image-observer.py', Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=sources, sas_providers=providers, public_callers=callers,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Original UMB owner and physical alternative SAS/video files; actual public callers bound, no claim of complete guest memory-hook or physical hardware lifecycle')


if __name__ == '__main__':
    result = inspect()
    (A / 'memory-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print('PASS three memory source roles, seven UMB functions, three CPU40 SAS providers, two public VDD callers')
