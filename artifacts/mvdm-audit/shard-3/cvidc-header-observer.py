"""Observe three selected C-VID declarations in real compiler preprocessing."""
import csv
import hashlib
import json
import re
from pathlib import Path

A=Path('artifacts/mvdm-audit')
R=Path('build/t473-s10/shard-3-cvidc-header-r80')
UNITS=(('cpuint_c.h','obj_ccpu_localfm.i'),('fm_c.h','obj_host_nt_cpu.i'),('fpu_c.h','obj_cvidc_accessfn.i'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows=[]
    for name,unit in UNITS:
        relative='softpc.new/base/cvidc/'+name
        record=frozen[relative]
        source=Path('src/mvdm')/relative
        primary=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        alternate=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source)==sha(primary)==sha(alternate)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        text=(R/unit).read_text(encoding='latin1')
        matches=list(re.finditer(r'(?im)^#line\s+1\s+"[^\n]*'+re.escape(name)+r'"\s*$',text))
        assert len(matches)==1, (name,len(matches))
        end=re.search(r'(?m)^#line\s+\d+\s+"',text[matches[0].end():])
        assert end
        selected=text[matches[0].end():matches[0].end()+end.start()]
        nonblank=[s.strip() for s in selected.splitlines() if s.strip()]
        assert nonblank
        rows.append(dict(relative_path=relative,sha256=sha(source),wow_source_ids=record['wow_source_ids'],
            unit=unit,preprocessed_sha256=sha(R/unit),selected_nonblank_lines=len(nonblank),
            first_line=nonblank[0],last_line=nonblank[-1]))
    return dict(result='pass',sources=rows,scope='Selected declarations only; no CPU, FPU or video behavior claim')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
