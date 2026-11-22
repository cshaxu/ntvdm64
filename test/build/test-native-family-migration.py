"""Compare actual pre/post migration commands, without building a second graph."""
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import subprocess


def commands(tree, baseline=None):
    graph = baseline or tree / 'build.ninja'
    targets = re.findall(r'^build ([^ :]+\.exe)(?::| \| )', graph.read_text(), re.M)
    args = ['ninja', '-C', str(tree)]
    if baseline:
        args += ['-f', str(baseline.resolve())]
    return subprocess.check_output(args + ['-t', 'commands', *targets], text=True)


def inventory(text, tree):
    result = {'compile': Counter(), 'archive': Counter(), 'link': Counter()}
    for line in text.splitlines():
        match = re.search(r'(?:^|[ /\\"])(cl|lib|link)\.exe"?\s+(.+)', line, re.I)
        if not match:
            continue
        kind, args = match.groups()
        args = args.split(' && ')[0]
        tokens = re.findall(r'(?:[^\s"]|"[^"]*")+', args)
        tokens = [s.replace('"', '').replace('\\', '/').lower() for s in tokens]
        prefix = str(tree.resolve()).replace('\\', '/').lower() + '/'
        tokens = [s.replace(prefix, '') for s in tokens]
        tokens = [re.sub(r'[^\s]*/worker-reference-x86/generated/', 'generated/', s) for s in tokens]
        if kind.lower() == 'cl':
            source = tokens[-1]
            runtime = tuple(sorted({s.lstrip('/-') for s in tokens
                                    if s in ('/mt', '-mt', '/mtd', '-mtd',
                                             '/md', '-md', '/mdd', '-mdd')}))
            # CMake adds output names and MSVC dependency/PDB infrastructure;
            # ordered macro/include/forced-include flags remain significant.
            flags = []
            index = 0
            while index < len(tokens) - 1:
                flag = tokens[index]
                if flag in ('/i', '-i', '/fi', '-fi'):
                    index += 1
                    flag += tokens[index]
                if not flag.startswith(('/fo', '/fd')) and flag not in (
                        '/fs', '/showincludes', '/c', '-c', '/nologo', '/mt', '-mt'):
                    flags.append(flag)
                index += 1
            result['compile'][(source, tuple(flags), runtime)] += 1
        else:
            result['archive' if kind.lower() == 'lib' else 'link'][tuple(tokens)] += 1
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--tree', type=Path, required=True)
    args = parser.parse_args()
    report = {}
    for component in ('worker', 'frontend', 'monitor', 'launcher', 'service'):
        tree = args.tree / component
        before = inventory(commands(tree, args.baseline / (component + '-before.ninja')), tree)
        after = inventory(commands(tree), tree)
        for kind in before:
            assert before[kind] == after[kind], (
                component, kind, 'removed', before[kind] - after[kind],
                'added', after[kind] - before[kind])
        report[component] = {kind: sum(rows.values()) for kind, rows in after.items()}
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
