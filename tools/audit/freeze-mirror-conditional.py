"""Revalidate the inherited S13 file identities without changing mirrors."""
import argparse
import collections
import csv
import hashlib
import json
from pathlib import Path
import subprocess


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    evidence_root = (root / "build" / "t476-s1").resolve()
    if not output.is_relative_to(evidence_root) or output.exists():
        raise SystemExit("output must be a fresh file under build/t476-s1")
    census = root / "artifacts/mvdm-audit/s13-r1314-file-audit.csv"
    with census.open(encoding="utf-8-sig", newline="") as stream:
        rows = list(csv.DictReader(stream))
    listed = subprocess.check_output(
        ["git", "ls-files", "-z", "src/mvdm-host", "src/non-mvdm"], cwd=root
    ).decode("utf-8").split("\0")
    tracked = {name for name in listed if name.lower().endswith((".c", ".h"))}
    catalogued = {row["path"] for row in rows}
    counts = collections.Counter()
    files = []
    for row in rows:
        current, original = root / row["path"], Path(row["original"])
        current_hash = digest(current) if current.is_file() else None
        original_hash = digest(original) if original.is_file() else None
        current_state = ("missing" if current_hash is None else
                         "match" if current_hash.lower() == row["current_sha256"].lower()
                         else "changed")
        original_state = ("missing" if original_hash is None else
                          "match" if original_hash.lower() == row["original_sha256"].lower()
                          else "changed")
        counts["current_" + current_state] += 1
        counts["original_" + original_state] += 1
        files.append({"path": row["path"], "original": row["original"],
                      "current_sha256": current_hash, "original_sha256": original_hash,
                      "current_state": current_state, "original_state": original_state})
    head = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({"head": head, "census_sha256": digest(census),
                                  "counts": dict(counts), "tracked_mirror_c_h": len(tracked),
                                  "not_in_census": sorted(tracked - catalogued),
                                  "not_in_tree": sorted(catalogued - tracked),
                                  "files": files}, indent=2) + "\n",
                      encoding="utf-8")
    print(json.dumps({"head": head, "entries": len(rows), "counts": dict(counts),
                      "tracked_mirror_c_h": len(tracked),
                      "not_in_census": len(tracked - catalogued),
                      "not_in_tree": len(catalogued - tracked)}))


if __name__ == "__main__":
    main()
