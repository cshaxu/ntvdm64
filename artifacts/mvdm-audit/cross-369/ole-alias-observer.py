import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wole2.h','wow32/wole2.c','wow32/walias.c','wow32/wkernel.c','wow32/wuwind.c']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
graph=repo/'build/.ninja/msvc-x86-release/wow32/build.ninja'
p=subprocess.run(['ninja','-C',str(graph.parent),'-t','deps','obj/wole2.obj'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'ninja_edges':[f'{n}:{s}' for n,s in enumerate(graph.read_text(encoding='utf-8').splitlines(),1) if 'wole2.c' in s.lower()],'ninja_wole2_dep_status':[line.strip().replace('\\','/') for line in p.stdout.splitlines() if any(x in line.lower() for x in ('valid','stale','wole2.h','precomp.h'))]},indent=2))
