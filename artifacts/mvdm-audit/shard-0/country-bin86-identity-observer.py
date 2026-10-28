"""Compare retained and deployed COUNTRY.SYS entry identities without loading guest."""
import csv
import hashlib
import json
import struct
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
REL = 'bin86/country.sys'


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def parse(data):
    assert data[:8] == b'\xffCOUNTRY' and data[8:16] == bytes(8)
    assert struct.unpack_from('<H', data, 16)[0] == 1 and data[18] == 1
    offset = struct.unpack_from('<I', data, 19)[0]
    assert offset + 2 <= len(data)
    count = struct.unpack_from('<H', data, offset)[0]
    offset += 2
    rows = []
    for _ in range(count):
        assert offset + 14 <= len(data)
        size, country, codepage, reserved1, reserved2, ptr = struct.unpack_from('<HHHHHI', data, offset)
        assert size == 12 and ptr < len(data)
        rows.append(dict(country=country, codepage=codepage, reserved=[reserved1, reserved2], data_offset=ptr))
        offset += size + 2
    return rows


def inspect():
    frozen = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path'] == REL)
    path = Path('src/mvdm') / REL
    original = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / REL
    old, installed = path.read_bytes(), Path('O:/winnt/system32/COUNTRY.SYS').read_bytes()
    assert sha(old) == sha(original.read_bytes()) == frozen['current_sha256'] == frozen['opennt45_sha256']
    assert not frozen['opennt_sha256']
    with zipfile.ZipFile('assets/winnt.zip') as archive:
        matches = [n for n in archive.namelist() if n.lower() == 'winnt/system32/country.sys']
        assert len(matches) == 1 and archive.read(matches[0]) == installed
    assert Path('src/mvdm/dos/v86/dev/country/COUNTRY.SYS').read_bytes() == installed
    left, right = parse(old), parse(installed)
    lp = {(x['country'], x['codepage']) for x in left}
    rp = {(x['country'], x['codepage']) for x in right}
    return dict(result='pass', retained=dict(path=str(path), sha256=sha(old), size=len(old), entries=left),
                installed=dict(path='O:/winnt/system32/COUNTRY.SYS', sha256=sha(installed), size=len(installed), entries=right),
                unique_retained_pairs=len(lp), unique_installed_pairs=len(rp), common_pairs=len(lp & rp),
                retained_only=sorted(lp - rp), installed_only=sorted(rp - lp),
                scope='COUNTRY.SYS header and country/codepage index only; no data payload semantics, runtime or alternate paths')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
