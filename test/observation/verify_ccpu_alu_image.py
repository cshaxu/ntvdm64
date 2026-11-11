"""Match original ALU bodies and decoder function bindings in a tested image."""
import argparse
import json
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from verify_ccpu_sign_flag_image import Coff, Image, sha


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--image', type=Path, required=True)
    parser.add_argument('--worker-build', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    image = Image(args.image)
    names = 'add adc and cmp dec inc neg or sbb sub test xor'.split()
    objects = {name: Coff(args.worker_build / 'obj/ccpu' / (name + '.obj'))
               for name in names + ['cwde', 'lahf']}
    globals_ = {}
    for name in ['cwde', 'lahf'] + names:
        body, entries = objects[name].function('_' + name.upper())
        assert all(kind == 6 for _, _, kind in entries)
        ignored = [i for a, _, _ in entries for i in range(a, a + 4)]
        matches = image.find(body, ignored)
        assert len(matches) == 1, (name, matches)
        for address, symbol, _ in entries:
            addend = struct.unpack_from('<I', body, address)[0]
            target = (struct.unpack_from('<I', image.data, matches[0] + address)[0]
                      - addend) & 0xffffffff
            assert symbol not in globals_ or globals_[symbol] == target
            globals_[symbol] = target
    decoder = Coff(args.worker_build / 'obj/ccpu/c_main.obj')
    decoder_code, decoder_relocations = decoder.function('_ccpu')
    assert all(kind in (6, 20) for _, _, kind in decoder_relocations)
    ignored = [i for a, _, _ in decoder_relocations for i in range(a, a + 4)]
    decoder_matches = image.find(decoder_code, ignored)
    assert len(decoder_matches) == 1
    decoder_start = decoder_matches[0]
    records = []
    for name in names:
        symbol = '_' + name.upper()
        body, entries = objects[name].function(symbol)
        cooked = bytearray(body)
        for address, target, _ in entries:
            addend = struct.unpack_from('<I', body, address)[0]
            struct.pack_into('<I', cooked, address, (globals_[target] + addend) & 0xffffffff)
        matches = image.find(cooked)
        assert len(matches) == 1, name
        target = image.va(matches[0])
        references = []
        for address, callee, kind in decoder_relocations:
            if callee != symbol:
                continue
            reference = decoder_start + address
            assert struct.unpack_from('<I', decoder_code, address)[0] == 0
            if kind == 6:
                destination = struct.unpack_from('<I', image.data, reference)[0]
                role = 'function_pointer'
            else:
                assert decoder_code[address - 1] in (0xe8, 0xe9)
                delta = struct.unpack_from('<i', image.data, reference)[0]
                destination = image.va(reference) + 4 + delta
                role = 'relative_transfer'
            assert destination == target, (name, role, hex(destination), hex(target))
            references.append(dict(reference_va=image.va(reference),kind=role,target_va=destination))
        assert references, ('No matching decoder binding', name)
        records.append(dict(name=name,object_sha256=sha(objects[name].data),
                            bytes=len(body),absolute_relocations=len(entries),
                            function_va=target,decoder_references=references))
    result = dict(result='pass',image_sha256=sha(image.data),
                  checker_sha256=sha(Path(__file__).read_bytes()),
                  parser_sha256=sha(Path(__file__).with_name('verify_ccpu_sign_flag_image.py').read_bytes()),
                  decoder_object_sha256=sha(decoder.data),globals=globals_,functions=records,
                  decoder_function_bytes=len(decoder_code),
                  decoder_function_va=image.va(decoder_start),
                  decoder_relocations=len(decoder_relocations),
                  limitation='Code/binding correspondence; behavior and indirect call flow are separately audited')
    args.output.write_text(json.dumps(result,indent=2) + '\n',encoding='utf-8')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
