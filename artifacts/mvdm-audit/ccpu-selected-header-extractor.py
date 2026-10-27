"""Observe original CCPU declarations selected by actual MSVC /P output."""
import csv
import hashlib
import io
import json
import re
import subprocess
from collections import defaultdict
from pathlib import Path

BASE='9ca2db194'
A=Path('artifacts/mvdm-audit')
OUTPUT=Path('build/t473-s10/ccpu-selected-preprocessing/c_main.i')
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest().upper()

def inspect_output(path,cohort):
    repo=Path.cwd()
    canonical={str(repo/'src/mvdm'/p).replace('\\','/').casefold():p for p in cohort}
    selected=defaultdict(list);seen=set();current=None
    for line in path.read_text(encoding='utf-8-sig').splitlines():
        marker=re.match(r'^#(?:line)?\s+\d+\s+"([^"]+)"',line)
        if marker:
            current=canonical.get(re.sub(r'[\\/]+','/',marker[1]).casefold())
            if current:seen.add(current)
        elif current and line.strip() and not line.lstrip().startswith('#'):
            selected[current].append(line.strip())
    assert len(seen)>20, 'Expected actual selected CCPU header markers'
    records=[]
    for name in cohort:
        lines=selected[name];tokens='\n'.join(lines)
        records.append(dict(relative_path=name,source_sha256=sha(Path('src/mvdm')/name),
            translation_unit='softpc.new/base/ccpu386/c_main.c',selected_profile='CPU_40_STYLE CCPU SPC386 PROD',
            seen_in_line_markers=name in seen,selected_nonblank_lines=len(lines),
            selected_text_sha256=hashlib.sha256(tokens.encode()).hexdigest().upper() if lines else '',
            selected_excerpt=' | '.join(lines[:2])[:240],
            remaining_contract='Selected tokens are preprocessing evidence, not proven layout, call ABI, guest decoding, fault ordering or behavior' if lines else
                'No emitted declaration tokens in this translation unit; macros, inactive guards, local shadows and separate consumers require independent review'))
    return records,seen

def main():
    previous=subprocess.check_output(['git','show',BASE+':artifacts/mvdm-audit/combined-initial-status.csv']).decode('utf-8-sig')
    pending={r['relative_path']:r for r in csv.DictReader(io.StringIO(previous)) if r['status']=='pending'}
    cohort=sorted(p for p in pending if p.startswith('softpc.new/base/ccpu386/') and p.endswith('.h'))
    records,seen=inspect_output(OUTPUT,cohort)
    assert len(cohort)==96 and len(seen)==74
    assert sum(bool(r['selected_nonblank_lines']) for r in records)==71
    table=A/'ccpu-selected-header-observations.csv'
    with table.open('w',encoding='utf-8',newline='') as stream:
        writer=csv.DictWriter(stream,list(records[0]),lineterminator='\n');writer.writeheader();writer.writerows(records)
    summary=dict(result='pass',base=BASE,cohort_headers=96,headers_seen=74,headers_with_emitted_tokens=71,
        scope='Compiler-selected declarations of a selected decoder; not behavior acceptance',
        preprocessor_output=dict(path=str(OUTPUT),sha256=sha(OUTPUT)),table_sha256=sha(table),
        script_sha256=sha(Path(__file__)),product_source_changed=False,guest_changed=False)
    (A/'ccpu-selected-header-observations.json').write_text(json.dumps(summary,indent=2)+'\n')
    print(json.dumps(summary))

if __name__=='__main__':main()
