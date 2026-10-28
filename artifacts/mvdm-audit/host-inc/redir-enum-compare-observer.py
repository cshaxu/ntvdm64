import hashlib,json,re
from pathlib import Path
paths=[Path('src/mvdm/inc/enumapis.h'),Path('src/mvdm/dos/v86/redir/enumapis.inc')]
a={k.lower():int(v,16) for k,v in re.findall(r'(?m)^\s*#define\s+(\w+)\s+0x([\da-f]+)',paths[0].read_text(),re.I)}
b={k.lower():int(v,16) for k,v in re.findall(r'(?im)^\s*(\w+)\s+equ\s+([\da-f]+)h\s*$',paths[1].read_text())}
r={'inputs':[{'path':str(p).replace('\\','/'),'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper()} for p in paths],
   'c_count':len(a),'asm_count':len(b),'common':len(a.keys()&b.keys()),
   'different':[{'name':k,'c':a[k],'asm':b[k]} for k in sorted(a.keys()&b.keys()) if a[k]!=b[k]],
   'only_c':sorted(a.keys()-b.keys()),'only_asm':sorted(b.keys()-a.keys())}
Path('artifacts/mvdm-audit/host-inc/redir-enum-compare-evidence.json').write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
print(json.dumps(r,indent=2))
