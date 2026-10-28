"""Merge independently verified C-VID declaration and vector roles."""
import csv
import json
import runpy
import struct
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'de8fc1a69', 'parallel-cvidc-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
subprocess.run(['python', '-B', str(A / 'shard-1/cvidc-preprocessed-extract.py'), '--verify-retained'], check=True)
subprocess.run(['python', '-B', str(A / 'shard-3/cvidc-header-durable-check.py')], check=True)
table = json.loads((A / 'shard-1/parallel-cvidc-evidence.json').read_text(encoding='utf-8'))['table']
h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
build = h['BUILD']
image = h['Image'](Path('assets/release/ntvdm.exe'))
obj = h['Coff'](build / 'obj/cvidc/vglfunc.obj')
syms = h['symbols']((build / 'ntvdm.exe.map').read_text())
body, relocs = obj.sections[2]
actual = h['data_at'](image, table['va'], 324)
assert len(relocs) == len(table['nonnull']) == 78 and table['null_slots'] == [78, 79, 80]
for entry, (off, name, kind) in zip(table['nonnull'], relocs):
    assert off == entry['index'] * 4 and kind == 6 and name == entry['name']
    value = struct.unpack_from('<I', actual, off)[0]
    addend = struct.unpack_from('<I', body, off)[0]
    assert value == entry['va'] and any(s['name'] == name and s['owner'] == entry['owner']
        and ((s['va'] + addend) & 0xffffffff) == value for s in syms)
assert all(struct.unpack_from('<I', actual, index * 4)[0] == 0 for index in table['null_slots'])
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
contracts = {
    'cpu_c.h': 'Selected nt_cpu preprocessing emits 83 declaration lines',
    'pigger_c.h': 'Selected accessfn preprocessing emits 113 declaration lines; PIG capability is not inferred',
    'cpuint_c.h': 'Selected localfm preprocessing emits 35 declaration lines',
    'fm_c.h': 'Selected nt_cpu preprocessing emits 52 declaration lines',
    'fpu_c.h': 'Selected accessfn preprocessing emits 21 declaration lines',
    'vglfunc.c': 'Initial C_Video table has 78 bound original targets and three null entries',
}
proofs = []
for name, edge in contracts.items():
    path = 'softpc.new/base/cvidc/' + name
    row, f = ledger[path], frozen[path]
    assert row['status'] == 'pending'
    primary, secondary = Path('O:/repos.external/OpenNT/base/mvdm') / path, Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path
    assert H['sha'](Path('src/mvdm') / path) == H['sha'](primary) == H['sha'](secondary) == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    evidence = ('shard-1/parallel-cvidc-evidence.json' if name == 'vglfunc.c' else
                'shard-1/cvidc-preprocessed-excerpts.json' if name in ('cpu_c.h', 'pigger_c.h') else
                'shard-3/cvidc-header-durable-evidence.json')
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=f['current_sha256'], edition='OpenNT',
        original_path=str(primary), prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=evidence, original_edge=edge,
        limits='Declaration or initial vector only; ABI, actual behavior, dynamic state and lifecycle remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations or initial table do not establish complete runtime behavior',
        followup_stage='S11/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'parallel-cvidc-role-review.md',
    'Five selected C-VID declarations and one original vector owner remain partial.')
names = sorted({p['evidence_table'] for p in proofs})
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=n, sha256=H['sha'](A / n)) for n in names],
    host_input_observations=[], scope='Retained declaration excerpts and initial vector; no runtime behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
