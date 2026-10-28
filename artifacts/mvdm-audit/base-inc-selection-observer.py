"""Observe selected original base headers in eight real CCPU preprocesses."""
import csv
import hashlib
import io
import json
import re
import runpy
import subprocess
from pathlib import Path

BASE = '6c0df5292'
A = Path('artifacts/mvdm-audit')
ROOT = Path('build/t473-s10/base-inc-selection-r67')
parse = runpy.run_path(str(A / 'ccpu-selected-header-extractor.py'))['inspect_output']
sha = lambda p: hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    original = subprocess.check_output(['git', 'show', BASE + ':artifacts/mvdm-audit/combined-initial-status.csv'])
    pending = sorted(r['relative_path'] for r in csv.DictReader(io.StringIO(original.decode('utf-8-sig')))
                     if r['status'] == 'pending' and r['relative_path'].startswith('softpc.new/base/inc/'))
    assert len(pending) == 146 and all(p.endswith('.h') for p in pending)
    result = json.loads((ROOT / 'result.json').read_text(encoding='utf-8-sig'))
    assert result['result'] == 'pass' and result['production_changed'] is False
    assert len(result['units']) == 8
    deps = (ROOT / 'deps.stdout.txt').read_text()
    blocks = {b.split(':', 1)[0]: b for b in re.split(r'(?m)(?=^obj/)', deps) if b.startswith('obj/')}
    selected = {}
    for unit in result['units']:
        output = Path(unit['output'])
        assert sha(output) == unit['sha256']
        rows, _ = parse(output, pending, minimum_markers=1, translation_unit=unit['unit'])
        for row in rows:
            if not row['selected_nonblank_lines']:
                continue
            target = 'obj/ccpu/' + unit['unit'] + '.obj'
            block = blocks[target].replace('\\', '/')
            assert '(VALID)' in block and '(STALE)' not in block
            assert '../../../../src/mvdm/' + row['relative_path'] in block
            selected.setdefault(row['relative_path'], []).append(row)
    assert set(selected) == {'softpc.new/base/inc/' + n for n in
                            ('bios.h', 'config.h', 'cpu4.h', 'timer.h', 'xt.h')}
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline=''))}
    sources = []
    for name, matches in sorted(selected.items()):
        path = Path('src/mvdm') / name
        original = Path('O:/repos.external/OpenNT/base/mvdm') / name
        digest = sha(path)
        assert digest == sha(original) == frozen[name]['current_sha256'] == frozen[name]['opennt_sha256']
        assert digest == frozen[name]['opennt45_sha256']
        assert frozen[name]['dos_catalog_rows'] == '1' and frozen[name]['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=name, sha256=digest, original_path=str(original),
                            dos_catalog_rows=1, wow_catalog_rows=2,
                            wow_source_ids=frozen[name]['wow_source_ids'],
                            selections=matches, original_text_retained_percent='100'))
    return dict(result='pass', base=BASE, pending_cohort=146, selected_headers=5,
        nonselected_headers_still_pending=141, sources=sources,
        units=[dict(unit=u['unit'], sha256=u['sha256'], output=u['output'],
                    original_command=u['original_command'], preprocess_command=u['preprocess_command'])
               for u in result['units']],
        deps_sha256=sha(ROOT / 'deps.stdout.txt'), tool_sha256=sha('tests/observation/observe-ccpu-header-selection.ps1'),
        scope='Eight selected preprocessing contexts; not full declaration ABI, macro, lifecycle or behavior proof')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
