"""Read-only C-VID header identity, direct references and normalized VALID deps."""
from pathlib import Path
import csv,hashlib,json,re,subprocess
names='access_c.h bodge_c.h btest_c.h buffer_c.h c2cput_c.h contxt_c.h cpumod_c.h cpureg_c.h flags_c.h frag_c.h inst_c.h io_c.h lxs_c.h mark_c.h misc_c.h nanoc_c.h ppchst_c.h sas_c.h segreg_c.h sf_c.h strint_c.h strsub_c.h sub_c.h task_c.h test_c.h xchg_c.h'.split()
base=Path('src/mvdm/softpc.new/base/cvidc');refs=[Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/cvidc'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/base/cvidc')];g=Path('build/.ninja/msvc-x86-release/worker');sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((base/n).resolve()).lower():n for n in names};hits={n:[] for n in names}
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
