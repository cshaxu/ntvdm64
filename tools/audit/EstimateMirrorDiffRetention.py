#!/usr/bin/env python3
"""Measure current original-code text retention for original mirror differences.

This is a source-line estimate, never a claim of behavioral completeness.
"""
import csv
import difflib
import hashlib
import json
import runpy
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
AUDIT = ROOT / 'artifacts/mvdm-audit'
ORIGINAL = Path('O:/repos.external/OpenNT/base/mvdm')
SECONDARY = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
HELPER = runpy.run_path(str(ROOT / 'tools/audit/EstimateOriginalCodeRetention.py'))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def main():
    ledger = {r['relative_path']: r for r in rows(AUDIT / 'combined-initial-status.csv')}
    provenance_file = AUDIT / 'supplemental-original-provenance.csv'
    provenance_summary = json.loads((AUDIT / 'supplemental-original-provenance-summary.json').read_text())
    assert sha(provenance_file) == provenance_summary['csv_sha256']
    provenance = {r['relative_path']: r for r in rows(provenance_file)}
    sources = [r for r in rows(AUDIT / 'frozen-file-reconciliation.csv')
               if r['opennt_sha256'] or r['opennt45_sha256'] or
               (r['relative_path'] in provenance and provenance[r['relative_path']]['source_path'])]
    records = []
    missing = []
    for entry in sources:
        path = entry['relative_path']
        primary = bool(entry['opennt_sha256'])
        secondary = not primary and bool(entry['opennt45_sha256'])
        edition = 'OpenNT' if primary else ('OpenNT-4.5' if secondary else 'OpenNT-src-2')
        supplemental = provenance.get(path)
        old = (ORIGINAL / path if primary else SECONDARY / path if secondary
               else Path(supplemental['source_path']))
        current = ROOT / 'src/mvdm' / path
        if not current.is_file():
            missing.append(dict(relative_path=path, status=ledger[path]['status']))
            continue
        expected_hash = (entry['opennt_sha256'] if primary else entry['opennt45_sha256'] if secondary
                         else supplemental['source_sha256']).upper()
        current_hash = sha(current)
        if current_hash == expected_hash:
            continue
        original_hash = sha(old)
        assert original_hash == expected_hash, path
        assert ledger[path]['status'] == 'partial', path
        kind = current.suffix.lower()
        if kind not in ('.c', '.h', '.def', '.asm'):
            records.append(dict(relative_path=path, original_edition=edition, original_sha256=original_hash,
                current_sha256=current_hash, frozen_current_sha256=entry['current_sha256'],
                original_code_lines='', matched_code_lines='', text_retention_percent='',
                limitation='Non-source build input; original-code percentage not applicable.'))
            continue
        before, after = HELPER['source_lines'](old), HELPER['source_lines'](current)
        matched = sum(n for _, _, n in difflib.SequenceMatcher(None, before, after, autojunk=True).get_matching_blocks())
        records.append(dict(relative_path=path, original_edition=edition, original_sha256=original_hash,
            current_sha256=current_hash, frozen_current_sha256=entry['current_sha256'],
            original_code_lines=len(before), matched_code_lines=matched,
            text_retention_percent=f'{100 * matched / len(before):.2f}' if before else '',
            limitation='Matched original source lines after comment removal; not runtime or branch coverage.'))
    assert records and len({r['relative_path'] for r in records}) == len(records)
    output = AUDIT / 'mirror-diff-code-retention.csv'
    with output.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(records[0]), lineterminator='\n')
        writer.writeheader()
        writer.writerows(records)
    summary = dict(result='pass', count=len(records), measured=sum(bool(r['text_retention_percent']) for r in records),
                   absent_current_files=missing,
                   current_drift=[r['relative_path'] for r in records if r['current_sha256'] != r['frozen_current_sha256']],
                   csv_sha256=sha(output), frozen_sha256=sha(AUDIT / 'frozen-file-reconciliation.csv'),
                   supplemental_provenance_sha256=sha(provenance_file),
                   scope='Original code text estimate, not executed code or behavioral restoration')
    (AUDIT / 'mirror-diff-code-retention-summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))


if __name__ == '__main__':
    main()
