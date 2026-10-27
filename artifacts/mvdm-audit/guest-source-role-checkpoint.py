"""Verify the bounded S10 guest-source-role checkpoint, not guest behavior."""
import argparse
import collections
import csv
import hashlib
import io
import json
import re
import runpy
import struct
import subprocess
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'd050fa4b4'


def rows(path, delimiter=','):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream, delimiter=delimiter))


def original_rows(path, base):
    data = subprocess.check_output(['git', 'show', f'{base}:{path.as_posix()}'])
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main(output, base=BASE, prefix='guest-source-role-', expected_count=187):
    frozen_path = A / 'frozen-file-reconciliation.csv'
    assert sha(frozen_path) == 'F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    merged_path = A / 'combined-initial-status.csv'
    merged = {r['relative_path']: r for r in rows(merged_path)}
    old = {r['relative_path']: r for r in original_rows(merged_path, base)}
    dispositions = rows(A / (prefix + 'dispositions.csv'))
    changed = {r['relative_path']: r for r in dispositions}
    assert len(dispositions) == len(changed) == expected_count
    assert len(frozen) == len(merged) == len(old) == 4684
    assert set(frozen) == set(merged) == set(old)
    delta = {p for p in merged if merged[p] != old[p]}
    assert delta == set(changed)
    supplemental = None
    if any(r['edition'] == 'OpenNT-src-2' for r in dispositions):
        supplement_summary = json.loads((A / 'supplemental-original-provenance-summary.json').read_text())
        assert sha(A / 'supplemental-original-provenance.csv') == supplement_summary['csv_sha256']
        assert sha(Path(supplement_summary['manifest'])) == supplement_summary['manifest_sha256'] == '84D314A2BBBE9B78304EDBFFBB92BDB7ABA6FD209010CC1FF0341AE761910EDF'
        supplemental = {r['relative_path']: r for r in rows(A / 'supplemental-original-provenance.csv')}
        source_manifest = {r['source_relative_path']: r for r in rows(Path(supplement_summary['manifest']), '\t')}
    for path, row in merged.items():
        assert int.from_bytes(hashlib.sha256(path.encode()).digest()[:8], 'big') % 4 == int(row['shard'])
        if path in changed:
            proof = changed[path]
            assert proof['status'] in ('excluded', 'partial')
            expected = dict(old[path], status=proof['status'])
            assert old[path]['status'] == changed[path]['prior_status'] == 'pending'
            assert row == expected
            current_hash = sha(Path('src/mvdm') / path)
            original_hash = sha(Path(proof['original_path']))
            assert current_hash == proof['sha256']
            assert proof['sha256'] == frozen[path]['current_sha256']
            if proof['edition'] == 'OpenNT-src-2':
                assert frozen[path]['upstream_origin'] == 'current-only' and not frozen[path]['opennt_sha256'] and not frozen[path]['opennt45_sha256']
                provenance = supplemental[path]
                record = source_manifest[path]
                assert original_hash == provenance['source_sha256'] == record['source_sha256'].upper()
                assert record['source_sha256'] == record['target_sha256']
                assert provenance['git_match'] in ('raw', 'CRLF-normalized')
                assert Path(provenance['source_path']).resolve() == Path(proof['original_path']).resolve()
                if proof['status'] == 'excluded':
                    assert provenance['provenance'] == 'verified-supplemental-original'
            else:
                assert original_hash in (frozen[path]['opennt_sha256'], frozen[path]['opennt45_sha256'])
            if proof['status'] == 'excluded':
                legacy_roles = ('historical-guest-build-source-file-only', 'historical-guest-library-build-artifact-only',
                                'historical-cpu-test-source-only', 'historical-empty-printscreen-source-only',
                                'historical-build-recipe-file-only')
                artifact_roles = ('historical-symbol-artifact-only', 'historical-host-build-artifact-only',
                                  'historical-guest-build-object-artifact-only', 'historical-source-index-artifact-only',
                                  'historical-generated-message-artifact-only')
                assert proof['role'] in legacy_roles + artifact_roles and current_hash == original_hash
                if proof['role'] in artifact_roles:
                    assert prefix == 'generated-message-role-'
                if proof['role'] == 'historical-guest-library-build-artifact-only':
                    assert prefix == 'guest-library-role-' and path.lower().endswith('.lib')
                if proof['role'] == 'historical-cpu-test-source-only':
                    assert prefix == 'native-source-role-' and path in (
                        'softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c')
                if proof['role'] == 'historical-empty-printscreen-source-only':
                    assert prefix == 'native-source-next-role-' and path == 'softpc.new/base/comms/prt_scrn.c'
                if proof['role'] == 'historical-build-recipe-file-only':
                    assert prefix == 'historical-build-role-'
            else:
                if prefix in ('native-source-role-', 'native-source-next-role-', 'selected-ccpu-header-role-'):
                    assert proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                else:
                    assert proof['role'] in ('active-export-declaration-mirror-diff', 'partially-integrated-export-template')
                    assert (current_hash != original_hash) == (proof['role'] == 'active-export-declaration-mirror-diff')
            assert proof['original_edge'] and proof['limits']
    counts = dict(collections.Counter(r['status'] for r in merged.values()))
    expected_counts = collections.Counter(r['status'] for r in old.values())
    expected_counts['pending'] -= expected_count
    expected_counts.update(r['status'] for r in dispositions)
    assert counts == expected_counts
    summary = json.loads((A / 'initial-summary.json').read_text())
    assert counts == summary['counts'] and summary['complete'] is False
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    translations = {'待核验': 'pending', '部分复通': 'partial', '整体排除': 'excluded',
                    '全复通': 'full', '全部复通': 'full'}
    covered = set()
    for shard, (name, column) in enumerate(specs):
        source_path = A / name
        previous = {r['relative_path']: r for r in original_rows(source_path, base)}
        for row in rows(source_path):
            path = row['relative_path']
            assert path not in covered and int(merged[path]['shard']) == shard
            covered.add(path)
            assert translations.get(row[column], row[column]) == merged[path]['status']
            if path not in changed:
                assert row == previous[path], path
    assert covered == set(merged)
    observation = json.loads((A / (prefix + 'observations.json')).read_text())
    assert observation['result'] == 'pass' and observation['candidate_count'] == expected_count
    for record in observation['source_tables']:
        assert sha(A / record['path']) == record['sha256']
    if prefix == 'selected-ccpu-header-role-':
        cohort=sorted(p for p,r in old.items() if r['status']=='pending' and
                      p.startswith('softpc.new/base/ccpu386/') and p.endswith('.h'))
        assert len(cohort)==observation['reviewed_cohort_count']==96
        table_path=A/'ccpu-selected-header-observations.csv'
        selection=rows(table_path)
        inspector=runpy.run_path(str(A/'ccpu-selected-header-extractor.py'))
        pp=observation['preprocessor_output']
        assert sha(Path(pp['path']))==pp['sha256']
        actual,seen=inspector['inspect_output'](Path(pp['path']),cohort)
        assert len(seen)==74
        for first,second in zip(selection,actual):
            assert first=={k:str(v) for k,v in second.items()}
        assert len(selection)==len(actual)==96
        selected={r['relative_path'] for r in selection if int(r['selected_nonblank_lines'])>0}
        assert selected==set(changed) and len(selected)==71
        assert all(merged[p]['status']=='pending' for p in set(cohort)-selected)
        for record in observation['normal_cache_inputs']:
            assert sha(Path(record['Path']))==record['Hash']
        for record in observation['configuration_review_inputs']:
            assert sha(Path(record['path']))==record['sha256']
        for proof in dispositions:
            assert proof['status']=='partial' and proof['original_text_retained_percent']=='100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
    if prefix == 'historical-build-role-':
        cohort = {p for p, r in old.items() if r['status'] == 'pending' and
                  re.fullmatch(r'(sources|dirs|makefil\w*(\..*)?|_objects\.mac)', Path(p).name, re.I)}
        assert cohort == set(changed) and len(cohort) == observation['reviewed_cohort_count'] == 36
        assert len(observation['active_source_manifests']) == len(set(observation['active_source_manifests'])) == 19
        assert not cohort.intersection(observation['active_source_manifests'])
        for record in observation['configuration_review_inputs']:
            assert sha(Path(record['path'])) == record['sha256']
        for record in observation['host_graph_observations']:
            assert sha(Path(record['path'])) == record['sha256']
            text = Path(record['path']).read_text().replace('\\', '/').lower()
            assert all('src/mvdm/' + p.lower() not in text for p in cohort)
        for proof in dispositions:
            assert proof['status'] == 'excluded' and proof['original_text_retained_percent'] == '100'
            assert proof['restored_estimate_status'].startswith('not-applicable:')
            assert not proof['estimated_restored_code_percent']
    if prefix == 'native-source-role-':
        reviewed = {}
        for shard, record in enumerate(observation['source_tables'], 1):
            proposal = rows(A / f'shard-{shard}/native-pending-owner-review.csv')
            cohort = sorted(r['relative_path'] for r in proposal if old[r['relative_path']]['status'] == 'pending')[:30]
            batch = rows(A / record['path'])
            assert sorted(r['relative_path'] for r in batch) == cohort and len(batch) == 30
            for row in batch:
                path = row['relative_path']
                assert path not in reviewed and int(merged[path]['shard']) == shard
                assert sha(Path('src/mvdm') / path) == row['current_sha256'] == row['frozen_sha256']
                originals = [Path(p.strip()) for p in row['original_path'].split(';')]
                assert any(p.is_file() and sha(p) == row['original_sha256'] for p in originals)
                assert row['original_sha256'] == row['current_sha256']
                reviewed[path] = row
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('partial', 'excluded-file-role-only')}
        assert accepted == set(changed) and len(reviewed) == observation['reviewed_cohort_count'] == 90
        assert len(reviewed.keys() - accepted) == observation['remaining_cohort_pending'] == 52
        assert all(merged[p]['status'] == 'pending' for p in reviewed.keys() - accepted)
        for path in ('softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c'):
            text = (Path('src/mvdm') / path).read_text()
            assert re.search(r'#\s*if(?:def\s+PIG|\s+defined\(PIG\))', text)
        for record in observation['host_graph_observations']:
            graph = Path(record['path'])
            assert sha(graph) == record['sha256']
            text = graph.read_text()
            flags = [line for line in text.splitlines()
                     if line.startswith(('host_cflags = ', 'cflags = ', 'ccpu_flags = '))]
            assert flags and all('/DCPU_40_STYLE' in line and
                                 not re.search(r'/DPIG(?:\s|=|$)', line) for line in flags)
            assert all(not re.search(r'/DPIG(?:\s|=|$)', line) for line in text.splitlines()
                       if line.startswith('release_cflags = '))
            if any(line.startswith('ccpu_flags = ') for line in flags):
                rule = re.search(r'(?m)^rule cc_ccpu\r?\n([^\n]+)', text)
                assert rule and '$ccpu_flags' in rule[1] and '$release_cflags' in rule[1]
                assert all(f'build obj/ccpu/{name}.obj: cc_ccpu ' in text
                           for name in ('c_getset', 'ccpupig'))
        objects = observation['compiled_empty_objects']
        assert {r['relative_path'] for r in objects} == {
            'softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c'}
        for record in objects:
            obj = Path(record['object_path'])
            assert sha(obj) == record['object_sha256']
            assert sha(A / record['report_path']) == record['report_sha256']
            data = obj.read_bytes()
            machine, sections = struct.unpack_from('<HH', data)
            optional = struct.unpack_from('<H', data, 16)[0]
            assert machine == 0x14c and 0 < sections < 32 and optional == 0
            for i in range(sections):
                offset = 20 + i * 40
                size = struct.unpack_from('<I', data, offset + 16)[0]
                flags = struct.unpack_from('<I', data, offset + 36)[0]
                assert not size or not flags & (0x20 | 0x20000000)
    if prefix == 'native-source-next-role-':
        cohort_old = {r['relative_path']: r for r in original_rows(merged_path, observation['cohort_base'])}
        reviewed = {}
        for shard, record in enumerate(observation['source_tables'], 1):
            proposal = rows(A / f'shard-{shard}/native-pending-owner-review.csv')
            cohort = sorted(r['relative_path'] for r in proposal if cohort_old[r['relative_path']]['status'] == 'pending')[30:90]
            batch = rows(A / record['path'])
            assert sorted(r['relative_path'] for r in batch) == cohort and len(batch) == 60
            for item in batch:
                name = item['relative_path']
                assert name not in reviewed and int(old[name]['shard']) == shard
                assert sha(Path('src/mvdm') / name) == item['current_sha256'] == item['frozen_sha256']
                assert sha(Path(item['original_path'])) == item['original_sha256'] == item['current_sha256']
                reviewed[name] = item
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('partial', 'excluded-file-role-only')}
        assert accepted == set(changed) and len(reviewed) == observation['reviewed_cohort_count'] == 180
        assert len(reviewed.keys() - accepted) == observation['remaining_cohort_pending'] == 132
        assert all(merged[p]['status'] == 'pending' for p in reviewed.keys() - accepted)
        for record in observation['host_graph_observations']:
            assert sha(Path(record['path'])) == record['sha256']
        map_record = observation['selected_product_map']
        assert sha(Path(map_record['path'])) == map_record['sha256']
        selected_map = Path(map_record['path']).read_text().lower()
        dependencies = '\n'.join(Path(r['path']).read_text().replace('\\', '/').lower()
                                 for r in observation['host_input_observations'])
        graph = Path(observation['host_graph_observations'][0]['path']).read_text().replace('\\', '/').lower()
        for name, proof in changed.items():
            if name.endswith('.h'):
                assert 'src/mvdm/' + name.lower() in dependencies
            elif proof['status'] == 'partial':
                obj = Path(name).stem.lower() + '.obj'
                assert ':' + obj in selected_map and 'src/mvdm/' + name.lower() in graph
            assert proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
        empty = observation['compiled_empty_object']
        data = Path(empty['path']).read_bytes()
        assert sha(Path(empty['path'])) == empty['sha256']
        machine, sections = struct.unpack_from('<HH', data)
        assert machine == 0x14c and 0 < sections < 32 and struct.unpack_from('<H', data, 16)[0] == 0
        for i in range(sections):
            offset = 20 + i * 40
            size = struct.unpack_from('<I', data, offset + 16)[0]
            flags = struct.unpack_from('<I', data, offset + 36)[0]
            assert not size or not flags & (0x20 | 0x20000000)
        empty_name = 'softpc.new/base/comms/prt_scrn.c'
        empty_text = (Path('src/mvdm') / empty_name).read_text()
        assert not re.sub(r'/\*.*?\*/', '', empty_text, flags=re.S).strip()
        assert 'bios4.rom' in empty_text and 'src/mvdm/' + empty_name in graph
    if prefix in ('guest-declaration-resource-role-', 'generated-message-role-', 'shared-guest-include-role-'):
        generated = prefix == 'generated-message-role-'
        shared = prefix == 'shared-guest-include-role-'
        expected_reviewed, expected_remaining = (76, 0) if generated else (104, 60) if shared else (434, 305)
        extensions = {'.h', '.inc', '.mac', '.rc', '.rcv', '.dlg', '.bmp', '.ico', '.cur', '.bms', '.def', '.msg', '.skl', '.ext'}
        cohort = {p for p, r in old.items() if r['status'] == 'pending'
                  and (re.search(r'\.(CL[0-9A-Z]|CTL|pch|sym|OBJ|tmp)$', p, re.I) if generated
                       else p.startswith(('wow16/inc/', 'dos/v86/h/', 'dos/v86/inc/')) and Path(p).suffix.lower() in ('.h', '.inc', '.mac') if shared
                       else p.startswith(('dos/', 'wow16/')) and Path(p).suffix.lower() in extensions)}
        reviewed = {}
        for shard, record in enumerate(observation['source_tables']):
            for row in rows(A / record['path']):
                path = row['relative_path']
                assert path not in reviewed and int(old[path]['shard']) == shard
                assert sha(Path('src/mvdm') / path) == row['sha256'] == frozen[path]['current_sha256']
                originals = [Path(p.strip()) for p in row['original_path'].split(';')]
                assert any(p.is_file() and sha(p) == row['sha256'] for p in originals)
                reviewed[path] = row
        assert set(reviewed) == cohort and len(cohort) == observation['reviewed_cohort_count'] == expected_reviewed
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('excluded', 'excluded-file-role-only')}
        if generated:
            overrides = {r['relative_path']: r for r in observation['main_overrides']}
            assert set(overrides) == cohort - accepted and all(r['basis'] for r in overrides.values())
            accepted |= set(overrides)
        assert accepted == set(changed)
        assert len(cohort - accepted) == observation['remaining_cohort_pending'] == expected_remaining
        assert all(merged[p]['status'] == 'pending' for p in cohort - accepted)
        if shared:
            for record in observation['host_graph_observations']:
                assert sha(Path(record['path'])) == record['sha256']
    if prefix == 'generated-message-role-':
        format_path = A / observation['format_observations_path']
        assert sha(format_path) == observation['format_observations_sha256']
        formats = json.loads(format_path.read_text())
        script = A / 'generated-message-format-observations.py'
        assert formats['script_sha256'] == sha(script) and formats['paths'] == 76
        for record in formats['references']:
            assert sha(Path(record['path'])) == record['sha256']
        readers = runpy.run_path(str(script))
        assert {r['relative_path'] for r in formats['records']} == set(changed)
        for record in formats['records']:
            path = Path('src/mvdm') / record['relative_path']
            assert record['observation'] == 'pass' and sha(path) == record['sha256']
            if record['format'] in ('empirical-16-bit-SYM', 'OMF'):
                reader = 'inspect_sym' if record['format'] == 'empirical-16-bit-SYM' else 'inspect_omf'
                assert readers[reader](path.read_bytes()) == record['details']
            if 'compared_source_sha256' in record:
                assert sha(Path(record['compared_source'])) == record['compared_source_sha256']
        archive = Path(observation['archive_path'])
        assert sha(archive) == observation['archive_sha256']
        with zipfile.ZipFile(archive) as z:
            archive_hashes = {hashlib.sha256(z.read(i)).hexdigest().upper() for i in z.infolist() if not i.is_dir()}
        package_hashes = set()
        for record in observation['package_files']:
            assert sha(Path(record['path'])) == record['sha256']
            package_hashes.add(record['sha256'])
        assert all(r['sha256'] not in archive_hashes | package_hashes for r in dispositions)
    if prefix == 'guest-library-role-':
        assert observation['archive_count'] == 27 and observation['macro_source_count'] == 1
        assert sha(A / 'guest-library-format-observations.csv') == observation['format_observations_sha256']
        assert sha(A / 'guest-library-format-observations.json') == observation['format_details_sha256']
        formats = {r['relative_path']: r for r in rows(A / 'guest-library-format-observations.csv')}
        details = json.loads((A / 'guest-library-format-observations.json').read_text())
        assert details['result'] == 'pass' and details['paths'] == len(formats) == 32
        assert details['script_sha256'] == sha(A / 'guest-library-format-observations.py')
        assert details['nonzero_checksum_observations'] == 0
        assert set(observation['unresolved']) == set(formats) - set(changed)
        for path, proof in changed.items():
            fmt = formats[path]
            assert fmt['sha256'] == proof['sha256'] and fmt['checksum_nonzero_mismatches'] == '0'
            if proof['role'] == 'historical-guest-library-build-artifact-only':
                assert fmt['library_end'] == 'True' and int(fmt['modules']) > 0
                assert fmt['format'] == 'OMF LIBHDR/module framing/LIBEND/dictionary bounds'
            else:
                assert path == 'dos/v86/cmd/append/sysmac.lib' and fmt['modules'] == '0'
        supplements = {r['relative_path']: r for r in rows(A / 'supplemental-original-provenance.csv')}
        for record in observation['original_recipe_inputs']:
            path = record['relative_path']
            assert sha(Path('src/mvdm') / path) == record['sha256'] == frozen[path]['current_sha256']
            assert sha(Path(record['original_path'])) == record['sha256']
            if record['sha256'] not in (frozen[path]['opennt_sha256'], frozen[path]['opennt45_sha256']):
                source = supplements[path]
                assert frozen[path]['upstream_origin'] == 'current-only' and source['provenance'] == 'verified-supplemental-original'
                assert source['source_sha256'] == record['sha256'] and Path(source['source_path']) == Path(record['original_path'])
    if prefix == 'dos-core-source-role-':
        definition = json.loads((A / 'supplemental-export-definition-review.json').read_text())
        assert definition['result'] == 'pass' and definition['template_declared_names'] == 140 and definition['matched_profile_names'] == 138
        assert definition['missing_profile_names'] == ['VdmQueryFreeVirtualMemory', 'VdmReallocateVirtualMemory']
        assert definition['retained_definition_lines'] == definition['original_definition_lines'] == 136
        for record in definition['inputs']:
            assert sha(Path(record['path'])) == record['sha256']
    for record in observation['host_input_observations']:
        path = Path(record['path'])
        assert sha(path) == record['sha256'] and '(STALE)' not in path.read_text()
        text = path.read_text().replace('\\', '/').lower()
        for relative in changed:
            if changed[relative]['status'] == 'excluded' and changed[relative]['role'] not in ('historical-cpu-test-source-only', 'historical-empty-printscreen-source-only'):
                assert 'src/mvdm/' + relative.lower() not in text, relative
    for row in rows(A / 'full-selected-leaf.csv') + rows(A / 'full-selected-declarations.csv'):
        assert merged[row['relative_path']]['status'] == 'full'
        assert sha(Path('src/mvdm') / row['relative_path']) == row['source_sha256']
    drift = []
    for path, row in frozen.items():
        source = Path('src/mvdm') / path
        if not source.exists():
            assert not row['current_sha256']
            continue
        current = sha(source)
        if current != row['current_sha256']:
            drift.append(dict(relative_path=path, frozen=row['current_sha256'], current=current))
    assert drift == summary['live_source_hash_drift']
    result = dict(result='pass', base=base, counts=counts, source_role_changes=expected_count,
                  unrelated_shard_rows_unchanged=True, frozen_inventory_unchanged=True,
                  product_or_guest_changed_by_checkpoint=False,
                  scope='Source roles, identities, recorded input absence and ledger consistency; no guest capability acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--base', default=BASE)
    parser.add_argument('--prefix', choices=('guest-source-role-', 'kernel31-source-role-', 'dos-core-source-role-', 'wow16-core-source-role-', 'wow16-extension-source-role-', 'guest-library-role-', 'wow16-support-source-role-', 'guest-declaration-resource-role-', 'generated-message-role-', 'shared-guest-include-role-', 'native-source-role-', 'native-source-next-role-', 'historical-build-role-', 'selected-ccpu-header-role-'), default='guest-source-role-')
    parser.add_argument('--expected-count', type=int, default=187)
    args = parser.parse_args()
    main(args.output, args.base, args.prefix, args.expected_count)
