from pathlib import Path
import hashlib,json,os
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['softpc.new/host/inc/nt_mouse.h','softpc.new/host/src/nt_mouse.c','softpc.new/base/keymouse/mouse_io.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
for n in ['src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.c','src/ntvdm-exe/softpc/mvdm_softpc_mouse_guest.c']:
 p=Path(n);out[n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
if m.exists():
 b=m.read_bytes();keys=['_DoMouseInterrupt ','_MouseEoiHook ','_host_mouse_install1 ','_deinstall_host_mouse ','_mvdm_softpc_mouse_submit ','_mvdm_softpc_mouse_apply '];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve();target=os.path.normcase(os.path.abspath(roots['current']/names[0])); obj=None;found=[]
for line in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if not line.startswith(' ') and ': #deps ' in line:obj=line.split(': #deps ')[0]
 elif line.startswith('    ') and os.path.normcase(os.path.abspath(os.path.join(build,line.strip())))==target:found.append({'object':obj,'relative_path':line.strip()})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':found}
print(json.dumps(out,indent=2))
