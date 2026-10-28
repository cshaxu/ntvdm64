"""Read-only identity and selected-object observations for alternate vglob."""
import hashlib
import json
import pathlib
import re

paths = [
    'src/mvdm/softpc.new/base/ccpu386/vglob.c',
    'src/mvdm/softpc.new/base/cpu/src/evid/vglob.c',
    'O:/repos.external/OpenNT/base/mvdm/softpc.new/base/ccpu386/vglob.c',
    'O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/base/ccpu386/vglob.c',
    'build/.ninja/msvc-x86-release/worker/obj/cvidc/vglob.obj',
    'build/.ninja/msvc-x86-release/worker/build.ninja',
    'build/.ninja/msvc-x86-release/worker/ntvdm.exe.map',
    'tests/mvdm-host/cvidc_vector_binding_fixture.c',
    'artifacts/mvdm-audit/cvidc-vector-fixture-r74-evidence.json',
]
files = [pathlib.Path(x) for x in paths]
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
graph = files[5].read_text(encoding='utf-8', errors='replace')
map_text = files[6].read_text(encoding='utf-8', errors='replace')
fixture = json.loads(files[8].read_text(encoding='utf-8'))
prior_fixture_input = next(x['sha256'] for x in fixture['inputs']
                           if x['path'] == 'tests/mvdm-host/cvidc_vector_binding_fixture.c')
selected = [line.strip() for line in map_text.splitlines()
            if re.search(r'\b_[gs]etVideo\w+\b', line) and 'original-softpc-cvidc:vglob.obj' in line]
print(json.dumps({'inputs': [{'path': p, 'sha256': sha(f)} for p, f in zip(paths, files)],
                  'source_bytes_equal': len({sha(x) for x in files[:4]}) == 1,
                  'selected_build_edge': next((x for x in graph.splitlines()
                     if x.startswith('build obj/cvidc/vglob.obj:')), None),
                  'selected_accessor_map_symbols': selected,
                  'prior_fixture_result': fixture['result'],
                  'current_fixture_matches_prior': sha(files[7]) == prior_fixture_input,
                  'prior_fixture_scope': fixture['scope'],
                  'limits': 'Selected source is a different physical path/profile; source identity and fixture do not establish original CCPU variant runtime or all device/lifecycle behavior.'}, indent=2))
