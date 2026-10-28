import csv, hashlib, json
from pathlib import Path
repo = Path(__file__).resolve().parents[4]
mini = Path('O:/repos.external/OpenNT-4.5/nt/private/ntos/w32/ntgdi/printers/rasprint/mini')
with (repo / 'artifacts/mvdm-audit/nine-way/part-3/printer-owned-files.csv').open(encoding='utf-8-sig', newline='') as f:
    rows = list(csv.DictReader(f))
families = sorted({r['family'] for r in rows})
result = []
for name in families:
    p = mini / name
    sources = p / 'sources'
    if p.is_dir():
        data = sources.read_bytes() if sources.is_file() else None
        result.append({'family': name, 'owned_count': sum(r['family'] == name for r in rows), 'sources_sha256': hashlib.sha256(data).hexdigest().upper() if data else None, 'sources_text': data.decode('latin1') if data else None, 'rc_files': sorted(x.name for x in p.glob('*.rc'))})
    else:
        result.append({'root_file': name, 'owned_count': sum(r['family'] == name for r in rows), 'text': p.read_text(encoding='latin1') if p.is_file() else None})
print(json.dumps(result, indent=2))
