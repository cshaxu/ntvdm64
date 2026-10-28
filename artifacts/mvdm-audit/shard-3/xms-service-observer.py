from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/xmssvc.h','inc/XMSSVC.INC','xms.486/xmsdisp.c','xms.486/xmsblock.c','xms.486/xms.c','dos/v86/dev/himem/himem4.asm','softpc.new/host/src/nt_bop.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
if m.exists():
 b=m.read_bytes(); keys=['_MS_bop_2','_XMSDispatch','_XMSInit','_xmsAllocBlock','_xmsFreeBlock','_xmsReallocBlock'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
