"""Read-only selected WOW old-table declaration role census."""
import hashlib
import json
import pathlib
import re
import subprocess

headers = ('wgman.h', 'wgtbl.h', 'wkbtbl.h', 'wktbl.h', 'wmmtbl.h', 'wutbl.h')
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
sha = lambda b: hashlib.sha256(b).hexdigest().upper()
identities = {h: [sha((pathlib.Path(r) / 'wow32' / h).read_bytes()) for r in roots] for h in headers}
names = ('aw32GDI', 'aw32Keyboard', 'aw32Kernel', 'aw32MMED', 'aw32User',
         'iGDIMax', 'iKeyboardMax', 'iKernelMax', 'iMMEDMax', 'iUserMax')
pattern = r'\b(' + '|'.join(names) + r')\b'
search = subprocess.run(['rg', '-n', pattern, 'src/mvdm', '-g', '*.c', '-g', '*.h'],
                        capture_output=True, check=True)
build = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
deps = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
found = {h: [] for h in headers}
current = None
for line in deps.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        current = line
    elif current:
        path = line.strip().replace('\\', '/').lower()
        for h in headers:
            if path.endswith('/src/mvdm/wow32/' + h):
                found[h].append(current)
source = pathlib.Path('src/mvdm/wow32/wowtbl.c')
mapdata = (build / 'wow32.dll.map').read_bytes()
maptext = mapdata.decode('utf-8', errors='replace')
old_map_hits = [line.strip() for line in maptext.splitlines()
                if any(re.search(r'\b_' + re.escape(name) + r'\b', line) for name in names)]
new_map_hits = [line.strip() for line in maptext.splitlines() if '_aw32WOW ' in line]
gman = (pathlib.Path('src/mvdm/wow32') / 'wgman.h').read_text()
gman_without_comments = re.sub(r'/\*.*?\*/|//[^\r\n]*', '', gman, flags=re.S).strip()
print(json.dumps({'identities': identities, 'old_name_search': sorted(search.stdout.decode('utf-8', errors='replace').splitlines()),
                  'deps_sha256': sha(deps), 'dependency_groups': found,
                  'combined_table_source_sha256': sha(source.read_bytes()),
                  'combined_table_declaration_present': 'W32 aw32WOW[]' in source.read_text(),
                  'map_sha256': sha(mapdata), 'old_map_hits': old_map_hits, 'combined_map_hits': new_map_hits,
                  'wgman_noncomment_text': gman_without_comments,
                  'limits': 'Selected observed profile and retained C/H lexical search only; no claim about external debug/profile consumers or GDI/USER/keyboard/kernel/multimedia functionality.'}, indent=2))
