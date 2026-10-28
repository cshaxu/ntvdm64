import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));extroot=Path('O:/repos.external/OpenNT-4.5'); E=('SRC:a867497988c30606f28d','SRC:1d405e9fd6f5f186c8c4')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest().upper()
def inspect():
 own=list(csv.DictReader((A/'nine-way-ownership.csv').open(encoding='utf-8-sig')));m=[x for x in own if x['bucket']=='2' and x['kind']=='mvdm' and x['path'].startswith('fax/')];assert len(m)==16
 frozen={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};rows=[]
 for x in m:
  rel=x['path'];f=frozen[rel];hs=[sha(root/rel) for root in (M,*O)];assert hs==[f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']];rows.append(dict(key=x['key'],path=rel,hashes=hs))
 cat={x['id']:x for x in csv.DictReader(Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv').open(encoding='utf-8-sig'))};ext=[]
 for id in E:
  row=next(x for x in own if x['bucket']=='2' and x['kind']=='external' and x['key']==id);c=cat[id];p=extroot/row['path'];d=p.read_bytes();normalized=d.replace(b'\r\n',b'\n');blob=hashlib.sha1(b'blob '+str(len(normalized)).encode()+b'\0'+normalized).hexdigest();assert c['path']==row['path'] and c['edition']=='OpenNT-4.5' and c['git_blob']==blob;ext.append(dict(key=id,path=row['path'],edition=c['edition'],catalog_git_blob=c['git_blob'],normalized_git_blob=blob,sha256=sha(p),bytes=len(d),normalization='CRLF to LF'))
 root=M/'fax';driver=(root/'wowfax/wowfax.c').read_text(encoding='latin1');ui=(root/'wowfaxui/sources').read_text(encoding='latin1').lower();drv=(root/'wowfax/sources').read_text(encoding='latin1').lower();inf=(root/'printer.inf').read_text(encoding='latin1').lower(); assert all(s in drv for s in ('targetname=wowfax','wowfax.c','wowfax.rc')) and all(s in ui for s in ('targetname=wowfaxui','wfsheets.c','wfupgrad.c','wfhelper.c','wowfaxui.c','wowfaxui.rc')) and 'DrvEnableDriver' in driver and 'wowfaxui, wowfax, wowfax' in inf
 rs=(extroot/'nt/private/ntos/w32/ntgdi/printers/rasprint.sp1/rasdd/sources').read_text(encoding='latin1').lower();assert all(s in rs for s in ('targetname=rasdd','enabldrv.c','compress.c'))
 graphs=[]
 for b in ('worker','wow32'):
  p=Path('build/.ninja/msvc-x86-release')/b/'build.ninja';g=p.read_text().replace('$:',':').replace('\\','/').lower();assert all(str((M/x['path']).resolve()).replace('\\','/').lower() not in g for x in m); graphs.append(dict(path=str(p),sha256=sha(p)))
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);named=[x for x in z.namelist() if any(x.lower().endswith('/'+n) for n in ('wowfax.dll','wowfaxui.dll','rasdd.dll'))];return dict(result='pass',mvdm=rows,external=ext,graphs=graphs,archive_sha256=sha(zpath),archive_named_drivers=named,scope='Owned keys only; separate original drivers, static graph/package-name snapshot, no actual spooler or test execution')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
