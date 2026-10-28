"""Check declaration-less C-VID guards against all retained MVDM text inputs."""
import hashlib
import json
import re
from pathlib import Path

A = Path('artifacts/mvdm-audit/shard-1')
BASE = Path('src/mvdm/softpc.new/base/cvidc')
TEXT_EXTENSIONS = {'.c', '.h', '.inc', '.cpp', '.asm', '.s', '.inl', '.src'}


def inspect():
    family = json.loads((A / 'cvidc-header-family-evidence.json').read_text(encoding='utf-8-sig'))
    targets = {}
    for row in family['files']:
        if row['guard_only']:
            path = BASE / row['name']
            lines = path.read_text(encoding='latin1').splitlines()
            assert len(lines) == 3
            guard = re.fullmatch(r'\s*#ifndef\s+(\w+)\s*', lines[0]).group(1)
            assert re.fullmatch(r'\s*#define\s+' + re.escape(guard) + r'\s*', lines[1])
            assert re.fullmatch(r'\s*#endif\s*/\*.*\*/\s*', lines[2])
            targets[row['name']] = (path, guard)
    assert len(targets) == 11
    refs = {name: [] for name in targets}
    patterns = {name: re.compile(r'\b' + re.escape(guard) + r'\b') for name, (_, guard) in targets.items()}
    for path in Path('src/mvdm').rglob('*'):
        if not path.is_file() or path.suffix.lower() not in TEXT_EXTENSIONS:
            continue
        for number, line in enumerate(path.read_text(encoding='latin1').splitlines(), 1):
            for name, pattern in patterns.items():
                if path != targets[name][0] and pattern.search(line):
                    refs[name].append(f'{path}:{number}')
    return dict(result='pass', family_sha256=hashlib.sha256((A / 'cvidc-header-family-evidence.json').read_bytes()).hexdigest().upper(),
                files=[dict(name=name, guard=guard, other_references=refs[name]) for name, (_, guard) in targets.items()],
                scope='Literal macro references in retained MVDM text extensions; no dynamically constructed names or external profiles')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
