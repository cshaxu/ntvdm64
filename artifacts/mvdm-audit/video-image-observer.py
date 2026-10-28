"""Read-only video selection and original CPU40 dispatch correspondence."""
import json
import re
import runpy
import struct
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
rows = runpy.run_path('artifacts/mvdm-audit/disk-image-observer.py')['rows']
STEMS = ('ega_dmmy', 'ega_mode', 'gvi', 'vga_vide', 'video_io')


def inspect():
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    deps = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    root = Path('src/mvdm/softpc.new/base/video')
    recipe = root / 'sources'
    original_recipe = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/video/sources')
    assert recipe.read_bytes() == original_recipe.read_bytes()
    dos_path = Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv')
    wow_path = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv')
    scope_path = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/seam-scope.json')
    editions = {r['edition']: r for r in json.loads(scope_path.read_text())['editions']}
    dos, wow = {r['path']: r for r in rows(dos_path)}, {(r['edition'], r['path']): r for r in rows(wow_path)}
    frozen_path = A / 'frozen-file-reconciliation.csv'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-video.lib:'))
    symbols = []
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if match:
            symbols.append(dict(name=match[1], va=int(match[2], 16), function=bool(match[3]), owner=match[4]))
    sources = []
    for stem in STEMS + ('ega_trcr', 'ga_glue'):
        path = root / (stem + '.c')
        original = original_recipe.parent / path.name
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
        target = 'obj/video/' + stem + '.obj'
        if stem == 'ga_glue':
            assert re.search(r'(?m)^#\s+ga_glue\.c', recipe.read_text())
            assert path.resolve().as_posix() not in graph and target not in archive.split()
            sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                                dos_catalog=dos_row, wow_catalog=catalog, functions=[],
                                disposition='alternative-legacy-cpu-interface'))
            continue
        assert re.search(r'\b' + re.escape(path.name) + r'\b', recipe.read_text())
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert path.resolve().as_posix() in edge and target in archive.split()
        assert '(VALID)' in deps[target] and '(STALE)' not in deps[target]
        obj = Coff(BUILD / target)
        functions = []
        for symbol in symbols:
            if symbol['owner'] != 'original-softpc-video:' + stem + '.obj' or not symbol['function']:
                continue
            body, relocs = obj.function(symbol['name'])
            assert body and all(kind in (6, 20) for _, _, kind in relocs)
            ignored = {i for p, _, _ in relocs for i in range(p, p + 4)}
            actual = data_at(image, symbol['va'], len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), symbol
            functions.append(dict(symbol=symbol['name'], va=symbol['va'], bytes=len(body), relocation_words=len(relocs)))
        assert bool(functions) == (stem in STEMS)
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, compile_edge=edge, dependency_block=deps[target],
                            object_sha256=sha(obj.data), functions=functions,
                            disposition='partial-candidate' if functions else 'pending-diagnostic-profile'))

    def vector(stem, name, count):
        obj = Coff(BUILD / ('obj/video/' + stem + '.obj'))
        owner = 'original-softpc-video:' + stem + '.obj'
        definitions = [s for s in obj.symbols.values() if s[0] == name and s[2] > 0]
        assert len(definitions) == 1
        _, start, section = definitions[0]
        body, relocs = obj.sections[section - 1]
        table = [s for s in symbols if s['name'] == name and s['owner'] == owner]
        assert len(table) == 1
        selected = [(p - start, n, k) for p, n, k in relocs if start <= p < start + count * 4]
        assert len(selected) == count and {p for p, _, _ in selected} == set(range(0, count * 4, 4))
        actual = data_at(image, table[0]['va'], count * 4)
        slots = []
        for p, target, kind in selected:
            assert kind == 6
            addend = struct.unpack_from('<I', body, start + p)[0]
            value = struct.unpack_from('<I', actual, p)[0]
            local = any(s[0] == target and s[2] > 0 for s in obj.symbols.values())
            targets = [s for s in symbols if s['name'] == target and (not local or s['owner'] == owner)]
            assert value in {(s['va'] + addend) & 0xffffffff for s in targets}, (name, p, target)
            slots.append(dict(index=p // 4, symbol=target, va=value,
                              owners=[s['owner'] for s in targets if (s['va'] + addend) & 0xffffffff == value]))
        return slots

    generic = vector('video', '_video_func', 29)
    ega = vector('ega_vide', '_ega_video_func', 29)
    reads = vector('ega_read', '_Glue_reads', 3)
    writes = vector('ega_writ', '_Glue_writes', 6)
    assert all(r['owners'] == ['original-softpc-cvidc:ev_glue.obj'] for r in reads + writes)
    assert all(not re.fullmatch(r'_glue_(b|w|str)_.+', s['name']) for s in symbols)
    flags = next(line for line in graph.splitlines() if line.startswith('cflags ='))
    assert all(flag in flags.split() for flag in ('/DCPU_40_STYLE', '/DC_VID', '/DPROD'))
    bios = json.loads((A / 'bios-image-observations.json').read_text())
    assert bios['bios_slots'][0x10]['symbol'] == '_video_io' and bios['bios_slots'][0x42]['symbol'] == '_ega_video_io'
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-video.lib', graph_path, map_path, deps_path,
              recipe, original_recipe, dos_path, wow_path, scope_path, frozen_path, A / 'bios-image-observations.json',
              A / 'bios-image-observer.py', A / 'disk-image-observer.py',
              Path('src/mvdm/softpc.new/base/video/ega_read.c'), Path('src/mvdm/softpc.new/base/video/ega_writ.c'),
              Path('src/mvdm/softpc.new/base/cvidc/ev_glue.c'), Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=sources, generic_slots=generic, ega_slots=ega, cpu40_reads=reads, cpu40_writes=writes,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Selected initial image code/tables, original CPU-interface selection and catalog identities; no whole mode/VRAM/rendering or runtime-table lifetime proof')


if __name__ == '__main__':
    result = inspect()
    (A / 'video-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['sources']), functions=sum(len(r['functions']) for r in result['sources']), vector_slots=67)))
