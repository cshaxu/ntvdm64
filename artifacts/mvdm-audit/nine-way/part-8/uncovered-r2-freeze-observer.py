import hashlib,json,pathlib
r=pathlib.Path('artifacts/mvdm-audit');p=r/'nine-way/part-8'
fs=['nine-way/part-8/existing-classifications-uncovered-r2.csv','nine-way/part-8/uncovered-sample-r2-observer.py','nine-way/part-8/uncovered-r2-corrections-observer.py','nine-way/part-8/existing-classifications-sample100.csv','nine-way/part-8/existing-classifications-sample101-300.csv','nine-way/part-1/other-guest-excluded-scope-evidence.json']
current=json.loads((p/'existing-classifications-uncovered-r2-baseline.json').read_text(encoding='utf-8-sig'));current['frozen_inputs']=[{'path':x,'sha256':hashlib.sha256((r/x).read_bytes()).hexdigest().upper()} for x in fs];(p/'existing-classifications-uncovered-r2-baseline.json').write_text(json.dumps(current,indent=2),encoding='utf-8')
