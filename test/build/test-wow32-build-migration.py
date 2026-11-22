"""Compare actual WOW compile profiles and linked bytes during graph migration."""
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import struct
import subprocess


def commands(tree, graph=None):
    args = ['ninja', '-C', str(tree)]
    if graph:
        args += ['-f', str(graph.resolve())]
    return subprocess.check_output(args + ['-t', 'commands', 'wow32.dll'], text=True)


def profiles(text):
    result = Counter()
    for line in text.splitlines():
        if 'cl.exe ' not in line:
            continue
        source = re.search(r'("[^"]+\.(?:c|cxx)"|\S+\.(?:c|cxx))\s*$', line, re.I)
        assert source, line
        source = source[1].strip('"').replace('\\', '/').lower()
        # Ignore output locations and equivalent compiler infrastructure flags.
        flags = re.findall(r'(?:^|\s)([/-](?:FI|I)\s*(?:"[^"]+"|\S+)|[/-](?:D|U)\S+|/(?:Gz|MT|TC|O2))', line)
        flags = [re.sub(r'\s+', '', f.replace('\\', '/').replace('"', '')) for f in flags]
        ordered = tuple(f for f in flags if re.match(r'[/-](?:FI|I|D|U)', f))
        abi = tuple(sorted(set(flags) - set(ordered)))
        result[(source, ordered, abi)] += 1
    return result


def compare_bytes(old, new):
    a, b = old.read_bytes(), new.read_bytes()
    assert len(a) == len(b), 'Image size changed'
    pe = struct.unpack_from('<I', a, 60)[0]
    assert a[pe:pe+4] == b'PE\0\0'
    opt = pe + 24
    section_table = opt + struct.unpack_from('<H', a, pe+20)[0]
    def offset(rva):
        for j in range(struct.unpack_from('<H', a, pe+6)[0]):
            size, va, raw_size, raw = struct.unpack_from('<IIII', a, section_table+j*40+8)
            if va <= rva < va+max(size, raw_size):
                return raw+rva-va
        raise AssertionError('Unmapped directory')
    export_offset = offset(struct.unpack_from('<I', a, opt+96)[0])
    stamps = set(range(pe+8, pe+12)) | set(range(export_offset+4, export_offset+8))
    debug_rva, debug_size = struct.unpack_from('<II', a, opt+96+6*8)
    assert debug_size % 28 == 0
    if debug_rva:
        debug = offset(debug_rva)
        for j in range(0, debug_size, 28):
            stamps.update(range(debug+j+4, debug+j+8))
    paths = set()
    for match in re.finditer(rb'(?:[ -~]\x00){8,}', a):
        value = match[0].decode('utf-16le')
        if '/src/' in value and re.search(r'\.(?:c|cxx)$', value):
            for j, char in enumerate(value):
                if char == '/':
                    paths.add(match.start()+2*j)
    differences = [i for i, (x, y) in enumerate(zip(a, b)) if x != y]
    for i in differences:
        assert i in stamps or (i in paths and a[i] == 47 and b[i] == 92), f'Unexplained image delta: {i}'
    return dict(bytes=len(a), differing_bytes=len(differences),
                timestamp_bytes=sum(i in stamps for i in differences),
                diagnostic_path_separator_bytes=sum(i in paths for i in differences))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tree', type=Path, required=True)
    parser.add_argument('--baseline-graph', type=Path, required=True)
    parser.add_argument('--baseline-image', type=Path, required=True)
    args = parser.parse_args()
    before = profiles(commands(args.tree, args.baseline_graph))
    after = profiles(commands(args.tree))
    assert before == after, f'Compile profiles changed: removed={before-after}, added={after-before}'
    result = compare_bytes(args.baseline_image, args.tree/'wow32.dll')
    result['compile_units'] = sum(after.values())
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
