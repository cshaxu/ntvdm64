import hashlib,json,re,runpy,struct
from pathlib import Path
h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py');B=Path('build/.ninja/msvc-x86-release/wow32');mp=B/'wow32.dll.map';s=h['symbols'](mp.read_text());table=next(x for x in s if x['name']=='_aw32WOW' and x['owner'].lower()=='wowtbl.obj');un=next(x for x in s if x['name']=='_WOW32UnimplementedAPI@4' and x['owner'].lower()=='wow32.obj' and x['function']);im=h['Image'](Path('assets/release/WOW32.DLL'));cached=h['Image'](B/'wow32.dll');assert im.base==cached.base and im.code==cached.code and all(im.data[a:b]==cached.data[a:b] for a,b,_ in im.code);bound=h['bind'](im,h['Coff'](B/'obj/wow32.obj'),un)
seps=[]
for i in range(5000):
 val=struct.unpack('<I',h['data_at'](im,table['va']+4*i,4))[0]
 if val==0xffffffff:
  seps.append(i)
  if len(seps)==7:break
assert len(seps)==7;start=seps[-1]+1;slots=[struct.unpack('<I',h['data_at'](im,table['va']+4*(start+i),4))[0] for i in range(4)];assert slots==[un['va'],un['va'],un['va'],0xffffffff]
tabletext=Path('src/mvdm/wow32/wthtbl2.h').read_text(encoding='latin1');assert re.search(r'UNIMPLEMENTEDAPI\s*,\s*"CLASSFIRST"',tabletext) and re.search(r'UNIMPLEMENTEDAPI\s*,\s*"CLASSNEXT"',tabletext)
source=Path('src/mvdm/wow32/wow32.c').read_text(encoding='latin1');func=source[source.index('ULONG FASTCALL WOW32UnimplementedAPI('):];assert re.search(r'(?s)return FALSE;\s*}',func)
print(json.dumps(dict(result='pass',release_sha256=hashlib.sha256(Path('assets/release/WOW32.DLL').read_bytes()).hexdigest().upper(),map_sha256=hashlib.sha256(mp.read_bytes()).hexdigest().upper(),table=table,prior_module_separators=seps,toolhelp_start_index=start,slots=slots,unimplemented_binding=bound,scope='Published retail table pointer and target body binding, no runtime guest callback or class-list validation'),indent=2))
