"""Stage candidate unhit exclusions in build-owned files, preserving prior markers."""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re


DIRECTIVE = re.compile(rb'^[ \t]*#[ \t]*(?:line|error)\b')


def in_block_comment(data):
    state, index = "code", 0
    while index < len(data):
        pair = data[index:index + 2]
        char = data[index]
        if state == "block":
            if pair == b"*/":
                state, index = "code", index + 2
                continue
        elif state == "line":
            if char == 10:
                state = "code"
        elif state in ('"', "'"):
            if char == 92:
                index += 2
                continue
            if char == ord(state):
                state = "code"
        elif pair == b"/*":
            state, index = "block", index + 2
            continue
        elif pair == b"//":
            state, index = "line", index + 2
            continue
        elif char in (34, 39):
            state = chr(char)
        index += 1
    return state == "block"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plan", type=Path)
    parser.add_argument("marker_summary", type=Path)
    parser.add_argument("preprocess_summary", type=Path)
    parser.add_argument("preprocess_retry", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("require fresh output under build/t476-s1")
    marker = json.loads(args.marker_summary.read_text(encoding="utf-8"))
    preprocess = json.loads(args.preprocess_summary.read_text(encoding="utf-8"))
    retry = json.loads(args.preprocess_retry.read_text(encoding="utf-8"))
    if (marker.get("contexts"), marker.get("failures")) != (1018, 0):
        raise SystemExit("marker build incomplete")
    if (preprocess.get("contexts"), preprocess.get("failures")) != (1018, 0):
        raise SystemExit("full preprocess comparison incomplete")
    if (retry.get("contexts"), retry.get("failures")) != (4, 0):
        raise SystemExit("path and corrected-comment preprocess retry incomplete")
    plan = json.loads(args.plan.read_text(encoding="utf-8"))
    groups = collections.defaultdict(list)
    for row in plan["rows"]:
        if not row["path"].startswith(("src/mvdm-host/", "src/non-mvdm/")) or not row["path"].lower().endswith((".c", ".h")):
            raise SystemExit("non-host source in plan")
        groups[row["path"]].append(row)
    results = []
    for name, rows in groups.items():
        source = root / name
        data = source.read_bytes()
        digest = hashlib.sha256(data).hexdigest()
        if any(row["source_sha256"] != digest for row in rows):
            raise SystemExit(f"stale plan: {name}")
        lines = data.splitlines(keepends=True)
        before = [line.rstrip(b"\r\n") for line in lines if DIRECTIVE.match(line)]
        for row in sorted(rows, key=lambda item: item["body_start"], reverse=True):
            start, end = row["body_start"] - 1, row["body_end_exclusive"] - 1
            body = lines[start:end]
            if not body:
                raise SystemExit(f"empty arm requires separate handling: {name}:{row['directive_line']}")
            newline = b"\r\n" if any(line.endswith(b"\r\n") for line in body) else b"\n"
            replacement, inserted = [], False
            comment_open = in_block_comment(b"".join(lines[:start]))
            for line in body:
                if comment_open:
                    replacement.append(line)
                    if b"*/" in line:
                        comment_open = False
                elif DIRECTIVE.match(line):
                    replacement.append(line)
                elif not inserted:
                    replacement.append(b"#error T476-S1: branch not selected by product build" + newline)
                    inserted = True
                else:
                    replacement.append(b"\r\n" if line.endswith(b"\r\n") else b"\n")
            if not inserted:
                # An inherited #line is the sole body line. Keep it and
                # restore the next live arm's logical location via its
                # already-present #line after the next branch boundary.
                if end + 1 >= len(lines) or not re.match(rb'^[ \t]*#[ \t]*line\b', lines[end + 1]):
                    raise SystemExit(f"missing immediate following line anchor: {name}:{row['directive_line']}")
                replacement.append(b"#error T476-S1: branch not selected by product build" + newline)
            lines[start:end] = replacement
        after = [line.rstrip(b"\r\n") for line in lines if DIRECTIVE.match(line)
                 and b"T476-S1: branch not selected by product build" not in line]
        if before != after:
            raise SystemExit(f"existing #line or #error changed: {name}")
        changed = b"".join(lines)
        destination = output / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_bytes(changed)
        results.append({"path": name, "before_sha256": digest,
                        "after_sha256": hashlib.sha256(changed).hexdigest(), "excluded_arms": len(rows)})
    (output / "manifest.json").write_text(json.dumps({"files": results}, indent=2) + "\n", encoding="utf-8")
    print(f"staged {sum(row['excluded_arms'] for row in results)} exclusions in {len(results)} host files")


if __name__ == "__main__":
    main()
