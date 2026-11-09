"""Assert a captured NTCON text pointer moved and survived a child update.

Inputs are the stable kvm_text_cell arrays captured by the isolated window
observer, not a guest-memory snapshot or a synthetic source frame.
"""
import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('before', type=Path)
parser.add_argument('after', type=Path)
parser.add_argument('update', type=Path)
parser.add_argument('--columns', type=int, required=True)
parser.add_argument('--rows', type=int, required=True)
parser.add_argument('--x', type=int, required=True)
parser.add_argument('--y', type=int, required=True)
args = parser.parse_args()

if args.columns <= 0 or args.rows <= 0 or not (0 <= args.x < args.columns and 0 <= args.y < args.rows):
    parser.error('invalid geometry or pointer position')
frames = [p.read_bytes() for p in (args.before, args.after, args.update)]
if any(len(data) != args.columns * args.rows * 5 for data in frames):
    raise SystemExit('captured cell extent does not match expected geometry')
offset = (args.y * args.columns + args.x) * 5
before, after, update = (data[offset:offset + 5] for data in frames)
if (before[0] != after[0] or before[1] != after[1] or before[4] != after[4]
        or before[2] ^ after[2] != 7 or before[3] ^ after[3] != 7):
    raise SystemExit('target cell does not exhibit the mouse pointer inversion')
if update != after:
    raise SystemExit('pointer cell did not survive the subsequent frame update')
if frames[1] == frames[2]:
    raise SystemExit('no frame update was observed')
print(f'PASS: NTCON pointer at ({args.x},{args.y}) survived a changed frame')
