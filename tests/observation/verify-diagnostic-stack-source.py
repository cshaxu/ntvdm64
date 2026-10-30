from pathlib import Path
import hashlib

root = Path(__file__).resolve().parents[2]
items = [
    ('artifacts/mvdm-audit/shard-1/bop9-source-fixture.py',
     'src/mvdm/softpc.new/host/src/nt_bop.c',
     'C10228895446D6193AE6FD0215F5B93299359858DA1F7DD0626AC7CBB7E70153',
     "else 'reads=8'", "else 'reads=0'"),
    ('artifacts/mvdm-audit/shard-1/int15-source-fixture.py',
     'src/mvdm/softpc.new/base/bios/tape_io.c',
     '9073B54069F74950CB4B14D2B01E7AEAC0EDD2A482930AEF5DE6ABFF5CFCD5F3',
     "else 'reads=2'", "else 'reads=0'"),
    ('artifacts/mvdm-audit/shard-0/illegal-op-source-fixture.py',
     'src/mvdm/softpc.new/base/system/illegalp.c',
     'C3214520B0944064CF396D3E22814836A8DC193B0F6594EFAFA627D7C038FF94',
     'else "reads=8"', 'else "reads=4"'),
]
for script, source, old_hash, old_count, new_count in items:
    path = root / script
    text = path.read_text(encoding='utf8')
    digest = hashlib.sha256((root/source).read_bytes()).hexdigest().upper()
    assert old_hash in text and old_count in text
    text = text.replace(old_hash, digest).replace(old_count, new_count)
    text = text.replace('build/t473-s10', 'build/t473-s11')
    text = text.replace('unsigned e,unsigned f,unsigned g,unsigned h,unsigned i){(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i;reports++;}',
                        'unsigned e){(void)a;(void)b;(void)c;(void)d;(void)e;reports++;}')
    print(script, flush=True)
    exec(compile(text, str(path), 'exec'), {'__file__':str(path), '__name__':'__main__'})
