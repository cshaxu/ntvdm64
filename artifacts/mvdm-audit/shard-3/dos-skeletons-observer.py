from pathlib import Path
import hashlib,json,zipfile
names=['cmd/command/help.skl','cmd/exe2bin/exe2bin.skl','cmd/nlsfunc/nlsfunc.skl','cmd/command/makefile','cmd/exe2bin/makefile','cmd/nlsfunc/makefile','make.inc']
roots={'current':Path('src/mvdm/dos/v86'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86')};out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
 out['package_members']=[{'name':i.filename,'sha256':hashlib.sha256(z.read(i)).hexdigest()} for i in z.infolist() if i.filename.lower().endswith(('/command.com','/exe2bin.exe','/nlsfunc.exe'))]
for n in ['COMMAND.COM','EXE2BIN.EXE','NLSFUNC.EXE']:
 p=Path('O:/winnt/system32')/n;out['installed_'+n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
print(json.dumps(out,indent=2))
