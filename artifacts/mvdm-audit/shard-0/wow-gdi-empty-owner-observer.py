"""Inspect the selected original WGMAN translation unit and its object."""
import csv
import hashlib
import json
import re
import runpy
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
P = 'wow32/wgman.c'


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    row = next(r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig')) if r['relative_path'] == P)
    paths = (Path('src/mvdm') / P, Path('O:/repos.external/OpenNT/base/mvdm') / P,
             Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / P)
    assert sha(paths[0]) == sha(paths[1]) == sha(paths[2]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
    content = paths[0].read_text(encoding='latin1')
    text = re.sub(r'/\*.*?\*/', '', content, flags=re.S)
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    assert lines == ['#include "precomp.h"', '#pragma hdrstop', 'MODNAME(wgman.c);']
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    edge = next(x for x in graph.splitlines() if x.startswith('build obj/wgman.obj:'))
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll | wow32.lib:'))
    assert paths[0].resolve().as_posix().lower() in edge.lower() and 'obj/wgman.obj' in link.split()
    objpath = B / 'obj/wgman.obj'
    raw = objpath.read_bytes()
    machine, count, _, _, _, optional, _ = struct.unpack_from('<HHIIIHH', raw, 0)
    assert machine == 0x14c and optional == 0
    sections = []
    for i in range(count):
        offset = 20 + i * 40
        name = raw[offset:offset+8].rstrip(b'\0').decode('ascii')
        flags = struct.unpack_from('<I', raw, offset + 36)[0]
        sections.append(dict(name=name, flags=flags))
        assert not (flags & 0x20), (name, hex(flags))
    obj = h['Coff'](objpath)
    common = h['Coff'](B / 'obj/wgdi.obj')
    directive = obj.sections[0][0]
    assert directive == b'   /DEFAULTLIB:"LIBCMT" /DEFAULTLIB:"OLDNAMES" '
    assert directive in common.sections[0][0]
    maptext = (B / 'wow32.dll.map').read_text(encoding='latin1')
    assert not re.search(r'(?im)^.*\bwgman\.obj\s*$', maptext)
    return dict(result='pass', relative_path=P, source_sha256=sha(paths[0]), original_paths=[str(x) for x in paths[1:]],
        source_lines=lines, build_edge=edge, object_sha256=sha(objpath), sections=sections,
        object_directive=directive.decode('ascii'), shared_directive_object='obj/wgdi.obj',
        map_sha256=sha(B / 'wow32.dll.map'), limitation='No code section in this selected object; no claim that GDI behavior is absent or restored')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
