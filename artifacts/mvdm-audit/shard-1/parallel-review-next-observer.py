"""Inspect stored WOW USER assembly transformations without building guest."""
import csv
import hashlib
import json
import re
from pathlib import Path

ROOT = Path('src/mvdm/wow16/user')
DAY = ROOT / 'k/daytona'
UPSTREAM = Path('O:/repos.external/OpenNT/windows/core/ntuser')
RELEASE = Path('O:/winnt/system32/USER.EXE')
RULES = (
    (r'^if(.*ersion.*510)', r'if 1 ; \1'), (r'\.model', r'; .model'),
    ('USE32', 'USE16'), ('NEAR', 'FAR'), (r'\[ebp', r'[bp'),
    ('^FLAT', '; FLAT'), ('FLAT', 'NOTHING'),
    (r'(push.)([0-9][0-9]*)', r'\1dword ptr \2'),
    (r'^(EXTRN.*_GetAppCompatFlags@4:)', r'; \1'),
    (r'(rep.movs)d$', r'\1 dword ptr [edi], dword ptr [esi]'),
    (r'(rep.movs)b$', r'\1 byte ptr [edi], byte ptr [esi]'),
    (r'(rep[a-z]*.scas)b$', r'\1 byte ptr [edi]'),
    (r'(EXTRN.*_wow16.*DWORD)', r'; \1'),
)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def transform(first, second, short_fixes):
    before = (DAY / first).read_text(encoding='latin1').splitlines()
    after = (DAY / second).read_text(encoding='latin1').splitlines()
    assert len(before) == len(after)
    changed, exceptions = [], []
    for index, (original, actual) in enumerate(zip(before, after), 1):
        calculated = original
        for pattern, replacement in RULES:
            calculated = re.sub(pattern, replacement, calculated, count=1)
        if calculated != actual:
            assert calculated.replace('SHORT', '', 1) == actual
            exceptions.append(dict(line=index, generated=calculated, actual=actual))
        if original != actual:
            changed.append(index)
    assert len(exceptions) == short_fixes
    return dict(input=first, output=second, lines=len(before), changed_lines=len(changed),
                short_removals=exceptions)


def inspect():
    paths = ['wow16/user/k/daytona/wowk.asm', 'wow16/user/k/daytona/wowrtl.asm']
    frozen = {r['relative_path']: r for r in csv.DictReader(
        Path('artifacts/mvdm-audit/frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for path in paths:
        row = frozen[path]
        source = Path('src/mvdm') / path
        assert digest(source) == row['current_sha256']
        assert row['upstream_origin'] == 'current-only' and row['dos_catalog_rows'] == '0' and row['wow_catalog_rows'] == '0'
        sources.append(dict(relative_path=path, sha256=digest(source), origin=row['upstream_origin'],
                            dos_catalog_rows=0, wow_catalog_rows=0))
    script = (ROOT / 'wowasm.sed').read_text(encoding='latin1')
    assert len(script.splitlines()) == len(RULES)
    original_root = Path('O:/repos.external/OpenNT/base/mvdm/wow16/user')
    for name in ('makefile', 'wowasm.sed', 'wowasmk.cmd'):
        assert digest(ROOT / name) == digest(original_root / name)
    assert digest(ROOT / 'wowasm.sed') == 'E2CCB526CECA4ADE46CC3C1D88D8B04C2DC93A4F6C0E2EDD42FA82D2256D81A5'
    results = [transform('wowasmk.asm', 'wowk.asm', 0),
               transform('wowrtl.asm', 'wowkr1.asm', 3),
               transform('wowkr1.asm', 'wowkr.asm', 0)]
    assert [item['line'] for item in results[1]['short_removals']] == [239, 242, 506]
    makefile = (ROOT / 'makefile').read_text(encoding='latin1')
    for pattern in (r'OBJ386\s*=\s*\$\(O\)\\wowk\.obj\s+\$\(O\)\\wowkr\.obj',
                    r'\$\(O\)\\wowk\.obj:\s*\$\(O\)\\wowk\.asm',
                    r'\$\(O\)\\wowkr\.obj:\s*\$\(O\)\\wowkr\.asm'):
        assert re.search(pattern, makefile, re.I)
    response = (DAY / 'userk.lrf').read_text(encoding='latin1').lower()
    assert r'k\daytona\wowk.obj' in response and r'k\daytona\wowkr.obj' in response
    mapped = (DAY / 'user.map').read_text(encoding='latin1').lower()
    assert r'line numbers for k\daytona\wowk.obj(k\daytona\wowk.asm)' in mapped
    assert r'line numbers for k\daytona\wowkr.obj(k\daytona\wowkr.asm)' in mapped
    for symbol in ('@validatehwnd@4', '_getclassnamea@12', '@hmvalidatehandle@8'):
        assert symbol in mapped
    assert digest(DAY / 'USER.EXE') == digest(RELEASE)
    input_paths = [ROOT / 'makefile', ROOT / 'wowasm.sed', ROOT / 'wowasmk.cmd',
                   *(DAY / name for name in ('wowasmk.asm', 'wowk.asm', 'wowrtl.asm',
                      'wowkr1.asm', 'wowkr.asm', 'wowk.OBJ', 'wowkr.OBJ', 'userk.lrf',
                      'user.map', 'USER.EXE')), RELEASE,
                   UPSTREAM / 'client/wow.c', UPSTREAM / 'rtl/wow.c', UPSTREAM / 'rtl/client/wow.c']
    return dict(result='pass', sources=sources, transformations=results,
                inputs=[dict(path=str(p), sha256=digest(p)) for p in input_paths],
                scope='Stored source role and textual transformations; no assembly rebuild, byte provenance or runtime equivalence claim')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
