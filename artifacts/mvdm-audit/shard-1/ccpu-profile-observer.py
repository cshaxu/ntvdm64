"""Recheck shard-1 CCPU file-list and selected 386/486 boundaries."""
import csv
import hashlib
import json
import re
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
ROOT = Path('src/mvdm/softpc.new/base/ccpu386')
BUILD = Path('build/.ninja/msvc-x86-release/worker')
NAMES = ('bswap.h', 'c-files', 'ccpudefs.inc', 'gdpvar.h', 'invd.c', 'invd.h')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    sources = []
    for name in NAMES:
        path = 'softpc.new/base/ccpu386/' + name
        row = frozen[path]
        primary = Path('O:/repos.external/OpenNT/base/mvdm') / path
        secondary = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path
        value = sha(ROOT / name)
        assert value == sha(primary) == sha(secondary) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        assert row['dos_catalog_rows'] == '1' and row['wow_catalog_rows'] == '2'
        sources.append(dict(relative_path=path, sha256=value, dos_catalog_rows=1,
                            wow_catalog_rows=2, wow_source_ids=row['wow_source_ids']))
    list_names = [line.strip() for line in (ROOT / 'c-files').read_text().splitlines() if line.strip()]
    recipe = (ROOT / 'sources').read_text()
    body = recipe.split('SOURCES=', 1)[1].split('!INCLUDE', 1)[0]
    recipe_names = re.findall(r'(?i)\b[a-z0-9_]+\.c\b', body)
    assert len(list_names) == 129 and len(recipe_names) == 130
    assert set(list_names) - set(recipe_names) == {'p.c_main.c', 'p.c_tsksw.c'}
    assert set(recipe_names) - set(list_names) == {'ntstubs.c', 'ntthread.c', 'sasCdef.c'}
    assert re.search(r'(?mi)^!INCLUDE\s+ccpudefs\.inc', recipe)
    flags = next(line for line in (BUILD / 'build.ninja').read_text().splitlines() if line.startswith('cflags ='))
    assert all('/D' + n in flags for n in ('CPU_40_STYLE', 'CCPU', 'SPC386'))
    assert not re.search(r'/D(?:CPU_30_STYLE|SPC486|CPU_486)(?:\s|$)', flags)
    source = (ROOT / 'c_main.c').read_text(encoding='latin1')
    assert re.search(r'case\s+0x08:.*?#ifdef SPC486.*?INVD\(\);.*?#else\s+Int6\(\);', source, re.S)
    assert re.search(r'case\s+0x09:.*?#ifdef SPC486.*?WBINVD\(\);.*?#else\s+Int6\(\);', source, re.S)
    symbols = h['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    image = h['Image'](Path('assets/release/ntvdm.exe'))
    owner = next(s for s in symbols if s['name'] == '_c_cpu_simulate' and s['owner'] == 'original-ccpu386:c_main.obj')
    linked = h['bind'](image, h['Coff'](BUILD / 'obj/ccpu/c_main.obj'), owner)
    absence = []
    for name in ('invd', 'bswap'):
        obj = h['Coff'](BUILD / ('obj/ccpu/' + name + '.obj'))
        assert not any(v[0].upper() == '_' + name.upper() for v in obj.symbols.values())
        assert not any(s['name'].upper() == '_' + name.upper() for s in symbols)
        absence.append(dict(object=name + '.obj', sha256=sha(BUILD / ('obj/ccpu/' + name + '.obj')),
                            defined_symbol_absent='_' + name.upper()))
    prerecord = json.loads(Path('build/t473-s10/shard1-ccpu-preprocess-r1/result.json').read_text(encoding='utf-8-sig'))
    assert prerecord['production_changed'] is False
    inputs = [ROOT / 'sources', ROOT / 'c_main.c', ROOT / 'sascdef.c',
              BUILD / 'build.ninja', BUILD / 'ntvdm.exe.map', Path('assets/release/ntvdm.exe'),
              Path('tools/build/New-T310OriginalSoftpcNinja.ps1')]
    return dict(result='pass', sources=sources, list_count=len(list_names), recipe_count=len(recipe_names),
        list_only=sorted(set(list_names)-set(recipe_names)), recipe_only=sorted(set(recipe_names)-set(list_names)),
        flags=flags, linked_cpu_entry=linked, missing_486_symbols=absence,
        preprocess=[dict(target=x['target'], sha256=x['sha256']) for x in prerecord['units']],
        inputs=[dict(path=str(p), sha256=sha(p)) for p in inputs],
        scope='Selected 386 build and source-profile evidence; no 486 execution or guest acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
