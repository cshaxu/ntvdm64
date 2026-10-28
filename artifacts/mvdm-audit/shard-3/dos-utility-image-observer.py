from pathlib import Path
import hashlib,json,zipfile
roots={'current':Path('src/mvdm/dos/v86/cmd'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/cmd'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86/cmd')}; names=['backup/backup.exe','edlin/EDLIN.EXE','command/command.exc','backup/makefile','edlin/makefile','command/makefile']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'magic':p.read_bytes()[:4].hex()} if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
 out['package_members']=[{'name':i.filename,'size':i.file_size,'sha256':hashlib.sha256(z.read(i)).hexdigest()} for i in z.infolist() if i.filename.lower().endswith(('/backup.exe','/edlin.exe','/command.exc','/command.com'))]
for n in ['BACKUP.EXE','EDLIN.EXE','COMMAND.COM']:
 for dr in ['O:/winnt','O:/winnt/system32']:
  p=Path(dr)/n;out['installed_'+str(p)]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
