from pathlib import Path
import hashlib,json,struct
root=Path(__file__).resolve().parents[3]
exe=root/'src/mvdm/dos/v86/dev/country/country.exe'
sys=root/'src/mvdm/dos/v86/dev/country/COUNTRY.SYS'
installed=Path('O:/winnt/system32/COUNTRY.SYS')
def sha(b): return hashlib.sha256(b).hexdigest().upper()
b=exe.read_bytes(); assert b[:2]==b'MZ'; header=struct.unpack_from('<H',b,8)[0]*16; payload=b[header:]; s=sys.read_bytes(); i=installed.read_bytes();
r={'exe_sha256':sha(b),'exe_bytes':len(b),'mz_header_bytes':header,'payload_sha256':sha(payload),'payload_bytes':len(payload),'mirror_sys_sha256':sha(s),'installed_sys_sha256':sha(i),'payload_equals_mirror_sys':payload==s,'payload_equals_installed_sys':payload==i}
print(json.dumps(r,indent=2))
