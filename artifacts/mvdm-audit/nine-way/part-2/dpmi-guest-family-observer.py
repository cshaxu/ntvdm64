import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));own=list(csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')));sel=[x for x in own if x['bucket']=='2' and x['kind']=='mvdm' and x['path'].startswith('dpmi/')];assert len(sel)==32
frozen={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};root=M/'dpmi';textfiles=[]
for p in root.rglob('*'):
 if p.is_file() and (p.suffix.lower() in ('.asm','.inc','.lnk','.def','.cmd','.bat','.sed','.mas') or p.name.lower()=='makefile'):
  textfiles.append((p,p.read_text(encoding='latin1').splitlines()))
rows=[]
for x in sel:
 n=x['path'];f=frozen[n];hashes=[hashlib.sha256((r/n).read_bytes()).hexdigest().upper() for r in (M,*O)];assert hashes==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']];name=Path(n).name;refs=[]
 for p,lines in textfiles:
  if p.resolve()==(M/n).resolve():continue
  for i,line in enumerate(lines):
   if re.search(r'(?i)(?<![\w.])'+re.escape(name)+r'(?![\w.])',line):refs.append(dict(path=p.relative_to(M).as_posix(),line=i+1,text=line.strip()[:240]))
 rows.append(dict(key=x['key'],path=n,hashes=hashes,literal_reference_count=len(refs),literal_references=refs))
zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);e=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/dosx.exe');d=z.read(e);assert d==(M/'dpmi/486/dosx.exe').read_bytes()==Path('O:/winnt/system32/DOSX.EXE').read_bytes();print(json.dumps(dict(result='pass',files=rows,dosx_sha256=hashlib.sha256(d).hexdigest().upper(),archive_sha256=hashlib.sha256(zpath.read_bytes()).hexdigest().upper(),scope='Literal references across retained DPMI source and recipes plus image identity; does not prove exact build provenance or runtime'),indent=2))
