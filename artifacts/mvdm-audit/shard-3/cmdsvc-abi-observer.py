from pathlib import Path
import hashlib,json,re
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/CMDSVC.INC','inc/cmdsvc.h','dos/v86/cmd/command/tcode.asm','dos/command/cmddisp.c','dos/command/cmdmisc.c','softpc.new/host/src/nt_bop.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
asm=(roots['current']/names[0]).read_text(errors='replace'); hdr=(roots['current']/names[1]).read_text(errors='replace'); a=dict(re.findall(r'(?mi)^\s*(SVC_[A-Z0-9_]+)\s+EQU\s+(\d+)\s*$',asm));c=dict(re.findall(r'(?mi)^\s*#define\s+(SVC_[A-Z0-9_]+)\s+(\d+)\s*$',hdr));out['service_constants']={'asm':a,'c':c,'equal':a==c}
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
if m.exists():
 b=m.read_bytes();keys=['_MS_bop_4','_CmdDispatch','_cmdGetNextCmd','_cmdExitVDM','_cmdReturnExitCode'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
