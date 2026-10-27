"""Bind control-probe expectations to the actual selected decoder and objects.

Source/map/code correspondence only. Guest behavior and negative-oracle runs
are separate; notably the real-mode STR case does not enter the STR helper.
"""
import argparse
import json
import re
import struct
from pathlib import Path

from verify_ccpu_sign_flag_image import Coff, Image, sha


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--worker-build', type=Path, required=True)
    parser.add_argument('--image', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    image = Image(args.image)
    cached = Image(args.worker_build / 'ntvdm.exe')
    # Package sealing may change the UUID resource. Bind the map to identical
    # complete executable sections at identical VAs, not a guessed package hash.
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[start:end] == cached.data[start:end] for start, end, _ in image.code), 'Selected executable code differs from map-producing worker'
    map_path = args.worker_build / 'ntvdm.exe.map'
    functions = {}
    for line in map_path.read_text().splitlines():
        match = re.fullmatch(r'\s+\w+:\w+\s+(\S+)\s+([0-9a-fA-F]+)\s+f\s+original-ccpu386:(\S+)\s*', line)
        if match:
            functions[match[1]] = (int(match[2], 16), match[3])
    decoder = Coff(args.worker_build / 'obj/ccpu/c_main.obj')
    code, entries = decoder.function('_ccpu')
    assert all(kind in (6, 20) for _, _, kind in entries)
    ignored = [i for address, _, _ in entries for i in range(address, address + 4)]
    starts = image.find(code, ignored)
    assert len(starts) == 1 and image.va(starts[0]) == functions['_ccpu'][0]
    selected = {'cli': ['CLI'], 'clts': ['CLTS'], 'wait': ['WAIT'], 'rsrvd': ['RSRVD'],
                'div': ['DIV8', 'DIV16', 'DIV32'], 'lidt': ['LIDT16', 'LIDT32'],
                'sidt': ['SIDT16', 'SIDT32'], 'jcxz': ['JCXZ', 'JECXZ'], 'str': ['STR']}
    records = []
    for stem, names in selected.items():
        obj = Coff(args.worker_build / 'obj/ccpu' / (stem + '.obj'))
        source = Path('src/mvdm/softpc.new/base/ccpu386') / (stem + '.c')
        original = Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/ccpu386') / (stem + '.c')
        assert sha(source.read_bytes()) == sha(original.read_bytes())
        for name in names:
            symbol = '_' + name
            target, owner = functions[symbol]
            assert owner == stem + '.obj'
            body, relocations = obj.function(symbol)
            assert all(kind in (6, 20) for _, _, kind in relocations)
            masked = [i for address, _, _ in relocations for i in range(address, address + 4)]
            matches = image.find(body, masked)
            assert target in {image.va(offset) for offset in matches}, name
            references = []
            for address, callee, kind in entries:
                if callee != symbol:
                    continue
                position = starts[0] + address
                assert struct.unpack_from('<I', code, address)[0] == 0
                if kind == 6:
                    actual = struct.unpack_from('<I', image.data, position)[0]
                else:
                    assert code[address - 1] in (0xe8, 0xe9)
                    actual = image.va(position) + 4 + struct.unpack_from('<i', image.data, position)[0]
                assert actual == target, (name, hex(actual), hex(target))
                references.append(dict(reference_va=image.va(position), relocation_kind=kind))
            assert references, name
            records.append(dict(name=name, function_va=target, body_bytes=len(body),
                                masked_relocation_words=len(relocations), object_sha256=sha(obj.data),
                                source_sha256=sha(source.read_bytes()), decoder_references=references,
                                guest_scope='Real-mode rejection only; helper not entered' if name == 'STR' else 'See separate control guest assertions'))
    nop = Coff(args.worker_build / 'obj/ccpu/nop.obj')
    nop.function('_NOP')
    assert '_NOP' not in functions and all(symbol != '_NOP' for _, symbol, _ in entries)
    result = dict(result='pass', image_sha256=sha(image.data), map_sha256=sha(map_path.read_bytes()),
                  cached_worker_sha256=sha(cached.data), executable_sections_equal=True,
                  checker_sha256=sha(Path(__file__).read_bytes()),
                  parser_sha256=sha(Path(__file__).with_name('verify_ccpu_sign_flag_image.py').read_bytes()),
                  decoder_object_sha256=sha(decoder.data), decoder_bytes=len(code),
                  functions=records, unused_nop_helper_object_sha256=sha(nop.data),
                  limitation='Code bindings only; no privilege, operand, exception or lifecycle proof without guest evidence')
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(result='pass', functions=len(records), nop_helper_referenced=False)))


if __name__ == '__main__':
    main()
