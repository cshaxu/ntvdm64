"""Extract the supplied Win95 IO.SYS logo, without executing or modifying it.

DS bit decoder adapted from the already retained MIT-licensed analysis tool:
JHRobotics/patcher9x pe/ds_decompress.c, commit
b6e30d4b5a396dcd453b6c8e6733fd5b5cbce59e (Jaroslav Hensl, 2022).
See DS-LICENSE.txt. Output is user-supplied Microsoft artwork, not an add-on
release asset. This tool is offline only; it is never a runtime dependency.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def decode(data, limit):
    bits = int.from_bytes(data, 'little')
    position = 0
    result = bytearray()

    def take(count):
        nonlocal position
        if position + count > len(data) * 8:
            raise ValueError('Truncated DS stream')
        value = (bits >> position) & ((1 << count) - 1)
        position += count
        return value

    while len(result) < limit:
        tag = take(2)
        if tag in (1, 2):
            result.append(take(7) | (128 if tag == 1 else 0))
            continue
        if tag == 0:
            distance = take(6)
        elif take(1) == 0:
            distance = take(8) + 64
        else:
            distance = take(12) + 320
            if distance == 4415:
                continue
        if not 0 < distance <= len(result):
            raise ValueError('Invalid DS distance')
        zeros = 0
        while take(1) == 0:
            zeros += 1
            if zeros > 8:
                raise ValueError('Invalid DS length')
        count = (1 << zeros) + take(zeros) + 1
        if len(result) + count > limit:
            raise ValueError('DS output overflow')
        for _ in range(count):
            result.append(result[-distance])
    return bytes(result)


def extract(data):
    # Identity-bound retail input; never scan arbitrary data as executable code.
    if hashlib.sha256(data).hexdigest() != '325695a4b4022a6fb373263d41e51f8ca0947f449dba61741eafaabc8ad34673':
        raise ValueError('Unsupported IO.SYS identity')
    position = 0x1b830  # exact supplied localized retail IO.SYS
    result = bytearray()
    while True:
        size, count = struct.unpack_from('<HH', data, position)
        if not size & 0x8000 or data[position + 4:position + 8] != b'DS\0\0':
            raise ValueError('Invalid logo block')
        end = position + 4 + (size & 0x7fff)
        if not position + 8 <= end <= len(data) or not 0 < count <= 8192:
            raise ValueError('Invalid logo block extent')
        result.extend(decode(data[position + 8:end], count))
        position = end
        if count != 8192:
            break
    if len(result) != 129078 or result[:2] != b'BM' or struct.unpack_from('<iiHHI', result, 18) != (320, 400, 1, 8, 0):
        raise ValueError('Expected 320x400 uncompressed 8-bit bitmap')
    return bytes(result)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--io-sys', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise ValueError('Preserve existing output')
    source = args.io_sys.read_bytes()
    bitmap = extract(source)
    args.output.write_bytes(bitmap)
    print(json.dumps({'inputSha256': hashlib.sha256(source).hexdigest(),
                      'outputSha256': hashlib.sha256(bitmap).hexdigest(),
                      'bytes': len(bitmap), 'width': 320, 'height': 400}))


if __name__ == '__main__':
    main()
