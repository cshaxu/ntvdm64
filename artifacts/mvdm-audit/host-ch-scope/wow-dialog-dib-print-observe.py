"""Snapshot selected WOW dialog, DIB and printer source/declaration evidence."""
import hashlib
import json
import pathlib
import subprocess

roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
paths = ('wow32/wcommdlg.h', 'wow32/wdib.c', 'wow32/wdib.h',
         'wow32/wgprnset.c', 'wow32/wgprnset.h')
sha = lambda data: hashlib.sha256(data).hexdigest().upper()
identities = {p: [sha((pathlib.Path(r) / p).read_bytes()) if (pathlib.Path(r) / p).is_file() else None
                   for r in roots] for p in paths}
build = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
graph = (build / 'build.ninja').read_bytes()
deps = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
targets = []
current = None
for line in deps.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        current = line
    elif current:
        item = line.strip().replace('\\', '/').lower()
        if any(item.endswith('/src/mvdm/' + p) for p in paths):
            targets.append({'object': current, 'dependency': item})
map_data = (build / 'wow32.dll.map').read_bytes()
needles = ('_WCD32GetOpenFileName@4', '_WCD32ChooseColor@4', '_WCD32PrintDlg@4',
           '_W32HandleDibDrv@4', '_W32CheckAndFreeDibInfo@4',
           '_WG32DeviceMode@4', '_WG32ExtDeviceMode@4', '_WG32DeviceCapabilities@4')
map_lines = [line.strip() for line in map_data.decode('utf-8', errors='replace').splitlines()
             if any(name in line for name in needles)]
source_paths = ('src/mvdm/wow32/wcmdgtbl.h', 'src/mvdm/wow32/wcommdlg.c',
                'src/mvdm/wow32/wgdi.c', 'src/mvdm/wow32/wgtbl2.h')
print(json.dumps({'identities': identities, 'graph_sha256': sha(graph),
                  'deps_sha256': sha(deps), 'exact_dependencies': targets,
                  'map_sha256': sha(map_data), 'map_lines': map_lines,
                  'calling_source_sha256': {p: sha(pathlib.Path(p).read_bytes()) for p in source_paths},
                  'limits': 'Selected static calls and object ownership only, not live guest/device execution, complete ABI or lifecycle.'}, indent=2))
