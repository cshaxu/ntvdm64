import csv,hashlib,json,difflib,re
from pathlib import Path
from collections import Counter
repo=Path.cwd(); root=repo/'artifacts/mvdm-audit'; src=root/'frozen-file-reconciliation.csv'; raw=src.read_bytes(); manifest=json.loads((root/'manifest.json').read_text()); assert hashlib.sha256(raw).hexdigest().upper()==manifest['source_sha256']
orig=Path('O:/repos.external/OpenNT/base/mvdm'); alt=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'); current=repo/'src/mvdm'; out=root/'shard-1'; out.mkdir(exist_ok=True)
def h(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
def prior(path):
 p=path.lower()
 if p.startswith('bde/'): return 'S1 debugger-engine profile; separate debugger admission'
 if p.startswith('fax/'): return 'S1 optional Fax profile; distinct DLL and consumer absent'
 if p.startswith('ieuvddex/'): return 'S1 external IE VDD extension; consumer and contract not selected'
 if p.startswith(('sim16/','sim32/')): return 'S1 retired simulator profile, selected CCPU40 remains sole executor'
 if p.startswith('vdd/samples/'): return 'S1 example VDD; standalone example not selected product provider'
 if p.startswith('vdmexts/'): return 'S1 debugger extension profile, separately owned boundary'
 if p.startswith('vdmutils/'): return 'S1 independent original utility, not worker-link input'
 if p.startswith('softpc.new/bios/'): return 'S1 firmware/build input, not host-runtime source'
 if p.startswith('softpc.new/rename/'): return 'S1 generator/converter, not host runtime'
 if p.startswith('softpc.new/host/src/nt_'): return 'S1 says absent from original i386 manifest; review actual original sources'
 if p=='softpc.new/base/system/cmos.c': return 'S1 identifies alternate CMOS carrier; verify original profile and equivalent selected CMOSNT'
 if p.startswith('wow32/'): return 'S1 marks dormant/diagnostic WOW profile; complete caller/dependency audit required'
 return ''
with src.open(encoding='utf-8-sig',newline='') as f: rows=[r for r in csv.DictReader(f) if int.from_bytes(hashlib.sha256(r['relative_path'].encode()).digest()[:8],'big')%4==1]
assert len(rows)==1182
records=[]; mismatches=[]
for r in rows:
 rel=r['relative_path']; cur=current/rel; a=orig/rel; b=alt/rel; current_hash=h(cur)
 if current_hash!=r['current_sha256']: mismatches.append((rel,r['current_sha256'],current_hash))
 if h(a)!=r['opennt_sha256'] or h(b)!=r['opennt45_sha256']: mismatches.append((rel,'upstream-identity','changed'))
 cl=r['reporting_class']; status='待核验'; detail=''; reason=''; next_step=''; diff_file=''; divs=''; est=''
 if cl=='excluded-cpu30':
  status='整体排除'; detail='CPU30 kernel VDM executor profile, not selected CPU40 product'; reason='docs/etc/operations/policy/source-policy.md; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md'; next_step='Re-admit only on approved alternate executor/profile.'
 elif cl=='partial-mirror-diff':
  status='部分复通'; reason='Frozen original/current hash mismatch; mirror-diff rule'; next_step='Review semantic hunks, selected edition, runtime callers and exception/acceptance; matching lines are not behavioral proof.'
  selected=a if r['selected_comparison_edition']=='OpenNT' else b if r['selected_comparison_edition']=='OpenNT-4.5' else None
  if selected and selected.exists() and cur.exists():
   old=selected.read_bytes(); new=cur.read_bytes();
   if b'\0' not in old and b'\0' not in new:
    oldlines=old.decode('latin1').splitlines(); newlines=new.decode('latin1').splitlines()
    matcher=difflib.SequenceMatcher(None,oldlines,newlines,autojunk=True)
    matched=sum(x.size for x in matcher.get_matching_blocks()); est=f'{matched}/{len(oldlines)} lines ({100*matched/len(oldlines):.2f}%)' if oldlines else 'empty original'
    ids=sorted(set(re.findall(rb'MVDM-[A-Z]+-DIV-\d+',new))); divs=';'.join(x.decode() for x in ids)
    changes=[op for op in matcher.get_opcodes() if op[0]!='equal']; diff_file=f'{len(changes)} changed line blocks; recompute from recorded hashes and paths'
 else:
  role=r['audit_disposition']; reason='docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md; frozen inventory'
  detail=prior(rel) if role=='unselected-host-source-requires-s2-disposition' else role
  if role=='immutable-guest-or-guest-build-input': next_step='Guest/media retained immutable and excluded as host link input; need distinct guest execution evidence before claiming restored functionality.'
  elif role=='selected-product-source': next_step='Product selection is recorded; independently verify link extraction, original call dispatch and behavioral/lifecycle acceptance.'
  elif role=='unselected-host-source-requires-s2-disposition': next_step='Recheck prior profile disposition against original manifest, dependencies and owner authorization; do not equate no selection with proof of no capability.'
  else: next_step='Determine exact role/provenance and whether original provider or independent tool/firmware/guest resource; verify connection if required.'
 records.append({**r,'shard':'1','audit_status':status,'role_or_prior_finding':detail,'evidence':reason,'next_required_evidence':next_step,'source_line_similarity':est,'estimate_caveat':'matching original lines only; not a restored behavior or executable-code percentage' if est else '', 'local_divergence_markers':divs,'diff_evidence':diff_file,'identity_rechecked':'yes' if current_hash==r['current_sha256'] else 'NO'})
fields=list(records[0]);
with (out/'file-audit.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=fields); w.writeheader(); w.writerows(records)
summary={'source_sha256':manifest['source_sha256'],'shard':1,'files':len(records),'status':dict(Counter(x['audit_status'] for x in records)),'identity_mismatches':mismatches,'diff_sources':sum(bool(x['diff_evidence']) for x in records),'classification_limitation':'Most rows need per-package connection and behavioral evidence; guest non-link status does not imply guest capability is restored.'}
(out/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps(summary,ensure_ascii=False,indent=2))
