from pathlib import Path
import hashlib,json,subprocess
p=Path('src/mvdm/wow16/mmsystem/MMSYSTEM.DLL');b=p.read_bytes(); off=19630; data=b[off:off+153]
r=subprocess.run(['ndisasm','-b','16','-o','0xcce','-'],input=data,stdout=subprocess.PIPE,check=True)
print(json.dumps(dict(sha256=hashlib.sha256(b).hexdigest(),file_offset=off,segment_offset='4:0cce',disassembly=r.stdout.decode('ascii'),limitations='Disassembly is pre-relocation; identify far calls using NE fixups before asserting binding; no guest invocation.'),indent=2))
