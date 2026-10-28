import csv,hashlib,json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[3]
paths=['wow16/kernel31/debug3/kernstub.exe','wow16/kernel31/kernel.api','wow16/kernel31/m.msg','wow16/kernel31/newexe.h','wow16/kernel31/patch.txt','wow16/kernel31/qgrep.grp','wow16/kernel31/state.rst']
with (root/'artifacts/mvdm-audit/shard-3/file-review.csv').open(encoding='utf-8-sig',newline='') as f: rows={r['relative_path']:r for r in csv.DictReader(f)}
def sha(b):return hashlib.sha256(b).hexdigest().upper()
out=[]
for p in paths:
 r=rows[p];b=(root/'src/mvdm'/p).read_bytes();out.append({'path':p,'sha256':sha(b),'opennt_sha256':r['opennt_sha256'],'opennt45_sha256':r['opennt45_sha256'],'origin':r['upstream_origin']})
s=(root/'src/mvdm/wow16/kernel31/debug3/kernstub.exe').read_bytes(); k=Path('O:/winnt/system32/KRNL386.EXE').read_bytes(); hdr=16*struct.unpack_from('<H',s,8)[0]; ne=struct.unpack_from('<I',k,60)[0]; payload=s[hdr:]; print(json.dumps({'files':out,'stub_header_bytes':hdr,'stub_payload_bytes':len(payload),'stub_payload_offset_in_installed_kernel':k.find(payload),'installed_ne_offset':ne,'installed_krnl386_sha256':sha(k)},indent=2))
