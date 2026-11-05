"""Check selected MSVC line markers against the pinned modesw original."""
import argparse
import hashlib
from pathlib import Path
import re


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("original", type=Path)
    parser.add_argument("preprocessed", type=Path)
    args = parser.parse_args()
    expected_hash = "aa7157e2a39fead41e42e87f87e332e9464bf69a668671de976c69637ce06a67"
    data = args.original.read_bytes()
    if hashlib.sha256(data).hexdigest() != expected_hash:
        raise SystemExit("original identity mismatch")
    original = data.decode("utf-8").splitlines()
    checks = {
        28: ("DpmiSwitchToRealMode(", "DpmiSwitchToRealMode("),
        53: ("PWORD16 Data;", "PWORD16 Data;"),
        61: ("*(Data) = DosxStackSegment;", "*(Data) = DosxStackSegment;"),
        64: ("setCS(DosxRmCodeSegment);", "setCS(DosxRmCodeSegment);"),
        66: ("setMSW(getMSW() & ~MSW_PE);", "setMSW(getMSW() & ~0x1);"),
        74: ("}", "}"),
        77: ("DpmiSwitchToProtectedMode(", "DpmiSwitchToProtectedMode("),
        101: ("PWORD16 Data;", "PWORD16 Data;"),
        109: ("*(Data) = 0xb7;", "*(Data) = 0xb7;"),
        112: ("setMSW(getMSW() | MSW_PE);", "setMSW(getMSW() | 0x1);"),
        118: ("setCPL(3);", "setCPL(3);"),
        120: ("}", "}"),
    }
    for line, (before, _) in checks.items():
        if original[line - 1].strip() != before:
            raise SystemExit(f"original anchor mismatch at {line}")
    seen = set()
    logical = 0
    source = ""
    for text in args.preprocessed.read_text(encoding="utf-8", errors="replace").splitlines():
        directive = re.match(r'^#line\s+(\d+)\s+"([^"]+)"', text)
        if directive:
            logical = int(directive[1])
            source = directive[2].replace("\\", "/").lower()
            continue
        if (source.endswith("/src/mvdm-host/dpmi32/modesw.c") and logical in checks
                and text.strip() == checks[logical][1]):
            seen.add(logical)
        logical += 1
    if seen != set(checks):
        raise SystemExit(f"missing compiler anchors: {sorted(set(checks) - seen)}")
    print(f"PASS {len(seen)} actual MSVC selected-output line anchors")


if __name__ == "__main__":
    main()
