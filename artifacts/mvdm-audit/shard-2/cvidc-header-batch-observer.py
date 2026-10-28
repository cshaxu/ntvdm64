"""Read-only batch identity and normalized Ninja dependency scan for C-VID headers."""
import csv, hashlib, json, re, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/worker').resolve()
ROOT='softpc.new/base/cvidc/'
NAMES=('bound','c2cpug','conv','cpnote','dec','gdinit','hpphst','inc','light','nano','nanoa','page','pm','prot','simple','stub','vglob')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger={r['relative_path']:r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    out=subprocess.run(['ninja','-C',str(B),'-t','deps'],check=True,capture_output=True,text=True).stdout
    lookup={str((M/(ROOT+name+'_c.h')).resolve()).lower():name for name in NAMES}
    hits={name:[] for name in NAMES}; obj=''; valid=False
    for line in out.splitlines():
        if line and not line[0].isspace() and ': #deps ' in line:
            obj=line.split(': #deps ',1)[0]; valid='(VALID)' in line
        elif line.startswith('    ') and valid:
            name=lookup.get(str((B/line.strip()).resolve()).lower())
            if name is not None and obj not in hits[name]: hits[name].append(obj)
    rows=[]
    for name in NAMES:
        rel=ROOT+name+'_c.h'; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        assert ledger[rel]['shard']=='2' and ledger[rel]['status']=='pending'
        text=(M/rel).read_text(encoding='latin1')
        lines=[line.strip() for line in text.splitlines() if line.strip()]
        inert=(len(lines)==3 and lines[0].startswith('#ifndef ') and lines[1].startswith('#define ') and lines[2].startswith('#endif'))
        rows.append(dict(path=rel,hashes=hashes,guard_only=inert,nonblank_lines=len(lines),valid_deps_objects=hits[name]))
    return dict(result='pass',sources=rows,graph_sha256=sha(B/'build.ninja'),
                scope='Exact header bytes and normalized VALID deps snapshot; not a proof of all profiles, runtime or selected source semantics')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
