"""Observe remaining common headers in a dependency-selected source sweep."""
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '306e9dbba'
ROOT = Path('build/t473-s10/base-inc-wide-r69')
NAMES = '''cmosbios.h dma.h egavideo.h emm.h equip.h fdisk.h floppy.h
get_env.h host_com.h host_lpt.h lock.h printer.h rom.h sasp.h spcfile.h
suplib.h tape_io.h umb.h vga_dac.h'''.split()


def inspect():
    assert len(NAMES) == len(set(NAMES)) == 19
    helper = runpy.run_path(str(A / 'base-inc-selection-observer.py'))
    assert helper['sha'](A / 'base-inc-wide-preprocess.ps1') == helper['sha'](
        'build/t473-s10/base-inc-wide-r69-run.ps1')
    return helper['inspect'](base=BASE, root=ROOT, expected_cohort=111, expected_units=14,
        expected_names={'softpc.new/base/inc/' + name for name in NAMES},
        tool=A / 'base-inc-wide-preprocess.ps1',
        scope='Fourteen dependency-selected original source contexts; not full ABI, lifecycle or behavior proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
