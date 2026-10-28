from pathlib import Path
import hashlib
import tempfile

p = Path('artifacts/mvdm-audit/nine-way/part-8')
root = Path('build/t473-s10')
root.mkdir(parents=True, exist_ok=True)
with tempfile.TemporaryDirectory(prefix='r4-replay-', dir=root) as temporary:
    temp = Path(temporary)
    script = (p/'guest-quality-r4-freeze.py').read_text(encoding='utf-8-sig')
    script = script.replace("ledger=r/'combined-initial-status.csv'", "ledger=p/'guest-quality-r4-ledger-frozen.csv'")
    script = script.replace("out=p/'guest-quality-r4-frozen.csv'", "out=temp/'guest-quality-r4-replayed.csv'")
    script = script.replace("p/'guest-quality-r4-baseline.json'", "temp/'guest-quality-r4-replayed-baseline.json'")
    exec(compile(script, str(p/'guest-quality-r4-freeze.py'), 'exec'), {'__file__': str(p/'guest-quality-r4-freeze.py'), 'temp': temp})
    original = (p/'guest-quality-r4-frozen.csv').read_bytes()
    replayed = (temp/'guest-quality-r4-replayed.csv').read_bytes()
    assert original == replayed
    print('sample bytes equal', original == replayed, 'sha256', hashlib.sha256(replayed).hexdigest().upper())
