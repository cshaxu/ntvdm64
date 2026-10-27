"""Read-only external audit triage, not a runtime or whole-provider verdict."""
import argparse
import csv
import hashlib
import json
import subprocess
from collections import Counter
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
AUDIT = REPO / 'artifacts/mvdm-audit'
WOW = Path('O:/repos.hobby/ntvdm64-wow32-audit/management')
DOS = Path('O:/repos.hobby/ntvdm64-dos16-audit/management')


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    assert output.is_relative_to(REPO / 'build') and not output.exists()
    frozen_path = AUDIT / 'frozen-external-audit-rows.csv'
    assert sha(frozen_path) == '949D696E96F088A69EA437A41F15A09D50C8218BA29983A15A7B31E25AF95623'
    frozen = rows(frozen_path)
    catalog = {row['id']: row for row in rows(WOW / 'source-coverage.csv')}
    assert len(frozen) == len({r['id'] for r in frozen}) == 42963
    explicit = {r['id'] for r in rows(AUDIT / 'external-priority-boundaries.csv')}
    assert len(explicit) == 93
    dos = rows(DOS / 'source-file-catalog.csv')
    outside = [r for r in dos if not r['path'].replace('\\', '/').lower().startswith(
        ('base/mvdm/', 'nt/private/mvdm/'))]
    assert not outside
    scope = json.loads((WOW / 'seam-scope.json').read_text(encoding='utf-8-sig'))
    identities = {}
    source_roots = {}
    for edition in scope['editions']:
        root = Path(edition['root'])
        revision = subprocess.check_output(['git','-C',str(root),'rev-parse','HEAD'],text=True).strip()
        assert revision == edition['commit']
        entries = subprocess.check_output(['git','-C',str(root),'ls-files','--stage','-z']).decode('utf-8')
        tracked = {}
        for entry in entries.split('\0'):
            if entry:
                metadata, path = entry.split('\t',1)
                mode, blob, stage = metadata.split()
                tracked[path] = (blob, stage)
        identities[edition['edition']] = tracked
        source_roots[edition['edition']] = root
    overrides = {}
    for record in rows(AUDIT / 'shard-1/external-printer-recipe-examples.csv'):
        overrides[record['id']] = (record['actual_sha256'],record['bounded_role'])
    for record in rows(AUDIT / 'shard-3/external-device-provider-review.csv'):
        if record['bounded_disposition'] == 'provider-recipe-only':
            overrides[record['frozen_id']] = (record['original_sha256'],record['role_and_remaining'])
    assert len(overrides) == 9
    result = []
    for row in frozen:
        original = catalog[row['id']]
        assert row['path'] == original['path'] and row['edition'] == original['edition']
        blob, stage = identities[row['edition']][row['path']]
        assert blob == original['git_blob'] and stage == '0'
        if row['id'] in explicit:
            tier = 'explicit-boundary-review'
            assert original['relevance_disposition'] != 'provider_membership_pending'
        else:
            assert original['relevance_disposition'] == 'provider_membership_pending'
            tier = ('device-context-dependency-gated'
                    if original['role'] == 'device_provider_relevance_pending'
                    else 'host-envelope-unresolved')
        decision = 'pending-specific-contract-review'
        if row['id'] in overrides:
            expected, _ = overrides[row['id']]
            assert sha(source_roots[row['edition']] / row['path']) == expected.upper()
            decision = 'excluded-provider-recipe-file-only'
        result.append(dict(id=row['id'],source_role=original['role'],dependency_tier=tier,
                           disposition=decision,original_git_blob=blob))
    assert Counter(r['dependency_tier'] for r in result) == {
        'explicit-boundary-review':93,'device-context-dependency-gated':36736,
        'host-envelope-unresolved':6134}
    output.mkdir(parents=True)
    with (output / 'external-obligation-triage.csv').open('w',encoding='utf-8',newline='') as stream:
        writer = csv.DictWriter(stream,fieldnames=list(result[0]))
        writer.writeheader()
        writer.writerows(result)
    inputs = [frozen_path,WOW/'source-coverage.csv',WOW/'seam_registry.py',
              WOW/'seam-scope.json',DOS/'source-file-catalog.csv',
              AUDIT/'external-priority-boundaries.csv',Path(__file__)]
    summary = dict(result='pass',external_rows=len(result),
                   tiers=dict(Counter(r['dependency_tier'] for r in result)),
                   dispositions=dict(Counter(r['disposition'] for r in result)),
                   dos_catalog_rows=len(dos),dos_outside_mvdm_rows=len(outside),
                   original_git_index_identities_verified=len(result),
                   original_working_content_rehashed=len(overrides),
                   reviewed_recipe_ids=sorted(overrides),
                   inputs=[dict(path=str(p),sha256=sha(p)) for p in inputs],
                   limits=['Triage is not a call graph, runtime acceptance or bulk provider exclusion.',
                           'Git identities do not attest all current working file contents.',
                           'Context rows require a specific dependency before runtime admission; unknown relevance remains explicit.',
                           'Nine recipe file-role exclusions do not exclude guest or device functionality.'])
    (output/'summary.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(summary))


if __name__ == '__main__':
    main()
