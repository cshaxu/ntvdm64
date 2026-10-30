import csv,json,hashlib,pathlib,collections
base=pathlib.Path('artifacts/mvdm-audit'); scope=base/'host-ch-scope'
def rows(path):
 with path.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
inv=rows(scope/'inventory.csv'); diff=rows(base/'mirror-diff-code-retention.csv'); d={x['relative_path']:x for x in diff}; tables=['dos-pending-host-dispositions.csv','write-pending-host-dispositions.csv','ddeml-mmsystem-host-dispositions.csv','ole-pending-host-dispositions.csv','user-toolhelp-regedit-host-dispositions.csv','remaining-guest-pending-host-dispositions.csv']; cross={}
for file in tables:
 for r in rows(scope/file):
  if r['path'] in cross: raise ValueError('duplicate crosswalk '+r['path'])
  cross[r['path']]=(file,r)
normalize={'full':'full','全复通':'full','partial':'partial','部分复通':'partial','excluded':'excluded','整体排除':'excluded'}
output=[]
for r in inv:
 p=r['path']; st=r['ledger_status']; origin=r['shard']; note=r['scope_limit'];
 if st in ('pending','待核验'):
  if p not in cross: raise ValueError('missing disposition '+p)
  file,c=cross[p]; st=c['host_source_status']; origin='host-ch-scope/'+file; note=c['scope_limit']
 else: st=normalize.get(st,st)
 if st not in ('full','partial','excluded'): raise ValueError((p,st))
 delta=d.get(p)
 if delta and st!='partial': raise ValueError(('diff not partial',p,st))
 output.append({'path':p,'source_role_status':st,'scope':r['scope'],'scope_limit':note,'decision_evidence':origin,'mirror_diff':'yes' if delta else 'no','original_code_lines':delta['original_code_lines'] if delta else '', 'matched_code_lines':delta['matched_code_lines'] if delta else '', 'text_retention_percent':delta['text_retention_percent'] if delta else ''})
if len(output)!=len({r['path'] for r in output}):raise ValueError('duplicate inventory')
if sum(r['mirror_diff']=='yes' for r in output)!=sum(1 for k in d if k in {r['path'] for r in output}):raise ValueError('join count')
out=scope/'current-ch-disposition-index.csv'
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(output[0]));w.writeheader();w.writerows(output)
summary={'count':len(output),'source_role_status':dict(collections.Counter(r['source_role_status'] for r in output)),'scope':dict(collections.Counter(r['scope'] for r in output)),'mirror_diff_rows_in_index':sum(r['mirror_diff']=='yes' for r in output),'scoped_guest_exclusion_rows':sum(r['scope']=='guest-target-observed' for r in output),'index_sha256':hashlib.sha256(out.read_bytes()).hexdigest().upper(),'inventory_sha256':hashlib.sha256((scope/'inventory.csv').read_bytes()).hexdigest().upper(),'retention_sha256':hashlib.sha256((base/'mirror-diff-code-retention.csv').read_bytes()).hexdigest().upper(),'limitations':'source-role decisions for recorded selected profiles, not runtime capability; guest-target exclusions are host-source only; retention is textual estimate'}
(scope/'current-ch-disposition-index-summary.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8');print(json.dumps(summary,indent=2))
