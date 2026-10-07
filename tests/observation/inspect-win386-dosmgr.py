"""Read-only W3/LE object extraction for the supplied DOSMGR; no guest writes."""
import argparse, hashlib, json, pathlib, struct, subprocess
p = argparse.ArgumentParser()
p.add_argument('--image', required=True)
p.add_argument('--output', required=True)
p.add_argument('--ndisasm', required=True)
a = p.parse_args()
out = pathlib.Path(a.output).resolve()
repo = pathlib.Path(__file__).resolve().parents[2]
if not out.is_relative_to(repo / 'build') or out.exists():
    raise ValueError('fresh build-owned output required')
b = pathlib.Path(a.image).read_bytes()
u16 = lambda o: struct.unpack_from('<H', b, o)[0]
u32 = lambda o: struct.unpack_from('<I', b, o)[0]
base = u32(0x3c)
if b[base:base+2] != b'W3':
    raise ValueError('not W3')
entries = []
for i in range(u16(base+4)):
    off = base+16+i*16
    entries.append({'name': b[off:off+8].decode('ascii').strip(),
                    'offset': u32(off+8), 'extra': u32(off+12)})
module = next(x['offset'] for x in entries if x['name'] == 'DOSMGR')
if b[module:module+2] != b'LE':
    raise ValueError('DOSMGR is not LE')
page_size, last_size = u32(module+0x28), u32(module+0x2c)
objects, count = module+u32(module+0x40), u32(module+0x44)
pages = module+u32(module+0x48)
data_base = u32(module+0x80)
rows = []
out.mkdir(parents=True)
for i in range(count):
    o = objects+24*i
    size, address, flags, page_start, page_count, reserved = struct.unpack_from('<6I', b, o)
    data = bytearray()
    mappings = []
    for j in range(page_count):
        record = b[pages+4*(page_start-1+j):pages+4*(page_start+j)]
        number = int.from_bytes(record[:3], 'big')
        if record[3] != 0:
            raise ValueError(f'unsupported page type {record.hex()}')
        start = data_base+(number-1)*page_size
        data.extend(b[start:start+page_size])
        mappings.append({'object_offset': j*page_size, 'file_offset': start})
    data = bytes(data[:size])
    path = out / f'object-{i+1}.bin'
    path.write_bytes(data)
    bits = 32 if flags & 0x2000 else 16
    code = subprocess.run([a.ndisasm, '-b', str(bits), str(path)], capture_output=True, check=True).stdout
    (out / f'object-{i+1}.asm').write_bytes(code)
    strings = []
    needle = b'Unsupported MS-DOS'
    found = data.find(needle)
    if found >= 0:
        strings.append({'text': needle.decode(), 'offset': found})
    rows.append({'object': i+1, 'size': size, 'base': address, 'flags': hex(flags),
                 'bits': bits, 'pages': mappings, 'strings': strings})
result = {'image': a.image, 'sha256': hashlib.sha256(b).hexdigest(),
          'module_offset': module, 'data_base': data_base, 'objects': rows}
# Internal LE fixups identify cross-object string references even when the
# unrelocated instruction operand is zero. Refuse unfamiliar record shapes.
fixup_pages = module+u32(module+0x68)
fixup_records = module+u32(module+0x6c)
references = []
for page in range(u32(module+0x14)):
    pos, end = fixup_records+u32(fixup_pages+4*page), fixup_records+u32(fixup_pages+4*(page+1))
    while pos < end:
        source_type, flags = b[pos:pos+2]; pos += 2
        if flags & 3:
            raise ValueError(f'unsupported LE fixup {source_type:x}/{flags:x}')
        if source_type & 0x20:
            source_count = b[pos]; pos += 1
            sources = None
        else:
            sources = [struct.unpack_from('<h', b, pos)[0]]; pos += 2
        target = u16(pos) if flags & 0x40 else b[pos]; pos += 2 if flags & 0x40 else 1
        target_offset = u32(pos) if flags & 0x10 else u16(pos); pos += 4 if flags & 0x10 else 2
        if flags & 4: pos += 4 if flags & 0x20 else 2
        if sources is None:
            sources = list(struct.unpack_from('<'+'h'*source_count, b, pos)); pos += 2*source_count
        if target == 2 and 0x33e0 <= target_offset <= 0x3450:
            references.extend({'source_page': page+1, 'source_offset': source_offset,
                               'target_object': target, 'target_offset': hex(target_offset)} for source_offset in sources)
result['error_string_references'] = references
(out/'layout.json').write_text(json.dumps(result, indent=2))
print(json.dumps(result, indent=2))
