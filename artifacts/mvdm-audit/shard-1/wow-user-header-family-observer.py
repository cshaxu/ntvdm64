"""Bounded WOW headers: dual-source hashes, direct includes and VALID Ninja deps."""
from pathlib import Path
import csv,hashlib,json,re,subprocess
names='wkbdtbl2.h wmsg16.h wmsgbm.h wmsgem.h wowdde.h wowtable.h wparam.h wsdynmc.h wsubcls.h wucursor.h wuhook.h wulang.h wuwind.h'.split()
base=Path('src/mvdm/wow32');refs=[Path('O:/repos.external/OpenNT/base/mvdm/wow32'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32')];g=Path('build/.ninja/msvc-x86-release/wow32')
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
want={str((base/n).resolve()).lower():n for n in names};hits={n:[] for n in names};out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
for line in out.splitlines():
 if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
 elif valid:
  p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
  if key in want:hits[want[key]].append(owner)
inc={n:[] for n in names}
for p in list(base.rglob('*.c'))+list(base.rglob('*.h'))+list(Path('src/wow32-dll').rglob('*.c'))+list(Path('src/wow32-dll').rglob('*.h')):
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError: continue
 for i,line in enumerate(lines,1):
  for n in names:
   if re.search(r'^\s*#\s*include\s*[<"]'+re.escape(n)+r'[>"]',line,re.I):inc[n].append(f'{p}:{i}')
rows=[]
for n in names:
 h=sha(base/n); old=[sha(r/n) if (r/n).is_file() else None for r in refs];rows.append(dict(name=n,sha256=h,reference_hashes=old,all_equal=all(x==h for x in old),direct_includes=inc[n],valid_dependency_objects=sorted(set(hits[n]))))
print(json.dumps(dict(graph=str(g/'build.ninja'),graph_sha256=sha(g/'build.ninja'),files=rows),indent=2))
