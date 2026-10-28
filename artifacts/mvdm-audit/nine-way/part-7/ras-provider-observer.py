from pathlib import Path
import hashlib,json,subprocess,zipfile
p=Path('C:/Windows/SysWOW64/rasapi32.dll'); guest=Path('src/mvdm/wow16/rasapi16/RASAPI16.DLL')
sha=lambda b:hashlib.sha256(b).hexdigest()
text=subprocess.check_output(['objdump','-p',str(p)],text=True,errors='replace')
names=['RasDialWow','RasEnumConnectionsWow','RasEnumEntriesWow','RasGetConnectStatusWow','RasGetErrorStringWow','RasHangUpWow']
with zipfile.ZipFile('assets/winnt.zip') as z:
 n=next(n for n in z.namelist() if n.replace('\\','/').upper()=='WINNT/SYSTEM32/RASAPI16.DLL'); zg=sha(z.read(n))
print(json.dumps(dict(host=str(p),host_sha256=sha(p.read_bytes()),expected={x: any(line.rstrip().endswith(' '+x) for line in text.splitlines()) for x in names},native_rasdial=[line.strip() for line in text.splitlines() if 'RasDialA' in line or 'RasDialW' in line],guest=str(guest),guest_sha256=sha(guest.read_bytes()),zip_guest=n,zip_guest_sha256=zg,limitations='Host-specific PE export table and NE identity only; no Win16 invocation or source-to-binary proof.'),indent=2))
