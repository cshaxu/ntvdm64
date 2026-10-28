from pathlib import Path
import hashlib,json,os,re
roots={'current':Path('src/mvdm/softpc.new/host/link'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/link'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/host/link')}; names=['softerr.ico','softpc.ico','softpc.def','resource.rc','sources'];out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
for label,p in [('current_rc',Path('src/mvdm/softpc.new/obj.vdm/resource.rc')),('generated_rc',Path('build/.ninja/msvc-x86-release/worker/generated/softpc-embedded-roms.rc')),('current_def',Path('src/mvdm/softpc.new/obj.vdm/obj/i386/ntvdm.def')),('build_ninja',Path('build/.ninja/msvc-x86-release/worker/build.ninja'))]:
 if p.exists():
  b=p.read_bytes();s=b.decode(errors='replace'); out[label]={'sha256':hashlib.sha256(b).hexdigest(),'lines':[x.strip() for x in s.splitlines() if any(k in x.lower() for k in ['softpc.ico','softerr.ico','errorpanel','softpc-resource.res: rc'])][:12]}
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt'); build=Path('build/.ninja/msvc-x86-release/worker').resolve(); paths={os.path.normcase(os.path.abspath(roots['current']/n)):n for n in names[:3]};matched=[]
for ln in dep.read_text(encoding='utf-8',errors='replace').splitlines():
 if ln.startswith('    '):
  norm=os.path.normcase(os.path.abspath(os.path.join(build,ln.strip())))
  if norm in paths: matched.append({'name':paths[norm],'line':ln.strip()})
out['dependency_snapshot']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'matches':matched,'limitation':'C compile deps only; resource compiler dependencies require separately examined resource source'}
print(json.dumps(out,indent=2))
