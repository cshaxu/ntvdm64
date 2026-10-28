from pathlib import Path
import hashlib,json,os,re
roots={'current':Path('src/mvdm/softpc.new/base/inc'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/inc'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/base/inc')};names=['host_cfg.h','host_pth.h','host_sig.h','host_str.h'];out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve();targets={os.path.normcase(os.path.abspath(roots['current']/n)):n for n in names};obj=None;found={n:[] for n in names}
for line in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if not line.startswith(' ') and ': #deps ' in line:obj=line.split(': #deps ')[0]
 elif line.startswith('    '):
  norm=os.path.normcase(os.path.abspath(os.path.join(build,line.strip())))
  if norm in targets:found[targets[norm]].append({'object':obj,'relative_path':line.strip()})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':found}
print(json.dumps(out,indent=2))
