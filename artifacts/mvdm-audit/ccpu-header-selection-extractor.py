"""Observe the 22 remaining original headers across selected production TUs."""
import csv
import io
import json
import re
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = Path('build/t473-s10/ccpu-header-selection/run')
BASE = '70985c440'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
rows, sha = helpers['rows'], helpers['sha']


def inspect():
    old = subprocess.check_output(['git', 'show', BASE + ':artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')
    cohort = sorted(r['relative_path'] for r in csv.DictReader(io.StringIO(old))
                    if r['status'] == 'pending' and re.fullmatch(r'softpc.new/base/ccpu386/[^/]+\.h', r['relative_path']))
    assert len(cohort) == 22
    result = json.loads((ROOT / 'result.json').read_text(encoding='utf-8-sig'))
    assert result['result'] == 'pass' and len(result['units']) == 8
    parser = runpy.run_path(str(A / 'ccpu-selected-header-extractor.py'))['inspect_output']
    all_rows = []
    for unit in result['units']:
        output = Path(unit['output'])
        assert sha(output) == unit['sha256']
        selected, _ = parser(output, cohort, minimum_markers=1,
                             translation_unit='softpc.new/base/ccpu386/' + unit['unit'] + '.c')
        all_rows.extend(selected)
    deps = (ROOT / 'deps.stdout.txt').read_text()
    blocks = {block.split(':', 1)[0]: block for block in re.split(r'(?m)(?=^obj/)', deps) if block.startswith('obj/')}
    headers = []
    for name in cohort:
        consumers = []
        for target, block in blocks.items():
            if ('../../../../src/mvdm/' + name).casefold() in block.replace('\\', '/').casefold():
                assert '(VALID)' in block and '(STALE)' not in block
                consumers.append(target)
        selected = [r for r in all_rows if r['relative_path'] == name]
        headers.append(dict(relative_path=name, source_sha256=sha(Path('src/mvdm') / name),
                            valid_consumers=consumers, seen_units=[r['translation_unit'] for r in selected if r['seen_in_line_markers']],
                            emitted_declaration_units=[r for r in selected if r['selected_nonblank_lines']]))
    npx = (ROOT / 'zfrsrvd.i').read_text(encoding='utf-8-sig')
    snippets = {}
    for name in ('npx_frstor', 'npx_fsave'):
        match = re.search(r'(?ms)^static void ' + name + r'\(\) \{.*?^\}', npx)
        assert match
        snippet = match[0]
        compact = re.sub(r'\s+', '', snippet)
        assert 'limit_check(m_seg[0],m_off[0],(int)1,(int)94)' in compact
        assert 'limit_check(m_seg[0],m_off[0],(int)1,(int)106)' in compact
        assert '0x6c' in compact and '0x5e' in compact
        snippets[name] = snippet
    return dict(result='pass', base=BASE, headers=headers, source_header_count=22,
                selected_declaration_headers=sum(bool(h['emitted_declaration_units']) for h in headers),
                x87_selected_macro_expansions=snippets,
                records=[dict(path=str(p), sha256=sha(p)) for p in
                         (ROOT / 'result.json', ROOT / 'deps.stdout.txt', A / 'ccpu-selected-header-extractor.py', Path(__file__).resolve())],
                scope='Eight actual production preprocessing/dependency contexts; not whole-product absence or runtime/ABI acceptance')


if __name__ == '__main__':
    observed = inspect()
    (A / 'ccpu-header-selection-observations.json').write_text(json.dumps(observed, indent=2) + '\n')
    print(json.dumps(dict(result='pass', headers=22, declarations=observed['selected_declaration_headers'])))
