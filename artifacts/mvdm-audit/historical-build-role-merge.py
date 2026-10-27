"""Record the source-reviewed remaining historical build recipes, not capabilities."""
import collections
import csv
import json
import re
import runpy
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
helpers=runpy.run_path(str(A/'guest-source-role-checkpoint.py'))
rows,sha=helpers['rows'],helpers['sha']
base='35454cc49';prefix='historical-build-role-'
ledger_path=A/'combined-initial-status.csv'
ledger=rows(ledger_path)
frozen={r['relative_path']:r for r in rows(A/'frozen-file-reconciliation.csv')}
batch=[r for r in ledger if r['status']=='pending' and re.fullmatch(r'(sources|dirs|makefil\w*(\..*)?|_objects\.mac)',Path(r['relative_path']).name,re.I)]
assert len(batch)==36

special={
'bin86/makefile':('Original binplace list for immutable runtime payload/configuration files','Current audited package/ZIP staging owns delivery, not this NTMAKEENV recipe; every listed image remains independently reconciled','S17'),
'dos/v86/cmd/command/makefil0':('Builds COUNTRY/buildmsg.exe from buildmsg.c with cl16/forcedos link','No guest/tool rebuild; COMMAND source, locale messages and immutable executable lineage remain independently reviewed','S12'),
'inc/makefile':('Original h2inc conversion rules and USEBUILD trampoline for shared guest declarations','Current selected host ABI/declarations and immutable guest inputs are separately tracked; generated .inc layout and consumers are not accepted or excluded by this recipe decision','S11'),
'makefil0':('Original NT orchestration of inc, DPMI, DOS, WOW16, sample TSR/scanner and bin86; clean also binplaces ROM/profile inputs','Current CMake/Ninja and explicit package staging own their admitted outputs; all named guest/source/firmware capabilities remain in scope','S17'),
'softpc.new/base/unix/sources':('Original unix_emm.c library manifest with NT include/profile flags','T310 explicitly selects admitted component manifests; this Unix manifest is not parsed; unix_emm.c behavior remains independently reviewed','S12'),
'softpc.new/rename/sources':('Independent rename.c console build tool manifest','No selected runtime or configuration consumer of this manifest; rename.c tool behavior is independently tracked','S17'),
'vdd/samples/adlibvdd/sources':('Independent Ad Lib VDD sample DLL manifest and original NTVDM import-library contract','Current VDD loader/service and sample vdd.c remain independent capabilities; no sound or plugin functionality is excluded','S17'),
'vdmutils/graftabl/sources':('Original GRAFTABL native utility build/resource/profile manifest','Current text/font services and graftabl.c/resource behavior remain independently in scope; the old NT make metadata is not the provider','S13'),
'vdmutils/win/sources':('Original WIN.COM UMAPPL_NOLIB launcher build/resource/entry manifest','Run16/WOW launch and the original win.c utility are independently tracked; no launcher behavior is accepted by this file-role decision','S12'),
'ieuvddex/mips/sources':('MIPS debugger-extension architecture source-list fragment','Selected modern x86/x64 configuration does not parse it; debugger commands/context and architecture source files retain separate decisions','S17'),
}

proofs=[]
for row in batch:
    name=row['relative_path'];path=Path('src/mvdm')/name
    current=sha(path);f=frozen[name]
    original=None;edition=None
    for origin,root in [('OpenNT',Path('O:/repos.external/OpenNT/base/mvdm')),('OpenNT-4.5',Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')),('OpenNT-src-2',Path('O:/repos.external/opennt-src-2/nt/private/mvdm'))]:
        candidate=root/name
        if candidate.is_file() and sha(candidate)==current:
            original=candidate;edition=origin;break
    assert original is not None and current==f['current_sha256'],name
    text=path.read_text(encoding='utf-8-sig',errors='strict')
    leaf=path.name.lower()
    if name in special:contract,boundary,stage=special[name]
    elif leaf=='_objects.mac':
        assert '_OBJECTS=' in text
        contract='Generated architecture-specific object/resource lists; '+name.rsplit('/obj/',1)[0]
        boundary='Current manifests/generators enumerate actual objects and resources; all named providers retain independent source and behavior obligations'
        stage='S17'
    elif leaf=='dirs':
        assert 'DIRS=' in text
        contract='Historical NT component recursion/optional-directory list; '+name
        boundary='CMake and selected generators own current traversal; child packages/capabilities are not excluded by this directory-control decision'
        stage='S17'
    elif 'wow16/ddeml/tests/' in name:
        assert 'Test makefile' in text and 'link ' in text
        contract='Original Win16 DDE stress/demo client or server compiler/resource/link recipe; '+name
        boundary='No original guest/test binary is rebuilt; DDE behavior, original test sources and current acceptance remain separate obligations'
        stage='S15'
    elif 'NTMAKEENV' in text and 'makefile.def' in text:
        contract='Historical shared NT build-system trampoline; '+name
        boundary='Current CMake/Ninja invokes no NTMAKEENV makefile.def from this path; original listed component sources remain independently audited'
        stage='S17'
    elif name in ('vdd/samples/hpscan/makefile','vdd/samples/sample1/fax16/makefile','vdd/samples/mscdex/tsr/makefile'):
        assert 'link16' in text
        contract='Original MASM/link16 guest scanner/fax stub or MSCDEX TSR build; '+name
        boundary='Immutable guest/sample source, binaries and their VDD contracts remain separate; no scanner, fax or CD-ROM capability is excluded'
        stage='S17'
    else:raise AssertionError('Unreviewed recipe role: '+name)
    proofs.append(dict(relative_path=name,shard=row['shard'],sha256=current,edition=edition,
        original_path=str(original),prior_status='pending',status='excluded',role='historical-build-recipe-file-only',
        evidence_table=prefix+'review.md',original_edge=contract,
        limits=boundary+'; file-only build-role exclusion, not runtime capability completion or exclusion',
        original_text_retained_percent='100',estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical declarative recipe, not independently executed product behavior',followup_stage=stage))
    row['status']='excluded'

def write_csv(path,content):
    prior=subprocess.run(['git','show',base+':'+path.as_posix()],capture_output=True).stdout
    end='\r\n' if b'\r\n' in prior.split(b'\n',1)[0]+b'\n' else '\n'
    encoding='utf-8-sig' if prior.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w',encoding=encoding,newline='') as stream:
        writer=csv.DictWriter(stream,list(content[0]),lineterminator=end);writer.writeheader();writer.writerows(content)

write_csv(A/(prefix+'dispositions.csv'),sorted(proofs,key=lambda r:r['relative_path']))
changed={r['relative_path']:r for r in proofs}
specs=[('shard-0/review.csv','classification'),('shard-1/file-audit.csv','audit_status'),('shard-2/file-audit.csv','decision'),('shard-3/file-review.csv','review_class')]
for file,column in specs:
    content=rows(A/file)
    for row in content:
        if row['relative_path'] not in changed:continue
        proof=changed[row['relative_path']]
        row[column]='整体排除' if column in ('classification','audit_status') else 'excluded'
        row['estimated_restored_code_pct']=''
        if 'restored_estimate_status' in row:row['restored_estimate_status']=proof['restored_estimate_status']
        # Preserve each shard's existing schema: only explicit existing evidence fields change.
        for field in ('next_required_evidence','review_limits','remaining_proof','remaining_evidence'):
            if field in row:row[field]=proof['limits']
        for field in ('role_or_prior_finding','decision_basis','review_basis','finding'):
            if field in row:row[field]=proof['role']+'; '+proof['original_edge']
        for field in ('evidence','reviewed_test_evidence','specific_evidence'):
            if field in row:row[field]=prefix+'dispositions.csv; '+prefix+'review.md'
    write_csv(A/file,content)
write_csv(ledger_path,ledger)
summary=json.loads((A/'initial-summary.json').read_text())
summary['counts']=dict(collections.Counter(r['status'] for r in ledger))
summary['note']='36 remaining original build recipes classified by file role only; all source/provider capabilities retain their separate obligations. No runtime or guest change.'
(A/'initial-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
prior=json.loads((A/'native-source-next-role-observations.json').read_text())
configuration=['CMakeLists.txt','tools/build/New-T310OriginalSoftpcNinja.ps1','tools/build/New-T404S5Wow32ProviderNinja.ps1','tools/build/New-NativeWorkerNinja.ps1','tools/build/New-NativeHook64Ninja.ps1']
manifests=['softpc.new/base/'+name+'/sources' for name in ('ccpu386','bios','keymouse','system','disks','support','video','cvidc','comms','dos')]
manifests += [name+'/sources' for name in ('dos/dem','dos/command','vdmredir','xms.486','dpmi32','suballoc','oemuni','softpc.new/host/src','wow32')]
observation=dict(result='pass',base=base,candidate_count=36,reviewed_cohort_count=36,remaining_cohort_pending=0,
    source_tables=[dict(path=prefix+'dispositions.csv',sha256=sha(A/(prefix+'dispositions.csv')))],
    host_input_observations=prior['host_input_observations'],host_graph_observations=prior['host_graph_observations'],
    active_source_manifests=manifests,
    configuration_review_inputs=[dict(path=p,sha256=sha(Path(p))) for p in configuration],
    scope='Historical recipe file roles only; configuration consumers source-reviewed separately from compiler inputs')
(A/(prefix+'observations.json')).write_text(json.dumps(observation,indent=2)+'\n')
helpers['main'](A/(prefix+'verification.json'),base,prefix,36)
