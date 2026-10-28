"""Read-only ROM/CMOS family identity and embedded V7 ROM resource check."""
from pathlib import Path
import ctypes,hashlib,json
names=['cmos.ram','profile.spc','v7vga.rom'];roots=[Path('src/mvdm/softpc.new/roms'),Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/roms'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/roms')];sha=lambda b:hashlib.sha256(b).hexdigest();rows=[]
for n in names:
 rows.append(dict(name=n,identities=[dict(path=str(r/n),sha256=sha((r/n).read_bytes()) if (r/n).is_file() else None) for r in roots]))
k=ctypes.WinDLL('kernel32',use_last_error=True);k.LoadLibraryExW.argtypes=(ctypes.c_wchar_p,ctypes.c_void_p,ctypes.c_uint32);k.LoadLibraryExW.restype=ctypes.c_void_p;k.FindResourceW.argtypes=(ctypes.c_void_p,ctypes.c_wchar_p,ctypes.c_void_p);k.FindResourceW.restype=ctypes.c_void_p;k.SizeofResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.SizeofResource.restype=ctypes.c_uint32;k.LoadResource.argtypes=(ctypes.c_void_p,ctypes.c_void_p);k.LoadResource.restype=ctypes.c_void_p;k.LockResource.argtypes=(ctypes.c_void_p,);k.LockResource.restype=ctypes.c_void_p;k.FreeLibrary.argtypes=(ctypes.c_void_p,);k.FreeLibrary.restype=ctypes.c_int
res=[]
for path in [Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe'),Path('assets/release/ntvdm.exe'),Path('O:/winnt/system32/ntvdm.exe')]:
 if not path.is_file():res.append(dict(path=str(path),status='absent'));continue
 h=k.LoadLibraryExW(str(path.resolve()),None,2)
 if not h:res.append(dict(path=str(path),status='load_failed',error=ctypes.get_last_error()));continue
 try:
  r=k.FindResourceW(h,'SOFTPC_V7VGA',ctypes.c_void_p(10))
  if not r:res.append(dict(path=str(path),status='missing_resource',error=ctypes.get_last_error()));continue
  size=k.SizeofResource(h,r);data=k.LockResource(k.LoadResource(h,r)); assert data
  content=ctypes.string_at(data,size);res.append(dict(path=str(path),image_sha256=sha(path.read_bytes()),resource_size=size,resource_sha256=sha(content),matches_source=content==(roots[0]/'v7vga.rom').read_bytes()))
 finally:k.FreeLibrary(h)
print(json.dumps(dict(files=rows,resources=res),indent=2))
