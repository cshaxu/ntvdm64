from pathlib import Path
import hashlib,json,zipfile
names=['GRAPHICS.EXC','graphics.tag','grctrl.ext','grctrl.str','grinst.ext','grload3.ext','grmsg.equ','grparms.ext','makefile','graphics.lnk']
roots={'current':Path('src/mvdm/dos/v86/cmd/graphics'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/cmd/graphics'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86/cmd/graphics')}
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
 out['package_members']=[{'name':i.filename,'size':i.file_size,'sha256':hashlib.sha256(z.read(i)).hexdigest()} for i in z.infolist() if i.filename.lower().endswith(('/graphics.com','/graphics.pro','/graphics.exc'))]
for n in ['GRAPHICS.COM','GRAPHICS.PRO']:
 p=Path('O:/winnt/system32')/n;out['installed_'+n]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
