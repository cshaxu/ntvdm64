"""Read-only, pinned host C/H diff census; never infers build reachability."""
import argparse
import collections
import csv
import difflib
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
CENSUS = ROOT / "artifacts/mvdm-audit/s13-r1314-file-audit.csv"

def sha(data):
    return hashlib.sha256(data).hexdigest()

def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    out = args.output.resolve()
    if not out.is_relative_to((ROOT / "build").resolve()) or out.exists():
        parser.error("output must be fresh and under this worktree build directory")
    old = {r["path"]: r for r in csv.DictReader(CENSUS.open(encoding="utf-8-sig", newline=""))}
    supplement = ROOT / "artifacts/mvdm-audit/t480-s1-supplemental-pins.json"
    census_paths = set(old)
    for row in json.loads(supplement.read_text(encoding="utf-8")):
        if row["path"] in old:
            raise ValueError("Duplicate source pin: " + row["path"])
        old[row["path"]] = row
    tracked = subprocess.check_output(["git", "ls-files", "-z", "src/mvdm-host", "src/non-mvdm"], cwd=ROOT).decode().split("\0")
    tracked = sorted(p for p in tracked if p.lower().endswith((".c", ".h")))
    files, hunks = [], []
    counts = collections.Counter()
    for name in tracked:
        current = (ROOT / name).read_bytes()
        prior = old.get(name)
        row = dict(path=name, current_sha256=sha(current), original="", original_sha256="",
                   original_pin="unpaired", comparison="unresolved", hunk_count=0,
                   current_drift="unpaired", prior_evidence="", connection="not-yet-verified")
        if prior:
            original = Path(prior["original"])
            row.update(original=str(original), prior_evidence=prior["prior_evidence"],
                       current_drift="no-prior-current-pin" if not prior["current_sha256"] else "unchanged" if sha(current)==prior["current_sha256"].lower() else "changed")
            if original.is_file():
                raw = original.read_bytes()
                row["original_sha256"] = sha(raw)
                row["original_pin"] = "match" if sha(raw)==prior["original_sha256"].lower() else "mismatch"
                if row["original_pin"] == "match":
                    before = raw.decode("latin-1").replace("\r\n", "\n").replace("\r", "\n")
                    after = current.decode("latin-1").replace("\r\n", "\n").replace("\r", "\n")
                    strip_line = lambda s: re.sub(r"(?m)^\s*#\s*line\b[^\n]*\n?", "", s)
                    row["comparison"] = ("byte-identical" if raw==current else "newline-only" if before==after else
                        "line-directives-only" if strip_line(before)==strip_line(after) else "text-different")
                    left, right = before.splitlines(keepends=True), after.splitlines(keepends=True)
                    for tag,a,b,c,d in difflib.SequenceMatcher(None,left,right,autojunk=True).get_opcodes():
                        if tag=="equal": continue
                        body0,body1 = "".join(left[a:b]),"".join(right[c:d])
                        identity = name+"|"+row["original_sha256"]+"|"+str((a,b))+"|"+sha(body1.encode("latin-1"))
                        hunks.append(dict(id="D-"+sha(identity.encode())[:16],path=name,kind=tag,
                            original_start=a+1,original_count=b-a,current_start=c+1,current_count=d-c,
                            original_text_sha256=sha(body0.encode("latin-1")),current_text_sha256=sha(body1.encode("latin-1")),
                            has_error=bool(re.search(r"(?m)^\s*#\s*error\b",body1)),
                            has_line=bool(re.search(r"(?m)^\s*#\s*line\b",body1)),
                            classification="pending-source-and-consumer-review"))
                        row["hunk_count"] += 1
            else:
                row["original_pin"]="missing"
        counts[row["comparison"]] += 1
        files.append(row)
    out.mkdir(parents=True)
    with (out/"files.csv").open("w",encoding="utf-8",newline="") as stream:
        writer=csv.DictWriter(stream,fieldnames=list(files[0]),lineterminator="\n")
        writer.writeheader();writer.writerows(files)
    (out/"hunks.json").write_text(json.dumps(hunks,indent=2)+"\n",encoding="utf-8")
    summary=dict(head=subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip(),
        census_sha256=sha(CENSUS.read_bytes()), supplemental_sha256=sha(supplement.read_bytes()), script_sha256=sha(Path(__file__).read_bytes()),
        scope="Tracked host C/H only; no guest file contents read", counts=dict(counts),
        roots={root:dict(collections.Counter(r["comparison"] for r in files if r["path"].startswith(root+"/"))) for root in ["src/mvdm-host","src/non-mvdm"]},
        total_files=len(files),total_text_hunks=len(hunks),
        source_pins=dict(collections.Counter(r["original_pin"] for r in files)),
        drift=dict(collections.Counter(r["current_drift"] for r in files)),
        added_since_census=[p for p in tracked if p not in census_paths], not_in_census=[p for p in tracked if p not in old],not_in_tree=sorted(set(old)-set(tracked)),
        hunks_with_error=sum(r["has_error"] for r in hunks),hunks_with_line=sum(r["has_line"] for r in hunks),
        limits="Lexical SequenceMatcher blocks, not semantic fixes or minimal diff. Existing #error/#line policy retained; connection unverified. No runtime coverage inference.")
    (out/"summary.json").write_text(json.dumps(summary,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(summary,indent=2))

if __name__ == "__main__":
    main()
