import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3];roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wklocal.h','wow32/wklocal.c','wow32/wktbl2.h','wow32/wcall16.c','wow32/wcall32.c','wow32/walloc16.c','wow32/sources']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/wow32';g=build/'build.ninja';deps={}
for obj in ('wcall16','wcall32','walloc16'):
 p=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/'+obj+'.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
 deps[obj]=[s.strip().replace('\\','/') for s in p.stdout.splitlines() if any(x in s.lower() for x in ('valid','stale','wklocal.h'))]
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'edges':[f'{n}:{s}' for n,s in enumerate(g.read_text(encoding='utf-8').splitlines(),1) if any(x in s.lower() for x in ('wklocal.c','wcall16.c','wcall32.c','walloc16.c'))],'deps':deps},indent=2))
