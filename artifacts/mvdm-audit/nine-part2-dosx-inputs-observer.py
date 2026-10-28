"""Replay DOSX x86 build evidence for the nine-way part-2 source cohort."""
import json
import runpy


def inspect():
    module = runpy.run_path('artifacts/mvdm-audit/shard-0/dosx-x86-inputs-observer.py')
    fn = module['inspect']
    fn.__globals__['ASM'] = ('486/dxboot.asm', '486/dxutil.asm', 'dxbug.asm', 'dxdisk.asm',
                            'dxend.asm', 'dxendpm.asm', 'dxrom.asm')
    fn.__globals__['INCLUDES'] = ()
    return fn()


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
