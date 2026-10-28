"""Read-only C-VID header identity, direct references and normalized VALID deps."""
from pathlib import Path
import csv,hashlib,json,re,subprocess
names='dfa.gi error.h host_gfx.h hostsync.h nt_cgalt.h nt_com.h nt_det.h nt_egalt.h nt_extra.h nt_graph.h nt_mem.h nt_mon.h nt_smenu.h nt_timer.h nt_uis.h ntcheese.h'.split()
base=Path('src/mvdm/softpc.new/host/inc');refs=[Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/inc'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/host/inc')];g=Path('build/.ninja/msvc-x86-release/worker');sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((base/n).resolve()).lower():n for n in names};hits={n:[] for n in names}
out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
for line in out.splitlines():
 if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
 elif valid:
  p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
  if key in want:hits[want[key]].append(owner)
includes={n:[] for n in names};pattern=re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',re.I)
for p in Path('src/mvdm/softpc.new').rglob('*'):
 if not p.is_file() or p.suffix.lower() not in ('.c','.h','.inc','.cpp'):continue
 try:lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for i,line in enumerate(lines,1):
  m=pattern.search(line)
  if m:
   n=m.group(1).replace('\\','/').split('/')[-1].lower()
   if n in includes:includes[n].append(f'{p}:{i}')
rows=[]
for n in names:
 s=(base/n).read_text(encoding='latin1');lines=[x.strip() for x in s.splitlines() if x.strip()]; guard_only=len(lines)<=3 and lines and lines[0].startswith('#ifndef') and lines[1].startswith('#define') and lines[-1].startswith('#endif')
 rows.append(dict(name=n,sha256=sha(base/n),reference_hashes=[sha(r/n) if (r/n).is_file() else None for r in refs],guard_only=guard_only,line_count=len(lines),direct_includes=includes[n],valid_worker_objects=sorted(set(hits[n]))))
print(json.dumps(dict(graph=str(g/'build.ninja'),graph_sha256=sha(g/'build.ninja'),files=rows),indent=2))
