import csv,json
from pathlib import Path
A=Path('artifacts/mvdm-audit');P=A/'nine-way/part-5';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='5'];expected={(x['kind'],x['key']) for x in own};got={}
def add(kind,key,source):got.setdefault((kind,key),[]).append(source)
for name in ('bios-resource-family-observations.json','vdd-samples-family-observations.json','debugger-family-observations.json','host-input-family-observations.json'):
 d=json.loads((P/name).read_text(encoding='utf-8-sig'))
 for x in d['files']:add('mvdm',x['key'],name)
d=json.loads((P/'multimedia-external-observations.json').read_text(encoding='utf-8-sig'))
for x in d['external']:add('external',x['key'],'multimedia-external-observations.json')
for x in csv.DictReader((P/'external-envelope-owned.csv').open(encoding='utf-8-sig')):add('external',x['key'],'external-envelope-owned.csv')
missing=sorted(expected-set(got));extra=sorted(set(got)-expected);dup={str(k):v for k,v in got.items() if len(v)>1};assert not missing and not extra and not dup
print(json.dumps(dict(result='pass',owned_mvdm=sum(x['kind']=='mvdm' for x in own),owned_external=sum(x['kind']=='external' for x in own),recorded_keys=len(got),missing=missing,extra=extra,duplicates=dup,scope='One per-key evidence record, not functional or relevance completion'),indent=2))
