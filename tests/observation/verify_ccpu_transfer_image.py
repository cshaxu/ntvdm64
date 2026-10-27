"""Read-only selected NOT/XCHG/LEA code and decoder-reference correspondence."""
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
    decoder = Coff(args.worker_build / 'obj/ccpu/c_main.obj')
    code, relocations = decoder.function('_ccpu')
    assert all(kind in (6, 20) for _, _, kind in relocations)
    ignored = [i for a, _, _ in relocations for i in range(a, a + 4)]
    matches = image.find(code, ignored)
    assert len(matches) == 1
    decoder_start = matches[0]
    functions = []
    for name in ['not', 'xchg', 'lea']:
        obj = Coff(args.worker_build / 'obj/ccpu' / (name + '.obj'))
        symbol = '_' + name.upper()
        body, body_relocations = obj.function(symbol)
        assert not body_relocations, 'These pure pointer bodies have no external references'
        body_matches = image.find(body)
        assert body_matches
        addresses = {image.va(offset) for offset in body_matches}
        refs = []
        for address, target, kind in relocations:
            if target != symbol:
                continue
            reference = decoder_start + address
            assert struct.unpack_from('<I', code, address)[0] == 0
            if kind == 6:
                destination = struct.unpack_from('<I', image.data, reference)[0]
                role = 'function_pointer'
            else:
                assert code[address - 1] in (0xe8, 0xe9)
                displacement = struct.unpack_from('<i', image.data, reference)[0]
                destination = image.va(reference) + 4 + displacement
                role = 'relative_transfer'
            assert destination in addresses
            refs.append(dict(reference_va=image.va(reference),kind=role,target_va=destination))
        assert refs
        functions.append(dict(name=name,object_sha256=sha(obj.data),bytes=len(body),
                              body_match_vas=sorted(addresses),decoder_references=refs))
    result = dict(result='pass',image_sha256=sha(image.data),functions=functions,
                  checker_sha256=sha(Path(__file__).read_bytes()),
                  parser_sha256=sha(Path(__file__).with_name('verify_ccpu_sign_flag_image.py').read_bytes()),
                  decoder_object_sha256=sha(decoder.data),decoder_function_bytes=len(code),
                  decoder_relocations=len(relocations),
                  limitation='Code/reference evidence only; actual behavior is tested separately')
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
