"""Audit migrated worker commands; the reference graph is never executed."""
import argparse
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import sys

sys.dont_write_bytecode = True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--tree', type=Path, required=True)
    parser.add_argument('--report', type=Path, required=True)
    args = parser.parse_args()
    spec = importlib.util.spec_from_file_location(
        'migration', Path(__file__).with_name('test-native-family-migration.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    targets = re.findall(r'^build ([^ :]+\.(?:exe|dll))(?::| \| )',
                         args.baseline.read_text(), re.M | re.I)
    assert targets, 'No baseline images'

    def commands(baseline=False):
        command = ['ninja', '-C', str(args.tree)]
        if baseline:
            command += ['-f', str(args.baseline.resolve())]
        return subprocess.check_output(command + ['-t', 'commands', *targets], text=True)

    before = module.inventory(commands(True), args.tree)
    after = module.inventory(commands(), args.tree)
    # The old fixture closure omitted its existing RtlNtStatusToDosError import.
    # Account only for the explicit test-link repair, not any product change.
    for key, count in list(before['link'].items()):
        if '/out:ntvdm-command-capture-test.exe' in key and 'ntdll.lib' not in key:
            del before['link'][key]
            before['link'][key + ('ntdll.lib',)] += count
    report = {}
    for kind in before:
        missing, extra = before[kind] - after[kind], after[kind] - before[kind]
        report[kind] = dict(before=sum(before[kind].values()),
                            after=sum(after[kind].values()),
                            missing=[dict(key=k, count=v) for k, v in missing.items()],
                            extra=[dict(key=k, count=v) for k, v in extra.items()])
    args.report.write_text(json.dumps(report, indent=2) + '\n')
    assert not any(r['missing'] or r['extra'] for r in report.values()), report
    print(json.dumps({k: r['after'] for k, r in report.items()}))


if __name__ == '__main__':
    main()
