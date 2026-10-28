"""DOS utility/data family original and packaged identity, read-only."""
from pathlib import Path
import hashlib,json,zipfile
names=['cmd/loadfix/loadfix.exc','cmd/mem/mem.tag','cmd/qbasic/nibbles.bas','cmd/qbasic/remline.bas','cmd/share/share.EXE','dev/ansi/ANSI.EXS','dev/himem/HIMEM.BIN']; roots=[Path('src/mvdm/dos/v86'),Path('O:/repos.external/OpenNT/base/mvdm/dos/v86'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86'),Path('O:/repos.external/opennt-src-2/nt/private/mvdm/dos/v86')];sha=lambda b:hashlib.sha256(b).hexdigest();rows=[]
with zipfile.ZipFile('assets/winnt.zip') as z:
 for n in names:
  paths=[r/n for r in roots]; ent=[x for x in z.namelist() if Path(x.replace('\\','/')).name.lower()==Path(n).name.lower()]; installed=Path('O:/winnt/system32')/Path(n).name
  rows.append(dict(path=n,source_and_refs=[dict(path=str(p),sha256=sha(p.read_bytes()) if p.is_file() else None) for p in paths],zip_entries=[dict(path=e,sha256=sha(z.read(e))) for e in ent],installed_path=str(installed),installed_sha256=sha(installed.read_bytes()) if installed.is_file() else None))
print(json.dumps(dict(files=rows,limitations='No guest execution; ZIP basename and installed system32 only.'),indent=2))
