import collections,csv,hashlib,json
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
roots={'OpenNT':Path('O:/repos.external/OpenNT'),'OpenNT-4.5':Path('O:/repos.external/OpenNT-4.5')}
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if r['kind']=='external' and r['bucket']=='6']
with (repo/'artifacts/mvdm-audit/frozen-external-audit-rows.csv').open(encoding='utf-8-sig',newline='') as f: origins={r['id']:r for r in csv.DictReader(f)}
out=[]
for row in rows:
 origin=origins[row['key']];root=roots[origin['edition']];path=root/Path(origin['path'])
 out.append({'key':row['key'],'path':row['path'],'edition':origin['edition'],'edition_path':origin['path'],'exists':path.is_file(),'sha256':hashlib.sha256(path.read_bytes()).hexdigest().upper() if path.is_file() else ''})
dest=repo/'artifacts/mvdm-audit/nine-way/part-6/external-owned-files.csv'
with dest.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(out[0]));w.writeheader();w.writerows(out)
print(json.dumps({'keys':len(out),'missing':sum(not x['exists'] for x in out),'editions':dict(collections.Counter(x['edition'] for x in out)),'groups':dict(collections.Counter('/'.join(x['path'].split('/')[:6]) for x in out))},indent=2))
