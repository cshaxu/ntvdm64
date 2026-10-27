"""Bind the native NEG64 witness to selected worker code and signed callers.

This is code correspondence, not guest IMUL/IDIV behavior or fault acceptance.
"""
import argparse
import json
import struct
from pathlib import Path

from verify_ccpu_sign_flag_image import Coff, Image, sha


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--worker-build', type=Path, required=True)
    parser.add_argument('--image', type=Path, required=True)
    parser.add_argument('--fixture', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    obj = Coff(args.worker_build / 'obj/ccpu/c_neg64.obj')
    body, relocations = obj.function('_neg64')
    assert body and not relocations, 'NEG64 must have no relocated dependencies'
    records = []
    for role, path in [('worker', args.image), ('fixture', args.fixture)]:
        image = Image(path)
        matches = image.find(body)
        assert len(matches) == 1, (role, matches)
        target = image.va(matches[0])
        callers = []
        if role == 'worker':
            for name in ('mul64', 'div64'):
                caller_obj = Coff(args.worker_build / 'obj/ccpu' / ('c_' + name + '.obj'))
                code, entries = caller_obj.function('_' + name)
                assert all(kind in (6, 20) for _, _, kind in entries)
                ignored = [i for address, _, _ in entries for i in range(address, address + 4)]
                starts = image.find(code, ignored)
                assert len(starts) == 1, (name, starts)
                calls = []
                for address, symbol, kind in entries:
                    if symbol != '_neg64':
                        continue
                    assert kind == 20 and code[address - 1] == 0xe8
                    call = starts[0] + address
                    destination = image.va(call) + 4 + struct.unpack_from('<i', image.data, call)[0]
                    assert destination == target, (name, hex(destination), hex(target))
                    calls.append(image.va(call - 1))
                assert calls, ('Missing original signed helper call', name)
                callers.append(dict(name=name, object_sha256=sha(caller_obj.data),
                                    function_va=image.va(starts[0]), neg64_call_vas=calls))
        records.append(dict(role=role, path=str(path.resolve()), image_sha256=sha(image.data),
                            neg64_va=target, callers=callers))
    result = dict(result='pass', object_sha256=sha(obj.data), function_bytes=len(body),
                  function_sha256=sha(body), relocated_dependencies=0, images=records,
                  checker_sha256=sha(Path(__file__).read_bytes()),
                  parser_sha256=sha(Path(__file__).with_name('verify_ccpu_sign_flag_image.py').read_bytes()),
                  limitation='Selected code and signed-helper calls only; guest decoder and arithmetic faults remain separate')
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
