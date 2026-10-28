import hashlib,pathlib,json,re
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8');r=pathlib.Path('.');paths=['src/mvdm/dpmi32/vxd.c','O:/repos.external/OpenNT/base/mvdm/dpmi32/vxd.c','O:/repos.external/OpenNT-4.5/nt/private/mvdm/dpmi32/vxd.c','src/mvdm/dpmi32/dpmi32.c','src/mvdm/dpmi/486/dxfunc.asm','tests/observation/dpmi_vcd_service.asm','docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md','build/.ninja/msvc-x86-formal-release/build.ninja'];out=[]
for s in paths:
 z=pathlib.Path(s);data=z.read_bytes();out.append({'path':s,'sha256':hashlib.sha256(data).hexdigest().upper(),'bytes':len(data)})
assert out[1]['sha256']==out[2]['sha256']; assert pathlib.Path(paths[0]).read_bytes().splitlines()==pathlib.Path(paths[1]).read_bytes().splitlines()
(p/'dpmi-vcd-quality-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8');print('\n'.join(str(o) for o in out))
