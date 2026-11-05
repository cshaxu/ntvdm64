"""Assert the actual guest allocation/loader on-off control, not a model."""
import argparse
import json
from pathlib import Path


def blocks(lines, key):
    result = {}
    current = None
    for line in lines:
        name, _, value = line.partition('=')
        if name == key:
            current = value
            result[current] = {}
        elif current is not None:
            result[current][name] = value
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--off', type=Path, required=True)
    parser.add_argument('--on', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    logs = []
    for root in (args.off, args.on):
        result = json.loads((root / 'result.json').read_text(encoding='utf-8-sig'))
        assert result['diagnosticComplete'], result
        lines = (root / 'MENU95.LOG').read_bytes().decode('cp936').splitlines()
        assert 'VERSION=C0000004' in lines
        assert 'LOADER_DIAGNOSTIC_COMPLETE=1' in lines
        logs.append((result, lines))
    assert logs[0][0]['paging'] == 'Off'
    assert logs[1][0]['paging'] == 'On'
    for field in ('probeSha256', 'workerSha256'):
        assert logs[0][0][field] == logs[1][0][field], field
    for enabled, (_, lines) in enumerate(logs):
        matrix = blocks(lines, 'MATRIX_SIZE')
        assert len(matrix) == 6
        for size, values in matrix.items():
            assert int(values['RESERVE'], 16) != 0
            assert values['COMMIT_FIRST_PAGE'] == '00000001'
            expected = enabled or int(size, 16) <= 524288
            assert bool(int(values['COMMIT_FULL'], 16)) == bool(expected)
            assert int(values['FULL_ERROR'], 16) == (0 if expected else 8)
        dll = blocks(lines, 'DLL')['OLE32.DLL']
        assert bool(int(dll['LOAD'], 16)) == bool(enabled)
        assert int(dll['LOAD_ERROR'], 16) == (0 if enabled else 8)
        processes = blocks(lines, 'PROCESS')
        note = processes[r'O:\WINDOWS\NOTEPAD.EXE']
        assert int(note['CREATE_PROCESS'], 16) == enabled
        assert int(note['PROCESS_ERROR'], 16) == (0 if enabled else 8)
        child = next(value for key, value in processes.items() if key.endswith(' child'))
        assert child['CREATE_PROCESS'] == '00000001'
        assert child['PROCESS_EXIT'] == '00000000'
    args.output.write_text(json.dumps({
        'passed': True,
        'cause': 'Paging=off prevents larger pageable commitments in this guest',
        'sameProbe': logs[0][0]['probeSha256'],
        'sameWorker': logs[0][0]['workerSha256'],
        'checks': ['six allocation sizes', 'reservation versus commitment',
                   'OLE32 load', 'Notepad creation', 'self-child control'],
    }, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
