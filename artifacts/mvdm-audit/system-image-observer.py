"""Observe the selected original NT system providers and their alternatives."""
import json
import re
import runpy
from pathlib import Path

H = runpy.run_path('artifacts/mvdm-audit/bios-image-observer.py')
Coff, Image, sha, data_at = (H[name] for name in ('Coff', 'Image', 'sha', 'data_at'))
A, BUILD, IMAGE = H['A'], H['BUILD'], H['IMAGE']
STEMS = ('at_dma', 'cmosnt', 'dummy_nt', 'timer', 'timestrb', 'unexp_nt')


def inspect():
    image, cached = Image(IMAGE), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[start:end] == cached.data[start:end] for start, end, _ in image.code)
    graph_path, map_path = BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map'
    graph, mapped = graph_path.read_text().replace('$:', ':'), map_path.read_text()
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    deps = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    source_root = Path('src/mvdm/softpc.new/base/system')
    recipe = source_root / 'sources'
    original_recipe = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/system/sources')
    assert sha(recipe.read_bytes()) == sha(original_recipe.read_bytes())
    records = []
    archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-system.lib:'))
    for stem in STEMS:
        source = source_root / (stem + '.c')
        original = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/system') / source.name
        assert sha(source.read_bytes()) == sha(original.read_bytes())
        assert re.search(r'\b' + re.escape(source.name) + r'\b', recipe.read_text())
        target = 'obj/system/' + stem + '.obj'
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert source.resolve().as_posix() in edge and target in archive.split()
        assert '(VALID)' in deps[target] and '(STALE)' not in deps[target]
        obj_path = BUILD / target
        obj = Coff(obj_path)
        functions = []
        for line in mapped.splitlines():
            match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+f\s+original-softpc-system:' + stem + r'\.obj\s*', line)
            if not match:
                continue
            name, va = match[1], int(match[2], 16)
            body, relocations = obj.function(name)
            assert body and all(kind in (6, 20) for _, _, kind in relocations)
            ignored = {i for position, _, _ in relocations for i in range(position, position + 4)}
            actual = data_at(image, va, len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), name
            functions.append(dict(symbol=name, va=va, bytes=len(body), relocation_words=len(relocations),
                                  nonrelocated_bytes_matched=len(body) - len(ignored)))
        assert functions, stem
        if stem == 'dummy_nt':
            # MSVC emits near RET 0 (C2 0000), not the shorter C3 encoding.
            assert len(functions) == 1 and obj.function('_dummy_int') == (b'\xc2\x00\x00', [])
            body = re.sub(r'/\*.*?\*/', '', source.read_text(), flags=re.S)
            assert re.search(r'void\s+dummy_int\(\)\s*\{\s*;\s*\}', body)
        records.append(dict(relative_path=source.relative_to('src/mvdm').as_posix(), original_path=str(original),
                            sha256=sha(source.read_bytes()), compile_edge=edge, dependency_block=deps[target],
                            object_path=str(obj_path), object_sha256=sha(obj.data), functions=functions))
    alternatives = []
    for leaf in ('cmos.c', 'ica.old'):
        path = source_root / leaf
        original = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/system') / leaf
        assert sha(path.read_bytes()) == sha(original.read_bytes())
        assert not re.search(r'\b' + re.escape(leaf) + r'\b', recipe.read_text())
        assert path.resolve().as_posix() not in graph
        assert not any('src/mvdm/softpc.new/base/system/' + leaf in block.replace('\\', '/') for block in deps.values())
        alternatives.append(dict(relative_path=path.relative_to('src/mvdm').as_posix(), original_path=str(original), sha256=sha(path.read_bytes())))
    current_ica = source_root / 'ica.c'
    assert '1.31 10/7/94' in (source_root / 'ica.old').read_text()
    assert '1.38 10/19/95' in current_ica.read_text()
    nt_cmos = (source_root / 'cmosnt.c').read_text()
    assert 'Static init plus post is used instead of external files' in nt_cmos
    assert 'io_define_inb(CMOS_ADAPTOR, cmos_inb)' in nt_cmos
    assert 'io_define_outb(CMOS_ADAPTOR, cmos_outb)' in nt_cmos
    bios = json.loads((A / 'bios-image-observations.json').read_text())
    assert bios['bios_slots'][1]['symbol'] == '_dummy_int'
    assert bios['bios_slots'][2]['symbol'] == '_unexpected_int'
    return dict(result='pass', sources=records, alternatives=alternatives,
                dummy_leaf_code='C20000', dummy_bios_slot=1,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in
                        (IMAGE, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-system.lib', recipe, original_recipe, current_ica,
                         graph_path, map_path, deps_path, A / 'bios-image-observer.py',
                         A / 'bios-image-observations.json', Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve())],
                scope='Original NT provider selection and object/image correspondence; isolated dummy leaf has exact RET code; other device behavior remains separately tested')


if __name__ == '__main__':
    result = inspect()
    (A / 'system-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['sources']), functions=sum(len(r['functions']) for r in result['sources']), alternative_files=len(result['alternatives']))))
