"""Read-only audit of four unmirrored historical 16-bit build tools."""
import csv, hashlib, json, re
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
NAMES=('c23.err','fixexe.exe','lib16.exe','nosrvbld.exe')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger={r['relative_path']:r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    historical=Path('tools/historical/opennt-4.5')
    manifest=json.loads((historical/'MANIFEST.json').read_text(encoding='utf-8-sig'))
    indexed={r['Path'].lower():r for r in manifest['Files']}
    rows=[]
    for name in NAMES:
        rel='tools16/'+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        assert not hashes[0] and ledger[rel]['shard']=='2' and ledger[rel]['status']=='pending'
        archived=historical/name; archived_hash=sha(archived)
        assert archived_hash == hashes[2] == indexed[name.lower()]['Sha256'].upper()
        assert archived.stat().st_size == indexed[name.lower()]['Bytes']
        rows.append(dict(path=rel,hashes=hashes,origin=f['upstream_origin'],dos_catalog_rows=f['dos_catalog_rows'],wow_catalog_rows=f['wow_catalog_rows'],
                         historical_path=str(archived),historical_sha256=archived_hash,historical_size=archived.stat().st_size))
    names=('wow16/kernel31/makefile','wow16/kernel31/fixexe.c','wow16/lib/makefile','dos/v86/make.inc')
    ctx=[M/n for n in names]; kernel,fixsrc,lib,dos=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'fixexe kernel.exe' in kernel and 'fixexe.obj fixexe.lst:: fixexe.c' in kernel
    assert 'fixexe' in fixsrc and 'lib16 libw.lib +kernel.lib' in lib and 'nosrvbld $*.skl' in dos
    err=(O[0]/'tools16/c23.err').read_text(encoding='latin1').lower()
    assert 'internal compiler error' in err and 'error count exceeds' in err
    graphs=[Path('build/.ninja/msvc-x86-release/worker/build.ninja'),Path('build/.ninja/msvc-x86-release/wow32/build.ninja')]
    hits=[]
    for path in graphs:
        text=path.read_text(encoding='utf-8',errors='replace').lower()
        for name in NAMES:
            matches=[line.strip() for line in text.splitlines() if name.lower() in line]
            hits.append(dict(graph=str(path),name=name,lines=matches))
    return dict(result='pass',sources=rows,historical_manifest_sha256=sha(historical/'MANIFEST.json'),context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                graph_hits=hits,graphs=[dict(path=str(p),sha256=sha(p)) for p in graphs],
                scope='Unmirrored source identity, original build recipes and two current Ninja graph literal scans; no provenance of alternate tools or guest rebuild')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
