"""Non-atomic read-only CCPU40 state, with real guest PDE/PTE translation."""
import argparse, ctypes as c, hashlib, json, pathlib, re, struct
from ctypes import wintypes as w
p=argparse.ArgumentParser()
p.add_argument('--pid',type=int,required=True);p.add_argument('--image',required=True)
p.add_argument('--map',required=True);p.add_argument('--output',required=True)
p.add_argument('--freeze',action='store_true',help='Briefly quiesce the exact diagnostic process; not timing evidence')
a=p.parse_args();output=pathlib.Path(a.output).resolve();repo=pathlib.Path(__file__).resolve().parents[2]
if not output.is_relative_to(repo/'build'):raise ValueError('build-owned output required')
kernel=c.WinDLL('kernel32',use_last_error=True)
class Module(c.Structure):
    _fields_=[('size',w.DWORD),('id',w.DWORD),('pid',w.DWORD),('global_count',w.DWORD),('process_count',w.DWORD),('base',c.c_void_p),('image_size',w.DWORD),('module',c.c_void_p),('name',w.WCHAR*256),('path',w.WCHAR*260)]
kernel.CreateToolhelp32Snapshot.argtypes=[w.DWORD,w.DWORD];kernel.CreateToolhelp32Snapshot.restype=c.c_void_p
kernel.Module32FirstW.argtypes=[c.c_void_p,c.POINTER(Module)];kernel.Module32FirstW.restype=w.BOOL
kernel.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];kernel.OpenProcess.restype=c.c_void_p
kernel.ReadProcessMemory.argtypes=[c.c_void_p,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];kernel.ReadProcessMemory.restype=w.BOOL
kernel.CloseHandle.argtypes=[c.c_void_p];kernel.CloseHandle.restype=w.BOOL
snap=kernel.CreateToolhelp32Snapshot(0x18,a.pid)
if snap == c.c_void_p(-1).value:raise c.WinError(c.get_last_error())
module=Module();module.size=c.sizeof(module)
try:
    if not kernel.Module32FirstW(snap,c.byref(module)):raise c.WinError(c.get_last_error())
finally:kernel.CloseHandle(snap)
if pathlib.Path(module.path).resolve() != pathlib.Path(a.image).resolve():raise ValueError('wrong pinned image')
maps=pathlib.Path(a.map).read_text(errors='strict')
preferred=int(re.search(r'Preferred load address is\s+([0-9A-Fa-f]+)',maps)[1],16)
names=['_CCPU_IP','_CCPU_GR','_CCPU_SR','_CCPU_CR','_CCPU_FLAGS','_CCPU_CPL','_Start_of_M_area','_Length_of_M_area']
addresses={}
for name in names:
    match=re.search(r'^\s*\S+\s+'+re.escape(name)+r'\s+([0-9A-Fa-f]+)\s',maps,re.M)
    if not match:raise ValueError('missing map symbol '+name)
    addresses[name]=module.base+int(match[1],16)-preferred
handle=kernel.OpenProcess(0x410 | (0x800 if a.freeze else 0),False,a.pid)
if not handle:raise c.WinError(c.get_last_error())
suspended=False
ntdll=c.WinDLL('ntdll')
for name in ['NtSuspendProcess','NtResumeProcess']:
    fn=getattr(ntdll,name);fn.argtypes=[c.c_void_p];fn.restype=c.c_long
def read(address,count):
    buf=c.create_string_buffer(count);got=c.c_size_t()
    if not kernel.ReadProcessMemory(handle,address,buf,count,c.byref(got)) or got.value!=count:raise c.WinError(c.get_last_error())
    return buf.raw
try:
    if a.freeze:
        status=ntdll.NtSuspendProcess(handle)
        if status<0:raise OSError(f'NtSuspendProcess failed: {status&0xffffffff:#x}')
        suspended=True
    ip=struct.unpack('<I',read(addresses['_CCPU_IP'],4))[0]
    gr=struct.unpack('<8I',read(addresses['_CCPU_GR'],32));cr=struct.unpack('<4I',read(addresses['_CCPU_CR'],16))
    flags=struct.unpack('<32I',read(addresses['_CCPU_FLAGS'],128))
    cpl=struct.unpack('<I',read(addresses['_CCPU_CPL'],4))[0]
    segments=[struct.unpack('<HH8I',read(addresses['_CCPU_SR']+i*36,36)) for i in range(6)]
    backing=struct.unpack('<I',read(addresses['_Start_of_M_area'],4))[0]
    length=struct.unpack('<I',read(addresses['_Length_of_M_area'],4))[0]
    def physical(address,count):
        if address<0 or address+count>length:raise ValueError('physical memory outside guest RAM')
        return read(backing+address,count)
    def translate(linear):
        if not cr[0]&0x80000000:return linear,{}
        directory=(cr[3]&0xfffff000)+4*(linear>>22)
        pde=struct.unpack('<I',physical(directory,4))[0]
        if not pde&1:raise ValueError('nonpresent PDE')
        if pde&0x80:raise ValueError('large page not in selected 386 profile')
        pte=struct.unpack('<I',physical((pde&0xfffff000)+4*((linear>>12)&1023),4))[0]
        if not pte&1:raise ValueError('nonpresent PTE')
        return (pte&0xfffff000)+(linear&4095),{'pde':hex(pde),'pte':hex(pte)}
    def linear_bytes(linear,count):
        data=b'';walks=[]
        while len(data)<count:
            address=linear+len(data);page,walk=translate(address);n=min(count-len(data),4096-(address&4095))
            data+=physical(page,n);walks.append(walk)
        return data.hex(' '),walks
    code=(segments[1][8]+ip)&0xffffffff
    stack=(segments[2][8]+(gr[4] if segments[2][7] else gr[4]&0xffff))&0xffffffff
    result={'pid':a.pid,'nonAtomic':not suspended,'quiescedDiagnostic':suspended,'image':module.path,'base':hex(module.base),'ip':hex(ip),'cr':[hex(x) for x in cr],
            'cpuFlags':{'if':flags[9],'vm':flags[17],'iopl':flags[12],'cpl':cpl},
            'registers':[hex(x) for x in gr],'segments':[{'selector':hex(s[0]),'big':s[7],'base':hex(s[8]),'limit':hex(s[9])} for s in segments]}
    result['hostTimerDiagnostic']={}
    result['textGeometry']={}
    for name in ['_now_width','_now_height']:
        match=re.search(r'^\s*\S+\s+'+re.escape(name)+r'\s+([0-9A-Fa-f]+)\s',maps,re.M)
        if match:result['textGeometry'][name]=struct.unpack('<i',read(module.base+int(match[1],16)-preferred,4))[0]
    for label in ['physical','currentLinear']:
        try:
            data=physical(0x449,0x3e) if label=='physical' else bytes.fromhex(linear_bytes(0x449,0x3e)[0])
            result['textGeometry'][label]={'biosMode':data[0],
                'biosColumns':struct.unpack_from('<H',data,1)[0],
                'biosRows':data[0x3b]+1,'biosCharHeight':struct.unpack_from('<H',data,0x3c)[0]}
        except (ValueError,OSError) as e:result['textGeometry'][label]={'unavailable':str(e)}
    for name,size in [('_TimerEventUSec',8),('_CurrHeartBeat',8),
                      ('_LastTimeCounterZero',8),('_ticks_blocked',4),
                      ('_timer_int_enabled',4),('_timers',128)]:
        match=re.search(r'^\s*\S+\s+'+re.escape(name)+r'\s+([0-9A-Fa-f]+)\s',maps,re.M)
        if match:
            raw=read(module.base+int(match[1],16)-preferred,size)
            result['hostTimerDiagnostic'][name]=raw.hex(' ')
    # x86 HOST_COM's first four fields are handle/type/rx/dcbValid. Obtain
    # each instance through its original pointer array; do not guess stride.
    match=re.search(r'^\s*\S+\s+_host_com_ptr\s+([0-9A-Fa-f]+)\s',maps,re.M)
    if match:
        pointers=struct.unpack('<4I',read(module.base+int(match[1],16)-preferred,16))
        result['serialAdapters']=[]
        for index,pointer in enumerate(pointers):
            device,kind,rx,dcb=struct.unpack('<4I',read(pointer,16))
            result['serialAdapters'].append({'adapter':index,'type':kind,'rx':rx,'dcbValid':dcb})
    low=physical(0,min(length,0xa0000));marker=b'T436-GUESTLOG-v1'
    found=low.find(marker)
    if found>=0:
        count=struct.unpack_from('<H',low,found+len(marker))[0]
        if count<=4096:
            result['residentGuestLog']={'physical':hex(found),'length':count,
                'text':low[found+len(marker)+2:found+len(marker)+2+count].decode('cp437')}
    for label,address in [('code',code),('stack',stack)]:
        try:result[label]={'linear':hex(address),'bytes':linear_bytes(address,96)}
        except (ValueError,OSError) as e:result[label]={'linear':hex(address),'unavailable':str(e)}
    # Read-only evidence for the reviewed retail VMM timer routine. These
    # addresses are diagnostic inputs, not a runtime fix or stable guest ABI.
    # Preserve the walk and non-atomic warning rather than assuming a coherent
    # timer snapshot from independently read words.
    if cr[0]&0x80000000:
        try:
            raw,walk=linear_bytes(0x800224cc,32)
            result['retailTimerDiagnostic']={'linear':'0x800224cc',
                'words':[hex(x) for x in struct.unpack('<8I',bytes.fromhex(raw))],
                'walks':walk}
        except (ValueError,OSError) as e:
            result['retailTimerDiagnostic']={'unavailable':str(e)}
    result['textMemory']={}
    for mode in ['physical','paged']:
        try:
            if mode=='physical':cells=physical(0xb8000,8000)
            else:cells=bytes.fromhex(linear_bytes(0xb8000,8000)[0])
            lines=[cells[row*160:(row+1)*160:2].decode('cp437').rstrip() for row in range(50)]
            result['textMemory'][mode]=[{'row':i,'text':line} for i,line in enumerate(lines) if line.strip('\x00 ')]
        except (ValueError,OSError) as e:result['textMemory'][mode]={'unavailable':str(e)}
    # Video RAM is a device overlay, not necessarily present in plain SAS RAM
    # or the guest's page table. Inspect the original C-video interleaved planes.
    for name in ['_EGA_planes','_Video_mode','_Currently_emulated_video_mode','_IdleDisabledFromPIF','_IdleNoActivity','_ienabled']:
        match=re.search(r'^\s*\S+\s+'+re.escape(name)+r'\s+([0-9A-Fa-f]+)\s',maps,re.M)
        if not match:continue
        address=module.base+int(match[1],16)-preferred
        raw=read(address,4);result[name]=raw.hex(' ')
        if name=='_EGA_planes':
            pointer=struct.unpack('<I',raw)[0]
            if pointer:
                planes=read(pointer,32000)
                for stride in [4,8]:
                    chars=planes[::stride]
                    lines=[chars[row*80:(row+1)*80].decode('cp437').rstrip() for row in range(50)]
                    result['textMemory']['egaStride'+str(stride)]=[{'row':i,'text':line} for i,line in enumerate(lines) if line.strip('\x00 ')]
    match=re.search(r'^\s*\S+\s+_PCDisplay\s+([0-9A-Fa-f]+)\s',maps,re.M)
    if match:
        address=module.base+int(match[1],16)-preferred
        values=struct.unpack('<10I',read(address,40))
        keys=['modeChangeRequired','bytesPerLine','charsPerLine','charWidth','charHeight','screenStart','screenHeightRaw','screenPointer','screenLength','displayDisabled']
        result['display']=dict(zip(keys,values))
        if values[7] and values[8]:
            raw=read(values[7],min(values[8],16000))
            for stride in [2,4]:
                chars=raw[::stride]
                lines=[chars[row*80:(row+1)*80].decode('cp437').rstrip() for row in range(min(50,len(chars)//80))]
                result['textMemory']['screenPointerStride'+str(stride)]=[{'row':i,'text':line} for i,line in enumerate(lines) if line.strip('\x00 ')]
    # Never keep the target paused while formatting/writing evidence or while
    # stdout backpressure can block. finally still resumes on any read error.
    if suspended:
        status=ntdll.NtResumeProcess(handle)
        if status<0:raise OSError(f'NtResumeProcess failed: {status&0xffffffff:#x}')
        suspended=False
    output.write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2))
finally:
    try:
        if suspended:
            status=ntdll.NtResumeProcess(handle)
            if status<0:raise OSError(f'NtResumeProcess failed: {status&0xffffffff:#x}')
    finally:kernel.CloseHandle(handle)
