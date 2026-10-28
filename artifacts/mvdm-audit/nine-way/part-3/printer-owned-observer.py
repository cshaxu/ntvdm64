import csv,hashlib,json,re,collections
from pathlib import Path
repo=Path(__file__).resolve().parents[4]; source=Path('O:/repos.external/OpenNT-4.5'); mini=source/'nt/private/ntos/w32/ntgdi/printers/rasprint/mini'
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if r['kind']=='external' and r['bucket']=='3']
with (repo/'artifacts/mvdm-audit/frozen-external-audit-rows.csv').open(encoding='utf-8-sig',newline='') as f: origins={r['id']:r for r in csv.DictReader(f)}
cache={};out=[]
for r in rows:
 ref=origins[r['key']];p=source/Path(ref['path']);family=r['path'].split('/')[8];dir=mini/family
 if family not in cache:
  rcfiles=sorted(dir.glob('*.rc')) if dir.is_dir() else []
  cache[family]=[(x.name,x.read_text(encoding='latin1').splitlines()) for x in rcfiles]
 hits=[]
 for rcname,lines in cache[family]:
  pattern=re.compile(r'(?i)(?<![A-Za-z0-9_.-])'+re.escape(p.name)+r'(?![A-Za-z0-9_.-])')
  hits.extend(f'{rcname}:{i}' for i,line in enumerate(lines,1) if pattern.search(line))
 out.append({'key':r['key'],'path':r['path'],'edition':ref['edition'],'family':family,'exists':p.is_file(),'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else '', 'rc_basename_candidate_hits':';'.join(sorted(hits))})
output=repo/'artifacts/mvdm-audit/nine-way/part-3/printer-owned-files.csv'
with output.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(out[0]));w.writeheader();w.writerows(out)
print(json.dumps({'owned_keys':len(out),'missing':sum(not x['exists'] for x in out),'rc_basename_candidates':sum(bool(x['rc_basename_candidate_hits']) for x in out),'family_counts':dict(collections.Counter(x['family'] for x in out))},indent=2))
