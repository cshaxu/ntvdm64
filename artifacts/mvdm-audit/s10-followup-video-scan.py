"""Read-only token and include survey for pending C-VID physical roles.

Output is a hash-bound lexical snapshot, not preprocessing or runtime proof.
"""
import csv
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
AUDIT = ROOT / 'artifacts/mvdm-audit'
SRC = ROOT / 'src/mvdm'
CATALOGS = {
    'dos': Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv'),
    'wow': Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv'),
}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


inventory = AUDIT / 'host-ch-scope/inventory.csv'
frozen = AUDIT / 'frozen-file-reconciliation.csv'
selected = [r for r in rows(inventory) if r['path'].startswith('softpc.new/base/cvidc/')
            and r['ledger_status'] in ('pending', '待核验')]
frozen_rows = {r['relative_path']: r for r in rows(frozen)}
catalogs = {name: rows(path) for name, path in CATALOGS.items()}
entries = {}
targets = defaultdict(set)
for row in selected:
    relative = row['path']
    path = SRC / relative
    source = path.read_text(errors='replace')
    definitions = re.findall(r'^\s*#\s*define\s+([A-Za-z_]\w*)', source, re.M)
    guards = set(re.findall(r'^\s*#\s*ifndef\s+([A-Za-z_]\w*)', source, re.M))
    symbols = sorted(set(definitions) - guards)
    for symbol in symbols:
        targets[symbol].add(relative)
    record = frozen_rows[relative]
    matches = {name: [r for r in catalog if r['path'].replace('\\', '/').lower().endswith('/mvdm/' + relative.lower())]
               for name, catalog in catalogs.items()}
    entries[relative] = dict(current_sha256=sha(path), opennt_sha256=record['opennt_sha256'],
                             opennt45_sha256=record['opennt45_sha256'],
                             symbols=symbols, references=defaultdict(set), includes=set(),
                             catalogs={name: [dict(path=r['path'], sha256=r['sha256'],
                                 status=r.get('semantic_review', r.get('inventory_status', '')))
                                 for r in found] for name, found in matches.items()})

names = {Path(p).name.lower(): p for p in entries}
token = re.compile(r'\b[A-Za-z_]\w*\b')
include = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
scanned = 0
for path in SRC.rglob('*'):
    if not path.is_file() or path.suffix.lower() not in ('.c', '.h', '.inc', '.asm'):
        continue
    scanned += 1
    relative = path.relative_to(SRC).as_posix()
    text = path.read_text(errors='replace')
    for match in include.finditer(text):
        named = names.get(Path(match.group(1).replace('\\', '/')).name.lower())
        if named and named != relative:
            entries[named]['includes'].add(relative)
    for symbol in set(token.findall(text)) & targets.keys():
        for owner in targets[symbol]:
            if owner != relative:
                entries[owner]['references'][symbol].add(relative)

for entry in entries.values():
    entry['includes'] = sorted(entry['includes'])
    entry['references'] = {k: sorted(v) for k, v in sorted(entry['references'].items())}
snapshot = dict(inventory_sha256=sha(inventory), frozen_sha256=sha(frozen),
                catalogs={name: sha(path) for name, path in CATALOGS.items()},
                scanned_paths=scanned, entries=entries,
                limitation='Lexical references include comments, disabled branches and same-name declarations; no actual consumer or behavior inferred.')
(AUDIT / 's10-followup-video-scan.json').write_text(json.dumps(snapshot, indent=2) + '\n', encoding='utf-8')
print('pending', len(entries), 'source files scanned', scanned,
      'catalog coverage', {k: sum(bool(e['catalogs'][k]) for e in entries.values()) for k in CATALOGS})
