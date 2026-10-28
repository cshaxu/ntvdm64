"""Verify historical DDEML-init receipt and current selected source boundary."""
import hashlib
import json
import pathlib

root = pathlib.Path('artifacts/mvdm-audit/shard-2')
receipt_path = root / 'ddeml-init-evidence.json'
receipt = json.loads(receipt_path.read_text(encoding='utf-8'))
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
checks = {'source_matches_receipt': sha(root / 'ddeml-init-probe.asm') == receipt['source_sha256'],
          'runner_matches_receipt': sha(root / 'ddeml-init-run.ps1') == receipt['runner_sha256']}
for run in receipt['runs']:
    case = run['case']
    checks[case + '_observer_matches'] = sha(root / ('ddeml-init-' + case + '.observer.txt')) == run['observer_sha256']
    checks[case + '_result_matches'] = sha(root / ('ddeml-init-' + case + '.result.json')) == run['receipt_sha256']
sources = [pathlib.Path('src/mvdm/wow32/' + x) for x in ('wowtbl.c', 'sources', 'wddetbl2.h', 'wddeml32.c', 'wddeml32.h')]
graph = pathlib.Path('build/.ninja/msvc-x86-release/wow32/build.ninja')
graph_text = graph.read_text(encoding='utf-8', errors='replace').lower()
print(json.dumps({'historical_receipt_sha256': sha(receipt_path), 'historical_checks': checks,
                  'historical_scope': receipt['scope'], 'historical_runs': receipt['runs'],
                  'source_hashes': {str(p).replace('\\', '/'): sha(p) for p in sources},
                  'graph_sha256': sha(graph),
                  'original_master_names_old_table': 'wddetbl2.h' in sources[0].read_text().lower(),
                  'original_sources_names_old_provider': 'wddeml32.c' in sources[1].read_text().lower(),
                  'current_graph_names_old_provider': 'wddeml32.c' in graph_text,
                  'limits': 'Historical receipt integrity and current static selection only; no callback, transaction, original guest source lineage or current install identity proved.'}, indent=2))
