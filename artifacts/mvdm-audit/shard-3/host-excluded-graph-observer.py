from pathlib import Path
import hashlib,json,re,subprocess
src=Path('src/mvdm/softpc.new/host/src'); origs={'opennt':Path('O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/host/src')};o=Path('build/.ninja/msvc-x86-release/worker/obj');mapfile=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map');cmd=Path('artifacts/mvdm-audit/ccpu-remaining-header-source/commands.stdout.txt');out={'files':{},'objects':{},'map':{},'compile_snapshot':{}}
for n in ['nt_sas.c','nt_sec.c','nt_yoda.c']:
 p=src/n;out['files'][n]={k:hashlib.sha256((r/n).read_bytes()).hexdigest() if (r/n).is_file() else None for k,r in {'current':src,**origs}.items()}
for n in ['host/nt_sas.obj','host/nt_sec.obj','host/nt_yoda.obj','host/nt_det.obj','host/nt_event.obj','host/nt_fulsc.obj','support/main.obj']:
 p=o/n
 if p.exists():
  r=subprocess.run(['objdump','-t',str(p)],capture_output=True,text=True,check=True)
  out['objects'][n]={'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'relevant_symbols':[s.strip() for s in r.stdout.splitlines() if any(w in s for w in ['LoseRegenMemory','RegainRegenMemory','host_set_yoda_ints','host_sas_funcs'])]}
keys=['nt_sas.obj','nt_sec.obj','nt_yoda.obj','LoseRegenMemory','RegainRegenMemory','host_set_yoda_ints','host_sas_funcs'];b=mapfile.read_bytes();out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
b=cmd.read_bytes(); text=b.decode(errors='replace'); lines=[s for s in text.splitlines() if re.search(r'nt_(sas|sec|yoda)\.c\s*$',s,re.I)];out['compile_snapshot']={'sha256':hashlib.sha256(b).hexdigest(),'commands':[{'file':re.search(r'nt_(sas|sec|yoda)\.c\s*$',s,re.I).group(),'defines':re.findall(r'(?:^|\s)/D([^\s]+)',s)} for s in lines]}
print(json.dumps(out,indent=2))
