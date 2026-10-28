"""Recheck three graph-hit source roles and their current COFF sections."""
import csv
import hashlib
import json
import re
import struct
from pathlib import Path

A = Path('artifacts/mvdm-audit'); B = Path('build/.ninja/msvc-x86-release/worker')
ITEMS = (('softpc.new/base/comms/prt_scrn.c', 'obj/comms/prt_scrn.obj'),
         ('softpc.new/host/src/cpucstbs.c', 'obj/host/cpucstbs.obj'),
         ('softpc.new/host/src/x86_emm.c', 'obj/host/x86_emm.obj'))
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph = (B / 'build.ninja').read_text().replace('$:', ':'); mapping = (B / 'ntvdm.exe.map').read_text(errors='replace')
    result = []
    for path, objname in ITEMS:
        src = Path('src/mvdm') / path; row = frozen[path]
        origins = [Path('O:/repos.external/OpenNT/base/mvdm') / path, Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path]
        assert sha(src) == sha(origins[0]) == sha(origins[1]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        edge = next(line for line in graph.splitlines() if line.startswith('build ' + objname + ':'))
        assert src.resolve().as_posix().lower() in edge.lower()
        libraries = [line.split(':', 1)[0] for line in graph.splitlines() if line.startswith('build ') and line.startswith('build original-') and objname in line.split()]
        assert libraries
        raw = (B / objname).read_bytes(); machine, count, _, _, _, optional, _ = struct.unpack_from('<HHIIIHH', raw, 0)
        assert machine == 0x14c and optional == 0
        sections = []
        for i in range(count):
            offset = 20 + i * 40; name = raw[offset:offset+8].rstrip(b'\0').decode('ascii')
            flags = struct.unpack_from('<I', raw, offset + 36)[0]; size = struct.unpack_from('<I', raw, offset + 16)[0]
            assert not flags & 0x20
            sections.append(dict(name=name, flags=flags, size=size))
        assert not re.search(r'(?im)^.*\b' + re.escape(Path(objname).name) + r'\s*$', mapping)
        result.append(dict(path=path, source_sha256=sha(src), object=objname, object_sha256=sha(B / objname),
                           edge=edge, libraries=libraries, sections=sections, map_symbol_owner_found=False))
    return dict(result='pass', sources=result, graph_sha256=sha(B / 'build.ninja'), map_sha256=sha(B / 'ntvdm.exe.map'),
                limits='Selected object sections and map owner only; no proof of alternate profiles or affected guest features.')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
