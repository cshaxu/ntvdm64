import argparse,ctypes as c,json,pathlib,re,struct
from ctypes import wintypes as w
p=argparse.ArgumentParser();p.add_argument('--pid',type=int,required=True);p.add_argument('--image',required=True);p.add_argument('--map',required=True);p.add_argument('--trace',required=True);p.add_argument('--output',required=True);a=p.parse_args()
k=c.WinDLL('kernel32',use_last_error=True)
class M(c.Structure):_fields_=[('size',w.DWORD),('id',w.DWORD),('pid',w.DWORD),('global_count',w.DWORD),('process_count',w.DWORD),('base',c.c_void_p),('image_size',w.DWORD),('module',c.c_void_p),('name',w.WCHAR*256),('path',w.WCHAR*260)]
k.CreateToolhelp32Snapshot.argtypes=[w.DWORD,w.DWORD];k.CreateToolhelp32Snapshot.restype=c.c_void_p;k.Module32FirstW.argtypes=[c.c_void_p,c.POINTER(M)];k.Module32FirstW.restype=w.BOOL;k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=c.c_void_p;k.ReadProcessMemory.argtypes=[c.c_void_p,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.ReadProcessMemory.restype=w.BOOL;k.CloseHandle.argtypes=[c.c_void_p]
s=k.CreateToolhelp32Snapshot(0x18,a.pid);m=M();m.size=c.sizeof(m)
if s==c.c_void_p(-1).value or not k.Module32FirstW(s,c.byref(m)):raise c.WinError(c.get_last_error())
k.CloseHandle(s)
if pathlib.Path(m.path).resolve()!=pathlib.Path(a.image).resolve():raise ValueError('wrong image')
t=pathlib.Path(a.map).read_text();preferred=int(re.search(r'Preferred load address is\s+([0-9A-Fa-f]+)',t)[1],16)
def sym(n):
 x=re.search(r'^\s*\S+\s+'+re.escape(n)+r'\s+([0-9A-Fa-f]+)\s',t,re.M)
 if not x:raise ValueError(n)
 return m.base+int(x[1],16)-preferred
trace=pathlib.Path(a.trace).read_text(errors='replace').replace('\\r\\n','\n')
faults=re.findall(r'dpmi-fault-stack stage=dispatch saved=([0-9a-fA-F]+):([0-9a-fA-F]+).*?words=([^\n]+)',trace)
if not faults:raise ValueError('no fault frame')
ss,sp,frame=faults[-1];ss=int(ss,16);sp=int(sp,16);words=[int(z,16) for z in frame.split(',')]
if len(words)!=8 or words[2]!=0x271 or words[3]!=0x58ef or not ss&4:raise ValueError('unexpected fault frame')
h=k.OpenProcess(0x410,False,a.pid)
if not h:raise c.WinError(c.get_last_error())
def read(addr,n):
 b=c.create_string_buffer(n);got=c.c_size_t()
 if not k.ReadProcessMemory(h,addr,b,n,c.byref(got)) or got.value!=n:raise c.WinError(c.get_last_error())
 return b.raw
def u32(addr):return struct.unpack('<I',read(addr,4))[0]
try:
 ldt=u32(sym('_Ldt'));low,high=struct.unpack('<II',read(ldt+(ss&0xfff8),8));base=((low>>16)&0xffff)|((high&0xff)<<16)|(high&0xff000000);limit=(low&0xffff)|(high&0xf0000);limit=(limit<<12|0xfff) if high&0x800000 else limit
 backing=u32(sym('_Start_of_M_area'));length=u32(sym('_Length_of_M_area'));cr=struct.unpack('<4I',read(sym('_CCPU_CR'),16))
 def phys(addr,n):
  if addr+n>length:raise ValueError('outside guest RAM')
  return read(backing+addr,n)
 def linear(addr,n):
  out=b''
  while len(out)<n:
   v=addr+len(out); pde=struct.unpack('<I',phys((cr[3]&0xfffff000)+4*(v>>22),4))[0] if cr[0]&0x80000000 else 0
   if cr[0]&0x80000000:
    if not pde&1 or pde&0x80:raise ValueError('bad PDE')
    pte=struct.unpack('<I',phys((pde&0xfffff000)+4*((v>>12)&1023),4))[0]
    if not pte&1:raise ValueError('bad PTE')
    addrp=(pte&0xfffff000)|(v&4095)
   else:addrp=v
   nbytes=min(n-len(out),4096-(v&4095));out+=phys(addrp,nbytes)
  return out
 # Prologue PUSH BP / MOV BP,SP / SUB SP,0e, then 28 callback argument bytes.
 bp=(sp+0x0e+28)&0xffff;start=(bp+0x20)&0xffff
 if start+2>limit:raise ValueError('stack offset outside limit')
 pair=linear(base+start,2); value=struct.unpack('<H',pair)[0]
 result={'pid':a.pid,'image':m.path,'nonAtomic':True,'traceSourceSS':hex(ss),'traceSourceSP':hex(sp),'expectedFaultFrameWords':[hex(x) for x in words],'sourceSSDescriptor':[hex(low),hex(high)],'stackBase':hex(base),'stackLimit':hex(limit),'derivedBP':hex(bp),'derivedBPMethod':'SP + 0x0e local bytes + 28 pushed callback argument bytes, inferred from W31 DDEML disassembly','paiStackOffset':hex(start),'paiWord':hex(value),'limits':'post-fault read; inferred BP assumes exact uninterrupted shown frame and no hidden pushes; stack may have been modified; requires controls'}
 pathlib.Path(a.output).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
finally:k.CloseHandle(h)
