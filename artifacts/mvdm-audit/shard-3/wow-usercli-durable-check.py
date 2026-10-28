"""Recheck current USER client thunk bindings and retained preprocessor text."""
import hashlib
import json
import runpy
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def main():
    prior=json.loads((A/'wow-usercli-image-observations.json').read_text(encoding='utf-8'))
    current=runpy.run_path(str(A/'wow-usercli-image-observer.py'))['inspect']()
    assert current==prior
    excerpts=json.loads((A/'wow-usercli-preprocessed-excerpts.json').read_text(encoding='utf-8'))
    assert excerpts['result']=='pass' and len(excerpts['snippets'])==2 and ' /P ' in excerpts['command']
    for record in excerpts['snippets']:
        data=(A/record['path']).read_bytes()
        assert len(data)==record['size'] and sha(data)==record['sha256']
        assert b'WU32ClientToScreen' in data and b'WU32GetKeyState' in data
    print('PASS released USER client bodies, 32 dispatch slots and retained compiler excerpts; guest reachability not asserted')


if __name__=='__main__':
    main()
