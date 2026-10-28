from pathlib import Path
import hashlib,json,re
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/rdrsvc.h','inc/RDRSVC.INC','dos/v86/redir/int5c.asm','dos/v86/redir/resident.asm','vdmredir/vrdisp.c','vdmredir/vrdll.c','vdmredir/vrmisc.c','softpc.new/host/src/nt_bop.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
c=(roots['current']/names[0]).read_text(errors='replace');a=(roots['current']/names[1]).read_text(errors='replace');dc={k:int(v,16) for k,v in re.findall(r'(?mi)^\s*#define\s+(SVC_[A-Z0-9_]+)\s+0x([0-9a-f]+)',c)}; da={k:int(v,16) for k,v in re.findall(r'(?mi)^\s*(SVC_[A-Z0-9_]+)\s+EQU\s+([0-9a-f]+)H\b',a)}; common=sorted(dc.keys()&da.keys());out['constants']={'c_count':len(dc),'asm_count':len(da),'shared':len(common),'mismatch':{k:[dc[k],da[k]] for k in common if dc[k]!=da[k]},'only_c':sorted(dc.keys()-da.keys()),'only_asm':sorted(da.keys()-dc.keys())}
for label,p,keys in [('worker_map',Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map'),['_LoadVdmRedir','_MS_bop_7']),('redir_map',Path('artifacts/mvdm-audit/shard-3/redir-private-map.txt'),['_VrDispatch ','_VrInitialize ','_VrUninitialize ','_VrTerminateDosProcess '])]:
 if p.exists():
  b=p.read_bytes();out[label]={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
