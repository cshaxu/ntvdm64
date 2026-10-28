from pathlib import Path
import hashlib,json,os,re
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')};names=['softpc.new/host/src/nt_rflop.c','softpc.new/base/disks/gfi.c','softpc.new/base/disks/fla.c','softpc.new/base/disks/floppy_i.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');b=m.read_bytes();keys=['_nt_rflop_command ','_nt_rflop_reset ','_fdc_thread ','_fdc_read_write ','_nt_floppy_close ','_FloppyTerminatePDB ','_host_gfi_rdiskette_active '];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
dep=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/deps.stdout.txt');build=Path('build/.ninja/msvc-x86-release/worker').resolve();objs=['obj/host/nt_rflop.obj','obj/disks/gfi.obj'];blocks=[];text=dep.read_text(encoding='utf-8',errors='replace');repat=re.compile(r'(?m)^(obj/(?:host/nt_rflop|disks/gfi)\.obj): #deps[^\r\n]*(?:\r?\n[ \t]+[^\r\n]*)*')
for mat in repat.finditer(text):
 lines=mat.group().splitlines();blocks.append({'object':mat.group(1),'deps':[{'raw':s.strip(),'normalized':os.path.normcase(os.path.abspath(os.path.join(build,s.strip())))} for s in lines[1:]]})
out['deps']={'sha256':hashlib.sha256(dep.read_bytes()).hexdigest(),'blocks':blocks}
print(json.dumps(out,indent=2))
