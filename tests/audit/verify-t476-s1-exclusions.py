"""Verify each recorded outermost unhit arm now rejects compilation."""
import collections
import importlib.util
import json
from pathlib import Path
import re


root = Path(__file__).resolve().parents[2]


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, root / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


inventory = load("condition_inventory", "tools/audit/inventory-mirror-conditions.py")
planner = load("exclusion_planner", "tools/audit/plan-mirror-unhit-exclusions.py")
staging = load("exclusion_staging", "tools/audit/stage-mirror-unhit-exclusions.py")
if not staging.in_block_comment(b'#ifndef PROFILE /* description\n continuation\n'):
    raise SystemExit("multiline directive comment was not detected")
if staging.in_block_comment(b'#ifndef PROFILE /* description\n continuation */\n'):
    raise SystemExit("closed directive comment remains open")
ledger = json.loads((root / "artifacts/mvdm-audit/t476-s1-branch-ledger.json").read_text(encoding="utf-8"))
expected = collections.Counter()
for arm in ledger["arms"]:
    if arm["outermost_new_exclusion"]:
        path = ledger["files"][arm["file"]]["path"]
        expected[(path, arm["directive"], arm["expression"])] += 1

actual = collections.Counter()
for path in sorted({key[0] for key in expected}):
    if not path.startswith(("src/mvdm-host/", "src/non-mvdm/")) or not path.lower().endswith((".c", ".h")):
        raise SystemExit(f"non-host source: {path}")
    data = (root / path).read_bytes()
    arms, errors = inventory.arm_inventory(data)
    if errors:
        raise SystemExit(f"unbalanced preprocessing directives: {path}: {errors}")
    positions = planner.boundaries(data, arms)
    text = data.decode("latin-1")
    clean = re.sub(r'/\*[\s\S]*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                   lambda m: re.sub(r"[^\r\n]", " ", m.group()) if m.group().startswith("/") else m.group(), text)
    for line, value in enumerate(clean.split("\n"), 1):
        if not re.match(r'^\s*#\s*error\s+T476-S1: branch not selected by product build\s*$', value):
            continue
        containing = [i for i, bounds in positions.items()
                      if bounds["body_start"] <= line < bounds["body_end"]]
        if not containing:
            raise SystemExit(f"error is outside conditional arm: {path}:{line}")
        index = max(containing, key=lambda i: positions[i]["body_start"])
        arm = arms[index]
        actual[(path, arm["directive"], arm["expression"])] += 1

if actual != expected:
    raise SystemExit(f"exclusion mismatch: missing={expected - actual}; unexpected={actual - expected}")
print(f"PASS {sum(actual.values())} recorded unhit outer arms in {len({key[0] for key in actual})} host files")
