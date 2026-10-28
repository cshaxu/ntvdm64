from pathlib import Path
import hashlib,json,os
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')};names=['softpc.new/base/inc/base_def.h','softpc.new/host/inc/host_gen.h','softpc.new/host/inc/insignia.h','softpc.new/base/inc/xt.h','softpc.new/base/ccpu386/ccpusas4.c','softpc.new/host/src/nt_timer.c'];out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve();targets={os.path.normcase(os.path.abspath(roots['current']/n)):n for n in names[:2]};obj=None;valid=False;found={n:[] for n in names[:2]}
for line in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if not line.startswith(' ') and ': #deps ' in line:
  obj=line.split(': #deps ')[0];valid=line.rstrip().endswith('(VALID)')
 elif line.startswith('    '):
  norm=os.path.normcase(os.path.abspath(os.path.join(build,line.strip())))
  if valid and norm in targets:found[targets[norm]].append({'object':obj,'relative_path':line.strip()})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':found}
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');b=m.read_bytes();keys=['_sas_init ','_sas_term ','_c_sas_connect_memory ','_HeartBeatThread '];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
