"""Snapshot four selected WOW declaration headers and representative thunks."""
import hashlib
import json
import pathlib
import re
import subprocess

roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
headers = ('wgdi.h', 'wgdi31.h', 'wkernel.h', 'wuser.h')
sha = lambda b: hashlib.sha256(b).hexdigest().upper()
identities = {h: [sha((pathlib.Path(r) / 'wow32' / h).read_bytes()) for r in roots] for h in headers}
build = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
deps = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
consumers = {h: [] for h in headers}
target = None
for line in deps.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        target = line
    elif target:
        name = line.strip().replace('\\', '/').lower()
        for header in headers:
            if name.endswith('/src/mvdm/wow32/' + header):
                consumers[header].append(target)
maps = (build / 'wow32.dll.map').read_bytes()
names = ('WG32Arc', 'WG32AbortDoc', 'WK32GetDriveType', 'WU32BeginPaint')
lines = [line.strip() for line in maps.decode('utf-8', errors='replace').splitlines()
         if any(re.search(r'\b_' + re.escape(n) + r'@4\b', line) for n in names)]
tables = ('wgtbl2.h', 'wktbl2.h', 'wutbl2.h')
table_info = {}
for table in tables:
    data = (pathlib.Path('src/mvdm/wow32') / table).read_bytes()
    text = data.decode('utf-8', errors='replace')
    table_info[table] = {'sha256': sha(data), 'registered': [n for n in names
                        if re.search(r'W32FUN\(\s*' + re.escape(n) + r'\b', text)]}
print(json.dumps({'identities': identities, 'deps_sha256': sha(deps), 'consumers': consumers,
                  'map_sha256': sha(maps), 'map_lines': lines, 'tables': table_info,
                  'limits': 'Selected original declarations and representative table entries only; no runtime, full API coverage, failures or lifecycle.'}, indent=2))
