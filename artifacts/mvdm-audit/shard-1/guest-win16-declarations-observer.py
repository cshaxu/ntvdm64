"""Read-only pending WOW16 historical declaration and conditional inclusion audit."""
import csv,hashlib,json,re,subprocess
from pathlib import Path
A=Path('artifacts/mvdm-audit');ROOT=Path('src/mvdm');ORIG=Path('O:/repos.external/OpenNT/base/mvdm');ALT=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
N=['wow16/inc/gdidefs.inc','wow16/inc/paswindw.inc','wow16/inc/win3deb.inc']
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()
def inspect():
 with (A/'shard-1/file-audit.csv').open(encoding='utf-8-sig',newline='') as f:states={x['relative_path']:x for x in csv.DictReader(f)}
 with (A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig',newline='') as f:rows={x['relative_path']:x for x in csv.DictReader(f)}
 graphs=[Path('build/.ninja/msvc-x86-release/worker'),Path('build/.ninja/msvc-x86-release/wow32'),Path('build/.ninja/msvc-x64-release/frontend'),Path('build/.ninja/msvc-x64-release/service')];evidence=[]
 for b in graphs:
  g=(b/'build.ninja').read_text().replace('$:',':').lower();dep=subprocess.check_output(['ninja','-C',str(b),'-t','deps'],text=True,encoding='utf-8',errors='replace').lower().replace('\\','/');evidence.append((b,g,dep))
 entries=[]
 for n in N:
  assert states[n]['audit_status']=='待核验';r=rows[n];p=ROOT/n;assert sha(p)==r['current_sha256']==r['opennt_sha256']==r['opennt45_sha256']==sha(ORIG/n)==sha(ALT/n)
  full=p.resolve().as_posix().lower();hits=[str(b) for b,g,dep in evidence if full in g or full in dep];assert not hits
  entries.append(dict(path=n,sha256=sha(p),catalog_ids=r['wow_source_ids'],status=states[n]['audit_status'],host_graph_or_dep_hits=hits))
 fax=ROOT/'inc/wowfax.h';guest=ROOT/'wow16/test/shell/wowexfax.c';kernel=ROOT/'wow16/kernel31/kernel.inc'
 ft=fax.read_text(encoding='latin1');gt=guest.read_text(encoding='latin1');kt=kernel.read_text(encoding='latin1')
 assert re.search(r'#if\s+defined\(_WOWFAX16_\).*?#include\s+"[^\n]*gdidefs\.inc"',ft,re.I|re.S) and '#define _WOWFAX16_' in gt
 assert re.search(r'^\s*;\s*include\s+win3deb\.inc',kt,re.I|re.M)
 return dict(result='pass',ledger_sha256=sha(A/'shard-1/file-audit.csv'),entries=entries,conditional_consumers=[dict(path=str(p),sha256=sha(p)) for p in (fax,guest,kernel)],host_graphs=[dict(path=str(b),sha256=sha(b/'build.ninja')) for b in graphs],limitations='Literal source anchors, pending status and selected graph/dependency observations only; no preprocessing of alternate profiles, guest build or functionality verification.')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
