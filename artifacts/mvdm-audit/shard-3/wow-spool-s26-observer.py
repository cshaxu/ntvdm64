"""Original/source/object binding for S26; not live printer acceptance."""
import hashlib
import json
import os
from pathlib import Path
import re
import runpy
import subprocess
import struct
import sys

sys.dont_write_bytecode=True
h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
b=Path('build/.ninja/msvc-x86-release/wow32')
root=Path('build/t478-s26')
targets={'wspool':['WG32OpenJob','WG32CloseJob','GetPrn16','Prn32','FreePrn'],
         'wgdi':['WG32GetEnvironment'],
         'wgprnset':['WG32DeviceMode','WG32ExtDeviceMode','WG32DeviceCapabilities','LoadLibraryAndGetProcAddresses']}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def extract(s,name,prefix=False):
    m=re.search(r'^(?:ULONG\s+FASTCALL|BOOL|VOID|WORD|HANDLE)\s+'+name+r'\s*\(',s,re.M)
    assert m,name
    start=s.index('{',m.start())
    boundary=s.index('    if (!(*spoolerapis[',start) if prefix else start
    i=s.index('{',boundary)+1;depth=1
    while depth:
        if s[i]=='{':depth+=1
        elif s[i]=='}':depth-=1
        i+=1
    text=s[start:i]
    text=re.sub(r'^#line[^\n]*','',text,flags=re.M)
    text=re.sub(r'/\*.*?\*/|//[^\n]*','',text,flags=re.S)
    return re.sub(r'\s+','',text)
package=root/'package-final/system32/WOW32.DLL'
image,cached=h['Image'](package),h['Image'](b/'wow32.dll')
assert image.code==cached.code
assert all(image.data[a:z]==cached.data[a:z] for a,z,_ in image.code)
syms=h['symbols']((b/'wow32.dll.map').read_text(encoding='utf-8'))
graph=(b/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
functions=[];provenance=[];paths=[package,b/'wow32.dll.map',b/'build.ninja']
original_close=Path('O:/repos.external/OpenNT/printscan/print/spooler/spoolss/client/change.c')
close_text=original_close.read_text(encoding='cp1252')
start=close_text.index('\nClosePrinter(')
assert close_text.index('FreeSpool(pSpool)',start)<close_text.index('RpcClosePrinter(',start)
provenance.append(dict(path=str(original_close),sha256=sha(original_close)))
for stem,names in targets.items():
    source=Path('src/mvdm-host/wow32')/(stem+'.c')
    before=subprocess.check_output(['git','show','7a96b58ce:'+source.as_posix()]).decode('utf-8')
    current=source.read_text(encoding='utf-8')
    assert source.resolve().as_posix() in graph
    for base in ('O:/repos.external/OpenNT/base/mvdm','O:/repos.external/OpenNT-4.5/nt/private/mvdm'):
        original=Path(base)/'wow32'/(stem+'.c');text=original.read_text(encoding='cp1252')
        for name in names:
            prefix=stem!='wspool' and name!='LoadLibraryAndGetProcAddresses'
            assert extract(before,name,prefix)==extract(text,name,prefix),(stem,name,original)
        provenance.append(dict(path=str(original),sha256=sha(original)))
    obj=h['Coff'](b/'obj'/(stem+'.obj'))
    for name in names:
        matches=[s for s in syms if s['function'] and s['owner']==stem+'.obj' and re.fullmatch(r'[_@]'+name+r'(?:@\d+)?',s['name'])]
        assert len(matches)==1,(stem,name)
        functions.append(h['bind'](image,obj,matches[0]))
    if stem=='wspool':assert extract(current,'WG32CloseJob')==extract(before,'WG32CloseJob')
    if stem=='wgprnset':assert extract(current,'LoadLibraryAndGetProcAddresses')==extract(before,'LoadLibraryAndGetProcAddresses')
    paths.extend([source,b/'obj'/(stem+'.obj')])
fixtures=[]
for label in ('baseline','current','loader-baseline','loader-current'):
    p=root/label
    fixtures.append(dict(label=label,output=(p/'run.txt').read_text(encoding='utf-8'),
        hashes=json.loads((p/'hashes.json').read_text(encoding='utf-8-sig'))))
result=dict(result='pass',functions=functions,original_provenance=provenance,
    fixtures=fixtures,inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],
    close_contract='https://learn.microsoft.com/en-us/windows/win32/printdocs/closeprinter',
    limitations='Extracted bodies/prefixes use isolated providers. Static image binding does not execute a native raw job. CloseJob and shared loader are unchanged.')
provider=Path(os.environ['WINDIR'])/'SysWOW64/winspool.drv'
data=provider.read_bytes();pe=struct.unpack_from('<I',data,60)[0]
count=struct.unpack_from('<H',data,pe+6)[0];opt=pe+24
assert struct.unpack_from('<H',data,opt)[0]==0x10b
size=struct.unpack_from('<H',data,pe+20)[0];sections=[]
for i in range(count):
    v,r,s,o=struct.unpack_from('<IIII',data,opt+size+i*40+8)
    sections.append((r,max(v,s),o))
def offset(rva):
    for r,s,o in sections:
        if r<=rva<r+s:return o+rva-r
    raise ValueError(rva)
e=offset(struct.unpack_from('<I',data,opt+96)[0])
count=struct.unpack_from('<I',data,e+24)[0]
names=offset(struct.unpack_from('<I',data,e+32)[0]);exports=set()
for i in range(count):
    pos=offset(struct.unpack_from('<I',data,names+i*4)[0])
    exports.add(data[pos:data.index(b'\0',pos)].decode('ascii'))
table=Path('src/mvdm-host/wow32/wgprnset.c').read_text(encoding='utf-8').split('DLLENTRYPOINTS  spoolerapis',1)[1].split('};',1)[0]
required=re.findall(r'"([^"]+)"',table)
assert len(required)==15 and all(n in exports for n in required)
result['system_spooler_exports']=dict(path=str(provider),sha256=sha(provider),present=required,
    limitation='Static current-host x86 export availability only; not raw printer execution or missing-export tolerance.')
result['guest_controls']=[]
for name in ('raw-spool-profile','printdlg-default','printdlg-cancel'):
    p=root/('guest-'+name)
    assert (p/'result.exit').read_text(encoding='utf-8-sig').strip()=='0'
    result['guest_controls'].append(dict(case=name,result=(p/'result.txt').read_text(encoding='utf-8-sig'),
        report=json.loads((p/'guest-report.json').read_text(encoding='utf-8-sig')),
        inputs=json.loads((p/'inputs.json').read_text(encoding='utf-8-sig'))))
if (root/'verify-final-Full/timings.json').exists():result['full']=json.loads((root/'verify-final-Full/timings.json').read_text(encoding='utf-8-sig'))
if (root/'publication.txt').exists():result['publication']=(root/'publication.txt').read_text(encoding='utf-8-sig')
print(json.dumps(result,indent=2))
