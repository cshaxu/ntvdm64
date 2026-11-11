"""Check the retained IOS-consumed resident ABI against its exact old binary."""
import argparse
import hashlib
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--control', type=Path, required=True)
p.add_argument('--candidate', type=Path, required=True)
a = p.parse_args()
control, candidate = a.control.read_bytes(), a.candidate.read_bytes()
assert hashlib.sha256(control).hexdigest() == 'eda2454fd4a356d42fa2d6abb432e0e3f4b944b436e4b823cc045c981e7c1156'
# Original resident span ends at 086C. The sole relocated resident operand
# is the conditional jump to the discardable INIT handler (0F84 rel16).
assert control[0x39:0x3b] == candidate[0x39:0x3b] == b'\x0f\x84'
assert control[:0x3b] == candidate[:0x3b]
assert control[0x3d:0x86c] == candidate[0x3d:0x86c]
target = 0x3d + int.from_bytes(candidate[0x3b:0x3d], 'little', signed=True)
assert target >= 0x86c and candidate[target:target+6] == b'\x26\xc7\x47\x03\x0c\x81'
print('PASS retained resident code/data offsets; only discardable INIT branch relocated')
