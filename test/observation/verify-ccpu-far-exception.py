"""Run far-transfer fault-order checks against the selected CCPU fixture."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument("--build-root", required=True)
parser.add_argument("--worker-build", default="build/.ninja/msvc-x86-release/worker")
args = parser.parse_args()
root = (repo / args.build_root).resolve()
if not root.is_relative_to((repo / "build").resolve()) or root.exists():
    raise SystemExit("a fresh repository build directory is required")
root.mkdir(parents=True)
tree = (repo / args.worker_build).resolve()
exe = tree / "ccpu-far-exception-test.exe"
mapfile = tree / "ccpu-far-exception-test.exe.map"
maptext = mapfile.read_text(encoding="utf-8", errors="replace")
if not re.search(r"validate_far_dest.*(?:original-ccpu386|c_xfer)", maptext, re.IGNORECASE):
    raise SystemExit("link map did not establish selected far-transfer owner")
if not re.search(r"host_exint_hook.*ccpu_far_exception_fixture\.obj", maptext, re.IGNORECASE):
    raise SystemExit("link map did not establish the fixture exception hook")
owners = {
    "validate_far_dest": "c_xfer.obj",
    "descriptor_super_type": "c_bsic.obj",
    "xtrn2phy": "c_tlb.obj",
    "phy_w32": "ccpusas4.obj",
    "sas_init": "ccpusas4.obj",
    "sas_term": "ccpusas4.obj",
    "c_cpu_init": "c_main.obj",
}
for symbol, obj in owners.items():
    if not re.search(r"(?im)^\s*[0-9a-f]+:[0-9a-f]+\s+_" +
            re.escape(symbol) + r"\s+[^\n]*original-ccpu386:" +
            re.escape(obj) + r"\s*$", maptext):
        raise SystemExit("link map did not establish original owner for " + symbol)
results = []
for name, argv in [("off", []), ("matching", ["0008"]), ("other", ["0010"]), ("valid", ["0008", "valid"])]:
    env = os.environ.copy()
    env["CCPU_FAR_TRACE_PATH"] = str(root / (name + ".trace.txt"))
    run = subprocess.run([str(exe), *argv], cwd=tree, env=env, capture_output=True, text=True, errors="replace", timeout=60)
    (root / (name + ".stdout.txt")).write_text(run.stdout, encoding="utf-8")
    (root / (name + ".stderr.txt")).write_text(run.stderr, encoding="utf-8")
    if run.returncode:
        raise SystemExit(name + " failed: " + str(run.returncode) + " " + run.stdout + run.stderr)
    expected = "vector=0 error=0 cr2=12345000 valid=1" if name == "valid" else "vector=13 error=8 cr2=12345000 valid=0"
    if expected not in run.stderr:
        raise SystemExit(name + " did not report the asserted fault result: " + run.stderr)
    trace = root / (name + ".trace.txt")
    if name in ("matching", "valid"):
        if not trace.is_file() or "far-validate" not in trace.read_text(encoding="utf-8", errors="replace"):
            raise SystemExit(name + " did not execute the matching trace witness")
    elif trace.exists():
        raise SystemExit(name + " unexpectedly wrote the filtered trace")
    results.append({"case": name, "exit": run.returncode, "output": run.stderr.strip()})
digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
(root / "result.json").write_text(json.dumps({"result": "pass", "scope": "selected-object fixture; guest and full product remain separate",
    "fixture_sha256": digest(exe), "map_sha256": digest(mapfile), "object_sha256": digest(tree / "obj/ccpu/c_xfer.obj"),
    "original_symbol_owners": owners, "cases": results}, indent=2), encoding="utf-8")
print("PASS selected CCPU far-destination fault-order cases")
