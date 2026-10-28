"""Nine-way part 1 external CRT ctype/locale/string identity and exact graph use."""
from pathlib import Path
import csv,hashlib,json,subprocess
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='1' and x['kind']=='external' and x['path'].startswith('base/crts/crtw32/')];root=Path('O:/repos.external/OpenNT'); sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((root/x['path']).resolve()).lower():x['key'] for x in own};hits={x['key']:[] for x in own};graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')]
for g in graphs:
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
rows=[]
for x in own:
 p=root/x['path']; rows.append(dict(key=x['key'],path=x['path'],edition='OpenNT',exists=p.is_file(),sha256=sha(p) if p.is_file() else None,exact_valid_deps=hits[x['key']]))
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs]),indent=2))
