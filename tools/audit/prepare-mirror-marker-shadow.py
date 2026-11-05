"""Prepare an isolated host-only branch-marker shadow, never a release input.

Run only after conditional-structure reconciliation. A subsequent complete
compile and preprocessing-equivalence check is required before using hits.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil


def under_excluded(arms, index):
    while index is not None:
        if arms[index]["excluded"]:
            return True
        index = arms[index]["parent"]
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("inventory", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--dispositions", type=Path, help="reviewed lexical-match resolutions")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("require a fresh directory under build/t476-s1")
    inventory = json.loads(args.inventory.read_text(encoding="utf-8"))
    if inventory["counts"].get("parser_error_files"):
        raise SystemExit("condition inventory has parser errors")
    if inventory.get("extra_unresolved"):
        raise SystemExit("auxiliary host source identities are unresolved")
    dispositions = json.loads(args.dispositions.read_text(encoding="utf-8")) if args.dispositions else {}
    if any(not key.startswith(("src/mvdm-host/", "src/non-mvdm/")) for key in dispositions):
        raise SystemExit("dispositions contain a non-host mirror path")
    for row in inventory["files"]:
        for key in ("unmatched_original_leads", "ancestry_conflicts"):
            for index in row[key]:
                arm = row["original_arms"][index]
                item = dispositions.get(f'{row["path"]}:{arm["line"]}')
                if (not item or item.get("decision") not in ("matched-existing", "existing-exclusion")
                        or item.get("current_sha256") != row["current_sha256"]
                        or item.get("original_sha256") != row["original_sha256"]
                        or not item.get("evidence")):
                    raise SystemExit(f'unreconciled {key}: {row["path"]}:{arm["line"]}')
    for row in inventory["files"]:
        if not row["path"].startswith(("src/mvdm-host/", "src/non-mvdm/")) or not row["path"].lower().endswith((".c", ".h")):
            raise SystemExit("inventory contains a non-host mirror path")
        data = (root / row["path"]).read_bytes()
        if hashlib.sha256(data).hexdigest() != row["current_sha256"]:
            raise SystemExit(f"inventory identity is stale: {row['path']}")
    output.mkdir(parents=True)
    source_root = root / "src"

    def exclude_guest(directory, names):
        return {"mvdm-guest"} if Path(directory) == source_root and "mvdm-guest" in names else set()

    shadow = output / "src"
    shutil.copytree(source_root, shadow, ignore=exclude_guest, symlinks=True)
    if (shadow / "mvdm-guest").exists():
        raise SystemExit("guest mirror unexpectedly copied")
    records, files, marked, excluded = [], [], 0, 0
    comment_spec = importlib.util.spec_from_file_location(
        "mirror_exclusion_staging", root / "tools/audit/stage-mirror-unhit-exclusions.py")
    comment_module = importlib.util.module_from_spec(comment_spec)
    comment_spec.loader.exec_module(comment_module)
    for file_id, row in enumerate(inventory["files"]):
        original = root / row["path"]
        files.append({"marker": f"T476_FILE_{file_id:04d}", "path": row["path"],
                      "source_sha256": row["current_sha256"]})
        target = shadow / Path(row["path"]).relative_to("src")
        raw = target.read_bytes()
        lines = raw.splitlines(keepends=True)  # bytes.splitlines keeps form feeds intact
        insertions = {}
        for arm_id, arm in enumerate(row["current_arms"]):
            if under_excluded(row["current_arms"], arm_id):
                excluded += 1
                continue
            last = arm["line"] - 1
            while lines[last].rstrip(b"\r\n \t").endswith(b"\\"):
                last += 1
                if last >= len(lines):
                    raise SystemExit(f"unterminated conditional: {row['path']}:{arm['line']}")
            # A block comment may begin on the directive line and close on a
            # later physical line. Inserting before its closing delimiter
            # silently comments out the probe and produces a false no-hit.
            while comment_module.in_block_comment(b"".join(lines[:last + 1])):
                last += 1
                if last >= len(lines):
                    raise SystemExit(f"unterminated directive comment: {row['path']}:{arm['line']}")
            marker = f"T476_ARM_{file_id:04d}_{arm_id:05d}"
            next_logical = arm["logical_line"] + last - (arm["line"] - 1) + 1
            insertions.setdefault(last, []).append((marker, next_logical))
            records.append({"marker": marker, "path": row["path"], "line": arm["line"],
                            "directive": arm["directive"], "expression": arm["expression"],
                            "source_sha256": row["current_sha256"]})
            marked += 1
        name = original.as_posix().replace('"', '\\"')
        prefix = f'#pragma message("T476_FILE_{file_id:04d}")\n#line 1 "{name}"\n'.encode("utf-8")
        if lines and lines[0].startswith(b"\xef\xbb\xbf"):
            lines[0] = lines[0][3:]
            prefix = b"\xef\xbb\xbf" + prefix
        chunks = [prefix]
        for index, line in enumerate(lines):
            chunks.append(line)
            for marker, next_line in insertions.get(index, ()):
                if not line.endswith((b"\n", b"\r")):
                    chunks.append(b"\n")
                chunks.append(f'#pragma message("{marker}")\n#line {next_line} "{name}"\n'.encode("utf-8"))
        target.write_bytes(b"".join(chunks))
    (output / "markers.json").write_text(json.dumps({"file_count": len(files), "files": files,
        "marked_arms": marked, "skipped_excluded_arms": excluded, "arms": records}, indent=2) + "\n",
        encoding="utf-8")
    print(json.dumps({"files": len(inventory["files"]), "marked": marked, "skipped": excluded}))


if __name__ == "__main__":
    main()
