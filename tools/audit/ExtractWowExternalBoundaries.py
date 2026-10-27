#!/usr/bin/env python3
"""Extract explicitly identified non-MVDM audit boundaries, without deciding relevance."""
import csv
import hashlib
import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
AUDIT = REPO / 'artifacts/mvdm-audit'
CATALOG = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-coverage.csv')


def main():
    with CATALOG.open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    external = [r for r in rows if not r['path'].replace('\\', '/').lower().startswith(
        ('base/mvdm/', 'nt/private/mvdm/'))]
    with (AUDIT / 'frozen-external-audit-rows.csv').open(encoding='utf-8', newline='') as stream:
        frozen = list(csv.DictReader(stream))
    assert len(external) == len(frozen)
    assert {(r['id'], r['path']) for r in external} == {(r['id'], r['path']) for r in frozen}
    selected = [r for r in external if r['relevance_disposition'] != 'provider_membership_pending']
    fields = ['id', 'edition', 'original_path', 'original_sha256', 'audit_role',
              'audit_relevance', 'audit_reason', 'audit_reviewed_scope', 'project_candidates',
              'current_source_hashes', 'decision', 'remaining_proof']
    output = []
    mappings = {
        'nt/private/': ('src/opennt-host', 'src/opennt-abi/source/private'),
        'base/': ('src/opennt-host/base', 'src/opennt-abi/source/base'),
        'public/': ('src/opennt-host/public', 'src/opennt-abi/source/public'),
    }
    for row in selected:
        path = row['path'].replace('\\', '/')
        candidates = []
        for prefix, roots in mappings.items():
            if path.lower().startswith(prefix):
                candidates = [REPO / base / path[len(prefix):] for base in roots]
                candidates = [candidate for candidate in candidates if candidate.is_file()]
                break
        output.append({
            'id': row['id'], 'edition': row['edition'], 'original_path': path,
            'original_sha256': row['sha256'], 'audit_role': row['role'],
            'audit_relevance': row['relevance_disposition'], 'audit_reason': row['relevance_reason'],
            'audit_reviewed_scope': row['reviewed_scope'],
            'project_candidates': ';'.join(p.relative_to(REPO).as_posix() for p in candidates),
            'current_source_hashes': ';'.join(hashlib.sha256(p.read_bytes()).hexdigest().upper() for p in candidates),
            'decision': 'pending applicability and current owner review',
            'remaining_proof': 'Trace selected original slice and actual host/adapter boundary; do not import an entire NT4 subsystem or infer functional coverage from candidate presence.',
        })
    with (AUDIT / 'external-priority-boundaries.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(output)
    summary = {
        'catalog_path': CATALOG.as_posix(), 'catalog_sha256': hashlib.sha256(CATALOG.read_bytes()).hexdigest().upper(),
        'external_paths': len(external), 'upstream_explicit_boundary_records': len(output),
        'upstream_pending_provider_membership': len(external) - len(output),
        'direct_project_candidate_rows': sum(bool(r['project_candidates']) for r in output),
        'limitation': 'Audit source classifications are initial relevance hints, not automatically MVDM obligations, runtime evidence or exclusion decisions.',
    }
    (AUDIT / 'external-priority-boundaries-summary.json').write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    print(f'Checked {len(external)} external rows; {len(output)} have explicit source-audit boundary classifications.')


if __name__ == '__main__':
    main()
