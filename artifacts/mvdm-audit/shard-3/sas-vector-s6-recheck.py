# Bounded static recheck for T478 S6; run from the dedicated worktree root.
# Uses existing MSVC outputs; does not build or change a product.
from pathlib import Path
import hashlib,json,subprocess,re,struct,time
root=Path.cwd(); out=root/'build/t478-s6'; worker=root/'build/.ninja/msvc-x86-release/worker'
out.mkdir(parents=True,exist_ok=True)
dump=Path('C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x86/dumpbin.exe')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def run(args):
 p=subprocess.run([str(x) for x in args],capture_output=True,text=True,errors='replace',timeout=180)
 if p.returncode:raise RuntimeError(p.stderr+p.stdout[-2000:])
 return p.stdout
probe=(root/'artifacts/mvdm-audit/shard-3/sas-vector-pe-probe.py').read_text()
probe=probe.replace("root=Path(__file__).resolve().parents[3]","root=Path.cwd()")
probe=probe.replace("D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5",sha(worker/'ntvdm.exe'))
(out/'current-pe-probe.py').write_text(probe)
p=run(['python',out/'current-pe-probe.py']);(out/'pe-vector.json').write_text(p)
report=json.loads(p)
libs=list(worker.glob('*.lib'))+list((root/'build/.ninja/msvc-x86-release/wow32').glob('*.lib'))
refs=[]
for lib in libs:
 t=run([dump,'/symbols',lib]);hits=[l.strip() for l in t.splitlines() if re.search(r'\b_?(?:c_sas_touch|c_VirtualiseInstruction|IsPageInstanceData)\b',l)]
 if hits:refs.append({'library':str(lib.relative_to(root)),'sha256':sha(lib),'symbols':hits})
(out/'symbol-references.json').write_text(json.dumps({'libraries_scanned':len(libs),'references':refs},indent=2))
direct=[]
for folder,target in [('worker','ntvdm.exe'),('wow32','WOW32.dll')]:
 p=root/'build/.ninja/msvc-x86-release'/folder
 line=next(l for l in (p/'build.ninja').read_text().splitlines() if l.lower().startswith('build '+target.lower()+' ') or l.lower().startswith('build '+target.lower()+':'))
 for token in line.split():
  if not token.endswith('.obj'):continue
  obj=p/token;t=run([dump,'/symbols',obj])
  direct.append({'object':str(obj.relative_to(root)),'hits':[l.strip() for l in t.splitlines() if re.search(r'\b_?(?:c_sas_touch|c_VirtualiseInstruction|IsPageInstanceData)\b',l)]})
(out/'direct-object-symbols.json').write_text(json.dumps(direct,indent=2))
t=run([dump,'/relocations',worker/'obj/ccpu/sasCdef.obj']);(out/'initializer-relocations.txt').write_text(t)
t=run([dump,'/disasm',worker/'ntvdm.exe']);(out/'ntvdm-disassembly.txt').write_text(t)
maptext=(worker/'ntvdm.exe.map').read_text();symbols={}
for name in ['_Sas','_Cpu','_cSasPtrs','_c_sas_touch','_c_VirtualiseInstruction']:
 matches=re.findall(r'^\s*[0-9A-Fa-f]+:[0-9A-Fa-f]+\s+'+name+r'\s+([0-9A-Fa-f]+)\b',maptext,re.M)
 assert len(matches)==1,(name,matches)
 symbols[name]=int(matches[0],16)
tails=[symbols['_Sas']+n for n in [0xd8,0xe0,0xe4]]
addrs=[symbols['_c_sas_touch'],symbols['_c_VirtualiseInstruction']]+tails
patterns=[f'{a:08X}' for a in addrs]
lines=[l.strip() for l in t.splitlines() if any(x.lower() in l.lower().split(':',1)[-1] for x in patterns) or re.search(r'\bcall\b.*\+(?:0D8|0E0|0E4)h\]',l,re.I)]
(out/'disassembly-candidates.json').write_text(json.dumps({'symbols':{k:hex(v) for k,v in symbols.items()},'candidate_lines':lines,'limitation':'Textual disassembly filter, not proof against computed or two-stage indirect calls.'},indent=2))
exports={}
for name in ['ntvdm.exe','WOW32.DLL','VDMREDIR.DLL']:
 f=root/'assets/release'/name
 txt=run([dump,'/exports',f]);exports[name]=[l for l in txt.splitlines() if re.search(r'\b(?:Cpu|Sas|c_sas_touch|c_VirtualiseInstruction|IsPageInstanceData)\b',l)]
(out/'exports.json').write_text(json.dumps(exports,indent=2))
def sections(p):
 b=p.read_bytes();pe=struct.unpack_from('<I',b,60)[0];n=struct.unpack_from('<H',b,pe+6)[0];sz=struct.unpack_from('<H',b,pe+20)[0];res={}
 for i in range(n):
  o=pe+24+sz+i*40;name=b[o:o+8].split(b'\0')[0].decode();vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8);res[name]=(va,rs,hashlib.sha256(b[rp:rp+rs]).hexdigest())
 return res
raw=sections(worker/'ntvdm.exe');release=sections(root/'assets/release/ntvdm.exe')
for name in ['.text','.data','.reloc']:assert raw[name]==release[name],name
manifest=json.loads((root/'build/t478-s4/package-final/product-system32-manifest.json').read_text(encoding='utf-8-sig'))
for row in manifest:assert sha(root/'assets/release'/row['Name'])==row['Sha256'].upper(),row['Name']
(out/'identity.json').write_text(json.dumps({'raw_sha256':sha(worker/'ntvdm.exe'),'map_sha256':sha(worker/'ntvdm.exe.map'),'release_sha256':sha(root/'assets/release/ntvdm.exe'),'identical_sections':{k:raw[k] for k in ['.text','.data','.reloc']},'package_id':manifest[0]['PackageId'],'release_images_verified':len(manifest)},indent=2))
print('PASS current PE, symbols, relocations, exports, disassembly observations and ten-image release identity; no dynamic coverage claim')
