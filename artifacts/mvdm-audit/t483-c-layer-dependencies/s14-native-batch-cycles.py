"""Identify compiled dependency cycles; neither runtime coverage nor migration approval."""
import argparse
import hashlib
import json
from pathlib import Path
import sys

arguments = argparse.ArgumentParser()
arguments.add_argument('closure', type=Path)
arguments.add_argument('output', type=Path)
args = arguments.parse_args()
source = args.closure.read_bytes()
data = json.loads(source)
graph = {(obj, int(section)): set()
         for obj, sections in data['section_symbols'].items() for section in sections}
for obj, section, symbol, target, target_section in data['edges'] + data['local_edges']:
    graph[(obj, section)].add((target, target_section))
sys.setrecursionlimit(max(2000, len(graph) * 2))
indices, low, stack, active, components = {}, {}, [], set(), []


def visit(node):
    indices[node] = low[node] = len(indices)
    stack.append(node)
    active.add(node)
    for target in sorted(graph[node]):
        if target not in indices:
            visit(target)
            low[node] = min(low[node], low[target])
        elif target in active:
            low[node] = min(low[node], indices[target])
    if low[node] != indices[node]:
        return
    component = []
    while True:
        member = stack.pop()
        active.remove(member)
        component.append(member)
        if member == node:
            break
    if len(component) > 1:
        components.append(component)


for node in sorted(graph):
    if node not in indices:
        visit(node)
rows = []
for component in sorted(components, key=lambda c: (-len(c), sorted(c))):
    members = set(component)
    rows.append({
        'sections': len(component),
        'objects': sorted({obj for obj, section in component}),
        'symbols': sorted({name for obj, section in component
                           for name in data['section_symbols'][obj][str(section)]}),
        'root_symbols': sorted(name for name, node in data['roots'].items()
                               if tuple(node) in members),
    })
result = {
    'input_sha256': hashlib.sha256(source).hexdigest(),
    'method': 'Strongly connected selected COFF sections, including same-object references.',
    'limits': 'Static coupling only: not runtime coverage or permission to move entire files. '
              'Unresolved imports and dynamic callbacks are outside this graph.',
    'reachable_sections': len(graph),
    'cyclic_components': rows,
}
args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8', newline='\n')
print(f'{len(rows)} cyclic components; largest: {rows[0]["sections"]} sections, '
      f'{len(rows[0]["objects"])} objects, {len(rows[0]["root_symbols"])} public roots')
