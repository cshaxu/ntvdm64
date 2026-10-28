"""Extend common-header observation to real device/video build contexts."""
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '11c1c5abf'
ROOT = Path('build/t473-s10/base-inc-multi-r68')
NAMES = '''cga.h cmos.h cntlbop.h cpu.h egagraph.h egamode.h egaports.h egaread.h
fla.h ga_defs.h ga_mark.h gfi.h gfitest.h gvi.h host_nls.h idetect.h keyba.h
keyboard.h mouse.h mouse_io.h ppi.h quick_ev.h rs232.h rtc_bios.h timestmp.h
tmstrobe.h trace.h vgaports.h video.h virtual.h'''.split()


def inspect():
    assert len(NAMES) == len(set(NAMES)) == 30
    helper = runpy.run_path(str(A / 'base-inc-selection-observer.py'))
    assert helper['sha'](A / 'base-inc-multi-preprocess.ps1') == helper['sha'](
        'build/t473-s10/base-inc-multi-r68-run.ps1')
    return helper['inspect'](base=BASE, root=ROOT, expected_cohort=141, expected_units=13,
        expected_names={'softpc.new/base/inc/' + name for name in NAMES},
        tool=A / 'base-inc-multi-preprocess.ps1',
        scope='Thirteen selected device/video preprocessing contexts; not full ABI, lifecycle or behavior proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
