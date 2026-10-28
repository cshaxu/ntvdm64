from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['dos/command/cmdkeyb.h','dos/command/cmdkeyb.c','dos/command/cmddisp.c','dos/v86/cmd/command/tcode.asm','inc/CMDSVC.INC']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
for n in ['src/ntvdm-exe/softpc/mvdm_shadow_registry.c','src/ntvdm-exe/softpc/mvdm_softpc_firmware.c','src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h']:
 p=Path(n);out[n]=hashlib.sha256(p.read_bytes()).hexdigest()
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
if m.exists():
 b=m.read_bytes();keys=['_cmdGetKbdLayout','_ntvdm_shadow_reg_open_key_ex@20','_NtvdmGetSystemDirectoryA'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
