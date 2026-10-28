"""Part 1 remaining native headers/GDI/misc per-key same-basename candidate census."""
from pathlib import Path
import json,subprocess,collections,hashlib
items=[x for x in json.loads(Path('artifacts/mvdm-audit/nine-way/part-1/external-remaining-evidence.json').read_text(encoding='utf-8-sig'))['files'] if x['family'] in ('native-kernel-headers','gdi-other','native-other')];sources=subprocess.check_output(['rg','--files','src'],text=True,encoding='utf-8',errors='replace').splitlines();index=collections.defaultdict(list)
for s in sorted(sources, key=lambda s: s.lower()):index[Path(s.replace('\\','/')).name.lower()].append(Path(s))
graphs=[Path('build/.ninja/msvc-x86-release/worker/build.ninja'),Path('build/.ninja/msvc-x86-release/wow32/build.ninja'),Path('build/.ninja/msvc-x64-release/frontend/build.ninja'),Path('build/.ninja/msvc-x64-release/service/build.ninja')];line_index=[]
for g in graphs:
 for line in g.read_text(encoding='utf-8',errors='replace').replace('$:',':').splitlines():
  if line.startswith('build ') and ': cc' in line:line_index.append((str(g),line.replace('\\','/').lower(),line[:800]))
for x in items:
 name=Path(x['path']).name.lower();generic=name in ('sources','makefile','dirs','readme.txt') or len(index[name])>40;candidates=[] if generic else index[name];x['generic_name_unresolved']=generic;x['project_candidates']=[]
 for p in candidates:
  if not p.is_file():continue
  h=hashlib.sha256(p.read_bytes()).hexdigest();term=str(p.resolve()).replace('\\','/').lower();edges=[dict(graph=g,line=original) for g,line,original in line_index if term in line];x['project_candidates'].append(dict(path=str(p),sha256=h,exact_identity=h==x['sha256'],compile_edges=edges))
 x['suggested_status']='pending-specific-owner-and-semantics'
matches={str(Path(c['path']).resolve()).lower():c for x in items for c in x['project_candidates'] if c['exact_identity']}
for c in matches.values():c['valid_deps']=[]
for gp in graphs:
 g=gp.parent; out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   pth=Path(line.strip());key=str((pth if pth.is_absolute() else g/pth).resolve()).lower()
   if key in matches:matches[key]['valid_deps'].append(dict(graph=str(g),object=owner))
print(json.dumps(dict(files=items,limitations='Same basenames are candidates only; common basenames suppressed; includes and runtime symbol binding not inferred.'),indent=2))
