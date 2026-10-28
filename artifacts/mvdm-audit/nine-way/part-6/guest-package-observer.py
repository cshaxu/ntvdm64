import hashlib,json,zipfile
from pathlib import Path
repo=Path(__file__).resolve().parents[4]
paths={'COMM.DRV':'wow16/drivers/comm/comm.drv','KEYBOARD.DRV':'wow16/drivers/keyboard/keyboard.drv','VGA.DRV':'wow16/drivers/display/vga.drv','COMMDLG.DLL':'wow16/commdlg/COMMDLG.DLL','DDEML.DLL':'wow16/ddeml/debug/DDEML.DLL'}
def sha(data):return hashlib.sha256(data).hexdigest().upper()
out=[]
with zipfile.ZipFile(repo/'assets/winnt.zip') as z:
 for name,path in paths.items():
  p=repo/'src/mvdm'/path; member='WINNT/SYSTEM32/'+name
  out.append({'member':member,'zip_sha256':sha(z.read(member)) if member in z.namelist() else None,'mirror_path':path,'mirror_sha256':sha(p.read_bytes()) if p.is_file() else None})
print(json.dumps(out,indent=2))
