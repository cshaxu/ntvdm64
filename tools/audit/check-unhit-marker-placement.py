"""Reject unhit probes that were accidentally placed inside block comments."""
import argparse
import importlib.util
import json
from pathlib import Path


parser = argparse.ArgumentParser()
parser.add_argument("shadow", type=Path)
parser.add_argument("summary", type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
shadow = args.shadow.resolve()
if not shadow.is_relative_to((root / "build/t476-s1").resolve()) or (shadow / "src/mvdm-guest").exists():
    raise SystemExit("require host-only build-owned shadow")
spec = importlib.util.spec_from_file_location("stage", root / "tools/audit/stage-mirror-unhit-exclusions.py")
stage = importlib.util.module_from_spec(spec)
spec.loader.exec_module(stage)
markers = json.loads((shadow / "markers.json").read_text(encoding="utf-8"))
summary = json.loads(args.summary.read_text(encoding="utf-8"))
if (summary.get("contexts"), summary.get("failures")) != (1018, 0):
    raise SystemExit("full marker build not clean")
hits, checked = set(summary["hits"]), 0
cache = {}
for arm in markers["arms"]:
    if arm["marker"] in hits:
        continue
    if not arm["path"].startswith(("src/mvdm-host/", "src/non-mvdm/")) or not arm["path"].lower().endswith((".c", ".h")):
        raise SystemExit("non-host arm")
    if arm["path"] not in cache:
        cache[arm["path"]] = (shadow / arm["path"]).read_bytes()
    data = cache[arm["path"]]
    position = data.find(arm["marker"].encode("ascii"))
    if position < 0 or stage.in_block_comment(data[:position]):
        raise SystemExit(f"missing/commented probe: {arm['marker']} in {arm['path']}")
    checked += 1
print(f"PASS {checked} unhit probes outside block comments")
