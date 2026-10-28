"""Record selected debugger and WOW fax declaration/call-chain evidence."""
import hashlib
import json
import pathlib
import subprocess

sha = lambda b: hashlib.sha256(b).hexdigest().upper()
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
files = ('inc/dbginfo.h', 'inc/wowfax.h')
identities = {name: [sha((pathlib.Path(root) / name).read_bytes()) for root in roots]
              for name in files}
worker = pathlib.Path('build/.ninja/msvc-x86-release/worker')
wow = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
deps = {}
for name, build in (('worker', worker), ('wow32', wow)):
    data = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'],
                          capture_output=True, check=True).stdout
    target = None
    matched = []
    for line in data.decode('utf-8', errors='replace').splitlines():
        if line and not line[0].isspace():
            target = line
        elif target and any(line.strip().replace('\\', '/').lower().endswith('/src/mvdm/' + f)
                            for f in files):
            matched.append({'object': target, 'header': line.strip().replace('\\', '/')})
    deps[name] = {'sha256': sha(data), 'matches': matched}
source_paths = ['src/mvdm/dbg/dbg.c', 'src/mvdm/softpc.new/host/src/nt_bop.c',
                'src/mvdm/wow32/wkman.c', 'src/mvdm/wow32/wow32fax.c']
sources = {p: {'sha256': sha(pathlib.Path(p).read_bytes())} for p in source_paths}
maps = {}
for name, path, needles in (
    ('worker', worker / 'ntvdm.exe.map', ('_DBGDispatch ', '_GetNormalContext ')),
    ('wow32', wow / 'wow32.dll.map', ('_WOW32FaxHandler@8', '_WK32RegisterShellWindowHandle@4')),
):
    data = path.read_bytes()
    maps[name] = {'sha256': sha(data), 'lines': [line.strip() for line in data.decode('utf-8', errors='replace').splitlines()
                                                 if any(n in line for n in needles)]}
print(json.dumps({'identities': identities, 'deps': deps, 'sources': sources, 'maps': maps,
                  'limits': 'Source and selected-build ownership only; no event, fax or end-to-end runtime is established.'}, indent=2))
