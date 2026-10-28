"""Read-only bin86 original/reference/package identity and original binplace census."""
from pathlib import Path
import csv,hashlib,json,zipfile
names='autoexec.nt ctl3dv2.dll ctl3dv2.map lzexpand.dll netapi.dll regedit.hlp winhelp.exe'.split();root=Path('src/mvdm/bin86');refs=[Path('O:/repos.external/OpenNT/base/mvdm/bin86'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/bin86')];sha=lambda b:hashlib.sha256(b).hexdigest()
with zipfile.ZipFile('assets/winnt.zip') as z:
 entries={Path(n.replace('\\','/')).name.lower():n for n in z.namelist() if Path(n.replace('\\','/')).name.lower() in names}
 rows=[]
 for n in names:
  p=root/n;ref=[dict(path=str(r/n),sha256=sha((r/n).read_bytes()) if (r/n).is_file() else None) for r in refs]; en=entries.get(n);install=Path('O:/winnt/system32')/n
  rows.append(dict(name=n,source_sha256=sha(p.read_bytes()) if p.is_file() else None,references=ref,zip_entry=en,zip_sha256=sha(z.read(en)) if en else None,installed_path=str(install),installed_sha256=sha(install.read_bytes()) if install.is_file() else None))
print(json.dumps(dict(files=rows,limitations='Matching basename in ZIP may be a different internal directory; exact installed system32 only; no source lineage or runtime test.'),indent=2))
