"""Reconcile bounded original guest inputs and delivered image identities."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '02d23f2f7'
PREFIX = 'parallel-guest-delivery-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = ('dos-kernel-country', 'dos-include-owner', 'dos-command-package', 'win16-comm-guest')
tables = []
for name in groups:
    folder = A / 'shard-2'
    table = folder / (name + '-observations.json')
    actual = runpy.run_path(str(folder / (name + '-observer.py')))['inspect']()
    assert actual == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))

# Exclusions are file roles only; all described guest behavior remains in scope.
contracts = {
    'dos/v86/doskrnl/bios/msbio.skl': ('excluded', 'historical-guest-build-source-file-only', 'dos-kernel-country', 'Original NTIO recipe uses this message skeleton as a guest build input', 'NTIO messages, localization, errors and image provenance remain open.'),
    'dos/v86/doskrnl/dos/msdos.skl': ('excluded', 'historical-guest-build-source-file-only', 'dos-kernel-country', 'Original NTDOS recipe uses this message skeleton as a guest build input', 'DOS diagnostics, localization, errors and image provenance remain open.'),
    'dos/v86/doskrnl/dos/dos.tag': ('excluded', 'historical-source-note-only', 'dos-kernel-country', 'Original file is a historical DOS change log', 'DOS kernel behavior and errors remain in scope.'),
    'dos/v86/inc/oemnum.inc': ('excluded', 'historical-source-note-only', 'dos-include-owner', 'Seventeen original lines contain only documentation and comments', 'OEM identity and GetVersion behavior remain in scope.'),
    'dos/v86/dev/country/nls863.inc': ('partial', 'selected-guest-data-partial', 'dos-kernel-country', 'Exact source collation bytes occur in packaged and installed COUNTRY.SYS', 'Runtime code-page selection, other tables, errors and image provenance remain open.'),
    'dos/v86/cmd/loadfix/LOADFIX.COM': ('partial', 'selected-guest-image-partial', 'dos-command-package', 'Original guest image matches archive and installed image', 'Runtime selection, memory placement, failure and output remain open.'),
    'dos/v86/cmd/mem/mem.exe': ('partial', 'selected-guest-image-partial', 'dos-command-package', 'OpenNT-4.5 guest image matches archive and installed image', 'Runtime selection, DOS memory accounting and error paths remain open.'),
    'dos/v86/cmd/qbasic/edit.com': ('partial', 'selected-guest-image-partial', 'dos-command-package', 'Original guest image matches archive and installed image', 'Editor input, files, shell transitions and exit remain open.'),
    'dos/v86/cmd/qbasic/edit.hlp': ('partial', 'selected-guest-image-partial', 'dos-command-package', 'Original help image matches archive and installed image', 'Runtime help selection, encoding and navigation remain open.'),
    'wow16/drivers/comm/comm.drv': ('partial', 'selected-guest-image-partial', 'win16-comm-guest', 'Original COMM driver image matches archive and installed image', 'Guest loading, serial behavior, CTS, failure and lifecycle remain open.'),
}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (status, role, group, edge, limits) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and row['shard'] == '2'
    edition = 'OpenNT' if source['current_sha256'] == source['opennt_sha256'] else 'OpenNT-4.5'
    assert source['current_sha256'] == source['opennt_sha256' if edition == 'OpenNT' else 'opennt45_sha256']
    root = 'O:/repos.external/OpenNT/base/mvdm' if edition == 'OpenNT' else 'O:/repos.external/OpenNT-4.5/nt/private/mvdm'
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'], edition=edition,
        original_path=str(Path(root) / path), prior_status='pending', status=status, role=role,
        evidence_table='shard-2/' + group + '-observations.json', original_edge=edge, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('not-applicable: historical guest build or documentation file role only' if status == 'excluded'
            else 'unmeasurable: guest data or image identity does not prove runtime behavior'),
        followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Guest image/data identities are partial; historical build-input and documentation exclusions do not exclude guest behavior.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Guest source roles and packaged identities only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
