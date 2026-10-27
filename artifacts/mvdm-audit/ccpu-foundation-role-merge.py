"""Apply nine reviewed foundation partial integrations; no full promotion."""
import collections
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '094c0cd96'
PREFIX = 'ccpu-foundation-role-'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
rows, sha = helpers['rows'], helpers['sha']
observed = runpy.run_path(str(A / 'ccpu-foundation-observer.py'))['inspect']()
assert observed == json.loads((A / 'ccpu-foundation-observations.json').read_text())
contracts = {
    'c_addr': ('Original decoder -> d_mem/address_add/limit_check; ModRM/SIB, default segments, address wrapping and POP displacement', 'Guest 16/32 address and segment combinations, POP ESP, cross-page and limit fault ordering'),
    'c_bsic': ('Original CPU/segment consumers -> descriptor, selector and FLAGS helpers; CPL/IOPL/PE/VM remain original state', 'Guest privilege/FLAGS matrix, descriptor page faults and cached undefined-shift policy'),
    'c_intr': ('Original interrupt dispatch -> do_intrupt; real IVT, IDT gates, TSS stack and V86 frame transitions', 'Guest IRQ/software INT, gate widths, privilege/task transitions, nested faults and return recovery'),
    'c_neg64': ('Original mul64/div64 -> neg64; actual object/native and signed-caller image correspondence', 'Guest IMUL/IDIV decoder, arithmetic exceptions and caller contracts; no null/alias promise'),
    'c_prot': ('Original interrupt/task/segment callers -> SS, TSS and privilege validation helpers', 'Guest selector failure precedence, mixed TSS/stack sizes and no partial state commit'),
    'c_stack': ('Original instruction/control callers -> stack access and old/new stack validation', 'Guest caller prevalidation, SP update/fault recovery, mixed widths, expand-down and page permissions'),
}
ledger = rows(A / 'combined-initial-status.csv')
index = {r['relative_path']: r for r in ledger}
proofs = []
for record in observed['records']:
    source = record['relative_path']
    chain, limit = contracts[Path(source).stem]
    for path in [source] + [h['relative_path'] for h in record['headers']]:
        old = index[path]
        assert old['status'] == 'pending'
        header = path.endswith('.h')
        original = Path('O:/repos.external/OpenNT/base/mvdm') / path
        digest = sha(Path('src/mvdm') / path)
        assert digest == sha(original)
        proofs.append(dict(relative_path=path, shard=old['shard'], sha256=digest,
            edition='OpenNT', original_path=str(original), prior_status='pending', status='partial',
            role='selected-native-source-partial', evidence_table='ccpu-foundation-observations.json',
            original_edge=chain + ('; original header consumed by VALID selected provider dependency' if header else '; selected graph, original archive and map-bound function bytes'),
            limits=limit + '; original declaration ABI and source invariants are not a substitute for guest behavior',
            original_text_retained_percent='100', estimated_restored_code_percent='',
            restored_estimate_status='unmeasurable: source/object/dependency integration and bounded native tests do not measure complete guest-execution/fault coverage',
            followup_stage='S11'))
        old['status'] = 'partial'
assert len(proofs) == 9


def write_csv(path, content):
    prior = subprocess.run(['git', 'show', BASE + ':' + path.as_posix()], capture_output=True).stdout
    ending = '\r\n' if b'\r\n' in prior.split(b'\n', 1)[0] + b'\n' else '\n'
    encoding = 'utf-8-sig' if prior.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w', encoding=encoding, newline='') as stream:
        writer = csv.DictWriter(stream, list(content[0]), lineterminator=ending)
        writer.writeheader(); writer.writerows(content)


write_csv(A / (PREFIX + 'dispositions.csv'), sorted(proofs, key=lambda r: r['relative_path']))
changed = {r['relative_path']: r for r in proofs}
for file, column in [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
                     ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]:
    content = rows(A / file)
    for row in content:
        if row['relative_path'] not in changed:
            continue
        proof = changed[row['relative_path']]
        row[column] = '部分复通' if column in ('classification', 'audit_status') else 'partial'
        row['estimated_restored_code_pct'] = ''
        if 'restored_estimate_status' in row:
            row['restored_estimate_status'] = proof['restored_estimate_status']
        for field in ('finding', 'role_or_prior_finding', 'decision_basis', 'review_basis'):
            if field in row:
                row[field] = 'Partial original source integration; ' + proof['original_edge']
        for field in ('evidence', 'reviewed_test_evidence', 'specific_evidence'):
            if field in row:
                row[field] = PREFIX + 'dispositions.csv; ccpu-core-source-review.md; ccpu-foundation-observations.json'
        for field in ('remaining_evidence', 'next_required_evidence', 'review_limits', 'remaining_proof'):
            if field in row:
                row[field] = proof['limits']
    write_csv(A / file, content)
write_csv(A / 'combined-initial-status.csv', ledger)
summary = json.loads((A / 'initial-summary.json').read_text())
summary['counts'] = dict(collections.Counter(r['status'] for r in ledger))
summary['note'] = 'Nine original CPU foundation source/header integrations are partial; no new full acceptance, mirror change or runtime image.'
(A / 'initial-summary.json').write_text(json.dumps(summary, indent=2) + '\n')
prior = json.loads((A / 'selected-ccpu-header-role-observations.json').read_text())
observation = dict(result='pass', base=BASE, candidate_count=9,
    source_tables=[dict(path='ccpu-foundation-observations.json', sha256=sha(A / 'ccpu-foundation-observations.json'))],
    host_input_observations=prior['host_input_observations'], scope='Partial original source integration, not complete guest capability acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
helpers['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 9)
