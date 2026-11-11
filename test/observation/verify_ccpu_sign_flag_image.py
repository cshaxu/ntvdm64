"""Read-only correspondence of selected CCPU objects and a tested x86 image.

This checks emitted code and direct decoder calls; it does not execute guest
instructions or replace the separate real-guest behavioral witness.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


class Coff:
    def __init__(self, path):
        self.data = path.read_bytes()
        machine, count, _, symbols, total, optional, _ = struct.unpack_from(
            '<HHIIIHH', self.data)
        assert machine == 0x14c
        strings = self.data[symbols + total * 18:]
        self.symbols = {}
        index = 0
        while index < total:
            offset = symbols + index * 18
            name = self.data[offset:offset + 8]
            if name[:4] == b'\0' * 4:
                name = strings[struct.unpack_from('<I', name, 4)[0]:].split(b'\0')[0]
            else:
                name = name.rstrip(b'\0')
            value, section, _, _, auxiliary = struct.unpack_from(
                '<IhHBB', self.data, offset + 8)
            self.symbols[index] = (name.decode(), value, section)
            index += 1 + auxiliary
        self.sections = []
        for index in range(count):
            offset = 20 + optional + index * 40
            size, raw, relocations = struct.unpack_from('<III', self.data, offset + 16)
            relocation_count = struct.unpack_from('<H', self.data, offset + 32)[0]
            body = self.data[raw:raw + size]
            entries = []
            for j in range(relocation_count):
                address, symbol, kind = struct.unpack_from(
                    '<IIH', self.data, relocations + j * 10)
                entries.append((address, self.symbols[symbol][0], kind))
            self.sections.append((body, entries))

    def function(self, name):
        definitions = [s for s in self.symbols.values() if s[0] == name and s[2] > 0]
        assert len(definitions) == 1
        _, start, section = definitions[0]
        assert start == 0, 'Expected a separately emitted function section'
        return self.sections[section - 1]


class Image:
    def __init__(self, path):
        self.data = path.read_bytes()
        pe = struct.unpack_from('<I', self.data, 60)[0]
        assert self.data[pe:pe + 4] == b'PE\0\0'
        assert struct.unpack_from('<H', self.data, pe + 4)[0] == 0x14c
        assert struct.unpack_from('<H', self.data, pe + 24)[0] == 0x10b
        count = struct.unpack_from('<H', self.data, pe + 6)[0]
        optional = struct.unpack_from('<H', self.data, pe + 20)[0]
        self.base = struct.unpack_from('<I', self.data, pe + 52)[0]
        self.code = []
        for index in range(count):
            offset = pe + 24 + optional + 40 * index
            rva, size, raw = struct.unpack_from('<III', self.data, offset + 12)
            flags = struct.unpack_from('<I', self.data, offset + 36)[0]
            if flags & 0x20000000:
                self.code.append((raw, raw + size, rva))

    def va(self, offset):
        for start, end, rva in self.code:
            if start <= offset < end:
                return self.base + rva + offset - start
        raise ValueError('Not executable image data')

    def find(self, body, ignored=()):
        ignored = set(ignored)
        anchor = next(i for i in range(len(body)) if i not in ignored)
        result = []
        for start, end, _ in self.code:
            search = start + anchor
            while True:
                found = self.data.find(body[anchor:anchor + 1], search, end)
                if found < 0:
                    break
                offset = found - anchor
                search = found + 1
                if offset + len(body) > end:
                    break
                if all(i in ignored or self.data[offset + i] == value
                       for i, value in enumerate(body)):
                    result.append(offset)
        return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--image', type=Path, required=True)
    parser.add_argument('--worker-build', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    image = Image(args.image)
    names = 'cbw cwde cwd cdq lahf sahf stc clc cmc std cld'.split()
    objects = {name: Coff(args.worker_build / 'obj/ccpu' / (name + '.obj'))
               for name in names}
    globals_ = {}
    # Two independently unique emitted bodies identify their shared absolute
    # relocation targets. No cached map VA is assumed valid for another PE.
    for name in ('cwde', 'lahf'):
        body, relocs = objects[name].function('_' + name.upper())
        ignored = [i for address, _, kind in relocs for i in range(address, address + 4)]
        matches = image.find(body, ignored)
        assert len(matches) == 1, (name, matches)
        for address, symbol, kind in relocs:
            assert kind == 6
            addend = struct.unpack_from('<I', body, address)[0]
            value = (struct.unpack_from('<I', image.data, matches[0] + address)[0]
                     - addend) & 0xffffffff
            assert symbol not in globals_ or globals_[symbol] == value
            globals_[symbol] = value
    decoder = Coff(args.worker_build / 'obj/ccpu/c_main.obj')
    records = []
    for name in names:
        symbol = '_' + name.upper()
        body, relocs = objects[name].function(symbol)
        cooked = bytearray(body)
        for address, target, kind in relocs:
            assert kind == 6 and target in globals_
            addend = struct.unpack_from('<I', body, address)[0]
            struct.pack_into('<I', cooked, address, (globals_[target] + addend) & 0xffffffff)
        bodies = image.find(cooked)
        assert bodies, name
        target_vas = {image.va(offset) for offset in bodies}
        calls = []
        for code, entries in decoder.sections:
            for address, target, kind in entries:
                if target != symbol or kind != 20:
                    continue
                assert code[address - 1] == 0xe8
                start, end = max(0, address - 24), min(len(code), address + 28)
                assert all(k in (6, 20) for a, _, k in entries
                           if a < end and start < a + 4)
                ignored = [i - start for a, _, k in entries
                           for i in range(a, a + 4) if start <= i < end]
                for offset in image.find(code[start:end], ignored):
                    call = offset + address - start
                    displacement = struct.unpack_from('<i', image.data, call)[0]
                    destination = image.va(call) + 4 + displacement
                    if destination in target_vas:
                        calls.append(dict(call_va=image.va(call - 1), target_va=destination))
        assert calls, ('No corresponding direct decoder call', name)
        records.append(dict(name=name,object_sha256=sha(objects[name].data),
                            bytes=len(body),absolute_relocations=len(relocs),
                            code_match_vas=sorted(target_vas),decoder_calls=calls))
    result = dict(result='pass',image_sha256=sha(image.data),
                  checker_sha256=sha(Path(__file__).read_bytes()),
                  decoder_object_sha256=sha(decoder.data),globals=globals_,functions=records,
                  limitation='Code/call correspondence only; guest behavior is a separate test')
    args.output.write_text(json.dumps(result,indent=2) + '\n',encoding='utf-8')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
