from pathlib import Path
import hashlib,json,zipfile
names=['denmark.msg','finland.msg','italy.msg','portugal.msg','sweden.msg','state.rst','makefile']
roots={'current':Path('src/mvdm/dos/v86/messages'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/messages'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86/messages')}
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
 out['package_names']=[i.filename for i in z.infolist() if i.filename.lower().endswith(tuple('/'+x for x in names[:-1]))]
print(json.dumps(out,indent=2))
