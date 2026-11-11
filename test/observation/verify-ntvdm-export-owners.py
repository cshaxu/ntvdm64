#!/usr/bin/env python3
"""Compare original template aliases and DATA exports to linked map owners."""
import argparse
import re
from pathlib import Path

OWNERS = {
    'MGetVdmPointer': ('ExpSim32GetVDMPointer', True),
    'call_ica_hw_interrupt': ('ica_hw_interrupt', True),
    'CurrentMonitorTeb': ('CurrentMonitorTeb', False),
    'ExpVdmTib': ('VdmTib', False),
    'SelectorLimit': ('ExpSelectorLimit', False),
    'ExpLdt': ('Ldt', False),
    'FlatAddress': ('FlatAddress', False),
    'pDeviceChain': ('pDeviceChain', False),
    'fSeparateWow': ('fSeparateWow', False),
}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('dump', type=Path, help='dumpbin /exports output')
    parser.add_argument('map', type=Path, help='map for the exact linked image')
    args = parser.parse_args()
    dump = args.dump.read_text(encoding='utf-8-sig', errors='strict')
    image_map = args.map.read_text(encoding='utf-8-sig', errors='strict')
    base_match = re.search(r'(?im)^\s*Preferred load address is ([0-9a-f]+)', image_map)
    if not base_match:
        raise ValueError('missing map image base')
    base = int(base_match.group(1), 16)
    exports = {}
    for match in re.finditer(r'(?im)^\s*\d+\s+[0-9a-f]+\s+([0-9a-f]+)\s+(\S+)', dump):
        exports[match.group(2)] = int(match.group(1), 16)
    for name, (symbol, is_function) in OWNERS.items():
        # The selected i386 objects use C's leading underscore.
        pattern = r'(?im)^\s*[0-9a-f]+:[0-9a-f]+\s+_' + re.escape(symbol) + r'\s+([0-9a-f]+)\s+(?:(f)\s+)?'
        match = re.search(pattern, image_map)
        if not match:
            raise ValueError(f'missing map owner {symbol}')
        expected = int(match.group(1), 16) - base
        if exports.get(name) != expected or bool(match.group(2)) != is_function:
            raise ValueError(f'{name}: PE RVA {exports.get(name)}, map RVA {expected}, function={bool(match.group(2))}')
    print(f'PASS {len(OWNERS)} original template alias/DATA export owners')


if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError) as error:
        raise SystemExit(f'FAIL NTVDM export owners: {error}')
