"""Part-4 MVDM per-key original identities, direct edges and valid host dependencies."""
from pathlib import Path
import csv,hashlib,json,re,subprocess
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='4' and x['kind']=='mvdm'];root=Path('src/mvdm');refs=[Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),Path('O:/repos.external/opennt-src-2/nt/private/mvdm')];graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((root/x['path']).resolve()).lower():x['key'] for x in own};hits={x['key']:[] for x in own};edges={x['key']:[] for x in own}
for g in graphs:
 graph=(g/'build.ninja').read_text(encoding='utf-8',errors='replace').replace('$:',':')
 for line in graph.splitlines():
  if not line.startswith('build '):continue
  ls=line.replace('\\','/').lower()
  for key,k in want.items():
   if key.replace('\\','/') in ls:edges[k].append(dict(graph=str(g),edge=line[:800]))
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
rows=[]
for x in own:
 n=x['path'];p=root/n;data=p.read_bytes();lines=data.decode('latin1').splitlines() if p.suffix.lower() in ('.h','.c','.inc') else [];nonblank=[q.strip() for q in lines if q.strip()];guard=bool(len(nonblank)<=3 and len(nonblank)>=3 and nonblank[0].startswith('#ifndef') and nonblank[1].startswith('#define') and nonblank[-1].startswith('#endif'))
 rows.append(dict(key=x['key'],path=n,sha256=sha(p),reference_hashes=[sha(r/n) if (r/n).is_file() else None for r in refs],graph_edges=edges[x['key']],valid_dependencies=hits[x['key']],nonblank_lines=len(nonblank),guard_only=guard,suggested_status='pending-specific-owner-and-semantics'))
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs],limitations='Original identity, source edges and VALID deps are bounded; no runtime/function coverage.'),indent=2))
