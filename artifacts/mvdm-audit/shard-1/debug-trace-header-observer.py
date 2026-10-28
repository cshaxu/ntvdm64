"""Read-only exact normalized Ninja dependency audit for debug/trace headers."""
import csv,hashlib,json,re,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['softpc.new/base/inc/d_inst.h','softpc.new/base/inc/dsktrace.h','softpc.new/base/inc/harness.h','softpc.new/base/inc/hosthunt.h','softpc.new/base/inc/hunter.h']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def norm(p):return str(Path(p).resolve()).replace('\\','/').casefold()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];matches={n:[] for n in N}; target={norm(ROOT/n):n for n in N}
 for b in graphs:
  deps=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace');owner=None;valid=False
  for line in deps.splitlines():
   if not line.startswith(' '):
    owner=line.split(': #deps ',1)[0] if ': #deps ' in line else None;valid=bool(owner and '(VALID)' in line and '(STALE)' not in line)
   elif valid:
    path=line.strip(); p=Path(path); resolved=norm(p if p.is_absolute() else b/p)
    if resolved in target:matches[target[resolved]].append(dict(build=str(b),object=owner,raw=path,resolved=resolved))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],valid_dependency_matches=matches[n]))
 d='softpc.new/base/inc/dsktrace.h';assert any(x['object']=='obj/disks/fdisk.obj' for x in matches[d]) and any(x['object']=='obj/disks/diskbios.obj' for x in matches[d]);flags=next(line for line in (graphs[0]/'build.ninja').read_text().splitlines() if line.startswith('host_cflags ='));assert '/DPROD' in flags and not re.search(r'/DHUNTER(?:\s|$)',flags)
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],limitations='VALID Ninja dependencies for four checked trees only; header activity, other profiles, generated references and debug behavior are not implied by dependency presence or absence.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
