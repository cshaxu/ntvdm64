import pathlib,hashlib,json
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-2');files=['dpmi/smartdrv.inc','dpmi/dxboot.asm','dpmi/486/dxboot.asm','dpmi/dxstrt.asm','dpmi/486/dxstrt.asm','dpmi/makefile'];out=[]
for q in files:
 rec={'path':q}
 for k,b in [('current','src/mvdm'),('opennt','O:/repos.external/OpenNT/base/mvdm'),('opennt45','O:/repos.external/OpenNT-4.5/nt/private/mvdm')]:
  f=pathlib.Path(b)/q;rec[k]=hashlib.sha256(f.read_bytes()).hexdigest().upper() if f.is_file() else ''
 out.append(rec)
(p/'smartdrv-guest-abi-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
print(out)
