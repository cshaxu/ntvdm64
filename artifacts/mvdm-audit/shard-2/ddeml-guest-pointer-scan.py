import argparse,ctypes as c,hashlib,json,pathlib,re,struct
from ctypes import wintypes as w
p=argparse.ArgumentParser();p.add_argument('--pid',type=int,required=True);p.add_argument('--image',required=True);p.add_argument('--map',required=True);p.add_argument('--output',required=True);a=p.parse_args()
k=c.WinDLL('kernel32',use_last_error=True)
class M(c.Structure):_fields_=[('size',w.DWORD),('id',w.DWORD),('pid',w.DWORD),('global_count',w.DWORD),('process_count',w.DWORD),('base',c.c_void_p),('image_size',w.DWORD),('module',c.c_void_p),('name',w.WCHAR*256),('path',w.WCHAR*260)]
k.CreateToolhelp32Snapshot.argtypes=[w.DWORD,w.DWORD];k.CreateToolhelp32Snapshot.restype=c.c_void_p;k.Module32FirstW.argtypes=[c.c_void_p,c.POINTER(M)];k.Module32FirstW.restype=w.BOOL;k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=c.c_void_p;k.ReadProcessMemory.argtypes=[c.c_void_p,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.ReadProcessMemory.restype=w.BOOL;k.CloseHandle.argtypes=[c.c_void_p]
s=k.CreateToolhelp32Snapshot(0x18,a.pid);m=M();m.size=c.sizeof(m)
if s==c.c_void_p(-1).value or not k.Module32FirstW(s,c.byref(m)):raise c.WinError(c.get_last_error())
k.CloseHandle(s)
if pathlib.Path(m.path).resolve()!=pathlib.Path(a.image).resolve():raise ValueError('unexpected process image')
t=pathlib.Path(a.map).read_text();preferred=int(re.search(r'Preferred load address is\s+([0-9A-Fa-f]+)',t)[1],16)
def sym(n):
 x=re.search(r'^\s*\S+\s+'+re.escape(n)+r'\s+([0-9A-Fa-f]+)\s',t,re.M)
 if not x:raise ValueError(n)
 return m.base+int(x[1],16)-preferred
h=k.OpenProcess(0x410,False,a.pid)
if not h:raise c.WinError(c.get_last_error())
def read(addr,n):
 b=c.create_string_buffer(n);got=c.c_size_t()
 return b.raw if k.ReadProcessMemory(h,addr,b,n,c.byref(got)) and got.value==n else None
try:
 backing=struct.unpack('<I',read(sym('_Start_of_M_area'),4))[0];length=struct.unpack('<I',read(sym('_Length_of_M_area'),4))[0]
 if length>0x10000000:raise ValueError('unexpected guest backing extent')
 needles={'expected_thunk':bytes.fromhex('ba00af17'),'fault_target':bytes.fromhex('05007002')};hits={x:[] for x in needles};unreadable=0
 for pos in range(0,length,4096):
  b=read(backing+pos,min(4096,length-pos))
  if b is None:unreadable+=1;continue
  for name,pat in needles.items():
   start=0
   while len(hits[name])<128:
    off=b.find(pat,start)
    if off<0:break
    hits[name].append({'physical':hex(pos+off),'context':b[max(0,off-4):off+12].hex()});start=off+1
 result={'pid':a.pid,'image':m.path,'nonAtomic':True,'guestBackingLength':length,'unreadablePages':unreadable,'note':'physical raw scan only; matches may be accidental or stale; not a guest logical/DS:BX resolution','hits':hits}
 pathlib.Path(a.output).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({x:len(y) for x,y in hits.items()}))
finally:k.CloseHandle(h)
