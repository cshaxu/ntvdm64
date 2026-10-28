from pathlib import Path
import hashlib,json
names=['init.asm','rdata.asm','stub.asm'];a=Path('src/mvdm/dos/v86/cmd/command');b=Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/cmd/command');rows=[]
for n in names:
 x=(a/n).read_bytes(); y=(b/n).read_bytes();rows.append(dict(path=str(a/n),original=str(b/n),current_sha256=hashlib.sha256(x).hexdigest(),original_sha256=hashlib.sha256(y).hexdigest(),byte_equal=x==y,splitlines_equal=x.splitlines()==y.splitlines(),current_crlf=x.count(b'\r\n'),original_crlf=y.count(b'\r\n'),current_len=len(x),original_len=len(y)))
print(json.dumps(dict(rows=rows,limits='Line comparison does not prove source-to-binary identity or functionality; raw mirror differences require partial.'),indent=2))
