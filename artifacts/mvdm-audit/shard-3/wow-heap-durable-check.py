"""Validate WOW heap bindings and header excerpts without temporary preprocess files."""
import hashlib
import json
import runpy
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def main():
    previous=json.loads((A/'wow-heap-image-observations.json').read_text(encoding='utf-8'))
    current=runpy.run_path(str(A/'wow-heap-image-observer.py'))['inspect']()
    assert previous==current
    excerpts=json.loads((A/'wow-heap-header-excerpts.json').read_text(encoding='utf-8'))
    assert excerpts['result']=='pass' and len(excerpts['snippets'])==2
    assert sha(Path('src/mvdm/wow32/wheap.h').read_bytes())==excerpts['source_sha256']
    assert ' /P ' in excerpts['command']
    for row in excerpts['snippets']:
        data=(A/row['path']).read_bytes()
        assert len(data)==row['size'] and sha(data)==row['sha256']
        assert b'malloc_w_small' in data and b'CreateSmallHeap' in data
    print('PASS WOW heap source, image, call edges and retained preprocessor excerpts; no lifetime or concurrency claim')


if __name__=='__main__':
    main()
