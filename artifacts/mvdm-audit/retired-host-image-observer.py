"""Identify four original superseded NT host files and their selected owners."""
import json
import re
import runpy
import struct
import sys
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[n] for n in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = (H[n] for n in ('A', 'BUILD', 'IMAGE'))
rows = runpy.run_path(str(A / 'disk-image-observer.py'))['rows']
STEMS = ('nt_term', 'nt_smenu', 'nt_hfx', 'stf_conf')


def inspect(wrong_slot=False):
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    root = Path('src/mvdm/softpc.new/host/src')
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
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
    symbols = []
    for line in mapped.splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if match:
            symbols.append(dict(name=match[1], va=int(match[2], 16), function=bool(match[3]), owner=match[4]))
    sources = []
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
        assert not re.search(r'\b' + re.escape(path.name) + r'\b', recipe.read_text())
        assert path.resolve().as_posix() not in graph
        assert not any(s['owner'].endswith(':' + stem + '.obj') for s in symbols)
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, disposition='superseded-physical-file'))

    objects = {}
    providers = []
    provider_specs = {
        ('host', 'nt_reset', 'original-softpc-host-roots'): ('_host_applClose', '_host_terminate', '_TerminateVDM'),
        ('host', 'config', 'original-softpc-host-roots'): ('_config_inquire', '_host_runtime_inquire', '_host_runtime_set'),
        ('dem', 'demfile', 'original-mvdm-dem'): ('_demCreate', '_demOpen', '_demDelete', '_demRename'),
        ('dem', 'demdir', 'original-mvdm-dem'): ('_demCreateDir', '_demDeleteDir'),
        ('dem', 'demhndl', 'original-mvdm-dem'): ('_demRead', '_demWrite', '_demClose'),
    }
    for (group, stem, archive), names in provider_specs.items():
        target = 'obj/' + group + '/' + stem + '.obj'
        obj = objects[target] = Coff(BUILD / target)
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        for name in names:
            found = [s for s in symbols if s['name'] == name and s['owner'] == archive + ':' + stem + '.obj']
            assert len(found) == 1, name
            body, relocs = obj.function(name)
            assert body and all(k in (6, 20) for _, _, k in relocs)
            ignored = {i for p, _, _ in relocs for i in range(p, p + 4)}
            actual = data_at(image, found[0]['va'], len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), name
            providers.append(dict(**found[0], bytes=len(body), relocation_words=len(relocs), compile_edge=edge))

    def body_of(text, name):
        return re.search(r'\b' + name + r'\s*\([^)]*\)\s*\{(.*?)^\}', text, re.S | re.M)[1]

    cleanup = ('GfxCloseDown', 'host_lpt_close_all', 'host_com_close_all', 'MouseDetachMenuItem', 'DeleteConfigFiles')
    sequences = []
    for path, function in ((root / 'nt_term.c', 'host_term_cleanup'),
                           (original_recipe.parent / 'nt_reset.c', 'host_applClose'),
                           (root / 'nt_reset.c', 'host_applClose')):
        body = body_of(path.read_text(), function)
        calls = re.findall(r'\b(' + '|'.join(cleanup) + r')\s*\(', body)
        assert tuple(calls) == cleanup
        sequences.append(dict(path=str(path), function=function, calls=calls))
    old = (root / 'stf_conf.c').read_text()
    selected = (root / 'config.c').read_text()
    for name in ('host_runtime_inquire', 'host_runtime_set', 'host_runtime_init'):
        assert re.search(r'\b' + name + r'\s*(?:IFN\d)?\s*\(', old)
        assert re.search(r'\b' + name + r'\s*(?:IFN\d)?\s*\(', selected)
    assert 'Stubbs for HFX' in (root / 'nt_hfx.c').read_text()
    resource = Path('src/mvdm/softpc.new/obj.vdm/resource.rc')
    generated = BUILD / 'generated/softpc-embedded-roms.rc'
    assert 'dosbox' not in resource.read_text().lower() and 'dosbox' not in generated.read_text().lower()
    assert not any(s['name'] in ('_nt_settings_menu', '_DosDlgProc', '_bKillFlag', '_host_create', '_host_open') for s in symbols)
    archive_line = next(line for line in graph.splitlines() if line.startswith('build original-softpc-host-roots.lib:'))
    selected_sources = []
    for target in archive_line.split()[3:]:
        if not target.endswith('.obj'):
            continue
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        source = Path(re.fullmatch(r'build [^:]+: \S+ (.+)', edge)[1])
        text = source.read_text()
        assert not re.search(r'\b(?:nt_settings_menu|DosDlgProc|bKillFlag)\b', text), source
        selected_sources.append(source)

    dispatch_path = Path('src/mvdm/dos/dem/demdisp.c')
    initializer = re.search(r'PFNSVC\s+apfnSVC\s*\[\]\s*=\s*\{(.*?)\};', dispatch_path.read_text(), re.S)[1]
    initializer = re.sub(r'/\*.*?\*/|//[^\r\n]*', '', initializer, flags=re.S)
    names = ['_' + name.strip() for name in initializer.split(',') if name.strip()]
    obj = objects['obj/dem/demdisp.obj'] = Coff(BUILD / 'obj/dem/demdisp.obj')
    defined = [v for v in obj.symbols.values() if v[0] == '_apfnSVC' and v[2] > 0]
    assert len(defined) == 1
    _, start, section = defined[0]
    body, relocs = obj.sections[section - 1]
    table = [s for s in symbols if s['name'] == '_apfnSVC' and s['owner'] == 'original-mvdm-dem:demdisp.obj']
    assert len(table) == 1
    pointers = [(p - start, n, k) for p, n, k in relocs if start <= p < start + 4 * len(names)]
    assert len(pointers) == len(names) and {p for p, _, _ in pointers} == set(range(0, 4 * len(names), 4))
    slots = []
    for p, target, kind in pointers:
        expected = '_demCreateNew' if wrong_slot and p == 12 else names[p // 4]
        assert kind == 6 and target == expected, 'DEM_SLOT_EXPECTATION_MISMATCH'
        found = [s for s in symbols if s['name'] == target and s['function']]
        assert len(found) == 1, target
        value = struct.unpack('<I', data_at(image, table[0]['va'] + p, 4))[0]
        assert value == found[0]['va'] + struct.unpack_from('<I', body, start + p)[0]
        slots.append(dict(index=p // 4, symbol=target, va=value, owner=found[0]['owner']))
    inputs = [IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-host-roots.lib', graph_path, map_path,
              recipe, original_recipe, dos_path, wow_path, scope_path, frozen_path, resource, generated,
              original_recipe.parent / 'nt_reset.c', root / 'nt_reset.c', root / 'config.c', dispatch_path,
              A / 'bios-image-observer.py', A / 'disk-image-observer.py', Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    inputs += [BUILD / p for p in objects] + selected_sources
    return dict(result='pass', sources=sources, providers=providers, cleanup_sequences=sequences, dem_slots=slots,
                selected_host_sources=len(selected_sources),
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in dict.fromkeys(inputs)],
                scope='Four superseded physical files and selected NT owners; no assertion of complete DOS files/configuration/menu/termination behavior')


if __name__ == '__main__':
    result = inspect('--wrong-slot' in sys.argv)
    (A / 'retired-host-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(f"PASS four retired NT host files, {len(result['providers'])} selected functions, {len(result['dem_slots'])} DEM slots and original cleanup order")
