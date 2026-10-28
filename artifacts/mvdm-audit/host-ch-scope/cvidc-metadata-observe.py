"""Observe the original C-VID CPU vector names and selected generated input."""
import hashlib
import json
import pathlib
import re

rel = 'softpc.new/base/cvidc/c2cpusad.h'
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
inputs = [pathlib.Path(r) / rel for r in roots]
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
metadata = inputs[0].read_text()
groups = re.findall(r'char\s*\*\s*(\w+)\[\]\s*=\s*\{([\s\S]*?)\};', metadata)[:2]
names = [[name for name in re.findall(r'"([A-Za-z0-9_]+)"', body)] for _, body in groups]
generated = pathlib.Path('build/.ninja/msvc-x86-release/worker/generated/cvidc_cpu_binding.inc')
actual = re.findall(r'\bX\((\w+),', generated.read_text())
generator = pathlib.Path('tools/build/GenerateCvidcCpuBinding.mjs')
builder = pathlib.Path('tools/build/New-T310OriginalSoftpcNinja.ps1')
binding = pathlib.Path('src/ntvdm-exe/softpc/mvdm_cvidc_vector_binding.c')
graph = pathlib.Path('build/.ninja/msvc-x86-release/worker/build.ninja')
checks = {'generator_reads_metadata': "'cvidc', 'c2cpusad.h'" in generator.read_text(),
          'builder_invokes_generator': '$NodeExecutable $cvidcCpuBindingGenerator' in builder.read_text(encoding='utf-8', errors='replace'),
          'binding_includes_generated': '#include "cvidc_cpu_binding.inc"' in binding.read_text(),
          'graph_builds_binding': 'obj/cvidc/mvdm_cvidc_vector_binding.obj: cc_cvidc_rule' in graph.read_text(),
          'graph_selects_object': any('original-softpc-cvidc.lib:' in line and
                                      'obj/cvidc/mvdm_cvidc_vector_binding.obj' in line
                                      for line in graph.read_text().splitlines()),
          'all_ordered_names_match': sum(names, []) == actual}
print(json.dumps({'inputs': [{'path': str(p).replace('\\', '/'), 'sha256': sha(p)}
                              for p in inputs + [generated, generator, builder, binding, graph]],
                  'groups': [{'name': groups[i][0], 'count': len(names[i])} for i in range(len(groups))],
                  'generated_slots': len(actual), 'checks': checks,
                  'limits': 'Names and build selection only; provider semantics, null slots and runtime vector behavior are not proved.'}, indent=2))
