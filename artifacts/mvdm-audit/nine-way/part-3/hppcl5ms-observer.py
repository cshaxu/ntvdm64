import csv,hashlib,json,re
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
base=Path('O:/repos.external/OpenNT-4.5')
with (repo/'artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if r['kind']=='external' and r['bucket']=='3' and '/hppcl5ms/' in r['path'].lower()]
with (repo/'artifacts/mvdm-audit/frozen-external-audit-rows.csv').open(encoding='utf-8-sig',newline='') as f: origins={r['id']:r for r in csv.DictReader(f)}
rc=base/'nt/private/ntos/w32/ntgdi/printers/rasprint/mini/hppcl5ms/hppcl5ms.rc'
lines=rc.read_text(encoding='latin1').splitlines()
output=[]
for r in rows:
 ref=origins[r['key']];p=base/Path(ref['path']); matches=[]
 for no,line in enumerate(lines,1):
  if re.search(r"(?i)(?<![A-Za-z0-9_.-])" + re.escape(p.name) + r"(?![A-Za-z0-9_.-])", line):matches.append(no)
 output.append({'key':r['key'],'path':r['path'],'edition':ref['edition'],'exists':p.is_file(),'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else '', 'extension':p.suffix.lower(),'rc_filename_hit_lines':';'.join(map(str,matches))})
out=repo/'artifacts/mvdm-audit/nine-way/part-3/hppcl5ms-owned-files.csv'
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(output[0]));w.writeheader();w.writerows(output)
print(json.dumps({'count':len(output),'missing':sum(not r['exists'] for r in output),'rc_filename_hits':sum(bool(r['rc_filename_hit_lines']) for r in output),'rc_sha256':hashlib.sha256(rc.read_bytes()).hexdigest().upper(),'source_root':str(base)},indent=2))
