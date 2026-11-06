"""Reconcile current original metadata/profile and released ordinal continuity."""
from pathlib import Path
import argparse,csv,hashlib,io,json,re,runpy,subprocess,sys
sys.dont_write_bytecode=True
parser=argparse.ArgumentParser()
parser.add_argument('--image',type=Path,default=Path('assets/release/ntvdm.exe'))
args=parser.parse_args()
root=Path('build/t478-s32')
def declarations(path):
 out={}
 for line in path.read_text(encoding='utf-8-sig').splitlines():
  line=line.split(';')[0].strip()
  if not line or line.startswith('#') or line=='EXPORTS' or line.startswith('NAME '):continue
  m=re.fullmatch(r'(\w+)(?:\s*=\s*([\w@]+))?(?:\s+@(\d+))?(?:\s+(DATA))?',line)
  assert m,(str(path),line)
  name,target,ordinal,data=m.groups();assert name not in out,name
  out[name]={'target':target or name,'data':bool(data),'ordinal':int(ordinal) if ordinal else None}
 return out
original=declarations(root/'original-cpu40.def')
cached=declarations(Path('build/.ninja/msvc-x86-release/worker/generated/ntvdm-original-cpu40.def'))
selected=declarations(Path('build/.ninja/msvc-x86-release/worker/generated/ntvdm-wow32-provider.def'))
assert original==cached
for name,row in original.items():
 assert name in selected,name
 assert all(row[k]==selected[name][k] for k in ['target','data']),name
assert 'Sim32FlushVDMPointer' in selected and 'Sim32FlushVDMPointer' not in original
contract={r['name']:int(r['ordinal']) for r in csv.DictReader(Path('src/ntvdm-exe/ntvdm-export-ordinals.csv').read_text(encoding='utf-8').splitlines())}
oldtext=subprocess.check_output(['git','show','a8a0aef7e:src/ntvdm-exe/ntvdm-export-ordinals.csv']).decode('utf-8-sig')
old={r['name']:int(r['ordinal']) for r in csv.DictReader(oldtext.splitlines())}
assert all(contract.get(name)==ordinal for name,ordinal in old.items())
h=runpy.run_path('tests/observation/verify-ntvdm-export-ordinals.py');actual=h['read_exports'](args.image)
assert all(actual.get(name)==ordinal for name,ordinal in contract.items())
assert actual['VdmQueryFreeVirtualMemory']==270 and actual['VdmReallocateVirtualMemory']==271
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
template=Path('src/mvdm-host/softpc.new/obj.vdm/ntvdm.src')
original_files=[]
for base in ['O:/repos.external/OpenNT/base/mvdm','O:/repos.external/OpenNT-4.5/nt/private/mvdm']:
 reference=Path(base)/'softpc.new/obj.vdm/ntvdm.src'
 assert sha(reference)=='D47400179C1388B58FB8EAEC8331C6EEA16EE548BBA10E7AFBE588E9549BD0BC'
 assert template.read_bytes()==reference.read_bytes()
 original_files.append(dict(path=str(reference),sha256=sha(reference)))
supplement=Path('src/mvdm-host/softpc.new/obj.vdm/obj/i386/ntvdm.def')
assert sha(supplement)=='98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A'
result=dict(result='pass',original_count=len(original),selected_count=len(selected),pe_count=len(actual),contract_count=len(contract),prior_ordinal_count=len(old),prior_commit='a8a0aef7e',original_declarations=original,retained_extension='Sim32FlushVDMPointer',template_sha256=sha(template),original_files=original_files,template_difference="Restored exact original CRLF bytes; no declaration changes",supplemental_sha256=sha(supplement),image_sha256=sha(args.image),scope='Metadata/profile and ordinal continuity; actual providers and live calls are separate evidence')
print(json.dumps(result,indent=2))
