"""Apply only the main-reviewed partial integrations and empty file role."""
import collections
import csv
import hashlib
import json
import runpy
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
helpers=runpy.run_path(str(A/'guest-source-role-checkpoint.py'))
rows,sha=helpers['rows'],helpers['sha']
prefix='native-source-next-role-'
base='536087158'
ledger_path=A/'combined-initial-status.csv'
ledger=rows(ledger_path)
by_name={r['relative_path']:r for r in ledger}
proposals=[]
tables=[]
for shard in (1,2,3):
    table=f'shard-{shard}/native-pending-followup-2.csv'
    tables.append(dict(path=table,sha256=sha(A/table)))
    proposals += rows(A/table)
accepted=[r for r in proposals if r['proposal_status'] in ('partial','excluded-file-role-only')]
assert len(proposals)==180 and len(accepted)==48
assert collections.Counter(r['proposal_status'] for r in accepted)=={'partial':47,'excluded-file-role-only':1}
fields=list(rows(A/'native-source-role-dispositions.csv')[0])
dispositions=[]
for r in accepted:
    name=r['relative_path']; old=by_name[name]
    assert old['status']=='pending'
    assert sha(Path('src/mvdm')/name)==r['current_sha256']==r['original_sha256']
    excluded=r['proposal_status']=='excluded-file-role-only'
    status='excluded' if excluded else 'partial'
    extent=('unmeasurable: empty translation unit; no executable-body denominator; Print Screen capability is not excluded'
            if excluded else 'unmeasurable: selected source, declaration, map or bounded fixture does not measure guest execution and failure coverage')
    dispositions.append(dict(relative_path=name,shard=old['shard'],sha256=r['current_sha256'],
        edition='OpenNT',original_path=r['original_path'],prior_status='pending',status=status,
        role='historical-empty-printscreen-source-only' if excluded else 'selected-native-source-partial',
        evidence_table=f"shard-{old['shard']}/native-pending-followup-2.csv",
        original_edge=r['current_route_and_selection'],
        limits=r['evidence_and_limits']+' Remaining contract: '+r['concrete_remaining_gap'],
        original_text_retained_percent='100',estimated_restored_code_percent='',
        restored_estimate_status=extent,followup_stage=r['suggested_followup_owner']))
    old['status']=status

def write_csv(path, content, columns=None):
    columns=columns or list(content[0])
    prior=subprocess.run(['git','show',base+':'+path.as_posix()],capture_output=True)
    terminator='\r\n' if b'\r\n' in prior.stdout.split(b'\n',1)[0]+b'\n' else '\n'
    encoding='utf-8-sig' if prior.stdout.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w',encoding=encoding,newline='') as stream:
        writer=csv.DictWriter(stream,columns,lineterminator=terminator)
        writer.writeheader();writer.writerows(content)

write_csv(A/(prefix+'dispositions.csv'),sorted(dispositions,key=lambda r:r['relative_path']),fields)
specs=[('shard-1/file-audit.csv','audit_status'),('shard-2/file-audit.csv','decision'),('shard-3/file-review.csv','review_class')]
changed={r['relative_path']:r for r in dispositions}
for file,column in specs:
    content=rows(A/file)
    for row in content:
        name=row['relative_path']
        if name not in changed:continue
        proof=changed[name]
        row[column]= {'partial':'部分复通','excluded':'整体排除'}[proof['status']] if column=='audit_status' else proof['status']
        row['estimated_restored_code_pct']=''
        row['restored_estimate_status']=proof['restored_estimate_status']
        if column=='audit_status':
            row['role_or_prior_finding']=proof['role']
            row['evidence']=prefix+'dispositions.csv; '+proof['evidence_table']+'; '+proof['original_edge']
            row['next_required_evidence']=proof['limits']
        elif column=='decision':
            row['decision_basis']=proof['role']+'; source integration, not complete capability acceptance'
            row['reviewed_call_chain']=proof['original_edge']
            row['reviewed_test_evidence']=proof['evidence_table']
            row['review_limits']=proof['limits']
        else:
            row['review_basis']=proof['role']+'; source integration, not complete capability acceptance'
            row['source_owner_chain']=proof['original_edge']
            row['specific_evidence']=proof['evidence_table']
            row['remaining_proof']=proof['limits']
    write_csv(A/file,content)
write_csv(ledger_path,ledger)
summary=json.loads((A/'initial-summary.json').read_text())
summary['counts']=dict(collections.Counter(r['status'] for r in ledger))
summary['note']='Next native checkpoint reviews180 paths:47 partial integrations,1 original empty source file role excluded,132 remain pending. No new full capability or functional percentage is inferred.'
(A/'initial-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
previous=json.loads((A/'native-source-role-observations.json').read_text())
obj=Path('build/.ninja/msvc-x86-release/worker/obj/comms/prt_scrn.obj')
selected_map=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
observation=dict(result='pass',base=base,cohort_base='ecd4a604a',candidate_count=48,
    reviewed_cohort_count=180,remaining_cohort_pending=132,source_tables=tables,
    host_input_observations=previous['host_input_observations'],
    host_graph_observations=previous['host_graph_observations'],
    compiled_empty_object=dict(path=str(obj),sha256=sha(obj)),
    selected_product_map=dict(path=str(selected_map),sha256=sha(selected_map)),
    scope='Main-reviewed source integration and empty file role; no new functional acceptance')
(A/(prefix+'observations.json')).write_text(json.dumps(observation,indent=2)+'\n')
helpers['main'](A/(prefix+'verification.json'),base,prefix,48)
