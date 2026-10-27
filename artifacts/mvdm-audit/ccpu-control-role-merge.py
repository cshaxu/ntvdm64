"""Reconcile nine partial control owners and the unused inline-NOP helper."""
import collections
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '400c2e392'
PREFIX = 'ccpu-control-role-'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
rows, sha = helpers['rows'], helpers['sha']
ledger = rows(A / 'combined-initial-status.csv')
index = {r['relative_path']: r for r in ledger}
cohort = {'cli', 'clts', 'wait', 'rsrvd', 'div', 'lidt', 'sidt', 'jcxz', 'str', 'nop'}
proofs = []
for record in rows(A / 'ccpu-remaining-c-source-review.csv'):
    name = record['relative_path']
    stem = Path(name).stem
    if stem not in cohort:
        continue
    old = index[name]
    assert old['status'] == 'pending'
    excluded = stem == 'nop'
    limits = record['remaining_evidence']
    if stem == 'str':
        limits += '; new guest witness covers real-mode rejection only, not the positive protected-mode helper'
    if excluded:
        limits = 'Only the unused standalone helper file role is excluded: original c_main opcode 90 remains selected and passes 32 guest register/flag checks; decoder file and general prefix/fault/lifetime acceptance remain separate'
    proofs.append(dict(relative_path=name, shard=old['shard'], sha256=record['source_sha256'],
        edition='OpenNT', original_path=record['original_path'], prior_status='pending',
        status='excluded' if excluded else 'partial',
        role='historical-inline-helper-source-only' if excluded else 'selected-native-source-partial',
        evidence_table='ccpu-control/observations.json; ccpu-control/image/result.json',
        original_edge=record['entry_and_owner'] + '; ' + record['reviewed_behavior'],
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('not-applicable: standalone empty helper not referenced by the selected decoder; original inline instruction retained' if excluded else 'unmeasurable: bounded real-guest and code-binding evidence does not measure full privilege, operand, exception and lifecycle coverage'),
        followup_stage='S11'))
    old['status'] = proofs[-1]['status']
assert len(proofs) == 10


def write_csv(path, content):
    raw = subprocess.run(['git', 'show', BASE + ':' + path.as_posix()], capture_output=True).stdout
    ending = '\r\n' if b'\r\n' in raw.split(b'\n', 1)[0] + b'\n' else '\n'
    encoding = 'utf-8-sig' if raw.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w', encoding=encoding, newline='') as stream:
        writer = csv.DictWriter(stream, list(content[0]), lineterminator=ending)
        writer.writeheader(); writer.writerows(content)


write_csv(A / (PREFIX + 'dispositions.csv'), proofs)
changed = {r['relative_path']: r for r in proofs}
for file, column in [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
                     ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]:
    content = rows(A / file)
    for row in content:
        if row['relative_path'] not in changed:
            continue
        proof = changed[row['relative_path']]
        row[column] = ({'partial': '部分复通', 'excluded': '整体排除'}[proof['status']]
                       if column in ('classification', 'audit_status') else proof['status'])
        row['estimated_restored_code_pct'] = ''
        if 'restored_estimate_status' in row:
            row['restored_estimate_status'] = proof['restored_estimate_status']
        for field in ('finding', 'role_or_prior_finding', 'decision_basis', 'review_basis'):
            if field in row:
                row[field] = proof['role'] + '; ' + proof['original_edge']
        for field in ('evidence', 'reviewed_test_evidence', 'specific_evidence'):
            if field in row:
                row[field] = 'ccpu-control-review.md; ' + proof['evidence_table']
        for field in ('remaining_evidence', 'next_required_evidence', 'review_limits', 'remaining_proof'):
            if field in row:
                row[field] = proof['limits']
    write_csv(A / file, content)
write_csv(A / 'combined-initial-status.csv', ledger)
summary = json.loads((A / 'initial-summary.json').read_text())
summary['counts'] = dict(collections.Counter(r['status'] for r in ledger))
summary['note'] = 'Nine CPU control owners partial after real-guest/code witnesses; unused standalone NOP helper excluded without excluding the original inline instruction; no new full/source/runtime change.'
(A / 'initial-summary.json').write_text(json.dumps(summary, indent=2) + '\n')
prior = json.loads((A / 'ccpu-foundation-role-observations.json').read_text())
observation = dict(result='pass', base=BASE, candidate_count=10,
    source_tables=[dict(path=p, sha256=sha(A / p)) for p in ('ccpu-remaining-c-source-review.csv', 'ccpu-control/observations.json', 'ccpu-control/records.json')],
    host_input_observations=prior['host_input_observations'],
    scope='Nine partial original CPU control integrations and one unused helper file role; no full capability acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
helpers['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 10)
