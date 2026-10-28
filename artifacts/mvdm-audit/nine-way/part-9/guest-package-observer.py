import hashlib,json,zipfile
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
paths={'WINNT/SYSTEM32/WINSPOOL.EXE':'wow16/winspool/WINSPOOL.EXE','WINNT/WRITE.EXE':'wow16/write/WRITE.EXE','WINNT/SYSTEM32/WOWDEB.EXE':'wow16/wowdeb/WOWDEB.EXE'}
def sha(data):return hashlib.sha256(data).hexdigest().upper()
with zipfile.ZipFile(repo/'assets/winnt.zip') as z:
 out=[{'member':member,'zip_sha256':sha(z.read(member)) if member in z.namelist() else None,'mirror_path':path,'mirror_sha256':sha((repo/'src/mvdm'/path).read_bytes()) if (repo/'src/mvdm'/path).is_file() else None} for member,path in paths.items()]
print(json.dumps(out,indent=2))
