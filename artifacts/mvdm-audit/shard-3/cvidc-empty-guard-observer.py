from pathlib import Path
import csv,hashlib,json,os,re
names=['add_c.h','bswap_c.h','cmp_c.h','copies_c.h','fmevid_c.h','gdmain_c.h','gicvid_c.h','idmain_c.h','logic_c.h','move_c.h','nanoe_c.h','unchn_c.h']
root=Path('src/mvdm/softpc.new/base/cvidc').resolve(); refs={'current':root,'opennt':Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/cvidc'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/base/cvidc')}
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt'); build=Path('build/.ninja/msvc-x86-release/worker').resolve(); out={}
for n in names:
 row={}
 for k,r in refs.items():
  p=r/n; row[k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
 lines=[s.strip() for s in (root/n).read_text().splitlines() if s.strip()];row['guard_only']=len(lines)==3 and bool(re.fullmatch(r'#ifndef\s+\w+',lines[0])) and bool(re.fullmatch(r'#define\s+\w+',lines[1])) and lines[0].split()[1]==lines[1].split()[1] and lines[2].startswith('#endif')
 row['normalized_dep_lines']=[]
 if dep.exists():
  for ln in dep.read_text(encoding='utf-8',errors='replace').splitlines():
   if ln.startswith('    ') and os.path.normcase(os.path.abspath(os.path.join(build,ln.strip())))==os.path.normcase(os.path.abspath(root/n)): row['normalized_dep_lines'].append(ln.strip())
 out[n]=row
out['_dependency_snapshot_sha256']=hashlib.sha256(dep.read_bytes()).hexdigest() if dep.exists() else None
print(json.dumps(out,indent=2))
