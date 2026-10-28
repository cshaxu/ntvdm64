import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');OUT=A/'nine-way/part-8';root=Path('O:/repos.external/OpenNT'); prefix='windows/core/ntgdi/printers/rasprint/mini/';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='8' and x['kind']=='external' and x['path'].startswith(prefix)];assert len(own)==4773
ids={x['key'] for x in own};cat={x['id']:x for x in csv.DictReader(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv').open(encoding='utf-8-sig')) if x['id'] in ids};assert len(cat)==len(own);z=zipfile.ZipFile('assets/winnt.zip');zn={x.replace('\\','/').split('/')[-1].lower() for x in z.namelist()};profiles={}
for x in own:
 rel=x['path'][len(prefix):];model=rel.split('/')[0]
 if model not in profiles:
  d=root/prefix/model; contexts=[]
  for p in list(d.glob('*.rc'))+list(d.glob('sources'))+list(d.glob('*.inf')):
   text=p.read_text(encoding='latin1').replace('\\','/'); contexts.append((p,text.splitlines()))
  profiles[model]=contexts
rows=[]
for x in own:
 c=cat[x['key']];assert c['edition']=='OpenNT' and c['path']==x['path'];p=root/x['path'];d=p.read_bytes();blob=lambda v:hashlib.sha1(b'blob '+str(len(v)).encode()+b'\0'+v).hexdigest();raw=blob(d);norm=blob(d.replace(b'\r\n',b'\n'));assert c['git_blob'] in (raw,norm),x['path'];rel=x['path'][len(prefix):];model,_,sub=rel.partition('/');refs=[]
 for context,lines in profiles[model]:
  for i,line in enumerate(lines):
   if re.search(r'(?i)(?<![A-Za-z0-9_.-])'+re.escape(sub)+r'(?![A-Za-z0-9_.-])',line): refs.append(str(context.relative_to(root)).replace('\\','/')+':'+str(i+1)+':'+line.strip())
 rows.append(dict(key=x['key'],path=x['path'],edition=c['edition'],catalog_blob=c['git_blob'],raw_sha256=hashlib.sha256(d).hexdigest().upper(),bytes=len(d),model=model,relative_to_model=sub,source_resources=' | '.join(refs),archive_basename_present=p.name.lower() in zn,decision='relevance pending',boundary='Per-file resource references are static; selected device and runtime owner not demonstrated'))
with (OUT/'rasprint-mini-owned.csv').open('w',newline='',encoding='utf-8') as f:
 w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
summary=dict(result='pass',keys=len(rows),models=len(profiles),direct_resource_refs=sum(bool(x['source_resources']) for x in rows),archive_basename_hits=sum(x['archive_basename_present'] for x in rows),ownership_sha256=hashlib.sha256((A/'nine-way-ownership.csv').read_bytes()).hexdigest().upper(),scope='Only owned device assets; per-key identity, model and exact model-relative RC references, no blanket disposition');print(json.dumps(summary,indent=2))
