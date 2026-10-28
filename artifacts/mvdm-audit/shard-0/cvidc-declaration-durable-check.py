"""Check retained declaration excerpts without requiring disposable .i files."""
import csv
import hashlib
import json
from pathlib import Path

A = Path('artifacts/mvdm-audit')
table = json.loads((A / 'shard-0/cvidc-declaration-observations.json').read_text(encoding='utf-8'))
frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
expected = {'cpu4gen.h': 'CpuVector', 'evid_c.h': 'VGAGLOBALSETTINGS',
            'gdpvar.h': 'Gdp', 'univer_c.h': 'ConstraintGuessREC'}
assert table['result'] == 'pass' and len(table['sources']) == len(expected)
for item in table['sources']:
    path = item['relative_path']
    name = Path(path).name
    assert name in expected and expected[name] in item['excerpt']
    assert item['selected_nonblank_lines'] == sum(bool(line.strip()) for line in item['excerpt'].splitlines())
    assert hashlib.sha256((Path('src/mvdm') / path).read_bytes()).hexdigest().upper() == item['sha256'] == frozen[path]['current_sha256']
    assert len(item['object_sha256']) == len(item['preprocessed_sha256']) == 64
print('PASS retained selected C-VID declarations; runtime semantics not asserted')
