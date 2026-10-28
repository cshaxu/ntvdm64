"""Read-only DOS kernel message inputs and COUNTRY.SYS data identity."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES = ('dos/v86/doskrnl/bios/msbio.skl', 'dos/v86/doskrnl/dos/msdos.skl',
         'dos/v86/doskrnl/dos/dos.tag', 'dos/v86/dev/country/nls863.inc')


def digest(blob):
    return hashlib.sha256(blob).hexdigest().upper()


def sha(path):
    return digest(Path(path).read_bytes())


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in NAMES:
        f = frozen[name]; value = sha(M / name)
        assert value == sha(ORIGINALS[0] / name) == sha(ORIGINALS[1] / name)
        assert value == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
        assert f['dos_catalog_rows'] == '1' and f['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=name, sha256=value, original_path=str(ORIGINALS[0] / name),
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=f['wow_source_ids']))
    names = ('dos/v86/doskrnl/bios/makefile', 'dos/v86/doskrnl/dos/makefile',
             'dos/v86/dev/country/makefile', 'dos/v86/dev/country/country.asm')
    context = [M / n for n in names]
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in context)
    bios, dos, country_recipe, country = [p.read_text(encoding='latin1').lower() for p in context]
    assert re.search(r'msbio\.cl1\s*:\s*msbio\.skl', bios)
    assert re.search(r'msdos\.cl1\s*:\s*msdos\.skl', dos) and re.search(r'msdos\.cl2\s*:\s*msdos\.skl', dos)
    assert re.search(r'country\.obj\s*:\s*country\.asm', country_recipe)
    assert re.search(r'(?m)^\s*include\s+nls863\.inc\s*$', country)
    text = (M / NAMES[3]).read_text(encoding='latin1')
    section = text[text.index('FrnCdn_863_collate label word'):]
    section = section[:section.index('World Trade Case Mappings')]
    values = []
    for line in section.splitlines():
        match = re.match(r'(?i)^db\s+([\d,\s]+)$', line.split(';', 1)[0].strip())
        if match:
            values.extend(int(n) for n in re.findall(r'\d+', match[1]))
    assert len(values) == 256 and all(0 <= n <= 255 for n in values)
    collation = bytes(values)
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        members = [n for n in z.namelist() if n.lower() == 'winnt/system32/country.sys']
        assert len(members) == 1
        binary = z.read(members[0])
    source_image = M / 'dos/v86/dev/country/COUNTRY.SYS'
    installed = Path('O:/winnt/system32/COUNTRY.SYS')
    assert digest(binary) == sha(source_image) == sha(installed)
    offsets = [m.start() for m in re.finditer(re.escape(collation), binary)]
    assert offsets == [23517]
    return dict(result='pass', sources=sources,
        context=[dict(path=str(p), sha256=sha(p)) for p in context],
        country=dict(archive_path=str(archive), archive_sha256=sha(archive), member=members[0],
                     binary_sha256=digest(binary), source_image_path=str(source_image), installed_path=str(installed),
                     collation_size=len(collation), collation_sha256=digest(collation), match_offsets=offsets),
        scope='Historical message inputs and one exact country data sequence, not source-to-image provenance or guest locale behavior')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
