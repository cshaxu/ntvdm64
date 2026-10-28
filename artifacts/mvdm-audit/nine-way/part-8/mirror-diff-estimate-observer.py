import csv,hashlib,difflib,pathlib,json,collections
root=pathlib.Path('artifacts/mvdm-audit');out=root/'nine-way/part-8';table=root/'frozen-file-reconciliation.csv'; rows=list(csv.DictReader(table.open(encoding='utf-8-sig')))
rows=[r for r in rows if r['opennt_sha256'] and r['current_sha256'] and r['current_sha256'] not in (r['opennt_sha256'],r['opennt45_sha256'])]
bases={'OpenNT':pathlib.Path('O:/repos.external/OpenNT/base/mvdm'),'OpenNT-4.5':pathlib.Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; data=[]
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
for r in rows:
 path=r['relative_path']; edition=r['selected_comparison_edition'];cur=pathlib.Path('src/mvdm')/path;old=bases.get(edition,pathlib.Path('__unresolved__'))/path; live=sha(cur);oldlive=sha(old); expected=r['opennt_sha256'] if edition=='OpenNT' else r['opennt45_sha256'] if edition=='OpenNT-4.5' else ''
 result={'path':path,'edition':edition,'frozen_old_sha256':expected,'frozen_current_sha256':r['current_sha256'],'live_old_sha256':oldlive,'live_current_sha256':live,'frozen_old_lines':r['original_line_count'],'frozen_line_similarity_pct':r['original_line_similarity_pct'],'frozen_method':r['similarity_method'],'recomputed_match_lines':'','recomputed_old_lines':'','recomputed_text_pct':'','identity_status':'','functional_restoration_pct':'not established','functional_evidence_needed':'trace selected provider and lifecycle with version-bound behavior tests'}
 result['identity_status']='match' if expected and oldlive==expected and live==r['current_sha256'] else 'unresolved-or-changed'
 if result['identity_status']=='match':
  a=old.read_bytes();b=cur.read_bytes()
  if b'\0' not in a and b'\0' not in b:
   a=a.decode('latin1').splitlines();b=b.decode('latin1').splitlines();n=sum(q.size for q in difflib.SequenceMatcher(None,a,b,autojunk=True).get_matching_blocks());result.update(recomputed_match_lines=n,recomputed_old_lines=len(a),recomputed_text_pct=round(100*n/len(a),2) if a else '')
 data.append(result)
with (out/'mirror-diff-estimate-review.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(data[0]));w.writeheader();w.writerows(data)
print('rows',len(data),'identity',dict(collections.Counter(x['identity_status'] for x in data)),'computed',sum(x['recomputed_old_lines']!='' for x in data),'percent differs',sum(bool(x['recomputed_text_pct']!='' and x['frozen_line_similarity_pct'] and abs(float(x['frozen_line_similarity_pct'])-x['recomputed_text_pct'])>.01) for x in data))
print('frozen input sha256',sha(table))
