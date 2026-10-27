"""Apply only original declaration slices actually retained by the selected decoder."""
import collections
import csv
import json
import runpy
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit');base='9ca2db194';prefix='selected-ccpu-header-role-'
helpers=runpy.run_path(str(A/'guest-source-role-checkpoint.py'))
rows,sha=helpers['rows'],helpers['sha']
runpy.run_path(str(A/'ccpu-selected-header-extractor.py'))['main']()
selection=rows(A/'ccpu-selected-header-observations.csv')
accepted=[r for r in selection if int(r['selected_nonblank_lines'])>0]
assert len(selection)==96 and len(accepted)==71
ledger=rows(A/'combined-initial-status.csv');by_name={r['relative_path']:r for r in ledger}
proofs=[]
for row in accepted:
    name=row['relative_path'];old=by_name[name]
    assert old['status']=='pending'
    original=None;edition=None
    for label,root in [('OpenNT',Path('O:/repos.external/OpenNT/base/mvdm')),('OpenNT-4.5',Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))]:
        candidate=root/name
        if candidate.is_file() and sha(candidate)==row['source_sha256']:
            original=candidate;edition=label;break
    assert original is not None,name
    proof=dict(relative_path=name,shard=old['shard'],sha256=row['source_sha256'],edition=edition,
        original_path=str(original),prior_status='pending',status='partial',role='selected-native-source-partial',
        evidence_table='ccpu-selected-header-observations.csv',
        original_edge='Original '+Path(name).name+' -> selected CPU40/SPC386/PROD c_main.c preprocessed declaration slice; '+row['selected_excerpt'],
        limits=row['remaining_contract']+'; '+Path(name).stem+' caller/provider signature, operand/state and applicable fault contract remain S11',
        original_text_retained_percent='100',estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declaration slice has no independent executed-body denominator; preprocessing is not ABI/behavior acceptance',
        followup_stage='S11')
    proofs.append(proof);old['status']='partial'

def write_csv(path,content):
    raw=subprocess.run(['git','show',base+':'+path.as_posix()],capture_output=True).stdout
    terminator='\r\n' if b'\r\n' in raw.split(b'\n',1)[0]+b'\n' else '\n'
    encoding='utf-8-sig' if raw.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w',encoding=encoding,newline='') as stream:
        writer=csv.DictWriter(stream,list(content[0]),lineterminator=terminator);writer.writeheader();writer.writerows(content)

write_csv(A/(prefix+'dispositions.csv'),proofs)
changed={r['relative_path']:r for r in proofs}
for file,column in [('shard-0/review.csv','classification'),('shard-1/file-audit.csv','audit_status'),('shard-2/file-audit.csv','decision'),('shard-3/file-review.csv','review_class')]:
    content=rows(A/file)
    for row in content:
        if row['relative_path'] not in changed:continue
        proof=changed[row['relative_path']]
        row[column]='部分复通' if column in ('classification','audit_status') else 'partial'
        row['estimated_restored_code_pct']=''
        if 'restored_estimate_status' in row:row['restored_estimate_status']=proof['restored_estimate_status']
        for field in ('finding','role_or_prior_finding','decision_basis','review_basis'):
            if field in row:row[field]='Compiler-selected original declaration integration; '+proof['original_edge']
        for field in ('evidence','reviewed_test_evidence','specific_evidence'):
            if field in row:row[field]=prefix+'dispositions.csv; ccpu-selected-header-observations.csv'
        for field in ('remaining_evidence','next_required_evidence','review_limits','remaining_proof'):
            if field in row:row[field]=proof['limits']
    write_csv(A/file,content)
write_csv(A/'combined-initial-status.csv',ledger)
summary=json.loads((A/'initial-summary.json').read_text())
summary['counts']=dict(collections.Counter(r['status'] for r in ledger))
summary['note']='71 original CPU declarations classified partial from actual selected /P output;25 header roles remain pending. No code/guest/binary change or new functional acceptance.'
(A/'initial-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
prior=json.loads((A/'historical-build-role-observations.json').read_text())
pp=json.loads((A/'ccpu-selected-header-observations.json').read_text())
cache=json.loads(Path('build/t473-s10/ccpu-selected-preprocessing/normal-cache-inputs.json').read_text(encoding='utf-8-sig'))
configuration=['artifacts/mvdm-audit/ccpu-selected-header-extractor.py','build/.ninja/msvc-x86-release/worker/build.ninja','src/mvdm/softpc.new/base/ccpu386/c_main.c','build/t473-s10/ccpu-selected-preprocessing/original-command.txt','build/t473-s10/ccpu-selected-preprocessing/preprocess-command.txt']
observation=dict(result='pass',base=base,candidate_count=71,reviewed_cohort_count=96,remaining_cohort_pending=25,
    source_tables=[dict(path='ccpu-selected-header-observations.csv',sha256=sha(A/'ccpu-selected-header-observations.csv'))],
    preprocessor_output=pp['preprocessor_output'],normal_cache_inputs=cache,
    configuration_review_inputs=[dict(path=p,sha256=sha(Path(p))) for p in configuration],
    host_input_observations=prior['host_input_observations'],scope='Compiler-selected declaration integration only; no new functional acceptance')
(A/(prefix+'observations.json')).write_text(json.dumps(observation,indent=2)+'\n')
helpers['main'](A/(prefix+'verification.json'),base,prefix,71)
