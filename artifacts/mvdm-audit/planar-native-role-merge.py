"""Record three image-bound original planar source owners as partial."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'bea69f8b4', 'planar-native-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = runpy.run_path(str(A / 'planar-image-observer.py'))['inspect']()
assert observed == json.loads((A / 'planar-image-observations.json').read_text())
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
contracts = {
    'nt_cga.c': ('Original mode dispatch -> colour PAINTFUNCS -> original medium CGA LUT painter -> mutex-protected DIB -> invalidation',
                 'Three medium colour scales pass actual-object pixel/rectangle/guard tests; real text/mono and high-colour paths, mode switching, source geometry, publication failure and teardown remain S13. Explicit test traps do not implement those services.'),
    'nt_ega.c': ('Original colour PAINTFUNCS -> original medium/high planar painters -> original LUT initialization and nt_munge -> DIB invalidation',
                 'All six medium/high scale paths pass bounded actual-object mask/pitch/rectangle tests. Low-resolution, user-font/mono alternatives, real caller bounds, mode changes, concurrent updates, transport failure and retirement remain S13.'),
    'nt_munge.c': ('Original medium/high planar EGA painters -> three original interleaved-plane/LUT conversion bodies -> caller-owned DIB',
                   'All 16 plane masks and 256 sample patterns at three scales pass through real callers. Legal input geometry, lifetime of source/LUT/destination, concurrent plane-mask changes and real guest frame transport remain S13.'),
}
proofs = []
for row in observed['sources']:
    leaf = Path(row['relative_path']).name
    chain, limits = contracts[leaf]
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'], sha256=row['sha256'],
        edition='OpenNT', original_path=row['original_path'], prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table='planar-image-observations.json', original_edge=chain, limits=limits,
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: original text and all 40 mapped functions are retained, but bounded pixel cases do not measure complete guest mode/geometry/failure/retirement behavior',
        followup_stage='S13'))
assert len(proofs) == 3
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'planar-native-role-review.md',
    'Three original planar owners are partial: 40 image-bound functions, 33 dispatch slots and actual-object pixels. No original source or image changed; mode/caller/transport lifetimes remain S13.')
observation = dict(result='pass', base=BASE, candidate_count=3,
    source_tables=[dict(path='planar-image-observations.json', sha256=H['sha'](A / 'planar-image-observations.json'))],
    host_input_observations=[], scope='Original CGA/EGA/munge integration with bounded pixel contracts; no complete guest rendering or release qualification')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 3)
