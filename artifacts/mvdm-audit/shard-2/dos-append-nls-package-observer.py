import csv,hashlib,json,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit'); M=Path('src/mvdm'); O=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'); O1=Path('O:/repos.external/OpenNT/base/mvdm'); paths=('dos/v86/cmd/append/append.exe','dos/v86/cmd/nlsfunc/nlsfunc.exe')
def sha(b):return hashlib.sha256(b).hexdigest().upper()
def inspect():
 frozen={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};ledger={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}; zpath=Path('assets/winnt.zip'); z=zipfile.ZipFile(zpath); fs=[x for x in z.infolist() if not x.is_dir()]; rows=[]
 for rel in paths:
  d=(M/rel).read_bytes(); f=frozen[rel]; assert sha(d)==sha((O/rel).read_bytes())==f['current_sha256']==f['opennt45_sha256'];assert not (O1/rel).exists() and ledger[rel]['shard']=='2' and ledger[rel]['status']=='pending'
  same=[x.filename for x in fs if x.file_size==len(d) and sha(z.read(x))==sha(d)];named=[x.filename for x in fs if x.filename.replace('\\','/').lower().endswith('/'+Path(rel).name.lower())]; installed=Path('O:/winnt/system32')/Path(rel).name;assert not same and not named and not installed.exists();rows.append(dict(source=rel,sha256=sha(d),size=len(d),archive_equal_members=same,archive_named_members=named,installed_exists=installed.exists()))
 src=(M/'dos/v86/cmd/nlsfunc/nlsfunc.asm').read_text(encoding='latin1');assert 'MULT_NLSFUNC' in src and 'COUNTRY.SYS' in src and 'INT_2f_NEXT' in src
 country=next(x for x in fs if x.filename.lower()=='winnt/system32/country.sys');countrysha=sha(z.read(country));installedcountry=Path('O:/winnt/system32/COUNTRY.SYS');assert sha(installedcountry.read_bytes())==countrysha
 return dict(result='pass',files=rows,archive_sha256=sha(zpath.read_bytes()),archive_file_count=len(fs),country_member=country.filename,country_sha256=countrysha,installed_country_sha256=sha(installedcountry.read_bytes()),nlsfunc_source_sha256=sha((M/'dos/v86/cmd/nlsfunc/nlsfunc.asm').read_bytes()),scope='Exact archive binary identities and source-level NLS path only; no guest execution or alternate media conclusion')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
