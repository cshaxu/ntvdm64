import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wowdde.h','wow32/wdde.h','wow32/wdde.c','wow32/precomp.h','wow32/sources']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/wow32';p=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/wdde.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
s=subprocess.run(['rg','-n','-i','-e',r'#\s*include\s*[<\"]wowdde\.h','-e',r'#\s*include\s*[<\"]wdde\.h','src/mvdm','src/wow32-dll'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=False)
if s.returncode not in (0,1):raise RuntimeError(s.stderr)
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'deps':[x.strip().replace('\\','/') for x in p.stdout.splitlines() if any(a in x.lower() for a in ('valid','stale','wowdde.h','wdde.h'))],'includes':sorted(s.stdout.splitlines())},indent=2))
