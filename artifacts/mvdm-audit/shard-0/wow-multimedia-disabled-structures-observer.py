"""Observe original disabled multimedia converter bodies and selected objects."""
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
NAMES = ('wmmstru1', 'wmmstruc')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll |'))
    mapped = (B / 'wow32.dll.map').read_text().lower()
    records = []
    for name in NAMES:
        relative = 'wow32/' + name + '.c'
        sources = [Path('src/mvdm') / relative, Path('O:/repos.external/OpenNT/base/mvdm') / relative,
                   Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / relative]
        hashes = [sha(x) for x in sources]
        f = frozen[relative]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        lines = sources[0].read_text(encoding='latin1').splitlines()
        disabled = next(i for i, line in enumerate(lines) if re.fullmatch(r'\s*#\s*if\s+0\s*', line))
        depth = 0
        ending = None
        for i in range(disabled, len(lines)):
            if depth == 1:
                assert not re.match(r'\s*#\s*(else|elif)\b', lines[i])
            directive = re.match(r'\s*#\s*(if|ifdef|ifndef|endif)\b', lines[i])
            if not directive:
                continue
            depth += -1 if directive.group(1) == 'endif' else 1
            if depth == 0:
                ending = i
                break
        assert ending is not None and all(not x.strip() for x in lines[ending + 1:])
        edge = next(x for x in graph.splitlines() if x.startswith('build obj/' + name + '.obj:'))
        assert 'obj/' + name + '.obj' in link.split()
        obj = B / ('obj/' + name + '.obj')
        sections = subprocess.check_output(['objdump', '-h', str(obj)], text=True)
        assert not re.search(r'\b(?:CODE|ALLOC|DATA)\b', sections)
        assert not re.search(r'(?m)^\s*[0-9a-f]+:[0-9a-f]+\s+\S+\s+[0-9a-f]+\s+(?:f\s+)?' + name + r'\.obj\s*$', mapped)
        records.append(dict(relative_path=relative, hashes=hashes, disabled_lines=[disabled + 1, ending + 1],
                            compile_edge=edge, object_sha256=sha(obj), object_sections=sections.strip()))
    return dict(result='pass', files=records, graph_sha256=sha(B / 'build.ninja'), map_sha256=sha(B / 'wow32.dll.map'),
                scope='Original disabled body and current no-code object only; multimedia guest and host behavior not excluded')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
