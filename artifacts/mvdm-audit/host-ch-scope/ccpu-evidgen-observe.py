"""Record two selected original VideoVector schemas and actual object deps."""
import hashlib
import json
import pathlib
import re
import subprocess

root = pathlib.Path('.')
build = root / 'build/.ninja/msvc-x86-release/worker'
headers = {'ccpu': root / 'src/mvdm/softpc.new/base/ccpu386/evidgen.h',
           'cvidc': root / 'src/mvdm/softpc.new/base/cvidc/evidgen.h'}
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
fields = {}
for name, path in headers.items():
    body = re.search(r'struct\s+VideoVector\s*\{(.*?)\};', path.read_text(), re.S).group(1)
    fields[name] = re.findall(r'\(\*\s*(\w+)\s*\)', body)
dep = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
consumers = {'ccpu': [], 'cvidc': []}
target = None
for line in dep.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        target = line
    elif target and '/evidgen.h' in line.replace('\\', '/').lower():
        normalized = line.replace('\\', '/').lower()
        for name, directory in (('ccpu', 'ccpu386'), ('cvidc', 'cvidc')):
            if '/base/' + directory + '/evidgen.h' in normalized:
                consumers[name].append(target)
ccpu_objects = []
for target in consumers['ccpu']:
    name = target.split(':', 1)[0]
    path = build / name
    result = subprocess.run(['objdump', '-r', str(path)], capture_output=True, check=True)
    relocs = [line.strip() for line in result.stdout.decode('utf-8', errors='replace').splitlines()
              if re.search(r'\b_(?:C_)?Video\b', line)]
    ccpu_objects.append({'object': name, 'sha256': sha(path), 'direct_video_relocations': relocs})
names = ('GetVideolatches', 'SetVideolatches', 'setWritePointers', 'setReadPointers', 'setMarkPointers')
print(json.dumps({'headers': {k: {'path': str(v).replace('\\', '/'), 'sha256': sha(v),
                                  'fields': fields[k],
                                  'x86_pointer_offsets': {n: fields[k].index(n) * 4 for n in names}}
                              for k, v in headers.items()},
                  'graph_sha256': sha(build / 'build.ninja'),
                  'deps_sha256': hashlib.sha256(dep).hexdigest().upper(),
                  'consumers': consumers, 'ccpu_object_relocations': ccpu_objects,
                  'limits': 'Offsets assume x86 four-byte pointers; dependency inclusion and absent named relocations do not prove macro expansion, indirect access, runtime behavior or alternate profiles.'}, indent=2))
