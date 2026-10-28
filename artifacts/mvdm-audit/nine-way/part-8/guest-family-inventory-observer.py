import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));edit={'wow16/user/'+n+'.c' for n in ('editec','edecrare','editsl','editml','edmlonce','edmlrare','edslrare')}|{'wow16/user/edit.h'};own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='8' and x['kind']=='mvdm' and x['path'] not in edit];assert len(own)==112
f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};root=M/'wow16';lookup={};rows={}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
for x in own:
 n=x['path'];p=M/n;v=f[n];hs=[sha(r/n) for r in (M,*O)];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];lookup.setdefault(p.name.lower(),[]).append(n);rows[n]=dict(key=x['key'],path=n,hashes=hs,basename_match_count=0,literal_refs=[],archive_identical=[],archive_named=[])
pat=re.compile(r'(?i)(?<![\w.])(?:'+ '|'.join(re.escape(n) for n in sorted(lookup,key=len,reverse=True))+r')(?![\w.])')
for p in root.rglob('*'):
 if not p.is_file() or not (p.suffix.lower() in ('.c','.h','.inc','.asm','.cmd','.bat','.lnk','.def','.sed','.rc','.rcv','.mak') or p.name.lower() in ('makefile','sources','system','hpsystem','romsys')):continue
 try:lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for i,line in enumerate(lines):
  for name in {m.group(0).lower() for m in pat.finditer(line)}:
   for n in lookup[name]:
    if p.resolve()==(M/n).resolve():continue
    # Restrict leads to the same immediate functional family, not coincidental names elsewhere.
    family='/'.join(n.split('/')[:2]);q=p.relative_to(M).as_posix()
    if not q.startswith(family+'/'):continue
    r=rows[n];r['basename_match_count']+=1
    if len(r['literal_refs'])<20:r['literal_refs'].append(dict(path=q,line=i+1,text=line.strip()[:180]))
zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);fs=[e for e in z.infolist() if not e.is_dir()]
for n,r in rows.items():
 p=M/n
 if p.suffix.lower() in ('.exe','.dll','.drv','.mod'):
  d=p.read_bytes();r['archive_identical']=[e.filename for e in fs if e.file_size==len(d) and hashlib.sha256(z.read(e)).hexdigest().upper()==r['hashes'][0]];r['archive_named']=[e.filename for e in fs if e.filename.replace('\\','/').split('/')[-1].lower()==p.name.lower()]
print(json.dumps(dict(result='pass',files=list(rows.values()),archive_sha256=sha(zpath),scope='Frozen source identities, family-local literal references and selected binary ZIP exact matches; no runtime, provenance or alternate build inference'),indent=2))
