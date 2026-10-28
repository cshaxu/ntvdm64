from pathlib import Path
import hashlib,json,zipfile
names=['money.bas','msherc.com','qbasic.exe','makefile','qbasic.hlp','edit.com']
roots={'current':Path('src/mvdm/dos/v86/cmd/qbasic'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/cmd/qbasic'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86/cmd/qbasic')};out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
 out['package_members']=[{'name':i.filename,'size':i.file_size,'sha256':hashlib.sha256(z.read(i)).hexdigest()} for i in z.infolist() if i.filename.lower().endswith(tuple('/'+x for x in names[:-3]))]
for n in names[:-3]:
 p=Path('O:/winnt/system32')/n;out['installed_'+n]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
p=Path('O:/winnt/QBASIC.EXE');out['installed_root_qbasic']={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
