import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wddeml32.c','wow32/wddeml32.h','wow32/wddetbl2.h','wow32/sources','wow16/ddeml/ddeml.c','wow16/ddeml/makefile','wow16/ddeml/ddeml.def']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
p=subprocess.run(['rg','-n','-i','-e','wddetbl2','-e','wddeml32','-e','WD32DdeInitialize','-e','W32DdemlCallBack','src/mvdm/wow32','src/wow32-dll'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=False)
if p.returncode not in (0,1):raise RuntimeError(p.stderr)
graph=repo/'build/.ninja/msvc-x86-release/wow32/build.ninja'
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'references':sorted(p.stdout.splitlines()),'graph_hits':[f'{n}:{line}' for n,line in enumerate(graph.read_text(encoding='utf-8').splitlines(),1) if 'wddeml32' in line.lower() or 'wddetbl2' in line.lower()]},indent=2))
