import csv,hashlib,json
from pathlib import Path
A=Path('artifacts/mvdm-audit');P=A/'nine-way/part-2';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='2'];expected={(x['kind'],x['key']) for x in own};recorded={}
def add(kind,key,source):recorded.setdefault((kind,key),[]).append(source)
for name in ('fax-boundary-observations.json','dos-locales-observations.json','dpmi-guest-family-observations.json','dos-shared-inputs-observations.json','dos-tools-dispatch-observations.json'):
 d=json.loads((P/name).read_text(encoding='utf-8-sig'))
 for x in d.get('mvdm',d.get('files',[])):add('mvdm',x['key'],name)
for name in ('psprint-ppd-owned.csv','rasprint-mini-owned.csv','printer-implementation-owned.csv'):
 for x in csv.DictReader((P/name).open(encoding='utf-8-sig')):add('external',x['key'],name)
missing=sorted(expected-set(recorded));extra=sorted(set(recorded)-expected);duplicate={str(k):v for k,v in recorded.items() if len(v)>1};assert not missing and not extra and not duplicate
summary=dict(result='pass',owned_mvdm=sum(x['kind']=='mvdm' for x in own),owned_external=sum(x['kind']=='external' for x in own),recorded_keys=len(recorded),missing=missing,extra=extra,duplicates=duplicate,scope='Coverage of key evidence records only; not proof of behavioral restoration, relevance decisions, or T/S closure')
print(json.dumps(summary,indent=2))
