"""Part-1 evidence coverage: frozen assigned keys versus individual receipts."""
from pathlib import Path
import csv,json,collections
p=Path('artifacts/mvdm-audit/nine-way/part-1');own=list(csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')));expected={k:{x['key'] for x in own if x['bucket']=='1' and x['kind']==k} for k in ('mvdm','external')}
files={'mvdm':[('bde-evidence.json','bde/'),('bin86-evidence.json',''),('dos-keyboard-evidence.json',''),('dos-graphics-evidence.json','dos/v86/cmd/graphics/'),('dos-utilities-evidence.json',''),('mvdm-core-evidence.json','')], 'external':[(s,'') for s in ['external-crt-evidence.json','external-psprint-evidence.json','external-hprasdd-evidence.json','external-display-evidence.json','external-font-evidence.json','external-printer-other-evidence.json','external-direct-evidence.json','external-gdi-engine-evidence.json','external-nls-evidence.json','external-rtl-evidence.json','external-base-evidence.json','external-native-other-evidence.json']]};results={}
for kind,ls in files.items():
 c=collections.Counter();unknown=[]
 for name,prefix in ls:
  for x in json.loads((p/name).read_text(encoding='utf-8-sig'))['files']:
   key=prefix+(x.get('key') or x.get('path') or '')
   if key in expected[kind]:c[key]+=1
   else:unknown.append(dict(receipt=name,key=key))
 results[kind]=dict(expected=len(expected[kind]),observed=len(c),missing=sorted(expected[kind]-set(c)),duplicates={k:v for k,v in c.items() if v>1},out_of_scope=unknown)
print(json.dumps(results,indent=2))
