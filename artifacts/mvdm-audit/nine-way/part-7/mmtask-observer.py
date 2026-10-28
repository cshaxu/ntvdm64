from pathlib import Path
import hashlib,json,zipfile
paths=['src/mvdm/wow16/mmsystem/mmtask.tsk','src/mvdm/wow16/mmsystem/mmtask/MMTASK.TSK','src/mvdm/wow16/mmsystem/MMSYSTEM.DLL','assets/release/mmtask.tsk','O:/winnt/system32/mmtask.tsk','O:/winnt/system32/MMSYSTEM.DLL']
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
with zipfile.ZipFile('assets/winnt.zip') as z:
  entries=[dict(name=i,sha256=hashlib.sha256(z.read(i)).hexdigest()) for i in z.namelist() if Path(i.replace('\\','/')).name.lower() in ('mmtask.tsk','mmsystem.dll')]
print(json.dumps(dict(paths=[dict(path=p,sha256=sha(Path(p))) for p in paths],zip_entries=entries,limits='Named paths and ZIP only, no complete executable search path or guest call; source-to-image provenance not proved.'),indent=2))
