import csv, hashlib, json
from pathlib import Path
root=Path(__file__).resolve().parents[3]
names=['c13216.exe','c33216.exe','cl.def','cvpack.err','cvpack16.exe','getsize.exe','link16.exe','stripz.exe']
manifest=json.loads((root/'tools/historical/opennt-4.5/MANIFEST.json').read_text(encoding='utf-8-sig'))
entries={r['Path'].lower():r for r in manifest['Files']}
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f:
 rows={r['relative_path'].lower():r for r in csv.DictReader(f)}
out=[]
for n in names:
 p=root/'tools/historical/opennt-4.5'/n
 digest=hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
 row=rows['tools16/'+n]
 out.append({'name':n,'present_at_original_mirror_path':(root/'src/mvdm/tools16'/n).exists(),'historical_copy_present':p.is_file(),'historical_sha256':digest,'manifest_sha256':entries[n]['Sha256'],'opennt_sha256':row['opennt_sha256'],'opennt45_sha256':row['opennt45_sha256'],'matches_45_inventory':digest==row['opennt45_sha256'],'matches_manifest':digest==entries[n]['Sha256']})
print(json.dumps(out,indent=2))
