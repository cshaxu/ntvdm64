"""Persist host-only branch hits and previous exclusions for the frozen build."""
import argparse
import collections
import hashlib
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("inventory", type=Path)
    parser.add_argument("markers", type=Path)
    parser.add_argument("contexts", type=Path)
    parser.add_argument("plan", type=Path)
    parser.add_argument("compile_plan", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    out = args.output.resolve()
    if not out.is_relative_to((root / "artifacts/mvdm-audit").resolve()) or out.exists():
        raise SystemExit("require a new audit artifact")
    inv = json.loads(args.inventory.read_text(encoding="utf-8"))
    markers = json.loads(args.markers.read_text(encoding="utf-8"))
    plan = json.loads(args.plan.read_text(encoding="utf-8"))
    compile_plan = json.loads(args.compile_plan.read_text(encoding="utf-8"))
    contexts = [json.loads(line) for line in args.contexts.read_text(encoding="utf-8").splitlines()]
    if len(contexts) != 1018 or len(compile_plan["commands"]) != 1018 or any(row.get("exit") or row.get("unknown_markers") or row.get("mirror_leaks") for row in contexts):
        raise SystemExit("full marker compile not clean")
    observed = collections.defaultdict(list)
    for context in contexts:
        for marker in context["markers"]:
            observed[marker].append(context["index"])
    planned = {(row["path"], row["arm"]) for row in plan["rows"]}
    arms, files, counts = [], [], collections.Counter()
    for file_index, row in enumerate(inv["files"]):
        path = row["path"]
        if not path.startswith(("src/mvdm-host/", "src/non-mvdm/")) or not path.lower().endswith((".c", ".h")):
            raise SystemExit("non-host source in inventory")
        if hashlib.sha256((root / path).read_bytes()).hexdigest() != row["current_sha256"]:
            raise SystemExit(f"stale source identity: {path}")
        files.append({"path": path, "source_sha256": row["current_sha256"],
                      "original_sha256": row["original_sha256"],
                      "file_marker_hit": bool(observed[f"T476_FILE_{file_index:04d}"])})
        for index, arm in enumerate(row["current_arms"]):
            marker = f"T476_ARM_{file_index:04d}_{index:05d}"
            parent = index
            while parent is not None and not row["current_arms"][parent]["excluded"]:
                parent = row["current_arms"][parent]["parent"]
            excluded = parent is not None
            status = "previously-excluded" if excluded else "hit" if observed[marker] else "unhit"
            counts[status] += 1
            arms.append({"file": file_index, "index": index, "line": arm["line"],
                         "directive": arm["directive"], "expression": arm["expression"],
                         "parent": arm["parent"], "status": status,
                         "hit_contexts": observed[marker] if not excluded else [],
                         "outermost_new_exclusion": (path, index) in planned})
    if counts["previously-excluded"] != markers["skipped_excluded_arms"] or counts["hit"] + counts["unhit"] != markers["marked_arms"]:
        raise SystemExit("branch count mismatch")
    context_map = [{"index": i, "arch": item["arch"], "source": item["file"],
                    "object": item["output"], "directory": item["directory"]}
                   for i, item in enumerate(compile_plan["commands"])]
    out.write_text(json.dumps({"build_contexts": 1018, "graphs": compile_plan["graphs"],
                               "contexts": context_map, "counts": dict(counts), "files": files, "arms": arms},
                              ensure_ascii=False, separators=(",", ":")) + "\n", encoding="utf-8")
    print(json.dumps(dict(counts)))


if __name__ == "__main__":
    main()
