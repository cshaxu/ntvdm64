#!/usr/bin/env python3
"""Check paired real Windows prior-owner mouse observations.

Each directory is an independently run standard-mode Windows probe. The
control does not cycle the driver; the candidate does. The input sink must
acknowledge the same movement in both before callback results are meaningful.
"""

import argparse
from pathlib import Path
import re
import struct
import sys


def read_observation(directory: Path) -> int:
    data = (directory / "M31OWNR.BIN").read_bytes()
    if len(data) != 20:
        raise ValueError(f"{directory}: expected exactly two checkpoints")
    first = struct.unpack_from("<4sHHH", data, 0)
    second = struct.unpack_from("<4sHHH", data, 10)
    if first != (b"M31O", 1, 0, 0):
        raise ValueError(f"{directory}: invalid initial checkpoint {first!r}")
    if second[:3] != (b"M31O", 2, 0):
        raise ValueError(f"{directory}: invalid final checkpoint {second!r}")
    sink = (directory / "mouse-corner.txt").read_text(encoding="utf-8")
    if not re.search(r"\bsink-accepted=1\b.*\bdx=1000\s+dy=1000\s+buttons=0\b", sink):
        raise ValueError(f"{directory}: expected movement was not acknowledged")
    return second[3]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("control", type=Path)
    parser.add_argument("candidate", type=Path)
    args = parser.parse_args()
    try:
        if args.control.resolve(strict=True) == args.candidate.resolve(strict=True):
            raise ValueError("control and candidate must be independent runs")
        control = read_observation(args.control)
        candidate = read_observation(args.candidate)
    except (OSError, ValueError, struct.error) as exc:
        print(f"INVALID: {exc}", file=sys.stderr)
        return 2
    if control == 0:
        print("INVALID: prior-owner positive control did not receive input", file=sys.stderr)
        return 2
    if candidate == 0:
        print(f"FAIL: control={control}, after driver cycle={candidate}", file=sys.stderr)
        return 1
    print(f"PASS: control={control}, after driver cycle={candidate}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
