from pathlib import Path
import hashlib,json,subprocess
names=['src/mvdm/softpc.new/host/src/nt_rflop.c','src/mvdm/softpc.new/host/src/nt_fdisk.c','src/ntvdm-exe/session/session.c','src/ntvdm-exe/command/source/mvdm_command_guest_state.c','src/ntvdm-exe/softpc/mvdm_guest_location.c','artifacts/mvdm-audit/shard-3/disk-close-failure-probe.c','artifacts/mvdm-audit/shard-3/disk-close-failure-audit.md','src/ntvdm-exe/win32/thread_start_compat.c','src/opennt-abi/host-compat/include/thread_start_compat.h','tools/build/New-T310OriginalSoftpcNinja.ps1']
out={}
for n in names:
 p=Path(n);out[n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
for n in ['nt_rflop.c','nt_fdisk.c']:
 for label,root in [('opennt',Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src')),('opennt45',Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/host/src'))]:
  p=root/n;out[label+'/'+n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
p=Path('build/.ninja/msvc-x86-release/worker/obj/host/nt_rflop.obj')
if p.exists():
 b=p.read_bytes();r=subprocess.run(['objdump','-r',str(p)],capture_output=True,text=True,check=True);keys=['session_thread_bind','mvdm_command_guest_state_add_fd_access','_get_drive_handle','_nt_floppy_close','__imp__CreateThread@24','_fdc_thread'];out['object']={'sha256':hashlib.sha256(b).hexdigest(),'relocations':[s for s in r.stdout.splitlines() if any(k in s for k in keys)]}
p=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/commands.stdout.txt'); b=p.read_bytes(); lines=[s for s in b.decode(errors='replace').splitlines() if s.lower().endswith('nt_rflop.c')];out['compile_snapshot']={'sha256':hashlib.sha256(b).hexdigest(),'lines':lines,'compat_forced':[('thread_start_compat.h' in s.lower()) for s in lines]}
print(json.dumps(out,indent=2))
