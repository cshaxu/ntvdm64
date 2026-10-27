"""Observe selected original foundation objects; never claim guest behavior."""
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path('tests/observation').resolve()))
from verify_ccpu_sign_flag_image import Coff, Image, sha

ROOT = Path('build/.ninja/msvc-x86-release/worker')
STEMS = ('c_addr', 'c_bsic', 'c_intr', 'c_neg64', 'c_prot', 'c_stack')


def inspect():
    graph_path = ROOT / 'build.ninja'
    graph = graph_path.read_text().replace('$:', ':')
    map_path = ROOT / 'ntvdm.exe.map'
    map_text = map_path.read_text()
    image_path = ROOT / 'ntvdm.exe'
    image = Image(image_path)
    deps_path = Path('build/t473-s10/ccpu-foundation/deps.txt')
    blocks = re.split(r'(?=^obj/ccpu/)', deps_path.read_text(), flags=re.M)
    deps = {b.split(':', 1)[0]: b for b in blocks if b.startswith('obj/')}
    records = []
    for stem in STEMS:
        relative = 'softpc.new/base/ccpu386/' + stem + '.c'
        obj_name = 'obj/ccpu/' + stem + '.obj'
        source = Path('src/mvdm') / relative
        original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
        assert sha(source.read_bytes()) == sha(original.read_bytes())
        edge = f'build {obj_name}: cc_ccpu O:/repos.hobby/ntvdm64/src/mvdm/{relative}'
        assert edge in graph
        library = next(line for line in graph.splitlines() if line.startswith('build original-ccpu386.lib:'))
        assert obj_name in library.split()
        block = deps[obj_name]
        assert '(VALID)' in block and '(STALE)' not in block
        object_path = ROOT / obj_name
        obj = Coff(object_path)
        functions = []
        for line in map_text.splitlines():
            match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+f\s+original-ccpu386:' + stem + r'\.obj\s*', line)
            if not match:
                continue
            symbol, va = match[1], int(match[2], 16)
            code, entries = obj.function(symbol)
            assert code and all(kind in (6, 20) for _, _, kind in entries)
            positions = [start + va - image.base - rva for start, end, rva in image.code
                         if image.base + rva <= va < image.base + rva + end - start]
            assert len(positions) == 1
            offset = positions[0]
            ignored = {i for address, _, _ in entries for i in range(address, address + 4)}
            assert all(i in ignored or value == image.data[offset + i] for i, value in enumerate(code)), symbol
            functions.append(dict(symbol=symbol, va=va, bytes=len(code),
                                  relocation_words=len(entries), nonrelocated_bytes_matched=len(code) - len(ignored)))
        assert functions, stem
        headers = []
        if stem in ('c_neg64', 'c_prot', 'c_stack'):
            header = 'softpc.new/base/ccpu386/' + stem + '.h'
            assert '../../../../src/mvdm/' + header in block.replace('\\', '/')
            headers.append(dict(relative_path=header, sha256=sha((Path('src/mvdm') / header).read_bytes())))
        records.append(dict(relative_path=relative, sha256=sha(source.read_bytes()),
                            original_path=str(original), object_path=str(object_path),
                            object_sha256=sha(obj.data), compile_edge=edge,
                            dependency_block=block, functions=functions, headers=headers))
    inputs = [graph_path, map_path, image_path, deps_path,
              Path('tests/observation/verify_ccpu_sign_flag_image.py'), Path(__file__).resolve()]
    return dict(result='pass', records=records,
                inputs=[dict(path=str(p), sha256=sha(p.read_bytes())) for p in inputs],
                scope='Original source, VALID dependency, archive selection and map-bound nonrelocated function-byte correspondence; not guest execution or relocation-target validation')


if __name__ == '__main__':
    result = inspect()
    Path('artifacts/mvdm-audit/ccpu-foundation-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result='pass', sources=len(result['records']),
                          functions=sum(len(r['functions']) for r in result['records']))))
