"""Remaining part-1 external rows: edition-aware identity, triage and exact deps."""
from pathlib import Path
import csv,hashlib,json,subprocess,zipfile
skip=('base/crts/crtw32/','nt/private/ntos/w32/ntgdi/printers/','nt/private/ntos/w32/ntgdi/displays/','nt/private/ntos/w32/ntgdi/fonts/','nt/private/ntos/w32/ntgdi/fondrv/','nt/private/ntos/w32/ntgdi/direct/','nt/private/ntos/w32/ntgdi/gre/','nt/private/ntos/w32/ntgdi/client/','nt/private/ntos/w32/ntgdi/math/','nt/private/ntos/w32/ntgdi/halftone/','nt/private/ntos/w32/ntgdi/icm/')
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='1' and x['kind']=='external' and not x['path'].startswith(skip)];keys={x['key'] for x in own};frozen={x['id']:x for x in csv.DictReader(Path('artifacts/mvdm-audit/frozen-external-audit-rows.csv').open(encoding='utf-8-sig')) if x['id'] in keys};pri={x['id']:x for x in csv.DictReader(Path('artifacts/mvdm-audit/external-priority-boundaries.csv').open(encoding='utf-8-sig')) if x['id'] in keys}; roots={'OpenNT':Path('O:/repos.external/OpenNT'),'OpenNT-4.5':Path('O:/repos.external/OpenNT-4.5')};sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest(); paths={x['key']:roots[frozen[x['key']]['edition']]/x['path'] for x in own};want={str(p.resolve()).lower():k for k,p in paths.items()};hits={x['key']:[] for x in own};graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')]
for g in graphs:
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
with zipfile.ZipFile('assets/winnt.zip') as z:zn={Path(n.replace('\\','/')).name.lower() for n in z.namelist()}
rows=[]
for x in own:
 p=paths[x['key']];s=x['path'].lower();family=('nls' if '/winnls/' in s else 'rtl' if '/rtl/' in s else 'gdi-other' if '/ntgdi/' in s else 'native-user-client' if '/win32/client/' in s or '/win32/server/' in s else 'native-kernel-headers' if '/inc/' in s else 'native-other');priority=pri.get(x['key'],{})
 rows.append(dict(key=x['key'],path=x['path'],edition=frozen[x['key']]['edition'],sha256=sha(p) if p.is_file() else None,size=p.stat().st_size if p.is_file() else None,family=family,priority_relevance=priority.get('audit_relevance'),priority_reason=priority.get('audit_reason'),exact_valid_deps=hits[x['key']],zip_basename_present=p.name.lower() in zn,suggested_status='pending-specific-provider-semantics'))
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs],limitations='Initial source/provider census; each row remains pending detailed applicability; exact deps and basename only.'),indent=2))
