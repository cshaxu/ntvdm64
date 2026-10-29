from pathlib import Path
import csv,re,hashlib,collections
root=Path('src/mvdm'); src=Path('artifacts/mvdm-audit/s10-followup-wow16-recipe-roles.csv'); rows=[x for x in csv.DictReader(src.open(encoding='utf8',newline='')) if x['role']=='unresolved']
paths={str((root/x['path']).resolve()).casefold():x['path'] for x in rows}; exact=collections.defaultdict(list); basename=collections.defaultdict(list)
# Only local quoted includes can be resolved without configured search paths.
for p in Path('src').rglob('*'):
 if not p.is_file() or p.suffix.lower() not in ('.c','.h'):continue
 try: lines=p.read_text(encoding='latin1').splitlines()
 except OSError:continue
 for i,line in enumerate(lines,1):
  m=re.match(r'^\s*#\s*include\s*(["<])([^">]+)[">]',line)
  if not m:continue
  inc=m.group(2).replace('\\','/'); k=Path(inc).name.casefold(); loc=f'{p.as_posix()}:{i}: {line.strip()}'
  basename[k].append(loc)
  if m.group(1)=='"':
   resolved=str((p.parent/inc).resolve()).casefold()
   if resolved in paths:exact[paths[resolved]].append(loc)
stage={'commdlg':'S15','ctl3dv2':'S15','drivers':'S13','gdi':'S16','kernel31':'S14','mciole':'S16','mmsystem':'S16','ole':'S16','rasapi16':'S17','sherlock':'S17','test':'S17','toolhelp':'S17','user':'S15','wfwnet':'S17','win87em':'S16','write':'S15/S16'}
inc_stage={'cderr.h':'S15','cmstrip.c':'S17','commdlg.h':'S15','cpl.h':'S16','custcntl.h':'S15','dbcs.c':'S12/S14','dbcs.h':'S12/S14','dde.h':'S14','ddeml.h':'S14','debug.h':'S17','dlgs.h':'S15','drivinit.h':'S16','ime.h':'S15','io.h':'S12/S14','logerror.h':'S14/S16','lzdos.h':'S12/S16','lzexpand.h':'S16','memory.h':'S12/S14','mmddk.h':'S16','mmsystem.h':'S16','multires.h':'S16','ole.h':'S16','penwin.h':'S15','print.h':'S16','printer.h':'S16','shellapi.h':'S14','spl_wnt.h':'S16/S17','spool.h':'S16','stdarg.h':'S14','stdio.h':'S12/S14','stdlib.h':'S12/S14','string.h':'S12/S14','strtable.h':'S16','style.h':'S15','testing.h':'S17','time.h':'S12/S14','userproc.h':'S15','ver.h':'S16','version.h':'S17','w32sys.h':'S14/S16','wfwnet.h':'S17','wife.h':'S16/S17','windows.h':'S14/S15/S16','windowsx.h':'S15','winexp.h':'S15','winnet.h':'S17','winnls.h':'S15','wmsyserr.h':'S15'}
output=[]
for x in rows:
 rel=x['path']; p=root/rel; body=p.read_text(encoding='latin1'); lines=body.splitlines(); hints=[]
 for i,l in enumerate(lines[:100],1):
  t=l.strip().strip('/* \t');
  if len(t)<12 or len(t)>180 or re.search(r'copyright|microsoft|history:|created|version:|author:|revision|provided "as is"|warranty|^#|^-+$',t,re.I):continue
  if re.search(r'(?i)(\bheader\b|\bfunctions?\b|\bdefinitions?\b|\bdescrip|\bmetafile|\bwin16\b|\bwin32\b|\bedit control\b|\bcommunication\b|\bprinter\b|\bresource|\btest|\bwindows\b|\bdriver\b|\bapi\b|\bexample\b|\bstring|\bprofile\b|\bfile\b|\bcommand|\berror|\bobsolete|\bcolou?r|\bexport|\bsubclass|\btype|\blibrary|\bversion|\bmacro|\bconversion)',t):hints.append(f'{i}: {t}');
  if len(hints)>=2:break
 if not hints:
  for i,l in enumerate(lines,1):
   t=l.strip()
   if re.match(r'(?i)(?:#\s*define|typedef|(?:extern\s+)?(?:void|int|word|bool|handle|long|dword|char|static)\b)',t):
    hints.append(f'{i}: {t[:180]}');break
 group=rel.split('/')[1]; handoff=inc_stage.get(p.name.lower(),'S14/S15/S16') if group=='inc' else stage.get(group,'S17')
 host=[v for v in basename[p.name.lower()] if '/wow16/' not in v.replace('\\','/').lower() and '/dos/v86/' not in v.replace('\\','/').lower()]
 own=exact.get(rel,[]); role='source role/consumer unresolved'
 if rel in {'wow16/gdi/gdi16.h','wow16/gdi/meta.c','wow16/gdi/metarec.c','wow16/gdi/metasup.c'}:role='historical dual Win16/Win32 metafile source design; actual build consumers not yet established'
 elif rel=='wow16/inc/cmstrip.c':role='source comment describes assembler text-stripping utility; target and current use unproven'
 elif rel in {'wow16/inc/dbcs.c','wow16/inc/dbcs.h'}:role='source comment describes DOS-app DBCS implementation/interface; current users unproven'
 elif own:role='local quoted include witness; exact build profile and other consumers unresolved'
 elif hints:role='source-described role only; build/consumer unresolved'
 output.append({'path':rel,'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'bounded_role':role,'source_role_evidence':' | '.join(hints),'local_quoted_consumer_count':len(own),'local_quoted_consumers':' | '.join(own[:12]),'host_same_basename_count':len(host),'host_same_basename_includes':' | '.join(host[:8]),'prior_host_collision_notes':x['host_collision_or_limit'],'functional_handoff':handoff,'limit':'Lexical evidence only; host basename can resolve another header; local include may be conditional; no current execution or guest-binary claim.'})
out=Path('artifacts/mvdm-audit/s10-followup-wow16-unresolved-trace.csv')
with out.open('w',encoding='utf8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=output[0].keys());w.writeheader();w.writerows(output)
print('rows',len(output),'local quoted witness',sum(bool(x['local_quoted_consumers']) for x in output),'host basename collisions',sum(x['host_same_basename_count']>0 for x in output),'source description',sum(bool(x['source_role_evidence']) for x in output))
