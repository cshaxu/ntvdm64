import csv, hashlib, json, re, subprocess
from pathlib import Path
repo = Path(__file__).resolve().parents[4]
roots = {'current': repo/'src/mvdm', 'opennt': Path('O:/repos.external/OpenNT/base/mvdm'), 'opennt45': Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f:
 rows = [r for r in csv.DictReader(f) if r['kind']=='mvdm' and r['bucket']=='3' and r['path'].startswith('inc/')]
names = sorted({Path(r['path']).name for r in rows},key=len,reverse=True)
pattern = r'(?i)(?:#\s*include\s*[<\"]|\binclude\s+)(' + '|'.join(re.escape(n) for n in names) + r')(?:[>\"\s;]|$)'
proc = subprocess.run(['rg','-n','--no-messages','-i',pattern,'src/mvdm'],cwd=repo,text=True,encoding='utf-8',errors='replace',capture_output=True,check=False)
if proc.returncode not in (0,1): raise RuntimeError(proc.stderr)
rx=re.compile(pattern)
refs={n.lower():[] for n in names}
for line in proc.stdout.splitlines():
 m=rx.search(line)
 if m: refs[m.group(1).lower()].append(line)
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
out=[]
for r in rows:
 name=Path(r['path']).name
 out.append({'key':r['key'],'path':r['path'],'sha256':{k:sha(v/r['path']) for k,v in roots.items()},'textual_include_candidates':sorted(refs[name.lower()])})
print(json.dumps(out,indent=2))
