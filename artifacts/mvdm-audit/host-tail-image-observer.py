"""Reconcile the seven remaining original host/src file roles for CPU40 PROD."""
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
STEMS = ('fprt', 'nt_det', 'nt_hunt', 'nt_ntfun', 'nt_vflop', 'nt_wcom', 'nt_yoda')
SELECTED = {'fprt', 'nt_det', 'nt_ntfun', 'nt_wcom', 'nt_yoda'}


def symbols(mapped):
    result = []
    for line in mapped.splitlines():
        m = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if m:
            result.append(dict(name=m[1], va=int(m[2], 16), function=bool(m[3]), owner=m[4]))
    return result


def bind(image, obj, symbol):
    body, relocs = obj.function(symbol['name'])
    assert body and all(kind in (6, 20) for _, _, kind in relocs)
    mask = {i for offset, _, _ in relocs for i in range(offset, offset + 4)}
    actual = data_at(image, symbol['va'], len(body))
    assert all(i in mask or b == actual[i] for i, b in enumerate(body)), symbol['name']
    return dict(**symbol, bytes=len(body), relocation_words=len(relocs))


def inspect():
    image_path = Path('assets/release/ntvdm.exe')
    image = Image(image_path)
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    syms = symbols(mapped)
    flags = next(line for line in graph.splitlines() if line.startswith('host_cflags ='))
    for flag in ('CPU_40_STYLE', 'CCPU', 'C_VID', 'PROD'):
        assert '/D' + flag in flags
    for flag in ('X86GFX', 'HUNTER', 'YODA', 'VFLOPPY', 'MONITOR'):
        assert not re.search(r'/D' + flag + r'(?:\s|$)', flags)
    recipe = Path('src/mvdm/softpc.new/host/src/sources')
    original_root = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src')
    assert recipe.read_bytes() == (original_root / 'sources').read_bytes()
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-host-roots.lib:'))
    wow_catalog = {(r['edition'], r['path']): r for r in rows(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv'))}
    dos = {r['path']: r for r in rows(Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv'))}
    scope = json.loads(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/seam-scope.json').read_text())
    sources, functions, inputs = [], [], [image_path, graph_path, map_path, recipe]
    for stem in STEMS:
        relative = 'softpc.new/host/src/' + stem + '.c'
        path, original = Path('src/mvdm') / relative, original_root / (stem + '.c')
        assert path.read_bytes() == original.read_bytes()
        catalog = []
        for edition in scope['editions']:
            name = edition['mvdm'] + '/' + relative
            row = wow_catalog[(edition['edition'], name)]
            assert row['source_version'] == edition['commit']
            catalog.append(dict(id=row['id'], edition=edition['edition'], path=name,
                                sha256=sha((Path(edition['root']) / name).read_bytes())))
        dos_row = dos['base/mvdm/' + relative]
        assert dos_row['sha256'].upper() == sha(path.read_bytes())
        assert bool(re.search(r'\b' + stem + r'\.c\b', recipe.read_text())) == (stem in SELECTED)
        target = 'obj/host/' + stem + '.obj'
        edges = [line for line in graph.splitlines() if line.startswith('build ' + target + ':')]
        own = [s for s in syms if s['function'] and s['owner'] == 'original-softpc-host-roots:' + stem + '.obj' and not s['name'].startswith('$')]
        deps = ''
        if stem in SELECTED:
            assert len(edges) == 1 and path.resolve().as_posix() in edges[0]
            assert target in archive.split()
            deps = subprocess.check_output([shutil.which('ninja'), '-C', str(BUILD), '-t', 'deps', target], text=True)
            assert '(VALID)' in deps and '(STALE)' not in deps
            obj = Coff(BUILD / target)
            functions.extend(bind(image, obj, s) for s in own)
            inputs.append(BUILD / target)
        else:
            assert not edges and target not in archive.split() and not own
        sources.append(dict(relative_path=relative, sha256=sha(path.read_bytes()), original_path=str(original),
                            dos_catalog=dos_row, wow_catalog=catalog, compile_edges=edges,
                            dependency_block=deps, linked_function_labels=len(own)))
    assert [r['linked_function_labels'] for r in sources] == [2, 2, 0, 11, 0, 3, 0]
    main_obj = Coff(BUILD / 'obj/support/main.obj')
    assert '_host_set_yoda_ints' not in {s[0] for s in main_obj.symbols.values()}
    yoda_obj = Coff(BUILD / 'obj/host/nt_yoda.obj')
    stub, stub_relocs = yoda_obj.function('_host_set_yoda_ints')
    assert stub == b'\xc2\x00\x00' and not stub_relocs
    assert '#ifdef HUNTER' in Path('src/mvdm/softpc.new/host/src/nt_hunt.c').read_text()
    assert '#ifdef VFLOPPY' in Path('src/mvdm/softpc.new/host/src/nt_vflop.c').read_text()
    disk_providers = []
    for name, object_path, owner in (
            ('_gfi_fdc_command', 'obj/disks/gfi.obj', 'original-softpc-disks:gfi.obj'),
            ('_gfi_init', 'obj/disks/gfi.obj', 'original-softpc-disks:gfi.obj'),
            ('_nt_gfi_rdiskette_init', 'obj/host/nt_rflop.obj', 'original-softpc-host-roots:nt_rflop.obj')):
        symbol = next(s for s in syms if s['name'] == name)
        assert symbol['owner'] == owner
        disk_providers.append(bind(image, Coff(BUILD / object_path), symbol))
        inputs.append(BUILD / object_path)
    inputs += [BUILD / 'obj/support/main.obj', BUILD / 'obj/host/nt_yoda.obj']
    # The two original Console entrypoints reach the project-owned bounded
    # host registration implementation, not a private NT4 server pointer.
    registration = next(s for s in syms if s['name'] == '_RegisterConsoleVDM@44')
    assert registration['owner'] == 'softpc-win32-bindings:console_compat.obj'
    boundary_obj = Coff(BUILD / 'obj/adapter-win32/console_compat.obj')
    boundary = bind(image, boundary_obj, registration)
    calls = []
    det = Coff(BUILD / 'obj/host/nt_det.obj')
    for symbol in [s for s in syms if s['name'] in ('_doNullRegister', '_initTextSection')]:
        body, relocs = det.function(symbol['name'])
        for offset, target, kind in relocs:
            if target != registration['name']:
                continue
            assert kind == 20 and body[offset - 1] in (0xe8, 0xe9)
            displacement = struct.unpack('<i', data_at(image, symbol['va'] + offset, 4))[0]
            assert symbol['va'] + offset + 4 + displacement == registration['va'] + struct.unpack_from('<i', body, offset)[0]
            calls.append(dict(caller=symbol['name'], target=target, va=registration['va']))
    assert len(calls) == 2
    wow_build = Path('build/.ninja/msvc-x86-release/wow32')
    wow_image_path = Path('assets/release/WOW32.DLL')
    wow_map = wow_build / 'wow32.dll.map'
    wow_image, wow_obj = Image(wow_image_path), Coff(wow_build / 'obj/wucomm.obj')
    exports = [s for s in symbols(wow_map.read_text()) if s['name'] in ('_GetCommHandle@4', '_GetCommShadowMSR@4')]
    assert len(exports) == 2 and all(s['owner'] == 'wucomm.obj' for s in exports)
    wow_functions = [bind(wow_image, wow_obj, s) for s in exports]
    inputs += [BUILD / 'obj/adapter-win32/console_compat.obj', wow_image_path, wow_map, wow_build / 'obj/wucomm.obj']
    for relative in ('src/mvdm/softpc.new/base/support/main.c', 'src/mvdm/softpc.new/host/src/nt_bop.c',
                     'src/ntvdm-exe/win32/console_compat.c', 'src/mvdm/wow32/wucomm.c'):
        inputs.append(Path(relative))
    return dict(sources=sources, flags=flags, functions=functions, registration=boundary,
                yoda_prod_stub_bytes=stub.hex(), yoda_prod_caller_absent=True, disk_providers=disk_providers,
                registration_calls=calls, wow_comm_functions=wow_functions,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Physical roles, original source/composition, 18 mapped labels and bounded host/WOW entry identity; not complete UART or display lifecycle proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
