from pathlib import Path
import hashlib,json,re,struct
root=Path(__file__).resolve().parents[3]
exe=root/'build/.ninja/msvc-x86-release/worker/ntvdm.exe'
mp=root/'build/.ninja/msvc-x86-release/worker/ntvdm.exe.map'
hdr=root/'src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h'
b=exe.read_bytes(); sha=hashlib.sha256(b).hexdigest().upper()
expected='D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5'
if sha!=expected: raise SystemExit('Executable differs from audited r10; select a fresh matching map and baseline.')
t=mp.read_text(errors='replace'); names=['_cSasPtrs','_c_sas_touch','_c_VirtualiseInstruction','_EDL_fast_bop'];add={}
for name in names:
 matches=re.findall(r'^\s*[0-9A-Fa-f]+:[0-9A-Fa-f]+\s+'+re.escape(name)+r'\s+([0-9A-Fa-f]+)\b',t,re.M)
 if len(matches)!=1: raise SystemExit(f'{name}: expected unique map symbol, got {len(matches)}')
 add[name]=int(matches[0],16)
content=hdr.read_text(errors='replace'); vector=content.split('struct\tSasVector\t{',1)[1].split('};',1)[0]; fields=re.findall(r'\(\*(\w+)\)',vector)
pe=struct.unpack_from('<I',b,0x3c)[0]; n=struct.unpack_from('<H',b,pe+6)[0]; sz=struct.unpack_from('<H',b,pe+20)[0];opt=pe+24
if struct.unpack_from('<H',b,opt)[0]!=0x10b:raise SystemExit('expected PE32 image')
base=struct.unpack_from('<I',b,opt+28)[0]; sections=[]
for i in range(n):
 off=opt+sz+i*40;vs,va,raw,ptr=struct.unpack_from('<IIII',b,off+8);sections.append((va,raw,ptr))
def raw_offset(va):
 rva=va-base
 for v,rs,ptr in sections:
  if v<=rva<v+rs:return ptr+rva-v
 raise ValueError(f'address {va:x} not in raw section')
positions={name:fields.index(name) for name in ('Sas_touch','SasVirtualiseInstruction','IsPageInstanceData')}
values={name:struct.unpack_from('<I',b,raw_offset(add['_cSasPtrs'])+index*4)[0] for name,index in positions.items()}
assert values['Sas_touch']==add['_c_sas_touch']
assert values['SasVirtualiseInstruction']==add['_c_VirtualiseInstruction']
assert values['IsPageInstanceData']==0
assert b[raw_offset(add['_c_sas_touch'])]==0xcc
assert b[raw_offset(add['_c_VirtualiseInstruction'])]==0xcc
report={'ntvdm_sha256':sha,'map_symbols':{k:hex(v) for k,v in add.items()},'fields_count':len(fields),'indices':positions,'vector_values':{k:hex(v) for k,v in values.items()},'result':'STATIC_PE_VECTOR_POINTERS_AND_BREAKPOINT_OPCODE_OK','limitation':'Static PE inspection only; no claim about dynamic invocation, actual relocated addresses or application execution.'}
print(json.dumps(report,indent=2))
