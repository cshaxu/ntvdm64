"""Read-only DOS message input, original recipe and image identities."""
import csv
import hashlib
import json
import re
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ORIGINALS = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES = ('dos/v86/messages/germany.msg', 'dos/v86/messages/holland.msg',
         'dos/v86/messages/loadmsg.msg', 'dos/v86/messages/usa-ms.idx')


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
    names = ('dos/v86/messages/makefile', 'dos/v86/make.inc', 'dos/v86/cmd/loadfix/makefile',
             'dos/v86/cmd/loadfix/loadfix.asm', 'dos/v86/cmd/command/buildmsg.c',
             'dos/v86/tools/src/buildidx/buildidx.c')
    context = [M / n for n in names]
    assert all(sha(p) == sha(ORIGINALS[0] / p.relative_to(M)) for p in context)
    messages, base, recipe, body, buildmsg, buildidx = [p.read_text(encoding='latin1').lower() for p in context]
    assert re.search(r'(?m)^country\s*=\s*usa-ms', messages)
    assert re.search(r'\$\(country\)\.idx\s*:\s*\$\(country\)\.msg', messages)
    assert 'buildidx $(country).msg' in messages and 'buildmsg $(msg)\\$(country)' in base
    assert '$(msg)\\loadmsg.msg' in recipe and re.search(r'(?m)^\s*include\s+loadmsg\.msg', body)
    assert all(re.search(r'(?im)\boffset\s+' + n + r'\b', body) for n in ('errmsg', 'helpmsg', 'noparms', 'nocomspec'))
    assert 'strcat(countryidx, ".idx")' in buildmsg and re.search(r'dosopen\s*\(\s*\(char far \*\)\s*countryidx', buildmsg)
    assert 'fopen(fnameb,"w")' in buildidx
    archive = Path('assets/winnt.zip')
    with zipfile.ZipFile(archive) as z:
        matches = [n for n in z.namelist() if n.lower() == 'winnt/system32/loadfix.com']
        assert len(matches) == 1
        zipped = digest(z.read(matches[0]))
    image = M / 'dos/v86/cmd/loadfix/LOADFIX.COM'
    installed = Path('O:/winnt/system32/LOADFIX.COM')
    assert zipped == sha(image) == sha(installed)
    return dict(result='pass', sources=sources,
        context=[dict(path=str(p), sha256=sha(p)) for p in context],
        loadfix=dict(archive_path=str(archive), archive_sha256=sha(archive), member=matches[0],
                     image_sha256=zipped, source_image_path=str(image), installed_path=str(installed)),
        scope='Original message identities, historical source/recipe edges and captured image identity; no source-to-image derivation or guest behavior claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
