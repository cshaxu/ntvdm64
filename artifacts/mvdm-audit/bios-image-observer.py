"""Read-only original BIOS source/object/vector correspondence, not guest proof."""
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path('tests/observation').resolve()))
from verify_ccpu_sign_flag_image import Coff, Image, sha

A = Path('artifacts/mvdm-audit')
BUILD = Path('build/.ninja/msvc-x86-release/worker')
IMAGE = Path('build/t473-s10/candidate-validation/package/system32/ntvdm.exe')
STEMS = ('bios', 'cmos_bis', 'cntlbop', 'emm_fncs', 'ill_bop', 'rom_basc', 'slave_bs')


def data_at(image, address, count):
    pe = struct.unpack_from('<I', image.data, 60)[0]
    sections = struct.unpack_from('<H', image.data, pe + 6)[0]
    optional = struct.unpack_from('<H', image.data, pe + 20)[0]
    for index in range(sections):
        offset = pe + 24 + optional + 40 * index
        virtual, rva, size, raw = struct.unpack_from('<IIII', image.data, offset + 8)
        delta = address - image.base - rva
        if 0 <= delta and delta + count <= max(virtual, size):
            backed = max(0, min(count, size - delta))
            return image.data[raw + delta:raw + delta + backed] + bytes(count - backed)
    raise AssertionError('Unmapped image address')


def inspect(image_path=IMAGE):
    image, cached = Image(image_path), Image(BUILD / 'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[start:end] == cached.data[start:end] for start, end, _ in image.code)
    map_path = BUILD / 'ntvdm.exe.map'
    symbols = []
    for line in map_path.read_text().splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+(?:(f)\s+)?(\S+)\s*', line)
        if match:
            symbols.append(dict(name=match[1], va=int(match[2], 16), function=bool(match[3]), owner=match[4]))
    graph_path = BUILD / 'build.ninja'
    graph = graph_path.read_text().replace('$:', ':')
    deps_path = A / 'ccpu-remaining-header-source/deps.stdout.txt'
    blocks = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps_path.read_text()) if b.startswith('obj/')}
    records = []
    for stem in STEMS:
        relative = 'softpc.new/base/bios/' + stem + '.c'
        source = Path('src/mvdm') / relative
        original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
        assert sha(source.read_bytes()) == sha(original.read_bytes())
        target = 'obj/bios/' + stem + '.obj'
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + target + ':'))
        assert source.resolve().as_posix() in edge
        archive = next(line for line in graph.splitlines() if line.startswith('build original-softpc-bios.lib:'))
        assert target in archive.split()
        assert '(VALID)' in blocks[target] and '(STALE)' not in blocks[target]
        obj_path = BUILD / target
        obj = Coff(obj_path)
        functions = []
        for symbol in symbols:
            if symbol['owner'] != 'original-softpc-bios:' + stem + '.obj':
                continue
            # Data such as BIOS is checked separately below.
            if not symbol['function']:
                continue
            body, relocations = obj.function(symbol['name'])
            assert body and all(kind in (6, 20) for _, _, kind in relocations)
            ignored = {i for position, _, _ in relocations for i in range(position, position + 4)}
            actual = data_at(image, symbol['va'], len(body))
            assert all(i in ignored or value == actual[i] for i, value in enumerate(body)), symbol
            functions.append(dict(symbol=symbol['name'], va=symbol['va'], bytes=len(body),
                                  relocation_words=len(relocations), nonrelocated_bytes_matched=len(body) - len(ignored)))
        records.append(dict(relative_path=relative, source_sha256=sha(source.read_bytes()),
                            original_path=str(original), compile_edge=edge, dependency_block=blocks[target],
                            object_path=str(obj_path), object_sha256=sha(obj.data), functions=functions))
    def bind_vector(stem, table_name, count):
        obj = Coff(BUILD / ('obj/bios/' + stem + '.obj'))
        owner = 'original-softpc-bios:' + stem + '.obj'
        definition = [s for s in obj.symbols.values() if s[0] == table_name and s[2] > 0]
        assert len(definition) == 1
        _, start, section = definition[0]
        body, relocations = obj.sections[section - 1]
        size = count * 4
        pointers = [(pos - start, name, kind) for pos, name, kind in relocations if start <= pos < start + size]
        assert len(pointers) == count and {p for p, _, _ in pointers} == set(range(0, size, 4))
        table = [s for s in symbols if s['name'] == table_name and s['owner'] == owner]
        assert len(table) == 1
        vector = data_at(image, table[0]['va'], size)
        slots = []
        for position, name, kind in pointers:
            assert kind == 6
            addend = struct.unpack_from('<I', body, start + position)[0]
            value = struct.unpack_from('<I', vector, position)[0]
            definitions = [s for s in obj.symbols.values() if s[0] == name and s[2] > 0]
            candidates = [s for s in symbols if s['name'] == name and (not definitions or s['owner'] == owner)]
            assert value in {(s['va'] + addend) & 0xffffffff for s in candidates}, (position // 4, name)
            slots.append(dict(index=position // 4, symbol=name, va=value))
        return slots

    slots = bind_vector('bios', '_BIOS', 256)
    ems = bind_vector('emm_fncs', '_emm_func', 30)
    by_number = {r['index']: r['symbol'] for r in slots}
    assert by_number[0xff] == '_control_bop' and by_number[0x67] == '_emm_io'
    assert by_number[0xf0] == by_number[0xf7] == '_illegal_bop'
    for name in ('_base_bop_table', '_host_bop_table'):
        table = [s for s in symbols if s['name'] == name]
        assert len(table) == 1 and data_at(image, table[0]['va'], 8) == bytes(8)
    inputs = [image_path, BUILD / 'ntvdm.exe', BUILD / 'original-softpc-bios.lib', graph_path, map_path, deps_path,
              Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', sources=records, bios_slots=slots, ems_slots=ems, empty_control_tables=True,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Original source, VALID compile/archive selection, function bytes excluding relocations, and all actual BIOS initializer relocation targets; not whole guest behavior')


if __name__ == '__main__':
    result = inspect()
    (A / 'bios-image-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['sources']), functions=sum(len(r['functions']) for r in result['sources']), bios_slots=len(result['bios_slots']), ems_slots=len(result['ems_slots']))))
