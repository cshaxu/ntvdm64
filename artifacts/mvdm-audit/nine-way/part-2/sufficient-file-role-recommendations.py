import csv,pathlib
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-2')
with (p/'semantic-pending-recommendations.csv').open(encoding='utf-8-sig',newline='') as f: rows=list(csv.DictReader(f))
owners={
'ntio-bootstrap-character-reconciliation.md':'Retained immutable NTIO guest; historical source-to-image provenance and all individual runtime behavior are unproved.',
'ntio-ems-int2f-semantic-reconciliation.md':'Retained NTIO guest for INT2F and EMS identification; real EMS allocation/mapping is owned by separate selected EMS implementation.',
'ntio-input-semantic-reconciliation.md':'Retained NTIO guest owns entry/vector; selected host keymouse and PIC owners carry device state. Exact released behavior remains unproved.',
'dosx-layout-semantic-reconciliation.md':'Retained immutable DOSX guest owns segment layout and mode continuation; source-to-image provenance and every teardown case remain unproved.',
'dos-parser-semantic-reconciliation.md':'Retained guest command parser is the behavioral owner; historical MEM recipe selects its command-local wrapper and common parser. Current binary lineage remains unproved.',
'dos-country-semantic-reconciliation.md':'Retained guest images own compiled messages; actual installed locale and each alternate locale are unproved. BUILDIDX is historical build-only tool.',
'user-generated-thunks-reconciliation.md':'Retained immutable USER guest and selected WOW/USER host own behavior; generated artifact provenance and exact guest composition remain unproved.',
}
excluded={'dpmi/dosx.def','softpc.new/bios/sysconf.asm','wow16/user/k/daytona/user.res','wow16/user/k/daytona/wowasmk.asm','wow16/user/k/daytona/wowk.asm','wow16/user/k/daytona/wowrtl.asm','wow16/user/k/daytona/wowkr1.asm','wow16/user/k/daytona/wowkr.asm','dos/v86/tools/src/buildidx/buildidx.def'}
fields=['path','bucket','source_shard','current_status','disposition','report','file_role_evidence','current_provider_and_scope','overall_exclusion']
chosen=[]
for row in rows:
 report=pathlib.Path(row['report']).name
 if report not in owners or row['path'] in excluded:continue
 r=dict(row)
 r['file_role_evidence']='Original source and/or historical build recipe supports stated role; see report for exact lines. This does not establish current absence, binary origin or feature completeness.'
 r['current_provider_and_scope']=owners[report]
 r['overall_exclusion']='NO: retain source/capability; current provider and alternate profiles require separately scoped evidence.'
 chosen.append(r)
with (p/'sufficient-file-role-recommendations.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(chosen)
print(len(chosen),'bounded roles; unique fields:',len(fields)==len(set(fields)))
