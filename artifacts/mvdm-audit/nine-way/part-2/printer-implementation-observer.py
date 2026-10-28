import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit'); OUT=A/'nine-way/part-2';root=Path('O:/repos.external/OpenNT-4.5');prefix='nt/private/ntos/w32/ntgdi/printers/';own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='2' and x['kind']=='external' and '/mini/' not in x['path'] and '/psprint/ppd/' not in x['path']];assert len(own)==488
ids={x['key'] for x in own};cat={x['id']:x for x in csv.DictReader(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv').open(encoding='utf-8-sig')) if x['id'] in ids};assert len(cat)==len(own);z=zipfile.ZipFile('assets/winnt.zip');zn={x.replace('\\','/').split('/')[-1].lower() for x in z.namelist()};contexts={};rows=[]
for x in own:
 c=cat[x['key']];p=root/x['path'];d=p.read_bytes();normal=d.replace(b'\r\n',b'\n');blob=lambda q:hashlib.sha1(b'blob '+str(len(q)).encode()+b'\0'+q).hexdigest();raw=blob(d);norm=blob(normal);assert c['path']==x['path'] and c['edition']=='OpenNT-4.5' and c['git_blob'] in (raw,norm),x['path'];directory=p.parent
 if directory not in contexts:
  targets=[q for q in directory.iterdir() if q.is_file() and (q.name.lower() in ('sources','makefile','dirs') or q.suffix.lower() in ('.rc','.inf','.def'))];contexts[directory]=[(q,q.read_text(encoding='latin1').replace('\\','/').splitlines()) for q in targets]
 refs=[]
 for q,lines in contexts[directory]:
  if q==p:continue
  for i,line in enumerate(lines):
   if re.search(r'(?i)(?<![a-z0-9_.-])'+re.escape(p.name)+r'(?![a-z0-9_.-])',line):refs.append(str(q.relative_to(root)).replace('\\','/')+':'+str(i+1)+':'+line.strip())
 suffix=p.suffix.lower();role={'c':'source','h':'declaration','rc':'resource script','def':'module definition','afm':'font metrics','ntm':'font metrics','bat':'build script'}.get(suffix[1:],'other original data or build input')
 rows.append(dict(key=x['key'],path=x['path'],edition=c['edition'],catalog_blob=c['git_blob'],raw_sha256=hashlib.sha256(d).hexdigest().upper(),bytes=len(d),file_role=role,same_directory_recipe_refs=' | '.join(refs),archive_basename_present=p.name.lower() in zn,decision='relevance pending',boundary='Only same-directory recipe filename references checked; no product seam, installed host or runtime inference'))
with (OUT/'printer-implementation-owned.csv').open('w',newline='',encoding='utf-8') as f:
 w=csv.DictWriter(f,fieldnames=rows[0].keys());w.writeheader();w.writerows(rows)
print(json.dumps(dict(result='pass',count=len(rows),recipe_name_refs=sum(bool(x['same_directory_recipe_refs']) for x in rows),archive_basename_hits=sum(x['archive_basename_present'] for x in rows),ownership_sha256=hashlib.sha256((A/'nine-way-ownership.csv').read_bytes()).hexdigest().upper(),scope='Individual owned external identities and same-directory build metadata, not blanket relevance'),indent=2))
