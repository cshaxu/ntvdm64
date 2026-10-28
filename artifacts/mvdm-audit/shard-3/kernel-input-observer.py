from pathlib import Path
import hashlib,json
names=['debug3/kernstub.exe','kernel.api','m.msg','newexe.h','patch.txt','qgrep.grp','state.rst']
roots={'current':Path('src/mvdm/wow16/kernel31'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/wow16/kernel31'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/kernel31')}
out={}
for n in names:
 out[n]={}
 for label,root in roots.items():
  p=root/n
  out[n][label]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
