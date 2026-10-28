import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='5' and x['kind']=='mvdm' and x['path'].startswith('vdd/')];assert len(own)==31;f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};root=M/'vdd/samples';contexts=[]
for p in root.rglob('*'):
 if p.is_file() and (p.suffix.lower() in ('.asm','.inc','.c','.h','.rc','.def','.lnk','.cmd','.bat') or p.name.lower() in ('makefile','sources')):contexts.append((p,p.read_text(encoding='latin1').splitlines()))
zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);files=[x for x in z.infolist() if not x.is_dir()];rows=[]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
for x in own:
 n=x['path'];v=f[n];hs=[sha(r/n) for r in (M,*O)];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];p=M/n;refs=[]
 for q,lines in contexts:
  if q.resolve()==p.resolve():continue
  for i,line in enumerate(lines):
   if re.search(r'(?i)(?<![\w.])'+re.escape(p.name)+r'(?![\w.])',line):refs.append(dict(path=q.relative_to(M).as_posix(),line=i+1,text=line.strip()[:180]))
 exact=[];named=[]
 if p.suffix.lower() in ('.exe','.sys','.dll'):
  data=p.read_bytes();exact=[e.filename for e in files if e.file_size==len(data) and hashlib.sha256(z.read(e)).hexdigest().upper()==hs[0]];named=[e.filename for e in files if e.filename.replace('\\','/').split('/')[-1].lower()==p.name.lower()]
 rows.append(dict(key=x['key'],path=n,hashes=hs,reference_count=len(refs),literal_refs=refs[:20],archive_identical_members=exact,archive_named_members=named))
print(json.dumps(dict(result='pass',files=rows,archive_sha256=sha(zpath),scope='Literal sample source references and exact sample-binary package identity; does not establish runtime or generic VDD capability'),indent=2))
