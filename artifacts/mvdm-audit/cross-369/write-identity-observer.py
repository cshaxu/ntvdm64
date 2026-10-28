import hashlib,json,zipfile
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
digest=lambda data:hashlib.sha256(data).hexdigest().upper()
paths={'installed':Path('O:/winnt/WRITE.EXE'),'mirror':repo/'src/mvdm/wow16/write/WRITE.EXE','mirror_write16':repo/'src/mvdm/wow16/write/write16.exe','original_45_write16':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow16/write/write16.exe')}
out={'files':{k:digest(p.read_bytes()) if p.is_file() else None for k,p in paths.items()}}
with zipfile.ZipFile(repo/'assets/winnt.zip') as z:out['zip_write']=digest(z.read('WINNT/WRITE.EXE'))
manifest=json.loads((repo/'assets/winnt-guest-manifest.json').read_text(encoding='utf-8'))
out['manifest']=[r for r in manifest['files'] if r['path'].upper()=='WINNT/WRITE.EXE']
historical=repo/'build/t471-s2/write-r6/inputs.json'
out['t471_inputs']=json.loads(historical.read_text(encoding='utf-8-sig')) if historical.is_file() else None
print(json.dumps(out,indent=2))
