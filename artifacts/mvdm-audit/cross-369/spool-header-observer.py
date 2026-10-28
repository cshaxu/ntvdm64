import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wspool.h','wow32/winspldl.h','wow32/wspool.c','wow32/wgprnset.c','wow32/wow32fax.c','wow32/walias.h','wow32/sources']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/wow32';g=build/'build.ninja'
p=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/wspool.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True)
search=subprocess.run(['rg','-n','-i','-e','winspldl.h','-e','LoadWinspoolAndGetProcAddresses','-e','fWinspoolLoaded','src/mvdm/wow32','src/wow32-dll'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=False)
if search.returncode not in (0,1):raise RuntimeError(search.stderr)
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'edge':[f'{n}:{s}' for n,s in enumerate(g.read_text(encoding='utf-8').splitlines(),1) if 'wspool.c' in s.lower()],'deps':[s.strip().replace('\\','/') for s in p.stdout.splitlines() if any(x in s.lower() for x in ('valid','stale','wspool.h','winspldl.h','walias.h'))],'historical_references':sorted(search.stdout.splitlines())},indent=2))
