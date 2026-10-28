import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3];roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['inc/rdrsvc.h','inc/RDRSVC.INC','inc/vdmredir.h','vdmredir/vrdisp.c','softpc.new/host/src/nt_bop.c']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/worker';p=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/redir/vrdisp.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'deps':[s.strip().replace('\\','/') for s in p.stdout.splitlines() if any(x in s.lower() for x in ('valid','stale','rdrsvc.h','vdmredir.h'))]},indent=2))
