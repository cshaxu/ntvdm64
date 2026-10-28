"""Bound Unix/SoftWindows-only original header roles on the NT profile."""
import csv
import hashlib
import json
import re
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BUILD = Path('build/.ninja/msvc-x86-release/worker')
NAMES = ('host_fio.h', 'unix_asy.h', 'unix_cnf.h', 'unix_loc.h', 'swinhelp.h', 'swinmgre.h')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    H = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    frozen = {r['relative_path']: r for r in csv.DictReader(
        (A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig', newline=''))}
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    blocks = [b for b in re.split(r'(?m)(?=^obj/)', deps) if b.strip()]
    assert blocks and all('(VALID)' in b.splitlines()[0] and '(STALE)' not in b for b in blocks)
    flags = next(s for s in (BUILD / 'build.ninja').read_text().splitlines() if s.startswith('host_cflags ='))
    assert '/DNTVDM' in flags and '/DPROD' in flags and '/DCPU_40_STYLE' in flags
    assert not re.search(r'/D(?:SOFTWINDOWS|UNIX)(?:\s|$)', flags)
    sources = []
    for name in NAMES:
        relative = 'softpc.new/base/inc/' + name
        source = Path('src/mvdm') / relative
        original = Path('O:/repos.external/OpenNT/base/mvdm') / relative
        record = frozen[relative]
        digest = sha(source)
        assert digest == sha(original) == record['current_sha256'] == record['opennt_sha256'] == record['opennt45_sha256']
        assert record['dos_catalog_rows'] == '1' and record['wow_catalog_rows'] == '2'
        assert not any('../../../../src/mvdm/' + relative.lower() in b.lower().replace('\\', '/') for b in blocks)
        contents = source.read_text(encoding='latin1').lower()
        assert ('softwindows' in contents if name.startswith('swin') else 'unix' in contents)
        if name == 'swinhelp.h':
            assert 'hyperhelp' in contents
        sources.append(dict(relative_path=relative, original_path=str(original), sha256=digest,
                            dos_catalog_rows=1, wow_catalog_rows=2, wow_source_ids=record['wow_source_ids']))
    expression = r'^\s*#\s*include\s*[<"][^">]*(?:' + '|'.join(re.escape(n) for n in NAMES) + r')[">]'
    matches = subprocess.run(['rg', '-n', '-i', expression, 'src/mvdm', '-g', '*.[ch]'],
                             text=True, capture_output=True, check=False)
    assert matches.returncode == 1 and not matches.stdout
    image_path = Path('assets/release/ntvdm.exe')
    image = H['Image'](image_path)
    symbols = H['symbols']((BUILD / 'ntvdm.exe.map').read_text())
    providers = []
    for name, obj, owner in (
        ('_nt_event_loop', 'obj/host/nt_event.obj', 'original-softpc-host-roots:nt_event.obj'),
        ('_config_inquire', 'obj/host/config.obj', 'original-softpc-host-roots:config.obj'),
        ('_host_start_cpu', 'obj/host/nt_cpu.obj', 'original-softpc-host-roots:nt_cpu.obj'),
        ('_demOpen', 'obj/dem/demfile.obj', 'original-mvdm-dem:demfile.obj'),
        ('_demRead', 'obj/dem/demhndl.obj', 'original-mvdm-dem:demhndl.obj'),
        ('_demWrite', 'obj/dem/demhndl.obj', 'original-mvdm-dem:demhndl.obj')):
        symbol = next(s for s in symbols if s['name'] == name)
        assert symbol['owner'] == owner
        providers.append(H['bind'](image, H['Coff'](BUILD / obj), symbol))
    return dict(result='pass', base='cb592628d', sources=sources, flags=flags, dependency_blocks=len(blocks),
        dependency_sha256=hashlib.sha256(deps.encode()).hexdigest().upper(),
        source_include_matches=0, providers=providers,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in
                (image_path, BUILD / 'ntvdm.exe.map', BUILD / 'build.ninja',
                 BUILD / 'obj/host/nt_event.obj', BUILD / 'obj/host/config.obj',
                 BUILD / 'obj/host/nt_cpu.obj', BUILD / 'obj/dem/demfile.obj',
                 BUILD / 'obj/dem/demhndl.obj')],
        scope='Six alternate host/product headers only; not exclusion or behavioral proof of selected NT capabilities')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
