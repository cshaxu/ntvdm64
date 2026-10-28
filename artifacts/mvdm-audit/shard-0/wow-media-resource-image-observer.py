"""Bind selected original multimedia and resource thunks to released WOW32."""
import json
import runpy
from pathlib import Path


def inspect():
    module = runpy.run_path('artifacts/mvdm-audit/shard-0/wow-shell-hook-image-observer.py')
    fn = module['inspect']
    fn.__globals__['STEMS'] = ('wmmedia', 'wres32')
    return fn()


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
