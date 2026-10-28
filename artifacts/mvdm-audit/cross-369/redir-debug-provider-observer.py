import hashlib, json, subprocess
from pathlib import Path
repo = Path(__file__).resolve().parents[3]
roots = {'current': repo/'src/mvdm', 'opennt': Path('O:/repos.external/OpenNT/base/mvdm'), 'opennt45': Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
files = ['vdmredir/vrdebug.c', 'vdmredir/vrdlcdbg.c']
build = repo/'build/.ninja/msvc-x86-release/worker'
def digest(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None
records = {}
for f in files:
    obj = build/'obj/redir'/(Path(f).stem+'.obj')
    cmd = subprocess.run(['ninja','-C',str(build),'-t','commands','obj/redir/'+obj.name],capture_output=True,text=True,encoding='utf-8',errors='replace',check=True).stdout
    syms = subprocess.run(['objdump','-t',str(obj)],capture_output=True,text=True,encoding='utf-8',errors='replace',check=True).stdout
    records[f] = {'sha256': {n:digest(r/f) for n,r in roots.items()}, 'object_sha256':digest(obj), 'command_defines_DBG': '/DDBG' in cmd, 'command_defines_PROD': '/DPROD' in cmd, 'symbol_lines': [s.strip() for s in syms.splitlines() if s.lstrip().startswith('[')]}
print(json.dumps(records,indent=2))
