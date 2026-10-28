import csv,hashlib,json,re,struct,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/commdlg/colordlg.h','wow16/commdlg/COMMDLG.DLL','wow16/commdlg/parse.h','wow16/commdlg/privcomd.h','wow16/commdlg/smfldr2.bmp','wow16/commdlg/smfldr3.bmp','wow16/commdlg/STRIPPED.DEF')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 root=M/'wow16/commdlg';dll=(root/'COMMDLG.DLL').read_bytes();zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/commdlg.dll');assert z.read(entry)==dll==Path('O:/winnt/system32/COMMDLG.DLL').read_bytes()
 assert dll[:2]==b'MZ';ne=struct.unpack_from('<I',dll,60)[0];assert dll[ne:ne+2]==b'NE';r=ne+struct.unpack_from('<H',dll,ne+36)[0];shift=struct.unpack_from('<H',dll,r)[0];p=r+2; resources=[]
 while True:
  typ=struct.unpack_from('<H',dll,p)[0];p+=2
  if typ==0:break
  count=struct.unpack_from('<H',dll,p)[0];p+=6
  for i in range(count):
   off,size,flags,ident,handle,usage=struct.unpack_from('<HHHHHH',dll,p);p+=12;off<<=shift;size<<=shift
   if typ==0x8002:resources.append(dict(type=typ,id=ident,offset=off,size=size,sha256=hashlib.sha256(dll[off:off+size]).hexdigest().upper()))
 dib=(root/'filebmps.bmp').read_bytes()[14:];matches=[x for x in resources if dll[x['offset']:x['offset']+len(dib)]==dib];assert matches
 rc=(root/'messages/usa/commdlg.rc').read_text(encoding='latin1').lower();assert 'filebmps.bmp' in rc and all(x not in rc for x in ('smfldr2.bmp','smfldr3.bmp'))
 return dict(result='pass',files=rows,archive_sha256=sha(zpath),dll_sha256=sha(root/'COMMDLG.DLL'),filebmps_sha256=sha(root/'filebmps.bmp'),bitmap_resources=resources,filebmps_dib_matches=matches,scope='NE resource byte match, not bitmap rendering or source-to-image build provenance')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
