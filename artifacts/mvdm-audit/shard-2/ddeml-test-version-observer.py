import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));N=('wow16/ddeml/tests/ddestrs/ddestrs.txt','wow16/ddeml/tests/ddestrs/runddes.bat','wow16/ddeml/verddeml.h')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for n in N:
  hs=[sha(root/n) for root in (M,*O)];assert hs==[f[n]['current_sha256'],f[n]['opennt_sha256'],f[n]['opennt45_sha256']] and l[n]['shard']=='2' and l[n]['status']=='pending';rows.append(dict(path=n,hashes=hs))
 root=M/'wow16/ddeml'; bat=(root/'tests/ddestrs/runddes.bat').read_text(encoding='latin1').lower();doc=(root/'tests/ddestrs/ddestrs.txt').read_text(encoding='latin1').lower();testmake=(root/'tests/ddestrs/makefile.dos').read_text(encoding='latin1').lower();c=(root/'ddeml.c').read_text(encoding='latin1').lower();mk=(root/'makefile').read_text(encoding='latin1').lower();assert 'ddestrs.exe' in bat and 'goto loop' in bat and 'stress' in doc and 'ddestrs' in testmake and 'ddeml' in testmake and '#include "verddeml.h"' in c and 'ddeml.obj' in mk and 'ddeml.dll:' in mk
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);named=[x for x in z.namelist() if x.lower().endswith(('/ddestrs.exe','/ddeml.dll'))];assert not named
 return dict(result='pass',files=rows,archive_sha256=sha(zpath),archive_named_test_or_module=named,ddeml_c_sha256=sha(root/'ddeml.c'),driver_make_sha256=sha(root/'makefile'),test_make_sha256=sha(root/'tests/ddestrs/makefile.dos'),scope='Separate test source/recipe and guest module source; archive names only, no test execution, dynamic delivery or runtime DDE conclusion')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
