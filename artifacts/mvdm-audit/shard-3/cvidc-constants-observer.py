from pathlib import Path
import hashlib,json,os
roots={'current':Path('src/mvdm/softpc.new/base/cvidc'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/base/cvidc'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/base/cvidc'),'mips_prod':Path('src/mvdm/softpc.new/host/inc/mips/prod'),'x86_prod':Path('src/ntvdm-exe/softpc/include/generated/x86/prod')}; names=['adjust_c.h','ea_c.h','muldiv_c.h','ports_c.h','stack_c.h','xfer_c.h'];out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve(); targets={os.path.normcase(os.path.abspath(r/n)):(k,n) for k,r in roots.items() if k in ('current','mips_prod','x86_prod') for n in names}; obj=None; found=[]
for line in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if not line.startswith(' ') and ': #deps ' in line:obj=line.split(': #deps ')[0]
 elif line.startswith('    '):
  norm=os.path.normcase(os.path.abspath(os.path.join(build,line.strip())))
  if norm in targets:found.append({'target':targets[norm],'object':obj,'relative_path':line.strip()})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':found}
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');b=m.read_bytes();keys=['_AAD ','_AAM ','_CALLF ','_DIV16 ','_MUL16 ','_POP ','_PUSH '];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
