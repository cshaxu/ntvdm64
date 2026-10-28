"""Win16 common headers, original identity and textual consumer census."""
from pathlib import Path
import hashlib,json,re
names='commdlg.h drivinit.h io.h stdarg.h stdio.h style.h userproc.h w32sys.h wife.h'.split();base=Path('src/mvdm/wow16/inc');refs=[Path('O:/repos.external/OpenNT/base/mvdm/wow16/inc'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/inc')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();inc={n:[] for n in names}; pat=re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',re.I)
for p in Path('src/mvdm/wow16').rglob('*'):
 if not p.is_file() or p.suffix.lower() not in ('.c','.h','.inc','.cpp'):continue
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for i,line in enumerate(lines,1):
  m=pat.search(line)
  if m:
   n=m.group(1).replace('\\','/').split('/')[-1].lower()
   if n in inc:inc[n].append(f'{p}:{i}')
rows=[]
for n in names:rows.append(dict(name=n,sha256=sha(base/n),reference_hashes=[sha(r/n) if (r/n).is_file() else None for r in refs],direct_basename_includes=inc[n]))
print(json.dumps(dict(files=rows,limitations='Direct basename includes only, not search path resolution, preprocessor selection, Ninja deps, or runtime.'),indent=2))
