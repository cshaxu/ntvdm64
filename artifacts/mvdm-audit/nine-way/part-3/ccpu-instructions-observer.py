import csv, hashlib, json
from pathlib import Path
repo = Path(__file__).resolve().parents[4]
roots = {'current': repo/'src/mvdm', 'opennt': Path('O:/repos.external/OpenNT/base/mvdm'), 'opennt45': Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
names = {'bswap','cmpxchg','invd','invlpg','wbinvd','xadd'}
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f:
 rows = [r for r in csv.DictReader(f) if r['kind']=='mvdm' and r['bucket']=='3' and Path(r['path']).parent.as_posix()=='softpc.new/base/ccpu386' and Path(r['path']).stem in names and Path(r['path']).suffix in ('.c','.h')]
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
graph = repo/'build/.ninja/msvc-x86-formal-release/build.ninja'
lines = graph.read_text(encoding='utf-8').splitlines() if graph.is_file() else []
out = []
for r in rows:
 p = r['path']; needle = p.lower().replace('/','\\')
 hits = [i for i,line in enumerate(lines,1) if needle in line.lower().replace('/','\\')]
 out.append({'key':r['key'],'path':p,'sha256':{k:sha(v/p) for k,v in roots.items()},'ninja_lines':hits})
print(json.dumps(out,indent=2))
