import csv,hashlib,json,struct,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='8' and x['kind']=='mvdm' and x['path'].startswith('wow16/toolhelp/')];assert len(own)==13;f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};rows=[]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
for x in own:
 n=x['path'];hs=[sha(r/n) for r in (M,*O)];v=f[n];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];rows.append(dict(key=x['key'],path=n,hashes=hs))
root=M/'wow16/toolhelp';mk=(root/'makefile').read_text(encoding='latin1').lower();task=(root/'task1.c').read_text(encoding='latin1').lower();th=(root/'ththunks.asm').read_text(encoding='latin1').lower();tab=(M/'wow32/wthtbl2.h').read_text(encoding='latin1').lower();assert all(t in mk for t in ('# class1.obj:','# class2.obj:','task1.obj','ththunks.obj')) and 'taskinfo' in task and 'toolhelpthunk classfirst' in th and 'toolhelpthunk classnext' in th and 'unimplementedapi' in tab and '"classfirst"' in tab and '"classnext"' in tab
zp=Path('assets/winnt.zip');z=zipfile.ZipFile(zp);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/toolhelp.dll');d=z.read(entry);assert d==(root/'TOOLHELP.DLL').read_bytes() and d[:2]==b'MZ';ne=struct.unpack_from('<I',d,60)[0];assert d[ne:ne+2]==b'NE';ptrs=(ne+struct.unpack_from('<H',d,ne+0x26)[0],struct.unpack_from('<I',d,ne+0x2c)[0]);exports={}
for p in ptrs:
 while d[p]:
  n=d[p];p+=1;name=d[p:p+n].decode('latin1');p+=n;ordinal=struct.unpack_from('<H',d,p)[0];p+=2
  if ordinal:exports[name.upper()]=ordinal
assert all(exports.get(k)==v for k,v in {'TASKFIRST':63,'TASKNEXT':64,'CLASSFIRST':69,'CLASSNEXT':70}.items())
print(json.dumps(dict(result='pass',files=rows,ne_exports=exports,archive_sha256=sha(zp),image_sha256=hashlib.sha256(d).hexdigest().upper(),makefile_sha256=sha(root/'makefile'),wow_table_sha256=sha(M/'wow32/wthtbl2.h'),scope='NE names and original source-table behavior; no live class/task or emitted table binding'),indent=2))
