"""Enrich the part-1 NLS census with per-key codepage metadata and file roles."""
from pathlib import Path
import json,re
items=[x for x in json.loads(Path('artifacts/mvdm-audit/nine-way/part-1/external-remaining-evidence.json').read_text(encoding='utf-8-sig'))['files'] if x['family']=='nls'];root=Path('O:/repos.external/OpenNT')
for x in items:
 p=root/x['path'];data=p.read_bytes();t=data[:16384].decode('latin1','replace');m=re.search(r'^\s*CODEPAGE\s+(\d+)',t,re.M|re.I);x['declared_codepage']=int(m.group(1)) if m else None;s=x['path'].lower();x['observed_role']=('nls-test-input-or-source' if '/test/' in s else 'font-support-input-or-source' if '/fontsup/' in s else 'codepage-data' if m else 'nls-code-or-build-data');x['suggested_status']='pending-specific-codepage-and-provider-semantics'
print(json.dumps(dict(files=items,limitations='Role and CODEPAGE header are source inspection; no actual locale/provider access or runtime test.'),indent=2))
