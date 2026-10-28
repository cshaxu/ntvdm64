"""Original input/NLS source, object, table and resource-owner audit."""
import json
import re
import runpy
import struct
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
rows = runpy.run_path('artifacts/mvdm-audit/disk-image-observer.py')['rows']
STEMS = ('nt_input', 'nt_keycd', 'nt_nls')


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
    sources = []
    for stem in STEMS + ('nt_ertbl', 'nt_mess'):
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
        target = 'obj/host/' + stem + '.obj'
        if stem not in STEMS:
            assert not re.search(r'\b' + re.escape(path.name) + r'\b', recipe.read_text())
            assert path.resolve().as_posix() not in graph and target not in archive.split()
            sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                                dos_catalog=dos_row, wow_catalog=catalog, functions=[], disposition='legacy-generic-error-table'))
            continue
        assert re.search(r'\b' + re.escape(path.name) + r'\b', recipe.read_text())
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert path.resolve().as_posix() in edge and target in archive.split()
        assert '(VALID)' in deps[target] and '(STALE)' not in deps[target]
        obj = Coff(BUILD / target)
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
        assert functions
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, compile_edge=edge, dependency_block=deps[target],
                            object_sha256=sha(obj.data), functions=functions, disposition='partial-candidate'))

    def located(name, owner):
        found = [s for s in symbols if s['name'] == name and s['owner'] == owner]
        assert len(found) == 1
        return found[0]

    keycode = (root / 'nt_keycd.c').read_text()
    arrays = []
    for name, size in (('Scan1ToKeynum', 133), ('Scan1ToKeynumExtended', 84)):
        body = re.search(r'BYTE\s+' + name + r'\[\]\s*=\s*\{(.*?)\};', keycode, re.S)[1]
        body = re.sub(r'/\*.*?\*/|//[^\r\n]*', '', body, flags=re.S)
        values = bytes(int(s.strip().replace('UNDEFINED', '0'), 0) for s in body.split(',') if s.strip())
        assert len(values) == size
        entry = located('_' + name, 'original-softpc-host-roots:nt_keycd.obj')
        assert data_at(image, entry['va'], size) == values
        assert 'wVirtualScanCode > sizeof(' + name + ')' in keycode
        arrays.append(dict(symbol='_' + name, va=entry['va'], bytes=size, initializer_sha256=sha(values), bound_predicate='scan > sizeof(table)'))
    assert not any(s['name'] in ('_host_errors', '_hs_err_message') for s in symbols)
    key_obj = Coff(BUILD / 'obj/host/nt_input.obj')
    defined = [s for s in key_obj.symbols.values() if s[0] == '_nt_keybd_funcs' and s[2] > 0]
    assert len(defined) == 1
    _, start, section = defined[0]
    body, relocs = key_obj.sections[section - 1]
    table = located('_nt_keybd_funcs', 'original-softpc-host-roots:nt_input.obj')
    pointers = [(p - start, n, k) for p, n, k in relocs if start <= p < start + 24]
    assert len(pointers) == 6 and {p for p, _, _ in pointers} == set(range(0, 24, 4))
    slots = []
    actual = data_at(image, table['va'], 24)
    for p, name, kind in pointers:
        assert kind == 6
        expected = located(name, 'original-softpc-host-roots:nt_input.obj')['va'] + struct.unpack_from('<I', body, start + p)[0]
        value = struct.unpack_from('<I', actual, p)[0]
        assert value == expected
        slots.append(dict(index=p // 4, symbol=name, va=value))
    assert [r['symbol'] for r in slots] == ['_nt_kb_prepare'] * 4 + ['_nt_kb_light_on'] * 2
    callback = located('_SyncToggleKeys', 'original-softpc-host-roots:nt_event.obj')
    light = located('_nt_kb_light_on', 'original-softpc-host-roots:nt_input.obj')
    code, relocs = key_obj.function('_nt_kb_light_on')
    calls = [(p, n, k) for p, n, k in relocs if n == '_SyncToggleKeys']
    assert len(calls) == 1
    position, _, kind = calls[0]
    assert kind == 20 and code[position - 1] in (0xe8, 0xe9)
    displacement = struct.unpack('<i', data_at(image, light['va'] + position, 4))[0]
    assert light['va'] + position + 4 + displacement == callback['va'] + struct.unpack_from('<i', code, position)[0]
    resource = Path('src/mvdm/softpc.new/obj.vdm/resource.rc')
    generated = BUILD / 'generated/softpc-embedded-roms.rc'
    assert all(name in resource.read_text() and name in generated.read_text() for name in ('EHS_FUNC_FAILED', 'EHS_SYSTEM_ERROR', 'SM_HIDE_MOUSE', 'SM_DISPLAY_MOUSE'))
    assert 'host_nls_get_msg(error_num, message, EHS_MSG_LEN)' in (root / 'nt_error.c').read_text()
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-host-roots.lib', graph_path, map_path, deps_path,
              recipe, original_recipe, dos_path, wow_path, scope_path, frozen_path, resource, generated,
              BUILD / 'obj/host/softpc-resource.res', root / 'nt_error.c', root / 'nt_event.c', root / 'nt_reset.c',
              A / 'bios-image-observer.py', A / 'disk-image-observer.py', Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=sources, key_arrays=arrays, keyboard_slots=slots, toggle_callback=callback,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Original selected input/NLS and legacy-error file roles; code/table identity and callback target, not whole guest keyboard/locale/error lifecycle acceptance')


if __name__ == '__main__':
    result = inspect()
    (A / 'input-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['sources']), functions=sum(len(r['functions']) for r in result['sources']), keyboard_slots=len(result['keyboard_slots']), scan_table_bytes=sum(r['bytes'] for r in result['key_arrays']))))
