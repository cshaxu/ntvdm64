import hashlib,json,runpy
from pathlib import Path
A=Path('artifacts/mvdm-audit');h=runpy.run_path(str(A/'host-tail-image-observer.py'));B=Path('build/.ninja/msvc-x86-release/worker');image=h['Image'](Path('assets/release/ntvdm.exe'));cached=h['Image'](B/'ntvdm.exe');assert image.base==cached.base and image.code==cached.code and all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code);syms=h['symbols']((B/'ntvdm.exe.map').read_text());obj=h['Coff'](B/'obj/system/cmosnt.obj');bindings=[]
for n in ('_cmos_pickup','_cmos_update','_cmos_init','_cmos_post'):
 sym=next(s for s in syms if s['name']==n and s['owner'].lower().endswith(':cmosnt.obj') and s['function']);bindings.append(h['bind'](image,obj,sym))
src=Path('src/mvdm/softpc.new/base/system/cmosnt.c').read_text(encoding='latin1');assert 'Static init plus post is used instead of external files' in src and 'GLOBAL void cmos_update IFN0()\n{\n    ; /* do nothing */' in src
assert 'src/mvdm/softpc.new/base/system/cmosnt.c' in (B/'build.ninja').read_text().replace('$:',':'); assert 'RtcIntEoiHook' in src and 'io_connect_port' in src
print(json.dumps(dict(result='pass',bindings=bindings,release_sha256=hashlib.sha256(Path('assets/release/ntvdm.exe').read_bytes()).hexdigest().upper(),source_sha256=hashlib.sha256(Path('src/mvdm/softpc.new/base/system/cmosnt.c').read_bytes()).hexdigest().upper(),scope='Selected original cmosnt exported bodies and source; no real guest or asynchronous RTC behavior'),indent=2))
