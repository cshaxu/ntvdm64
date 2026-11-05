"""Plan, but do not apply, outermost unhit host-mirror arm exclusions."""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re


def boundaries(data, arms):
    text = data.decode("latin-1")
    clean = re.sub(r'/\*[\s\S]*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                   lambda m: re.sub(r"[^\r\n]", " ", m.group()) if m.group().startswith("/") else m.group(), text)
    lines = clean.split("\n")
    positions = {arm["line"]: i for i, arm in enumerate(arms)}
    stack, result, index = [], {}, 0
    while index < len(lines):
        first = index + 1
        line = lines[index].rstrip("\r")
        while line.rstrip().endswith("\\") and index + 1 < len(lines):
            index += 1
            line = line.rstrip()[:-1] + " " + lines[index].lstrip()
        match = re.match(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b", line)
        if match:
            kind = match[1]
            if kind in ("if", "ifdef", "ifndef"):
                arm = positions[first]
                result[arm] = {"body_start": index + 2}
                stack.append(arm)
            elif kind in ("elif", "else"):
                if not stack:
                    raise ValueError(f"unexpected {kind} at {first}")
                result[stack[-1]]["body_end"] = first
                arm = positions[first]
                result[arm] = {"body_start": index + 2}
                stack[-1] = arm
            else:
                if not stack:
                    raise ValueError(f"unexpected endif at {first}")
                result[stack.pop()]["body_end"] = first
        index += 1
    if stack or len(result) != len(arms):
        raise ValueError("conditional parser mismatch")
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("inventory", type=Path)
    parser.add_argument("markers", type=Path)
    parser.add_argument("hits", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("require fresh output under build/t476-s1")
    inventory = json.loads(args.inventory.read_text(encoding="utf-8"))
    markers = json.loads(args.markers.read_text(encoding="utf-8"))
    hits = json.loads(args.hits.read_text(encoding="utf-8"))
    if hits.get("failures") or hits.get("contexts") != 1018:
        raise SystemExit("incomplete or failed full compile")
    hitset = set(hits["hits"])
    known = {m["marker"] for m in markers["arms"]}
    unhit = known - hitset
    rows, counts = [], collections.Counter()
    for file_index, row in enumerate(inventory["files"]):
        name = row["path"]
        if not name.startswith(("src/mvdm-host/", "src/non-mvdm/")) or not name.lower().endswith((".c", ".h")):
            raise SystemExit("non-host source in inventory")
        source = (root / name).read_bytes()
        if hashlib.sha256(source).hexdigest() != row["current_sha256"]:
            raise SystemExit(f"source identity changed: {name}")
        arms = row["current_arms"]
        bounds = boundaries(source, arms)
        selected = {index for index in range(len(arms)) if f"T476_ARM_{file_index:04d}_{index:05d}" in unhit}
        for index in sorted(selected):
            parent = arms[index]["parent"]
            while parent is not None and parent not in selected:
                parent = arms[parent]["parent"]
            if parent is not None:
                counts["covered_by_unhit_parent"] += 1
                continue
            start, end = bounds[index]["body_start"], bounds[index]["body_end"]
            counts["outermost_unhit_arms"] += 1
            counts["empty_body_arms"] += start == end
            rows.append({"path": name, "source_sha256": row["current_sha256"], "arm": index,
                         "directive_line": arms[index]["line"], "directive": arms[index]["directive"],
                         "body_start": start, "body_end_exclusive": end, "empty": start == end})
    counts["unhit_marked_arms"] = len(unhit)
    counts["files_with_outermost_unhit"] = len({r["path"] for r in rows})
    output.write_text(json.dumps({"counts": dict(counts), "rows": rows}, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(dict(counts)))


if __name__ == "__main__":
    main()
