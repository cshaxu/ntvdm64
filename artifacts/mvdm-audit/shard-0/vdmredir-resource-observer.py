"""Compare resource payloads and changed PE bytes of two VDMREDIR images."""
import hashlib
import json
import struct
from pathlib import Path

PATHS = (Path('build/.ninja/msvc-x86-release/worker/VDMREDIR.dll'),
         Path('assets/release/VDMREDIR.DLL'))


def inspect(path):
    data = path.read_bytes()
    u16 = lambda offset: struct.unpack_from('<H', data, offset)[0]
    u32 = lambda offset: struct.unpack_from('<I', data, offset)[0]
    pe = u32(0x3c)
    assert data[:2] == b'MZ' and data[pe:pe + 4] == b'PE\0\0'
    optional = pe + 24
    assert u16(optional) == 0x10b
    section_table = optional + u16(pe + 20)
    sections = []
    for index in range(u16(pe + 6)):
        offset = section_table + 40 * index
        name = data[offset:offset + 8].rstrip(b'\0').decode('ascii')
        virtual_size, rva, size, raw = struct.unpack_from('<IIII', data, offset + 8)
        assert raw + size <= len(data)
        sections.append((name, rva, virtual_size, raw, size))

    def file_offset(rva):
        for _, start, _, raw, size in sections:
            if start <= rva < start + size:
                return raw + rva - start
        raise ValueError('RVA outside raw sections')

    resource_rva, resource_size = struct.unpack_from('<II', data, optional + 112)
    base = file_offset(resource_rva)
    resources = []

    def visit(relative, parents):
        assert len(parents) <= 3 and relative + 16 <= resource_size
        directory = base + relative
        count = u16(directory + 12) + u16(directory + 14)
        assert relative + 16 + 8 * count <= resource_size
        for index in range(count):
            name, target = struct.unpack_from('<II', data, directory + 16 + index * 8)
            if name & 0x80000000:
                pos = base + (name & 0x7fffffff)
                label = data[pos + 2:pos + 2 + u16(pos) * 2].decode('utf-16le')
            else:
                label = name
            if target & 0x80000000:
                visit(target & 0x7fffffff, parents + [label])
            else:
                assert target + 16 <= resource_size
                rva, size, codepage, _ = struct.unpack_from('<IIII', data, base + target)
                offset = file_offset(rva)
                payload = data[offset:offset + size]
                assert len(payload) == size
                resources.append(dict(path=parents + [label], size=size, codepage=codepage,
                                      sha256=hashlib.sha256(payload).hexdigest().upper()))

    visit(0, [])
    return data, sections, resources


def main():
    left, sections, left_resources = inspect(PATHS[0])
    right, right_sections, right_resources = inspect(PATHS[1])
    assert len(left) == len(right)
    changes = {}
    for (name, _, _, offset, size), (other_name, _, _, other_offset, other_size) in zip(sections, right_sections):
        assert name == other_name and size == other_size
        changes[name] = sum(a != b for a, b in zip(left[offset:offset + size], right[other_offset:other_offset + size]))
    print(json.dumps(dict(files=[str(p) for p in PATHS], changed_section_bytes=changes,
                          cached_resources=left_resources, release_resources=right_resources,
                          limits='Resource hashes and raw section differences only; no runtime equivalence claim'), indent=2))


if __name__ == '__main__':
    main()
