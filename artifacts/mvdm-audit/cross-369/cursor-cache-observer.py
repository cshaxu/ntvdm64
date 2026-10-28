import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
files=['wow32/wcurcash.c','wow32/wcurcash.h','wow32/wcuricon.c','wow32/wcuricon.h','wow32/wucursor.c','wow32/wuser.c','wow32/sources']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
identities={p:{name:sha(root/p) for name,root in roots.items()} for p in files}
graph=repo/'build/.ninja/msvc-x86-release/wow32/build.ninja'
lines=graph.read_text(encoding='utf-8').splitlines()
terms=['W32CheckWOWCashforIconCursors','W32FindCursorIcon','W32AddCursorIconCash','W32DeleteCursorIconCash','W32DeleteCursorIconCashForTask']
p=subprocess.run(['rg','-n','-F','-e',terms[0],'-e',terms[1],'-e',terms[2],'-e',terms[3],'-e',terms[4],'src/mvdm','src/wow32-dll'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=False)
if p.returncode not in (0,1):raise RuntimeError(p.stderr)
print(json.dumps({'sha256':identities,'ninja_lines':[f'{n}:{line}' for n,line in enumerate(lines,1) if any(x in line.lower() for x in ('wcurcash.c','wcuricon.c','wucursor.c','wuser.c'))],'legacy_name_references':sorted(p.stdout.splitlines())},indent=2))
