"""Validate the three retained compiler snippets without the build tree."""
import hashlib
import json
import re
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def main():
    evidence=json.loads((A/'cvidc-header-durable-evidence.json').read_text(encoding='utf-8'))
    assert evidence['result']=='pass' and len(evidence['records'])==3
    names=set()
    for record in evidence['records']:
        relative=record['relative_path']
        assert relative not in names
        names.add(relative)
        source=Path('src/mvdm')/relative
        first=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        second=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source.read_bytes())==sha(first.read_bytes())==sha(second.read_bytes())==record['source_sha256']
        data=Path(record['snippet']).read_bytes()
        assert len(data)==record['snippet_bytes'] and sha(data)==record['snippet_sha256']
        assert re.match(rb'(?i)^#line\s+1\s+"[^\r\n]*'+re.escape(source.name.encode())+rb'"',data)
        assert sum(bool(line.strip()) for line in data.splitlines()[1:])==record['nonblank_lines']
        assert ' /P ' in record['command'] and re.fullmatch(r'[A-F0-9]{64}',record['full_preprocessed_sha256'])
    assert names=={'softpc.new/base/cvidc/cpuint_c.h','softpc.new/base/cvidc/fm_c.h','softpc.new/base/cvidc/fpu_c.h'}
    print('PASS original identities and three retained compiler snippets; runtime semantics not inferred')


if __name__=='__main__':
    main()
