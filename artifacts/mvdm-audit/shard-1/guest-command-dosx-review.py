from pathlib import Path
import csv,hashlib,re,os
base=Path('artifacts/mvdm-audit/shard-1'); repo=Path('src/mvdm');
def read(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
rows=read(base/'file-audit.csv'); guest=read(base/'guest-source-review.csv'); index={r['relative_path']:r for r in rows};paths=[r['relative_path'] for r in guest if r['guest_family'] in ('COMMAND guest source/build','DOSX 486 guest package','DPMI/DOSX guest package')];assert len(paths)==49
hashfile=lambda x:hashlib.sha256(Path(x).read_bytes()).hexdigest().upper()
cmdmirror=repo/'dos/v86/cmd/command/COMMAND.COM'; cmdinstall=Path('O:/winnt/system32/COMMAND.COM'); dxmirror=repo/'dpmi/486/dosx.exe'; dxinstall=Path('O:/winnt/system32/DOSX.EXE')
cmhash,cihash,dxhash,dihash=map(hashfile,(cmdmirror,cmdinstall,dxmirror,dxinstall)); assert cmhash==cihash and dxhash==dihash
cmdlink=(repo/'dos/v86/cmd/command/command.lnk').read_text(encoding='latin1'); cmdmake=(repo/'dos/v86/cmd/command/makefile').read_text(encoding='latin1').splitlines(); dxmake=(repo/'dpmi/makefile').read_text(encoding='latin1').splitlines(); dxlink=(repo/'dpmi/dosxi.lnk').read_text(encoding='latin1')
# The original recipe and retained input establish a build relationship, not provenance of any already-built executable.
output=[]
for path in paths:
 r=index[path]; p=repo/path; current=hashfile(p); assert current==r['current_sha256'],path
 command=path.lower().startswith('dos/v86/cmd/command/'); family='COMMAND' if command else 'DOSX'; binary='O:/winnt/system32/COMMAND.COM' if command else 'O:/winnt/system32/DOSX.EXE'; binaryhash=cmhash if command else dxhash; text=p.read_bytes().decode('latin1') if p.suffix.lower() in ('.asm','.inc','.skl') or p.name.lower()=='makefile' else ''; bop=[]
 for line_no,line in enumerate(text.splitlines(),1):
  if re.search(r'(?i)^\s*(?:CMDSVC\s+SVC_[A-Z0-9_]+|FBOP\s+BOP_DPMI\s*,\s*[A-Z0-9_]+|DPMIBOP\s+[A-Z0-9_]+)',line):bop.append(f'{line_no}: '+line.strip().split(';')[0].strip())
 bop='; '.join(bop[:12]); stem=p.stem.lower(); recipe=[]; link=''; condition=''; lines=cmdmake if command else dxmake
 if p.suffix.lower()=='.obj':
  obj=p.name.lower(); link=('named in original command.lnk' if re.search(r'(?i)(?<![\w])'+re.escape(obj)+r'(?![\w])',cmdlink) else 'not found in original command.lnk') if command else ('named in original dosxi.lnk' if re.search(r'(?i)(?<![\w])'+re.escape(obj)+r'(?![\w])',dxlink) else 'not found in original dosxi.lnk')
  for i,line in enumerate(lines):
   if re.search(r'(?i)\b'+re.escape(obj)+r'\b',line):recipe.append(f'makefile:{i+1}')
 elif p.suffix.lower()=='.asm':
  for i,line in enumerate(lines):
   if re.search(r'(?i)(?:^|[\\/\s:])'+re.escape(p.name)+r'(?=[\s\\,;]|$)',line):recipe.append(f'makefile:{i+1}')
  if not command and path.lower() in ('dpmi/dxfunc.asm','dpmi/dxintr.asm','dpmi/dxmain.asm'):condition='Selected 486 target recipe explicitly assembles 486/'+p.name+'; root-name dependency is not proof of selected assembly input.'
  if not command and path.lower() in ('dpmi/dxvcpi.asm','dpmi/dxvcpibt.asm'):condition='VCPIOBJ definition is under original !if 0 and not in selected OBJS486; individual recipes exist but selected link use is unproved.'
 elif path.lower()=='dpmi/486/dosx.exe':link='mirror and deployed executable independently hash-identical'; condition='Exact installed image identity established; no claim all source inputs were rebuilt into this binary.'
 elif p.suffix.lower()=='.sym':link='makefile invokes mapsym dosx; this symbol-file lineage to deployed DOSX not proven'
 elif p.name.lower()=='makefile':link='original recipe names 486 DOSX object set and dosxi.lnk for x86; not proof a retained image was produced by this invocation'
 else:link='source/include/build metadata; exact linkage and source-to-binary lineage need further evidence'
 if command:
  chain='Family route only: CMDSVC -> BOP_CMD 54h -> nt_bop.c MS_bop_4 -> CmdDispatch; SVC_CMDEXEC -> cmdExec. Individual source site is asserted only when listed.'
  if path.lower()=='dos/v86/cmd/command/rdata.asm': chain='Original rdata.asm SVC_GETINITENVIRONMENT -> BOP_CMD 54h -> MS_bop_4 -> CmdDispatch -> cmdGetInitEnvironment; actual binary attribution remains unproved.'
  elif path.lower()=='dos/v86/cmd/command/tcode.asm': chain='Original tcode.asm SVC_CMDEXEC, EXECCOMSPEC32, RETURNEXITCODE and other listed services -> BOP_CMD 54h -> MS_bop_4 -> CmdDispatch -> matching original cmd* owner; actual binary attribution remains unproved.'
  evidence='docs/etc/evidence/m0-t473-s4-dos-command-xms-lifecycle.md; docs/etc/evidence/m0-t473-s2-dos-command-xms-reconciliation.md';limits='Prior host/guest regressions do not prove this exact source file or all of its functions produced or executed in the installed COMMAND.COM.'
 else:
  chain='Family route only: FBOP/DPMIBOP -> BOP_DPMI 53h -> nt_bop.c MS_bop_3 -> DpmiDispatch; individual source site is asserted only when listed.'
  evidence='docs/etc/evidence/m0-t420-s19-dpmi-guest-package-recovery.md; docs/etc/evidence/m0-t420-s39-dpmi-guest-capability-review.md';limits='S39 reports historic normal/failure/interrupt guest witnesses; source attribution to these retained bytes and every per-file function remains unproved.'
  if path.lower()=='dpmi/486/dxutil.asm': evidence+='; S39 names dxutil.asm protected-mode entry/app-init guest witnesses'
 marker='per-file COMMAND/DOSX evidence: artifacts/mvdm-audit/shard-1/guest-command-dosx-review.csv'
 if marker not in r['evidence']:r['evidence']+='; '+marker
 output.append({'relative_path':path,'source_sha256':current,'classification':r['audit_status'],'guest_family':family,'installed_guest_path':binary,'installed_guest_sha256':binaryhash,'mirror_image_path':str(cmdmirror if command else dxmirror).replace('\\','/'),'mirror_matches_installed':'yes','original_recipe_evidence':'; '.join(recipe[:8]),'original_link_role':link,'selection_caveat':condition,'source_bop_sites':bop,'host_entry_path':chain,'historical_behavior_evidence':evidence,'remaining':limits,'source_to_deployed_binary':('DOSX binary itself matches installed hash' if path.lower()=='dpmi/486/dosx.exe' else 'not established by family hash equality or recipe alone')})
assert len(output)==49
out=base/'guest-command-dosx-review.csv'
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=output[0]);w.writeheader();w.writerows(output)
tmp=base/'file-audit-cmd.tmp'
with tmp.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=rows[0]);w.writeheader();w.writerows(rows)
os.replace(tmp,base/'file-audit.csv')
print('rows',len(output),'BOP-site rows',sum(bool(x['source_bop_sites']) for x in output),'explicit alternate recipes',sum(bool(x['selection_caveat']) for x in output))
