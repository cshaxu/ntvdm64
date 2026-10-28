"""VDD sample family: dual-reference identity and current four-graph deps."""
from pathlib import Path
import hashlib,json,subprocess,zipfile,re
names=['graphics.pro','grcommon.ext','grctrl.ext','grctrl.str','grinst.ext','grload3.ext','grmsg.equ','grparms.ext','grpattrn.str','grshar.str'];root=Path('src/mvdm/dos/v86/cmd/graphics');refs=[Path('O:/repos.external/OpenNT/base/mvdm/dos/v86/cmd/graphics'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86/cmd/graphics'),Path('O:/repos.external/opennt-src-2/nt/private/mvdm/dos/v86/cmd/graphics')];graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();want={str((root/n).resolve()).lower():n for n in names};hits={n:[] for n in names}
for g in graphs:
 out=subprocess.check_output(['ninja','-C',str(g),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
 for line in out.splitlines():
  if not line.startswith(' '):owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
  elif valid:
   p=Path(line.strip());key=str((p if p.is_absolute() else g/p).resolve()).lower()
   if key in want:hits[want[key]].append(dict(graph=str(g),object=owner))
with zipfile.ZipFile('assets/winnt.zip') as z:
 rows=[]
 for n in names:
  p=root/n; matches=[x for x in z.namelist() if Path(x.replace('\\','/')).name.lower()==Path(n).name.lower()]; i=Path('O:/winnt/system32')/Path(n).name
  rows.append(dict(path=n,sha256=sha(p),original_hashes=[sha(r/n) if (r/n).is_file() else None for r in refs],valid_dependencies=hits[n],zip_basename_matches=matches,installed_basename_present=i.is_file()))
for row in rows:
 row['asm_direct_includes']=[f'{q}:{i}' for q in root.glob('*.asm') for i,line in enumerate(q.read_text(encoding='latin1').splitlines(),1) if re.search(r'^\s*include\s+'+re.escape(row['path'])+r'\b',line,re.I)]
 row['make_mentions']=[dict(line=i,text=line.strip()) for i,line in enumerate((root/'makefile').read_text(encoding='latin1').splitlines(),1) if re.search(re.escape(row['path']),line,re.I)]
print(json.dumps(dict(files=rows,graphs=[dict(path=str(g),sha256=sha(g/'build.ninja')) for g in graphs]),indent=2))
