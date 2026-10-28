import hashlib,json,subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
roots={'current':repo/'src/mvdm','opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths=['wow32/wmmstru2.c','wow32/wmmstruc.h','wow32/wmmedia2.c','wow32/wmmedia.c','wow16/mmsystem/mci.c','wow16/mmsystem/init.c']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
build=repo/'build/.ninja/msvc-x86-release/wow32'; obj=build/'obj/wmmstru2.obj'
symbols=subprocess.run(['objdump','-t',str(obj)],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True).stdout.splitlines()
deps=subprocess.run(['ninja','-C',str(build),'-t','deps','obj/wmmstru2.obj'],text=True,encoding='utf-8',errors='replace',capture_output=True,check=True).stdout.splitlines()
print(json.dumps({'sha256':{p:{n:sha(root/p) for n,root in roots.items()} for p in paths},'object_sha256':sha(obj),'symbol_table':symbols,'selected_deps':[s.strip().replace('\\','/') for s in deps if any(x in s.lower() for x in ('valid','stale','wmmstruc.h','precomp.h'))]},indent=2))
