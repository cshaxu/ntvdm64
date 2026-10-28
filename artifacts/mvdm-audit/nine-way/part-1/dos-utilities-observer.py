"""Part-1 DOS utility/device family original/package and exact graph evidence."""
from pathlib import Path
import csv,hashlib,json,re,subprocess,zipfile
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='1' and x['kind']=='mvdm' and x['path'].startswith(('dos/v86/cmd/','dos/v86/dev/')) and not x['path'].startswith(('dos/v86/cmd/graphics/','dos/v86/dev/keyboard/'))];root=Path('src/mvdm');refs=[Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),Path('O:/repos.external/opennt-src-2/nt/private/mvdm')];graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((root/x['path']).resolve()).lower():x['key'] for x in own};hits={x['key']:[] for x in own}
for g in graphs:
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
with zipfile.ZipFile('assets/winnt.zip') as z:
 rows=[]
 for x in own:
  n=x['path'];p=root/n; m=p.parent/'makefile'; basename=p.name.lower();zipnames=[t for t in z.namelist() if Path(t.replace('\\','/')).name.lower()==basename];i=Path('O:/winnt/system32')/p.name
  mentions=[dict(line=k,text=t.strip()[:200]) for k,t in enumerate(m.read_text(encoding='latin1').splitlines(),1) if basename in t.lower()] if m.is_file() else []
  rows.append(dict(key=x['key'],sha256=sha(p) if p.is_file() else None,refs=[sha(r/n) if (r/n).is_file() else None for r in refs],makefile=str(m) if m.is_file() else None,make_mentions=mentions,valid_deps=hits[x['key']],zip_entries=[dict(path=t,sha256=hashlib.sha256(z.read(t)).hexdigest()) for t in zipnames],installed_sha256=sha(i) if i.is_file() else None))
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs],limitations='Literal recipe mentions only; filename may be generated implicitly; no execution or image lineage.'),indent=2))
