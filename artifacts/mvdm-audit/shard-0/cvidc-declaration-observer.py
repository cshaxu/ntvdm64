"""Capture selected original C-VID declarations from actual compiler output."""
import csv
import hashlib
import json
import re
from pathlib import Path

A = Path('artifacts/mvdm-audit')
R = Path('build/t473-s10/shard-0-cvidc-header-r92')
W = Path('build/.ninja/msvc-x86-release/worker')
UNITS = (('cpu4gen.h', 'obj_video_v7_video.i', 'obj/video/v7_video.obj'),
         ('evid_c.h', 'obj_cvidc_ev_glue.i', 'obj/cvidc/ev_glue.obj'),
         ('gdpvar.h', 'obj_cvidc_ev_glue.i', 'obj/cvidc/ev_glue.obj'),
         ('univer_c.h', 'obj_cvidc_accessfn.i', 'obj/cvidc/accessfn.obj'))


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    result = []
    for name, unit, obj in UNITS:
        relative = 'softpc.new/base/cvidc/' + name
        row = frozen[relative]
        paths = (Path('src/mvdm') / relative, Path('O:/repos.external/OpenNT/base/mvdm') / relative,
                 Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / relative)
        assert sha(paths[0]) == sha(paths[1]) == sha(paths[2]) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        assert row['dos_catalog_rows'] == '1' and row['wow_catalog_rows'] == '2'
        text = (R / unit).read_text(encoding='latin1')
        markers = list(re.finditer(r'(?im)^#line\s+\d+\s+"[^\n]*[\\/]'+re.escape(name)+r'"\s*$', text))
        blocks = []
        for marker in markers:
            following = re.search(r'(?m)^#line\s+\d+\s+"', text[marker.end():])
            blocks.append(text[marker.end():marker.end()+following.start() if following else len(text)])
        excerpt = ''.join(blocks)
        assert markers and excerpt.strip()
        result.append(dict(relative_path=relative, sha256=sha(paths[0]), wow_source_ids=row['wow_source_ids'],
            unit=unit, object=obj, object_sha256=sha(W / obj), preprocessed_sha256=sha(R / unit),
            selected_nonblank_lines=sum(bool(line.strip()) for line in excerpt.splitlines()), excerpt=excerpt))
    return dict(result='pass', sources=result, scope='Actual compiler-selected declaration text only; no ABI or runtime behavior acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
