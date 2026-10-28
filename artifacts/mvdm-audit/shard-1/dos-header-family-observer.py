"""DOS guest header family: identities, direct textual references, worker deps."""
from pathlib import Path
import hashlib,json,re,subprocess
names=['h/float.h','h/graph.h','h/math.h','h/search.h','h/stdarg.h','h/time.h','inc/chkcpu.inc','inc/macro.def','inc/mft.inc','inc/parse.asm','inc/smdossym.inc'];base=Path('src/mvdm/dos/v86');refs=[Path('O:/repos.external/OpenNT/base/mvdm/dos/v86'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86')];g=Path('build/.ninja/msvc-x86-release/worker');sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((base/n).resolve()).lower():n for n in names};hits={n:[] for n in names}
out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
for line in out.splitlines():
 if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
 elif valid:
  p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
  if key in want:hits[want[key]].append(owner)
refs_text={n:[] for n in names}
for p in base.rglob('*'):
 if not p.is_file() or (p.suffix.lower() not in ('.c','.h','.inc','.asm','.def','.mak') and p.name.lower()!='makefile'):continue
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for i,line in enumerate(lines,1):
  for n in names:
   if re.search(r'(?<![\w.])'+re.escape(Path(n).name)+r'(?![\w.])',line,re.I):refs_text[n].append(dict(file=str(p),line=i,text=line.strip()[:220]))
rows=[]
for n in names:
 h=sha(base/n); rows.append(dict(path=n,sha256=h,original_hashes=[sha(r/n) if (r/n).is_file() else None for r in refs],text_references=refs_text[n],valid_worker_objects=sorted(set(hits[n]))))
print(json.dumps(dict(graph=str(g/'build.ninja'),graph_sha256=sha(g/'build.ninja'),files=rows),indent=2))
