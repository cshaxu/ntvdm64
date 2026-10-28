import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wmsgcb.h','wow32/wmsglb.h','wow32/wmsgcb.c','wow32/wmsglb.c','wow32/wmsg16.c']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/wow32'; graph=build/'build.ninja';out={}
for obj in ('wmsgcb','wmsglb','wmsg16'):
 p=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/'+obj+'.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
 out[obj]=[s.strip().replace('\\','/') for s in p.stdout.splitlines() if any(x in s.lower() for x in ('valid','stale','wmsgcb.h','wmsglb.h'))]
print(json.dumps({'sha256':{p:{n:sha(r/p) for n,r in roots.items()} for p in paths},'edges':[f'{i}:{s}' for i,s in enumerate(graph.read_text(encoding='utf-8').splitlines(),1) if any(x in s.lower() for x in ('wmsgcb.c','wmsglb.c','wmsg16.c'))],'deps':out},indent=2))
