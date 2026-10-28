import pathlib,hashlib,json
files=['dpmi/dxhpbios.asm','dpmi/dxboot.asm','dpmi/486/dxboot.asm','dpmi/dosxi.lnk','dpmi/makefile']; roots=['src/mvdm','O:/repos.external/OpenNT/base/mvdm','O:/repos.external/OpenNT-4.5/nt/private/mvdm']; rows=[]
for f in files:
 row={'path':f}
 for root in roots:
  p=pathlib.Path(root)/f;row[root]=hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
 rows.append(row)
pathlib.Path('artifacts/mvdm-audit/nine-way/part-2/dosx-hp-bios-evidence.json').write_text(json.dumps(rows,indent=2),encoding='utf8')
