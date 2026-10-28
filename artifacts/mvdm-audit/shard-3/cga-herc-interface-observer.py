from pathlib import Path
import hashlib,json,os,subprocess
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')};names=['softpc.new/host/inc/nt_cga.h','softpc.new/base/inc/herc.h','softpc.new/base/video/gvi.c','softpc.new/host/src/nt_graph.c','softpc.new/host/src/nt_cga.c'];out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');b=m.read_bytes();keys=['_gvi_init ','_gvi_term ','_herc_init ','_herc_term ','_nt_cga_colour_hi_graph_std ','_nt_init_cga_colour_hi_graph '];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve();targets={os.path.normcase(os.path.abspath(roots['current']/n)):n for n in names[:2]};obj=None;found=[]
for line in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if not line.startswith(' ') and ': #deps ' in line:obj=line.split(': #deps ')[0]
 elif line.startswith('    '):
  norm=os.path.normcase(os.path.abspath(os.path.join(build,line.strip())))
  if norm in targets:found.append({'file':targets[norm],'object':obj,'relative_path':line.strip()})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':found}
p=Path('build/.ninja/msvc-x86-release/worker/obj/video/gvi.obj')
if p.exists():
 b=p.read_bytes(); r=subprocess.run(['objdump','-t',str(p)],capture_output=True,text=True,check=True);out['gvi_object']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in r.stdout.splitlines() if any(k in s for k in ['_gvi_init','_herc_init','_herc_term','_cga_init','_vga_init'])]}
print(json.dumps(out,indent=2))
