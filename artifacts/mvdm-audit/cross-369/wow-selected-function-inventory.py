import hashlib, json, re, subprocess
from pathlib import Path
repo = Path(__file__).resolve().parents[3]
build = repo/'build/.ninja/msvc-x86-release/wow32'
mapping = (build/'wow32.dll.map').read_text(encoding='latin1')
names = ['wdib','wgdi31','wgprnset','wkgthunk','wmmalias']
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
rows = {}
for n in names:
    src = repo/'src/mvdm/wow32'/(n+'.c')
    obj = build/'obj'/(n+'.obj')
    symbols = subprocess.run(['objdump','-t',str(obj)],capture_output=True,text=True,encoding='utf-8',errors='replace',check=True).stdout
    funcs=[]
    for line in symbols.splitlines():
        m=re.search(r'\(sec\s+([1-9]\d*)\).*\(ty\s+20\).*\(scl\s+2\).*\s(\S+)$',line)
        if m:
            symbol=m.group(2)
            funcs.append({'symbol':symbol,'map_line':next((s.strip() for s in mapping.splitlines() if symbol in s and n+'.obj' in s.lower()),None)})
    rows[n]={'source_sha256':sha(src),'object_sha256':sha(obj),'functions':sorted(funcs,key=lambda x:x['symbol'])}
print(json.dumps({'map_sha256':sha(build/'wow32.dll.map'),'files':rows},indent=2))
