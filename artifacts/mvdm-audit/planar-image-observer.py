"""Bind original planar painters, dispatch vectors and munge calls to the image."""
import json
import re
import runpy
import struct
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
rows = runpy.run_path(str(A / 'disk-image-observer.py'))['rows']
STEMS = ('nt_cga', 'nt_ega', 'nt_munge')


def inspect():
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    deps = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    root = Path('src/mvdm/softpc.new/host/src')
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

    def located(name, owner):
        found = [s for s in symbols if s['name'] == name and s['owner'] == 'original-softpc-host-roots:' + owner + '.obj']
        assert len(found) == 1, (name, owner)
        return found[0]

    sources, objects = [], {}
    for stem in STEMS:
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
        obj = objects[stem] = Coff(BUILD / target)
        functions = []
        for symbol in symbols:
            if symbol['owner'] != 'original-softpc-host-roots:' + stem + '.obj' or not symbol['function']:
                continue
            body, relocs = obj.function(symbol['name'])
            assert body and all(kind in (6, 20) for _, _, kind in relocs)
            ignored = {i for p, _, _ in relocs for i in range(p, p + 4)}
            actual = data_at(image, symbol['va'], len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), symbol
            functions.append(dict(symbol=symbol['name'], va=symbol['va'], bytes=len(body), relocation_words=len(relocs)))
        assert len(functions) == {'nt_cga': 14, 'nt_ega': 23, 'nt_munge': 3}[stem]
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, compile_edge=edge, dependency_block=deps[target],
                            object_sha256=sha(obj.data), functions=functions, disposition='partial-candidate'))

    graph_obj = Coff(BUILD / 'obj/host/nt_graph.obj')
    vectors = []
    for scale in ('std', 'big', 'huge'):
        name = '_' + scale + '_colour_paint_funcs'
        defined = [s for s in graph_obj.symbols.values() if s[0] == name and s[2] > 0]
        assert len(defined) == 1
        _, start, section = defined[0]
        body, relocs = graph_obj.sections[section - 1]
        table = located(name, 'nt_graph')
        pointers = [(p - start, n, k) for p, n, k in relocs if start <= p < start + 44]
        assert len(pointers) == 11 and {p for p, _, _ in pointers} == set(range(0, 44, 4))
        slots = []
        for p, target, kind in pointers:
            assert kind == 6
            matches = [s for s in symbols if s['name'] == target and s['function'] and s['owner'].startswith('original-softpc-host-roots:')]
            assert len(matches) == 1
            value = struct.unpack('<I', data_at(image, table['va'] + p, 4))[0]
            assert value == matches[0]['va'] + struct.unpack_from('<I', body, start + p)[0]
            slots.append(dict(index=p // 4, symbol=target, va=value, owner=matches[0]['owner']))
        assert slots[1]['symbol'] == '_nt_cga_colour_med_graph_' + scale
        assert slots[5]['symbol'] == '_nt_ega_med_graph_' + scale
        assert slots[6]['symbol'] == '_nt_ega_hi_graph_' + scale
        vectors.append(dict(symbol=name, va=table['va'], slots=slots))

    calls = []
    for family in ('med', 'hi'):
        for scale, suffix in (('std', ''), ('big', '_big'), ('huge', '_huge')):
            caller = located('_nt_ega_' + family + '_graph_' + scale, 'nt_ega')
            target = located('_ega_colour_hi_munge' + suffix, 'nt_munge')
            body, relocs = objects['nt_ega'].function(caller['name'])
            matches = [(p, k) for p, n, k in relocs if n == target['name']]
            assert len(matches) == 1
            p, kind = matches[0]
            assert kind == 20 and body[p - 1] in (0xe8, 0xe9)
            displacement = struct.unpack('<i', data_at(image, caller['va'] + p, 4))[0]
            assert caller['va'] + p + 4 + displacement == target['va'] + struct.unpack_from('<i', body, p)[0]
            calls.append(dict(caller=caller['name'], target=target['name'], va=caller['va'] + p - 1))
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-host-roots.lib', BUILD / 'obj/host/nt_graph.obj',
              graph_path, map_path, deps_path, recipe, original_recipe, dos_path, wow_path, scope_path, frozen_path,
              root / 'nt_graph.c', A / 'bios-image-observer.py', A / 'disk-image-observer.py',
              Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=sources, dispatch_vectors=vectors, munge_calls=calls,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Three original planar source roles, 40 selected functions, 33 dispatch pointers and six munge call targets; not guest rendering/lifecycle acceptance')


if __name__ == '__main__':
    result = inspect()
    (A / 'planar-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print('PASS three planar source roles, 40 functions, 33 dispatch pointers, six munge calls')
