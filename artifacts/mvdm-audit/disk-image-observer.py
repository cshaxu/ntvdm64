"""Selected disk code/dispatch correspondence; not real-media acceptance."""
import csv
import json
import re
import runpy
import struct
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
STEMS = ('diskbios', 'fdisk', 'fla', 'floppy_i', 'floppy', 'gfi_mpty', 'gfi')
GUARDED = ('gfi_ibm', 'gfi_sflp', 'gfi_test')


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def inspect():
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    deps = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    root = Path('src/mvdm/softpc.new/base/disks')
    dos_catalog = Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv')
    wow_catalog = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv')
    scope_path = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/seam-scope.json')
    editions = {r['edition']: r for r in json.loads(scope_path.read_text())['editions']}
    dos = {r['path']: r for r in rows(dos_catalog)}
    wow = {(r['edition'], r['path']): r for r in rows(wow_catalog)}
    frozen_path = A / 'frozen-file-reconciliation.csv'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    recipe = root / 'sources'
    original_recipe = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/disks/sources')
    assert recipe.read_bytes() == original_recipe.read_bytes()
    symbols = []
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if match:
            symbols.append(dict(name=match[1], va=int(match[2], 16), function=bool(match[3]), owner=match[4]))
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-disks.lib:'))
    records = []
    for stem in STEMS + GUARDED:
        source = root / (stem + '.c')
        original = original_recipe.parent / source.name
        assert source.read_bytes() == original.read_bytes()
        relative = source.relative_to('src/mvdm').as_posix()
        dos_row = dos['base/mvdm/' + relative]
        assert dos_row['sha256'].upper() == sha(source.read_bytes())
        catalog_rows = []
        for edition, metadata in editions.items():
            path = metadata['mvdm'] + '/' + relative
            row = wow[(edition, path)]
            original_edition = Path(metadata['root']) / path
            original_hash = sha(original_edition.read_bytes())
            frozen_hash = frozen[relative]['opennt_sha256' if edition == 'OpenNT' else 'opennt45_sha256']
            assert original_hash == frozen_hash and row['source_version'] == metadata['commit']
            catalog_rows.append(dict(id=row['id'], edition=edition, path=path, original_sha256=original_hash,
                                     catalog_git_blob=row['git_blob'], catalog_status=row['inventory_status']))
        assert len(catalog_rows) == 2 and frozen[relative]['dos_catalog_rows'] == '1' and frozen[relative]['wow_catalog_rows'] == '2'
        assert re.search(r'\b' + re.escape(source.name) + r'\b', recipe.read_text())
        target = 'obj/disks/' + stem + '.obj'
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert source.resolve().as_posix() in edge and target in archive.split()
        assert '(VALID)' in deps[target] and '(STALE)' not in deps[target]
        obj = Coff(BUILD / target)
        functions = []
        for symbol in symbols:
            if symbol['owner'] != 'original-softpc-disks:' + stem + '.obj' or not symbol['function']:
                continue
            body, relocations = obj.function(symbol['name'])
            assert body and all(kind in (6, 20) for _, _, kind in relocations)
            ignored = {i for p, _, _ in relocations for i in range(p, p + 4)}
            actual = data_at(image, symbol['va'], len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), symbol
            functions.append(dict(symbol=symbol['name'], va=symbol['va'], bytes=len(body), relocation_words=len(relocations)))
        if stem in STEMS:
            assert functions, stem
        else:
            assert not functions
            guard = '#ifndef PROD' if stem == 'gfi_test' else '#ifdef SLAVEPC'
            assert guard in source.read_text()
        records.append(dict(relative_path=source.relative_to('src/mvdm').as_posix(), sha256=sha(source.read_bytes()),
                            original_path=str(original), compile_edge=edge, dependency_block=deps[target],
                            object_sha256=sha(obj.data), functions=functions,
                            dos_catalog=dos_row, wow_catalog=catalog_rows,
                            disposition='partial-candidate' if stem in STEMS else 'pending-profile-boundary'))

    def bind_vector(stem, name, count):
        obj = Coff(BUILD / ('obj/disks/' + stem + '.obj'))
        definition = [s for s in obj.symbols.values() if s[0] == name and s[2] > 0]
        assert len(definition) == 1
        _, start, section = definition[0]
        body, relocs = obj.sections[section - 1]
        table = [s for s in symbols if s['name'] == name and s['owner'] == 'original-softpc-disks:' + stem + '.obj']
        assert len(table) == 1
        selected = [(p - start, n, k) for p, n, k in relocs if start <= p < start + count * 4]
        assert len(selected) == count and {p for p, _, _ in selected} == set(range(0, count * 4, 4))
        actual = data_at(image, table[0]['va'], count * 4)
        slots = []
        for p, target, kind in selected:
            assert kind == 6
            addend = struct.unpack_from('<I', body, start + p)[0]
            value = struct.unpack_from('<I', actual, p)[0]
            targets = [s['va'] for s in symbols if s['name'] == target and s['owner'] == table[0]['owner']]
            assert value in {(v + addend) & 0xffffffff for v in targets}, (p, target)
            slots.append(dict(index=p // 4, symbol=target, va=value))
        return slots

    disk_slots = bind_vector('diskbios', '_disk_func', 22)
    floppy_slots = bind_vector('floppy', '_fl_fnc_tab', 25)
    bios = json.loads((A / 'bios-image-observations.json').read_text())
    assert bios['bios_slots'][0x13]['symbol'] == '_disk_io'
    assert bios['bios_slots'][0x40]['symbol'] == '_diskette_io'
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-disks.lib', recipe, original_recipe,
              graph_path, map_path, deps_path, A / 'bios-image-observations.json',
              A / 'bios-image-observer.py', dos_catalog, wow_catalog, scope_path, frozen_path,
              Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=records, disk_slots=disk_slots, floppy_slots=floppy_slots,
                bios_edges={'13': '_disk_io', '40': '_diskette_io'},
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Seven selected code owners and three guarded profiles; exact source and initializer pointers; no whole disk/media/teardown acceptance')


if __name__ == '__main__':
    result = inspect()
    (A / 'disk-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['sources']), functions=sum(len(r['functions']) for r in result['sources']), vector_slots=len(result['disk_slots']) + len(result['floppy_slots']))))
