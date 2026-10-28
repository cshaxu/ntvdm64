from pathlib import Path
import hashlib,json,struct
root=Path(__file__).resolve().parents[3]
exe=(root/'src/mvdm/dos/v86/cmd/graphics/GRAPHICS.EXC').read_bytes()
com=Path('O:/winnt/system32/GRAPHICS.COM').read_bytes()
hdr=struct.unpack_from('<H',exe,8)[0]*16
payload=exe[hdr:]
def sha(x):return hashlib.sha256(x).hexdigest().upper()
print(json.dumps({'exe_sha256':sha(exe),'exe_bytes':len(exe),'mz_header_bytes':hdr,'payload_bytes':len(payload),'offset_of_com':payload.find(com),'leading_256_all_zero':payload[:256]==bytes(256),'suffix_matches_installed_com':payload[256:]==com,'installed_com_sha256':sha(com)},indent=2))
