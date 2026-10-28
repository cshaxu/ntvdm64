import csv,ctypes,hashlib,json
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')); names=('softpc.new/bios/biostruc.inc','softpc.new/bios/devmark.inc','softpc.new/bios/msgroup.inc','softpc.new/bios/sysimes.asm','softpc.new/data/bios1.rom','softpc.new/data/cmos.ram','softpc.new/roms/bios2.rom','softpc.new/roms/bios4.asm')
def sha(d):return hashlib.sha256(d).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in names:
  hs=[sha((root/n).read_bytes()) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending'; rows.append(dict(path=n,hashes=hs))
 k=ctypes.WinDLL('kernel32',use_last_error=True);k.LoadLibraryExW.argtypes=(ctypes.c_wchar_p,ctypes.c_void_p,ctypes.c_uint32);k.LoadLibraryExW.restype=ctypes.c_void_p;k.FindResourceW.argtypes=(ctypes.c_void_p,ctypes.c_wchar_p,ctypes.c_void_p);k.FindResourceW.restype=ctypes.c_void_p;k.SizeofResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.SizeofResource.restype=ctypes.c_uint32;k.LoadResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.LoadResource.restype=ctypes.c_void_p;k.LockResource.argtypes=(ctypes.c_void_p,);k.LockResource.restype=ctypes.c_void_p;k.FreeLibrary.argtypes=(ctypes.c_void_p,);k.FreeLibrary.restype=ctypes.c_int
 image=Path('assets/release/ntvdm.exe');h=k.LoadLibraryExW(str(image.resolve()),None,2);assert h,ctypes.get_last_error();res=[]
 try:
  for name,src in (('SOFTPC_BIOS1','softpc.new/roms/bios1.rom'),('SOFTPC_BIOS4','softpc.new/roms/bios4.rom')):
   r=k.FindResourceW(h,name,ctypes.c_void_p(10));assert r,ctypes.get_last_error();size=k.SizeofResource(h,r);p=k.LockResource(k.LoadResource(h,r));assert p;d=ctypes.string_at(p,size);assert d==(M/src).read_bytes();res.append(dict(name=name,source=src,size=size,sha256=sha(d)))
 finally:assert k.FreeLibrary(h)
 assert (M/'softpc.new/data/bios1.rom').read_bytes()==(M/'softpc.new/roms/bios1.rom').read_bytes()
 rom=(M/'softpc.new/base/system/rom.c').read_text(encoding='latin1'); cmos=(M/'softpc.new/base/system/cmos.c').read_text(encoding='latin1');assert all(s in rom for s in ('BIOS1ROM_FILENAME','BIOS2ROM_FILENAME','bios4.rom','bios2.rom')) and all(s in cmos for s in ('host_read_resource(CMOS_REZ_ID','host_write_resource(CMOS_REZ_ID','cmos_read_in && cmos_has_changed'))
 return dict(result='pass',sources=rows,release_sha256=sha(image.read_bytes()),resources=res,rom_source_sha256=sha((M/'softpc.new/base/system/rom.c').read_bytes()),cmos_source_sha256=sha((M/'softpc.new/base/system/cmos.c').read_bytes()),scope='Read-only PE resource binding and source path; firmware execution and CMOS persisted path unproven')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
