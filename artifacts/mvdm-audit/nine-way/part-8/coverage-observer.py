import csv,json
from pathlib import Path
A=Path('artifacts/mvdm-audit');P=A/'nine-way/part-8';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='8'];expected={(x['kind'],x['key']) for x in own};got={}
def add(kind,key,source):got.setdefault((kind,key),[]).append(source)
for name,field in (('edit-guest-host-observations.json','guest_files'),('guest-family-inventory-observations.json','files')):
 d=json.loads((P/name).read_text(encoding='utf-8-sig'))
 for x in d[field]:add('mvdm',x['key'],name)
for x in csv.DictReader((P/'rasprint-mini-owned.csv').open(encoding='utf-8-sig')):add('external',x['key'],'rasprint-mini-owned.csv')
missing=sorted(expected-set(got));extra=sorted(set(got)-expected);dup={str(k):v for k,v in got.items() if len(v)>1};assert not missing and not extra and not dup
print(json.dumps(dict(result='pass',owned_mvdm=sum(x['kind']=='mvdm' for x in own),owned_external=sum(x['kind']=='external' for x in own),recorded_keys=len(got),missing=missing,extra=extra,duplicates=dup,scope='Ownership evidence coverage only; not completion of semantic audit or provider relevance'),indent=2))
