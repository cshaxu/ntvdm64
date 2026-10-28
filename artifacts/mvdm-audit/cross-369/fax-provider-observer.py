import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wow32fax.c','wow32/wkman.c','wow32/wgprnset.c','wow32/sources']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
graph=repo/'build/.ninja/msvc-x86-release/wow32/build.ninja';mapfile=graph.parent/'wow32.dll.map'
terms=('wow32fax.c','wkman.c','wgprnset.c')
lines=graph.read_text(encoding='utf-8').splitlines();maps=mapfile.read_text(encoding='utf-8',errors='replace').splitlines()
names=('WOW32FaxHandler','GetSupportedFaxDrivers','WowFaxWndProc')
print(json.dumps({'sha256':{f:{n:sha(r/f) for n,r in roots.items()} for f in paths},'ninja_edges':[f'{i}:{s}' for i,s in enumerate(lines,1) if any(n in s.lower() for n in terms)],'map_hits':[f'{i}:{s.strip()}' for i,s in enumerate(maps,1) if any(n.lower() in s.lower() for n in names)]},indent=2))
