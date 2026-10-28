import csv,hashlib,json,re
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));owned=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='2' and x['kind']=='mvdm' and x['path'].startswith(('dos/v86/h/','dos/v86/inc/'))];assert len(owned)==39
f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};lookup={};rows={}
for x in owned:
 name=Path(x['path']).name.lower();lookup.setdefault(name,[]).append(x['path']);vals=[hashlib.sha256((r/x['path']).read_bytes()).hexdigest().upper() for r in (M,*O)];assert vals==[f[x['path']]['current_sha256'],f[x['path']]['opennt_sha256'],f[x['path']]['opennt45_sha256']];rows[x['path']]=dict(key=x['key'],path=x['path'],hashes=vals,matching_basename_references=[],match_count=0)
pat=re.compile(r'(?i)(?<![\w.])(?:'+ '|'.join(re.escape(n) for n in sorted(lookup,key=len,reverse=True))+r')(?![\w.])'); base=M/'dos/v86';num=0
for p in base.rglob('*'):
 if not p.is_file() or not (p.suffix.lower() in ('.c','.h','.inc','.asm','.sa','.cmd','.bat','.lnk','.def','.sed') or p.name.lower() in ('makefile','sources')):continue
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 num+=1
 for i,line in enumerate(lines):
  for name in {m.group(0).lower() for m in pat.finditer(line)}:
   for target in lookup[name]:
    if p.resolve()==(M/target).resolve():continue
    row=rows[target];row['match_count']+=1
    if len(row['matching_basename_references'])<20:row['matching_basename_references'].append(dict(path=p.relative_to(M).as_posix(),line=i+1,text=line.strip()[:180]))
print(json.dumps(dict(result='pass',files=list(rows.values()),scanned_files=num,scope='Basename matches are leads, not resolved includes, active preprocessing or exhaustive alternate build evidence'),indent=2))
