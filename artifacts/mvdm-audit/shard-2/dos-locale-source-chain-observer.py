import csv,hashlib,json,re,zipfile
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm');O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'));names=('germany.msg','holland.msg')
def sha(b):return hashlib.sha256(b).hexdigest().upper()
def inspect():
 f={x['relative_path']:x for x in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))};l={x['relative_path']:x for x in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))};rows=[]
 for name in names:
  rel='dos/v86/messages/'+name;h=[sha((p/rel).read_bytes()) for p in (M,*O)];assert h==[f[rel]['current_sha256'],f[rel]['opennt_sha256'],f[rel]['opennt45_sha256']] and l[rel]['shard']=='2' and l[rel]['status']=='pending';rows.append(dict(source=rel,hashes=h))
 rels=('dos/v86/make.inc','dos/v86/messages/makefile','dos/v86/cmd/command/makefile','dos/v86/doskrnl/dos/makefile'); files={r:(M/r).read_text(encoding='latin1').lower() for r in rels};common=files[rels[0]];msg=files[rels[1]];command=files[rels[2]];dos=files[rels[3]];assert '!ifndef country' in common and 'country =usa-ms' in common and r'msg = ..\..\messages\$(country)' in common and '$(country).idx: $(country).msg' in msg and r'buildmsg.exe $(msg)\$(country) command.skl' in command and r'$(msg)\$(country).msg' in dos
 zpath=Path('assets/winnt.zip');z=zipfile.ZipFile(zpath);out=[]
 for n in ('COMMAND.COM','NTDOS.SYS'):
  ent=next(x for x in z.infolist() if x.filename.lower()=='winnt/system32/'+n.lower()); installed=Path('O:/winnt/system32')/n;d=sha(z.read(ent));assert d==sha(installed.read_bytes());out.append(dict(member=ent.filename,sha256=d))
 return dict(result='pass',files=rows,recipe_sha256={r:sha((M/r).read_bytes()) for r in rels},archive_sha256=sha(zpath.read_bytes()),installed_matching_images=out,scope='Parameterized source recipes and artifact identity only; selected locale and source-to-image provenance unproven')
if __name__=='__main__':print(json.dumps(inspect(),indent=2))
