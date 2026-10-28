import csv,ctypes,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));own=[x for x in csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='5' and x['kind']=='mvdm' and x['path'].startswith(('softpc.new/bios/','softpc.new/data/','softpc.new/roms/'))];assert len(own)==33;f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
root=M/'softpc.new/bios';mk=(root/'makefile').read_text(encoding='latin1');lnk=(root/'msbio.lnk').read_text(encoding='latin1');rows=[]
for x in own:
 n=x['path'];v=f[n];hs=[sha(r/n) for r in (M,*O)];assert hs==[v['current_sha256'],v['opennt_sha256'],v['opennt45_sha256']];name=Path(n).name;refs=[]
 for kind,t in [('makefile',mk),('msbio.lnk',lnk)]:
  for i,line in enumerate(t.splitlines()):
   if re.search(r'(?i)(?<![\w.])'+re.escape(name)+r'(?![\w.])',line):refs.append(kind+':'+str(i+1)+':'+line.strip())
 rows.append(dict(key=x['key'],path=n,hashes=hs,recipe_references=refs))
k=ctypes.WinDLL('kernel32',use_last_error=True);k.LoadLibraryExW.argtypes=(ctypes.c_wchar_p,ctypes.c_void_p,ctypes.c_uint32);k.LoadLibraryExW.restype=ctypes.c_void_p;k.FindResourceW.argtypes=(ctypes.c_void_p,ctypes.c_wchar_p,ctypes.c_void_p);k.FindResourceW.restype=ctypes.c_void_p;k.SizeofResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.SizeofResource.restype=ctypes.c_uint32;k.LoadResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.LoadResource.restype=ctypes.c_void_p;k.LockResource.argtypes=(ctypes.c_void_p,);k.LockResource.restype=ctypes.c_void_p;k.FreeLibrary.argtypes=(ctypes.c_void_p,);k.FreeLibrary.restype=ctypes.c_int
image=Path('assets/release/ntvdm.exe');h=k.LoadLibraryExW(str(image.resolve()),None,2);assert h;res=[]
try:
 for resource,name in (('SOFTPC_BIOS1','bios1.rom'),('SOFTPC_BIOS4','bios4.rom'),('SOFTPC_V7VGA','v7vga.rom')):
  r=k.FindResourceW(h,resource,ctypes.c_void_p(10));assert r;size=k.SizeofResource(h,r);ptr=k.LockResource(k.LoadResource(h,r));assert ptr;d=ctypes.string_at(ptr,size);p=M/'softpc.new/roms'/name;q=M/'softpc.new/data'/name;assert d==p.read_bytes()==q.read_bytes();res.append(dict(name=resource,bytes=size,sha256=hashlib.sha256(d).hexdigest().upper(),rom_source=str(p),data_source=str(q)))
finally:assert k.FreeLibrary(h)
for name in ('cmos.ram','profile.spc'):assert (M/'softpc.new/data'/name).read_bytes()==(M/'softpc.new/roms'/name).read_bytes()
zp=Path('assets/winnt.zip');z=zipfile.ZipFile(zp);entry=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/ntio.sys');ntio=z.read(entry);assert ntio==Path('O:/winnt/system32/NTIO.SYS').read_bytes()
print(json.dumps(dict(result='pass',files=rows,resources=res,release_sha256=sha(image),archive_sha256=sha(zp),ntio_sha256=hashlib.sha256(ntio).hexdigest().upper(),scope='Owned BIOS source identity, original recipe and released resource bytes; no runtime/guest build or NTIO source lineage'),indent=2))
