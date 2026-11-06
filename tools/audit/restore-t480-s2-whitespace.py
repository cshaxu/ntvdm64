"""Plan/apply only the 146 pinned S2 whitespace leads; preserve #line/errors.

All nonblank physical-line text and its logical diagnostic coordinate must
remain identical. Original whitespace is merged around existing #line records,
never replacing those records. A rejected full restoration may still have a
safe same-line indentation restoration. No guest files are opened.
"""
import argparse
import collections
import csv
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
LINE = re.compile(r'^[ \t]*#[ \t]*line\s+(\d+)(.*)$')


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def normalized(raw):
    return raw.decode('latin-1').replace('\r\n', '\n').replace('\r', '\n')


def signature(text):
    """Conservative: retain comments/directives too, not just compiler tokens."""
    result, logical, filename = [], 1, ''
    for line in text.split('\n'):
        directive = LINE.match(line)
        if directive:
            logical = int(directive[1])
            if directive[2].strip():
                filename = directive[2].strip()
            continue
        if line.strip():
            result.append((logical, filename, line.strip()))
        logical += 1
    return result


def split_groups(lines):
    gaps, bodies, gap = [], [], []
    for line in lines:
        if not line.strip() or LINE.match(line):
            gap.append(line)
        else:
            gaps.append(gap)
            bodies.append(line)
            gap = []
    gaps.append(gap)
    return gaps, bodies


def merge(old, new, restore_gaps):
    og, ob = split_groups(old)
    ng, nb = split_groups(new)
    if [x.strip() for x in ob] != [x.strip() for x in nb]:
        raise ValueError('not a whitespace lead')
    result = []
    for i, gap in enumerate(ng):
        desired = [x for x in og[i] if not x.strip()]
        if restore_gaps:
            existing = [j for j, x in enumerate(gap) if not x.strip()]
            extra = len(existing) - len(desired)
            # Remove earliest blanks first: a later #line can reset their
            # coordinate effect. Every proposal is independently verified.
            removed = set(existing[:max(0, extra)])
            gap = [x for j, x in enumerate(gap) if j not in removed]
            if extra < 0:
                gap = ['\n'] * -extra + gap
            n = 0
            for j, line in enumerate(gap):
                if not line.strip():
                    gap[j] = desired[n]
                    n += 1
        result.extend(gap)
        if i < len(ob):
            # A changed quoted/macro-continued line is outside this simple
            # restoration proof. None of the admitted code/comment leads
            # requires changing a literal or a continuation.
            if ob[i] != nb[i] and any(c in ob[i] + nb[i] for c in ('"', "'", '\\')):
                raise ValueError('quoted or continued changed line')
            result.append(ob[i])
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output', type=Path)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    output = args.output.resolve()
    if not output.is_relative_to(ROOT / 'build/t480-s2') or output.exists():
        raise SystemExit('require fresh output under build/t480-s2')
    output.mkdir(parents=True)
    census = {r['path']: r for r in csv.DictReader((ROOT / 'artifacts/mvdm-audit/t480-s1-files.csv').open(encoding='utf-8'))}
    hunks = {r['id']: r for r in json.loads((ROOT / 'artifacts/mvdm-audit/t480-s1-hunks.json').read_text(encoding='utf-8'))}
    grouped = collections.defaultdict(list)
    for row in json.loads((ROOT / 'artifacts/mvdm-audit/t480-s1-hunk-classification.json').read_text(encoding='utf-8')):
        if row['category'] == 'whitespace-restoration-lead':
            grouped[row['path']].append(row)
    records, changes = [], []
    for name, candidates in grouped.items():
        if not name.startswith(('src/mvdm-host/', 'src/non-mvdm/')):
            raise ValueError('not a host mirror')
        path = ROOT / name
        raw, upstream = path.read_bytes(), Path(census[name]['original']).read_bytes()
        if sha(raw) != census[name]['current_sha256'] or sha(upstream) != census[name]['original_sha256']:
            raise ValueError('source identity changed: ' + name)
        text, original = normalized(raw), normalized(upstream)
        lines, old_lines = text.splitlines(keepends=True), original.splitlines(keepends=True)
        physical = raw.decode('latin-1').splitlines(keepends=True)
        if len(physical) != len(lines):
            raise ValueError('unsupported line encoding')
        # Offsets remain pinned by applying from the end of each file.
        for row in sorted(candidates, key=lambda x: hunks[x['id']]['current_start'], reverse=True):
            h = hunks[row['id']]
            a, n = h['current_start'] - 1, h['current_count']
            old = old_lines[h['original_start'] - 1:h['original_start'] - 1 + h['original_count']]
            new = lines[a:a+n]
            if sha(''.join(new).encode('latin-1')) != h['current_text_sha256']:
                raise ValueError('hunk identity changed')
            proposal = merge(old, new, True)
            partial = False
            current = ''.join(lines)
            candidate = ''.join(lines[:a] + proposal + lines[a+n:])
            # Blank restoration adjacent to a line splice cannot be inferred
            # from ordinary stripped-line equivalence.
            spliced = a > 0 and lines[a-1].rstrip('\n').endswith('\\')
            if spliced or signature(candidate) != signature(current):
                proposal = merge(old, new, False)
                partial = True
                candidate = ''.join(lines[:a] + proposal + lines[a+n:])
            assert signature(candidate) == signature(current), name
            assert [x for x in proposal if LINE.match(x)] == [x for x in new if LINE.match(x)]
            changed = proposal != new
            reason = ('original whitespace restored; text and logical coordinates identical'
                      if changed else 'retained existing diagnostic #line layout; no independent spacing change')
            if partial:
                reason = ('same-line spacing restored; blank-line restoration changes logical coordinates'
                          if changed else 'retained blank placement: restoring it changes logical coordinates before the next #line')
            records.append({**row, 'disposition': 'restore-partial' if changed and partial else 'restore' if changed else 'retain',
                            'reason': reason, 'before': ''.join(new), 'after': ''.join(proposal)})
            if changed:
                # Keep all bytes outside the hunk and the file's existing EOL.
                eol = '\r\n' if raw.count(b'\r\n') > raw.count(b'\n') // 2 else '\n'
                physical[a:a+n] = [x.replace('\n', eol) for x in proposal]
                lines[a:a+n] = proposal
        after = ''.join(physical).encode('latin-1')
        assert signature(normalized(after)) == signature(text)
        if after != raw:
            backup = output / 'before' / name
            backup.parent.mkdir(parents=True, exist_ok=True)
            backup.write_bytes(raw)
            proposed = output / 'after' / name
            proposed.parent.mkdir(parents=True, exist_ok=True)
            proposed.write_bytes(after)
            changes.append({'path': name, 'before_sha256': sha(raw), 'after_sha256': sha(after)})
    # Write only after every source identity and proposed edit passed.
    if args.apply:
        for change in changes:
            path = ROOT / change['path']
            if sha(path.read_bytes()) != change['before_sha256']:
                raise ValueError('concurrent modification')
            path.write_bytes((output / 'after' / change['path']).read_bytes())
    result = {'applied': args.apply, 'candidate_count': len(records), 'files_reviewed': len(grouped),
              'dispositions': dict(collections.Counter(r['disposition'] for r in records)),
              'changes': changes, 'candidates': sorted(records, key=lambda r: (r['path'], r['current_physical_line']))}
    (output / 'result.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: v for k, v in result.items() if k not in ('changes', 'candidates')}))
    print('changed files:', len(changes))


if __name__ == '__main__':
    main()
