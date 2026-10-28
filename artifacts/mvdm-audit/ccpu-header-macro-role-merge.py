"""Reconcile selected declarations/macros and the inactive PIG-test declaration."""
import collections
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '70985c440'
PREFIX = 'ccpu-header-macro-role-'
helpers = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
rows, sha = helpers['rows'], helpers['sha']
observed = runpy.run_path(str(A / 'ccpu-header-selection-extractor.py'))['inspect']()
assert observed == json.loads((A / 'ccpu-header-selection-observations.json').read_text())
contracts = {
    'c_div64.h': ('Original signed/unsigned two-word divide declaration -> selected divide helper TU', 'Actual caller/provider ABI, quotient/remainder ranges and guest zero/overflow recovery'),
    'c_tlb.h': ('Original TLB access constants and flush/invalidate/translation declarations -> selected TLB/page/transfer owners', 'Guest paging permissions, dirty/accessed bits, reset/flush and failure order'),
    'c_tsksw.h': ('Original nesting/return flags and switch_tasks declaration -> selected task-switch owner', 'Guest task gates, 16/32 TSS, nested return, partial commit and fault recovery'),
    'c_xfer.h': ('Original far-transfer classifications and validation/update declarations -> selected transfer owner', 'Descriptor cross-page faults, privilege/return ordering and diagnostic observations before validation'),
    'c_xtrn.h': ('Original CPU-call type and exception interface -> selected external CPU owner', 'Typed call/return ABI, host callback nesting and nonlocal failure recovery'),
    'fpu_c.h': ('Original FPU constants and FPSTACKENTRY/word-aggregate declarations -> selected register owner', 'Actual layout consumers, separate cfpu_def definitions and full x87 state/exception lifecycle; emitted types alone prove neither'),
    'newnpx.h': ('Original legacy parameter-unspecified x87 declarations and saved instruction/data pointer macros -> selected zfrsrvd', 'Match each real FPU definition and promoted call argument, operand/environment layouts and exception/restore behavior'),
    'c_oprnd.h': ('Original decode/fetch/commit/put macros -> selected main and x87 decoders; actual expanded npx save/restore prechecks retained', 'Resolve negative memory bit-index mismatch and x87 precheck/transfer lengths; guest permissions, string commit, wrap and fault ordering'),
    'fault.h': ('Original 147 fault-origin constants -> selected exception call sites; expanded GP arguments retain numeric owners', 'Guest fault origin/priority/nesting, precise saved state and disabled/enabled diagnostics'),
    'ccpupig.h': ('Original PIG-only cross-executor comparison declarations; included but no emitted tokens under selected non-PIG profile', 'Only inactive PIG test-header role excluded, consistent with earlier c_getset/ccpupig test-file dispositions; general CPU diagnostics and guest behavior remain in scope'),
}
ledger = rows(A / 'combined-initial-status.csv')
index = {r['relative_path']: r for r in ledger}
proofs = []
for record in observed['headers']:
    name = record['relative_path']
    leaf = Path(name).name
    if leaf not in contracts:
        continue
    old = index[name]
    assert old['status'] == 'pending' and record['valid_consumers'] and record['seen_units']
    excluded = leaf == 'ccpupig.h'
    chain, limits = contracts[leaf]
    original = Path('O:/repos.external/OpenNT/base/mvdm') / name
    assert sha(original) == record['source_sha256']
    proofs.append(dict(relative_path=name, shard=old['shard'], sha256=record['source_sha256'],
        edition='OpenNT', original_path=str(original), prior_status='pending', status='excluded' if excluded else 'partial',
        role='historical-cpu-test-declaration-only' if excluded else 'selected-native-source-partial',
        evidence_table='ccpu-header-selection-observations.json', original_edge=chain,
        limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status=('not-applicable: inactive declarations for the original nonselected differential CPU test' if excluded else 'unmeasurable: selected declarations, macro expansions and dependency inputs do not measure whole guest behavior or code coverage'),
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
                row[field] = 'ccpu-header-macro-review.md; ' + proof['evidence_table']
        for field in ('remaining_evidence', 'next_required_evidence', 'review_limits', 'remaining_proof'):
            if field in row:
                row[field] = proof['limits']
    write_csv(A / file, content)
write_csv(A / 'combined-initial-status.csv', ledger)
summary = json.loads((A / 'initial-summary.json').read_text())
summary['counts'] = dict(collections.Counter(r['status'] for r in ledger))
summary['note'] = 'Nine original CPU header/macro integrations partial; inactive PIG comparison-test declaration role excluded; twelve other header roles stay pending; no full/source/runtime change.'
(A / 'initial-summary.json').write_text(json.dumps(summary, indent=2) + '\n')
prior = json.loads((A / 'ccpu-control-role-observations.json').read_text())
observation = dict(result='pass', base=BASE, candidate_count=10,
    source_tables=[dict(path='ccpu-header-selection-observations.json', sha256=sha(A / 'ccpu-header-selection-observations.json'))],
    normal_cache_inputs=json.loads(Path('build/t473-s10/ccpu-header-selection/run/cache-inputs.json').read_text(encoding='utf-8-sig')),
    host_input_observations=prior['host_input_observations'], scope='Selected original header/macro integration, not whole-file guest acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
helpers['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 10)
