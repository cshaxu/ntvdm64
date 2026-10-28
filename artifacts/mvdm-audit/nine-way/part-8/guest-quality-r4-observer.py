import csv,pathlib,hashlib,json,subprocess
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8');rows=list(csv.DictReader((p/'guest-quality-r4-frozen.csv').open(encoding='utf-8-sig')));out=[]
def sha(x):return hashlib.sha256(x.read_bytes()).hexdigest().upper() if x.is_file() else ''
for r in rows:
 rel=r['relative_path'];cur=pathlib.Path('src/mvdm')/rel;orig=pathlib.Path('O:/repos.external/OpenNT/base/mvdm')/rel;alt=pathlib.Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/rel
 names=pathlib.PurePosixPath(rel).name
 proc=subprocess.run(['rg','-n','-i','-F','--max-count','3','--glob','!*.OBJ','--glob','!*.MAP','--glob','!*.EXE','--glob','!*.SYS','--glob','!*.COM',names,'src/mvdm/dos/v86'],capture_output=True,text=True,errors='replace')
 lines=sorted((x.replace('\\','/') for x in proc.stdout.splitlines() if not x.lower().replace('\\','/').startswith(str(cur).lower().replace('\\','/')+':')), key=lambda x:(x.lower(),x))[:10]
 live=sha(cur);a=sha(orig);b=sha(alt);diff=bool(live and a and live not in (a,b))
 role='build-tool source; selected historical recipe does not prove deployed tool provenance or all configuration scope' if '/tools/src/' in rel.lower() else 'guest declaration/message/build input; source role alone does not exclude the guest capability or all profiles'
 rec='partial if actual mirror diff' if diff else 'pending candidate; verify capability applicability and alternative provider before overall exclusion'
 out.append({'path':rel,'frozen_status':r['status'],'live_sha256':live,'original_sha256':a,'original45_sha256':b,'live_frozen_match':live==r['current_sha256'],'actual_diff':diff,'role':role,'references':lines,'suggestion':rec})
(p/'guest-quality-r4-observations.json').write_text(json.dumps(out,indent=2),encoding='utf8')
print('files',len(out),'diff',sum(x['actual_diff'] for x in out),'live mismatch',sum(not x['live_frozen_match'] for x in out))
