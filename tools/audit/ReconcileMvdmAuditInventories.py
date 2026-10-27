#!/usr/bin/env python3
"""Join frozen MVDM source identity with both read-only audit inventories.

This is an inventory aid, not a runtime-reachability or acceptance oracle.
Unknown rows stay pending; a source diff is always reported as partial.
"""
import argparse
import csv
import difflib
import hashlib
import json
from collections import Counter, defaultdict
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DOS = Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv')
WOW = Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-coverage.csv')
ORIGINAL = Path('O:/repos.external/OpenNT/base/mvdm')
ORIGINAL45 = Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')
TEXT_SUFFIXES = {'.c', '.h', '.asm', '.inc', '.def', '.rc', '.cxx', '.cpp'}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def relative(path):
    path = path.replace('\\', '/').lower()
    for prefix in ('base/mvdm/', 'nt/private/mvdm/'):
        if path.startswith(prefix):
            return path[len(prefix):]
    return None


def load_index(path):
    index = defaultdict(list)
    outside = []
    with path.open(encoding='utf-8-sig', newline='') as stream:
        for row in csv.DictReader(stream):
            key = relative(row['path'])
            if key is None:
                outside.append(row)
                continue
            index[key].append(row)
    return index, outside


def estimate(original, current):
    if original.suffix.lower() not in TEXT_SUFFIXES:
        return '', '', 'not-source-text'
    a, b = original.read_bytes(), current.read_bytes()
    if b'\0' in a or b'\0' in b:
        return '', '', 'binary-or-unknown-encoding'
    left, right = a.splitlines(), b.splitlines()
    if not left:
        return '', '0', 'empty-original'
    if len(left) > 20000 or len(right) > 20000:
        return '', str(len(left)), 'oversize-needs-manual-comparison'
    matches = sum(block.size for block in difflib.SequenceMatcher(
        None, left, right, autojunk=True).get_matching_blocks())
    return f'{100 * matches / len(left):.2f}', str(len(left)), 'matching-original-lines-heuristic'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--inventory', required=True)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    build = (REPO / 'build').resolve()
    inventory, output = Path(args.inventory).resolve(), Path(args.output).resolve()
    if not inventory.is_relative_to(build) or not output.is_relative_to(build):
        raise SystemExit('inventory and output must be under repository build')
    if output.exists():
        raise SystemExit('output directory must not already exist')
    dos, dos_outside = load_index(DOS)
    wow, wow_outside = load_index(WOW)
    with inventory.open(encoding='utf-8-sig', newline='') as stream:
        source = list(csv.DictReader(stream))
    rows = []
    for row in source:
        rel = row['relative_path']
        key = rel.lower()
        candidates = wow.get(key, [])
        current_hash = row['current_sha256']
        original_hashes = {row['opennt_sha256'], row['opennt45_sha256']} - {''}
        diff = bool(current_hash and original_hashes and current_hash not in original_hashes)
        selected = 'unresolved'
        percent = denominator = method = ''
        if diff:
            # The canonical union selects OpenNT where both editions exist;
            # edition-conflict choices still require independent adjudication.
            base = ORIGINAL if row['opennt_sha256'] else ORIGINAL45
            selected = 'OpenNT' if base == ORIGINAL else 'OpenNT-4.5'
            if row['upstream_origin'] == 'both-conflict':
                selected = 'conflict-unresolved'
            else:
                original = base / rel
                current = REPO / 'src/mvdm' / rel
                if original.is_file() and current.is_file():
                    percent, denominator, method = estimate(original, current)
                else:
                    method = 'path-identity-unresolved'
        rows.append({
            **row, 'dos_catalog_rows': len(dos.get(key, [])),
            'wow_catalog_rows': len(candidates),
            'wow_source_ids': ';'.join(sorted({item['id'] for item in candidates})),
            'reporting_class': 'partial-mirror-diff' if diff else 'pending-evidence',
            'selected_comparison_edition': selected, 'original_line_count': denominator,
            'original_line_similarity_pct': percent, 'similarity_method': method,
            # Similar text may be unlinked or unreachable; changed text may
            # preserve original behavior. Neither implies restored coverage.
            'estimated_restored_code_pct': '',
            'restored_estimate_status': 'pending-selection-and-semantic-review',
        })
    output.mkdir(parents=True)
    with (output / 'file-reconciliation.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    with (output / 'external-audit-rows.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=['audit', 'id', 'edition', 'path', 'status'])
        writer.writeheader()
        for label, external in (('dos16', dos_outside), ('wow32', wow_outside)):
            for item in external:
                writer.writerow({'audit': label, 'id': item.get('id', ''),
                    'edition': item.get('edition', ''), 'path': item['path'],
                    'status': 'pending-relevance-and-owner-review'})
    summary = {
        'input_sha256': {str(p): digest(p) for p in (inventory, DOS, WOW)},
        'rows': len(rows),
        'reporting_class': dict(Counter(r['reporting_class'] for r in rows)),
        'dos_matched_paths': sum(bool(r['dos_catalog_rows']) for r in rows),
        'wow_matched_paths': sum(bool(r['wow_catalog_rows']) for r in rows),
        'dos_outside_mvdm_rows': len(dos_outside),
        'wow_outside_mvdm_rows': len(wow_outside),
        'partial_line_similarity_measures': sum(bool(r['original_line_similarity_pct']) for r in rows),
        'limitations': 'Pending is not an exclusion; identity and line similarity do not prove build, dispatch or behavior. Outside-MVDM audit dependencies require separate reconciliation.',
    }
    (output / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(summary, ensure_ascii=False))


if __name__ == '__main__':
    main()
