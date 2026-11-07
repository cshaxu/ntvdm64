"""Final pinned host-mirror inventory and finite T480 accounting; no guest reads."""
import argparse
import collections
import csv
import difflib
import gzip
import hashlib
import json
from pathlib import Path
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
ART = ROOT / 'artifacts/mvdm-audit'


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    out = args.output.resolve()
    assert out.is_relative_to(ROOT / 'build/t480-s5') and not out.exists()
    out.mkdir(parents=True)
    pins = list(csv.DictReader((ART/'t480-s1-files.csv').open(encoding='utf-8-sig')))
    tracked = subprocess.check_output(['git','ls-files','-z','src/mvdm-host','src/non-mvdm'],cwd=ROOT).decode().split('\0')
    tracked = sorted(p for p in tracked if p.lower().endswith(('.c','.h')))
    assert tracked == sorted(r['path'] for r in pins) and len(tracked) == 1060
    evidence = json.loads(gzip.decompress((ART/'t480-s1-build-evidence.json.gz').read_bytes()))
    consumers = {f['path']:f['consumers'] for f in evidence['files']}
    s2 = json.loads((ART/'t480-s2-restorations.json').read_text())
    expected = {r['path']:r['after_sha256'] for r in s2['changes']}
    with zipfile.ZipFile(ART/'t480-s3-verification.zip') as archive:
        def receipt(name):return json.loads(archive.read(name).decode('utf-8-sig'))
        expected.update({r['path']:r['after_sha256'] for r in receipt('restorations-r1.json')})
        expected.update({r['path']:r['after_sha256'] for r in receipt('spelling-restorations.json')['files']})
        time = receipt('time-restoration.json');expected[time['path']] = time['after_sha256']
        expected['src/non-mvdm/base/win32/client/vdm.c'] = receipt('vdm-line-ending-correction.json')['after_sha256']
    s5 = json.loads((ROOT/'build/t480-s5/restorations.json').read_text())
    expected.update({r['path']:r['after_sha256'] for r in s5['changes']})
    raw = subprocess.check_output(['git','cat-file','--batch'],cwd=ROOT,input=''.join('d08e0ffc7:'+p+'\n' for p in tracked).encode())
    baseline = {};position = 0
    for path in tracked:
        end = raw.index(b'\n',position);size = int(raw[position:end].split()[-1]);position=end+1
        baseline[path]=raw[position:position+size];position+=size+1
    rows=[]
    for pin in pins:
        path=pin['path'];original=Path(pin['original']).read_bytes();current=(ROOT/path).read_bytes()
        assert sha(original)==pin['original_sha256'],path
        assert sha(baseline[path])==pin['current_sha256'],path
        assert sha(current)==expected.get(path,pin['current_sha256']),path
        if path in expected:assert consumers[path]
        def lines(data):return data.decode('latin1').replace('\r\n','\n').replace('\r','\n').splitlines(keepends=True)
        old=lines(original)
        def metrics(data):
            ops=[o for o in difflib.SequenceMatcher(None,old,lines(data),autojunk=True).get_opcodes() if o[0]!='equal']
            return dict(blocks=len(ops),added=sum(o[4]-o[3] for o in ops),removed=sum(o[2]-o[1] for o in ops))
        before=metrics(baseline[path]);after=metrics(current)
        if after['blocks']:assert consumers[path]
        rows.append(dict(path=path,original_sha256=sha(original),final_sha256=sha(current),S1=before,final=after,
                         status='restored-fragments' if path in expected else 'unchanged-from-S1',
                         consumer_count=len(consumers.get(path,[]))))
    s1h=json.loads((ART/'t480-s1-hunks.json').read_text())
    classes=json.loads((ART/'t480-s1-hunk-classification.json').read_text())
    assert len({h['id'] for h in s1h})==len(s1h)==9376
    assert {h['id'] for h in s1h}=={h['id'] for h in classes}
    s3=json.loads(gzip.decompress((ART/'t480-s3-dispositions.json.gz').read_bytes()))
    expected_s3={h['id'] for h in classes if h['category'] in ('product-input-substantive-review','contains-existing-exclusion-review-mixed-content')}
    assert {h['id'] for h in s3['rows']}==expected_s3 and len(s3['rows'])==5812
    # Existing exclusions remain authoritative; no guest/ASM change in this T's host review.
    guest=subprocess.check_output(['git','diff','d08e0ffc7','--name-only','--','src/mvdm-guest'],cwd=ROOT,text=True)
    assert not guest.strip()
    result=dict(passed=True,files=len(rows),changed_files=len(expected),differing_files=sum(r['final']['blocks']>0 for r in rows),
                totals={stage:{key:sum(r[stage][key] for r in rows) for key in ('blocks','added','removed')} for stage in ('S1','final')},
                classification_counts=dict(collections.Counter(h['category'] for h in classes)),
                S3_conservative_retention=s3['counts']['retain-no-safe-reversion-proved'],rows=rows,
                limits=['SequenceMatcher line accounting is alignment-sensitive; it is not semantic distance or number of bugs.',
                        'S3 conservative retention is not independently re-proved necessity or global mathematical minimality.',
                        'All unchanged original bug fixes and modern-host ownership/ABI bindings remain; no guest contents read.'])
    (out/'inventory.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in result.items() if k!='rows'},indent=2))


if __name__ == '__main__':main()
