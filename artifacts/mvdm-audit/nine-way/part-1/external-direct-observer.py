"""Per-key DirectDraw source census for part 1; no product execution."""
from pathlib import Path
import csv,hashlib,json,re,subprocess,zipfile
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='1' and x['kind']=='external' and x['path'].startswith('nt/private/ntos/w32/ntgdi/direct/')];root=Path('O:/repos.external/OpenNT-4.5');sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((root/x['path']).resolve()).lower():x['key'] for x in own};hits={x['key']:[] for x in own};graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')]
for g in graphs:
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
with zipfile.ZipFile('assets/winnt.zip') as z: zn={Path(n.replace('\\','/')).name.lower() for n in z.namelist()}
rows=[]
for x in own:
 p=root/x['path'];data=p.read_bytes() if p.is_file() else None; model=None
 if data and p.suffix.lower()=='.ppd':
  m=re.search(rb'^\*ModelName\s*:\s*([^\r\n]+)',data,re.M|re.I);model=m.group(1).decode('latin1','replace').strip() if m else None
 rows.append(dict(key=x['key'],path=x['path'],edition='OpenNT-4.5',sha256=sha(p) if data is not None else None,size=len(data) if data is not None else None,extension=p.suffix.lower(),ppd_model=model,role=('direct-draw-header-or-include' if p.suffix.lower() in ('.h','.inc') else 'direct-draw-code' if p.suffix.lower() in ('.c','.asm','.s') else 'direct-draw-resource-build-or-data'),suggested_status='pending-specific-provider-semantics',exact_valid_deps=hits[x['key']],zip_basename_present=p.name.lower() in zn))
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs],limitations='Exact paths/ZIP basename only; no provider selection, installation, binary tracing or runtime.'),indent=2))
