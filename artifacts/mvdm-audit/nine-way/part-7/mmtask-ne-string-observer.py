from pathlib import Path
import hashlib,json
p=Path('src/mvdm/wow16/mmsystem/MMSYSTEM.DLL'); b=p.read_bytes(); needle=b'mmtask.tsk'; pos=b.lower().find(needle)
print(json.dumps(dict(path=str(p),sha256=hashlib.sha256(b).hexdigest(),needle=needle.decode(),offset=pos,bytes_hex=b[pos:pos+len(needle)].hex() if pos>=0 else None,limits='Raw ASCII presence only; not a validated NE resource parse, invocation, or search-path test.'),indent=2))
