"""Read-only owned worker DIB check: PID, image, SHA, MAP and native layout bound. No UI or input."""
import ctypes as c,ctypes.wintypes as w,json,re,struct,sys,collections,hashlib
from pathlib import Path
pid=int(sys.argv[1]);out=Path(sys.argv[2]);r=Path(sys.argv[3]);expected=Path(sys.argv[4])
k=c.WinDLL('kernel32',use_last_error=True)
k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.ReadProcessMemory.restype=w.BOOL
k.CreateToolhelp32Snapshot.argtypes=[w.DWORD,w.DWORD];k.CreateToolhelp32Snapshot.restype=w.HANDLE
class Module(c.Structure):
 _fields_=[('size',w.DWORD),('id',w.DWORD),('pid',w.DWORD),('globaluse',w.DWORD),('use',w.DWORD),('base',c.c_void_p),('bytes',w.DWORD),('module',w.HMODULE),('name',w.WCHAR*256),('path',w.WCHAR*260)]
k.Module32FirstW.argtypes=[w.HANDLE,c.POINTER(Module)];k.Module32FirstW.restype=w.BOOL
k.CloseHandle.argtypes=[w.HANDLE]
h=k.OpenProcess(0x410,False,pid);assert h
s=k.CreateToolhelp32Snapshot(0x18,pid);m=Module();m.size=c.sizeof(m);assert k.Module32FirstW(s,c.byref(m));base=m.base;k.CloseHandle(s)
assert m.path.lower()==str(expected).lower(),m.path
assert hashlib.sha256(expected.read_bytes()).hexdigest()==(r/'current-worker.sha256').read_text().strip().lower()
layout=json.loads((r/'layout.json').read_text());symbols={}
for line in (r/'current-worker.map').read_text().splitlines():
 z=re.match(r'\s*\S+:\S+\s+(_sc|_PCDisplay|_EGA_GRAPH|_paint_screen|_DAC|_Start_of_M_area)\s+([0-9a-fA-F]+)\s',line)
 if z:symbols[z[1]]=base+int(z[2],16)-0x400000
assert len(symbols)==6
try:
 def read(at,size):
  b=c.create_string_buffer(size);n=c.c_size_t();assert k.ReadProcessMemory(h,at,b,size,c.byref(n)) and n.value==size,(hex(at),size,c.get_last_error());return b.raw
 def u32(at):return struct.unpack('<I',read(at,4))[0]
 sc=symbols['_sc'];info=u32(sc+layout['info']);pixels=u32(sc+layout['pixels']);header=read(info,40)
 _,width,height,planes,depth,compression,imagebytes,_,_,used,_=struct.unpack('<IiiHHIIiiII',header)
 stride=((width*depth+31)//32)*4;assert 0<width<4096 and 0<abs(height)<4096 and depth in (1,4,8,16,24,32)
 raw=read(pixels,stride*abs(height));pass # Write pixels only for the exact logo match below.
 rows=[sum(x!=0 for x in raw[i*stride:(i+1)*stride]) for i in range(abs(height))]
 result=dict(pid=pid,base=hex(base),width=width,height=height,depth=depth,stride=stride,scWidth=u32(sc+layout['width']),scHeight=u32(sc+layout['height']),paint=hex(u32(symbols['_paint_screen'])),nonzero=sum(rows),rowsNonzero=sum(x>0 for x in rows),rowNonzero=rows,topValues=collections.Counter(raw).most_common(12),pc={n:u32(symbols['_PCDisplay']+layout[n]) for n in ('pitch','chain4','seq4')},ega={n:u32(symbols['_EGA_GRAPH']+layout[n]) for n in ('actual_pitch','colour256')})
 bitmap=(r/'LOGO.SYS').read_bytes(); source=bitmap[1078:]; wanted=bytes(p for y in range(399,-1,-1) for p in source[y*320:(y+1)*320] for _ in range(2)); result['logoExact']=width==640 and abs(height)==400 and depth==8 and raw==wanted
 # Source-bound PC_palette is three bytes (gfx_upd.h / nt_mon.h). Observe
 # actual DAC cache as well as unchanged pixels, never a simulated animation.
 palette=read(u32(symbols['_DAC']),768)
 original=bytes(bitmap[54+i*4+j]>>2 for i in range(256) for j in (2,1,0))
 ring=original[236*3:]
 phases=[phase for phase in range(20) if palette[236*3:]==ring[phase*3:]+ring[:phase*3]]
 result['palettePhase']=phases[0] if phases and palette[:236*3]==original[:236*3] else None
 result['paletteSha256']=hashlib.sha256(palette).hexdigest()
 # Read-only real-mode driver state, bound to this build's NASM listing and
 # existing INT 2F segment. This also proves vector retirement after handoff.
 memory=u32(symbols['_Start_of_M_area'])
 # Later DOS/Windows providers legitimately sit at the head of INT 2F.
 # Locate the loaded device by its immutable header, not that chain's head.
 guest=read(memory,0x100000);image=(r/'compat95.sys').read_bytes()
 matches=[at-10 for at in range(10,len(guest)-8) if guest[at:at+8]==b'W95COMP$' and guest[at-6:at+8]==image[4:18] and (at-10)%16==0]
 assert len(matches)==1,matches
 segment=matches[0]//16;driver=memory+matches[0]
 listing=(r/'compat95.lst').read_text();offsets={}
 for label in ('logo_active','logo_timer_hooked','logo_frames','logo_stopped_frames','logo_handoffs'):
  match=re.search(r'^\s*\d+\s+([0-9A-Fa-f]{8})\s+\S+\s+.*?\b'+label+r'\s+(?:db|dw)\b',listing,re.M)
  assert match,label
  offsets[label]=int(match[1],16)
 result['driver']={label:int.from_bytes(read(driver+offset,1 if label in ('logo_active','logo_timer_hooked') else 2),'little') for label,offset in offsets.items()}
 ivt1c=struct.unpack('<HH',read(memory+0x1c*4,4));ivt10=struct.unpack('<HH',read(memory+0x10*4,4))
 result['driver']['timerVectorOwned']=ivt1c[1]==segment
 result['driver']['videoVectorOwned']=ivt10[1]==segment
 if result['logoExact']:out.with_suffix('.dib.bin').write_bytes(raw)
 out.with_suffix('.json').write_text(json.dumps(result,indent=2));print({a:b for a,b in result.items() if a not in ('rowNonzero',)})
finally:k.CloseHandle(h)
