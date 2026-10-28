"""Verify original C-VID headers containing only empty include guards."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = 'softpc.new/base/cvidc/'
NAMES = ('chain2_c.h', 'enter_c.h', 'nanob_c.h', 'nanod_c.h', 'pti_c.h',
         'read_c.h', 'rotate_c.h', 'shift_c.h', 'string_c.h', 'xlat_c.h')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    result = []
    for name in NAMES:
        path = ROOT + name
        paths = (Path('src/mvdm') / path, Path('O:/repos.external/OpenNT/base/mvdm') / path,
                 Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path)
        row = frozen[path]
        assert sha(paths[0]) == sha(paths[1]) == sha(paths[2]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        text = re.sub(r'/\*.*?\*/', '', paths[0].read_text(encoding='latin1'), flags=re.S)
        lines = [line.strip() for line in text.splitlines() if line.strip()]
        assert len(lines) == 3
        first = re.fullmatch(r'#ifndef\s+(\w+)', lines[0])
        assert first and re.fullmatch(r'#define\s+' + re.escape(first.group(1)), lines[1])
        assert lines[2] == '#endif'
        result.append(dict(relative_path=path, sha256=sha(paths[0]), guard=first.group(1),
                           wow_source_ids=row['wow_source_ids']))
    guards = [item['guard'] for item in result]
    command = ['rg', '-n', '-F']
    for guard in guards:
        command.extend(('-e', guard))
    command.extend(('-g', '*.[ch]', 'src/mvdm'))
    uses = subprocess.check_output(command, text=True).splitlines()
    for line in uses:
        directive = line.rsplit(':', 1)[-1].strip()
        assert re.fullmatch(r'#(?:ifndef|define)\s+\w+|#endif\s*/\*.*\*/', directive), line
    assert uses
    path = 'inc/APISTRUC.INC'
    files = (Path('src/mvdm') / path, Path('O:/repos.external/OpenNT/base/mvdm') / path,
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path)
    row = frozen[path]
    assert all(p.read_bytes() == b'' for p in files)
    assert sha(files[0]) == sha(files[1]) == sha(files[2]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
    recipe = Path('src/mvdm/inc/makefile')
    assert sha(recipe) == sha(Path('O:/repos.external/OpenNT/base/mvdm/inc/makefile'))
    assert 'apistruc.inc: apistruc.h' in recipe.read_text(encoding='latin1').lower()
    result.append(dict(relative_path=path, sha256=sha(files[0]), guard=None, wow_source_ids=row['wow_source_ids']))
    return dict(result='pass', sources=result, guard_reference_lines=len(uses), recipe_sha256=sha(recipe),
        scope='Only empty physical header roles; CPU, video and network functionality remain in scope')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
