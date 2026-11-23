"""Read-only inventory reconciliation; never invoked by the build chain."""
import csv
import hashlib
import json
import re
import subprocess
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "artifacts/build-migration/t484"


def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT, text=True,
                                   encoding="utf-8")


baseline = json.loads((OUT / "inventory.json").read_text())['baseline']
head_files = set(git('ls-tree', '-r', '--name-only', 'HEAD').splitlines())
rows = list(csv.DictReader((OUT / "files.csv").open(encoding="utf-8-sig")))
history = git("log", "--format=@@%H%x09%s", "--name-only", baseline + "..HEAD")
changes = {}
commits = {}
current = None
for line in history.splitlines():
    if line.startswith("@@"):
        current, subject = line[2:].split("\t", 1)
        commits[current] = {"subject": subject, "evidence": []}
    elif line and current:
        changes.setdefault(line, []).append(current)
        if line.startswith("docs/etc/evidence/"):
            commits[current]['evidence'].append(line)

result = []
decisions_path = OUT / 'dispositions.json'
decisions = json.loads(decisions_path.read_text()) if decisions_path.exists() else {}
for index, row in enumerate(rows, 1):
    path = ROOT / row['file']
    exists = path.is_file()
    digest = hashlib.sha256(path.read_bytes()).hexdigest() if exists else None
    state = "removed" if not exists else (
        "unchanged" if digest == row['sha256'] else "changed")
    revisions = changes.get(row['file'], [])
    # Mechanical states are deliberately not completion verdicts. In
    # particular, an unchanged CMD producer may already be compliant, and
    # a modified assertion runner may still contain a compiler subprocess.
    result.append({"id": f"B{index:04d}", "file": row['file'],
                   "stage": row['stage'], "state": state,
                   "current_sha256": digest,
                   "revisions": revisions,
                   "evidence": sorted({p for c in revisions
                                        for p in commits[c]['evidence']}),
                   "completion": ("retired-with-delivery-evidence" if not exists and row['file'] not in head_files and revisions
                                  and any(commits[c]['evidence'] for c in revisions)
                                  else decisions.get(row['file'], {}).get('status',
                                       "requires-disposition-reconciliation")),
                   "decision": decisions.get(row['file'])})

patterns = re.compile(
    r"\b(?:cl|link|ml64?|rc|nasm|cmake|ninja)(?:\.exe)?\b|"
    r"(?:python|powershell|pwsh|node|cscript|wscript)(?:\.exe)?\b", re.I)
remaining = []
for row in result:
    path = ROOT / row['file']
    if path.suffix.lower() not in ('.ps1', '.py', '.js', '.mjs') or not path.is_file():
        continue
    hits = [{"line": i, "text": line.strip()} for i, line in enumerate(
        path.read_text(encoding='utf-8-sig', errors='replace').splitlines(), 1)
        if patterns.search(line)]
    if hits:
        remaining.append({"id": row['id'], "file": row['file'], "hits": hits})

document = {"baseline": baseline, "head": git('rev-parse', 'HEAD').strip(),
            "worktree_status": git('status', '--short').splitlines(),
            "scope": "355 original candidates; mechanical states are not completion",
            "counts": dict(Counter(r['state'] for r in result)),
            "disposition_counts": dict(Counter(r['completion'] for r in result)),
            "existing_interpreted_candidates_with_command_hits": len(remaining),
            "files": result, "commits": commits,
            "command_review_queue": remaining}
(OUT / 'progress.json').write_text(json.dumps(document, indent=2) + '\n', encoding='utf-8')
print(json.dumps({key: document[key] for key in (
    'head', 'counts', 'existing_interpreted_candidates_with_command_hits')}, indent=2))
