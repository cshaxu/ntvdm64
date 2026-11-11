"""Compile the exact selected WOW32 function with bounded ownership observers.
Run in a background-owned process. This never opens the user's Clipboard.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build-root", type=Path, required=True)
parser.add_argument("--compiler-env", type=Path, required=True)
args = parser.parse_args()
repo = Path(__file__).resolve().parents[3]
root = args.build_root.resolve()
if not root.is_relative_to(repo / "build") or root.exists():
    parser.error("require a fresh evidence directory under repository build/")
root.mkdir(parents=True)
source = repo / "src/mvdm-host/wow32/wuclip.c"
fixture = Path(__file__).with_name("wow_clipboard_format_fixture.c")
raw = source.read_bytes()
start = raw.index(b"ULONG FASTCALL WU32SetClipboardData(")
position = raw.index(b"{", start) + 1
depth = 1
while depth:
    token = raw[position:position + 1]
    if not token:
        raise ValueError("unterminated selected function")
    depth += (token == b"{") - (token == b"}")
    position += 1
body = raw[start:position]
(root / "selected-set-clipboard.inc").write_bytes(body)
# The selected function's comments/strings have balanced braces; retain its
# bytes rather than reconstructing or translating its ownership decisions.
command = root / "compile.cmd"
command.write_text(
    '@echo off\ncall "' + str(args.compiler_env.resolve()) +
    '" -arch=x86 -host_arch=x64 >nul\n'
    'if errorlevel 1 exit /b %errorlevel%\n'
    'cl /nologo /TC /W3 /O2 /I"' + str(root) + '" "' + str(fixture) +
    '" /Fe"' + str(root / "fixture.exe") + '" /Fo"' +
    str(root / "fixture.obj") + '" /link user32.lib\n'
    'exit /b %errorlevel%\n', encoding="utf-8")
record = {"source_sha256": hashlib.sha256(raw).hexdigest(),
          "body_sha256": hashlib.sha256(body).hexdigest(),
          "fixture_sha256": hashlib.sha256(fixture.read_bytes()).hexdigest(),
          "scope": "controlled ownership only; no real guest/conversion claim"}
started = time.monotonic()
try:
    with (root / "compile.log").open("w", encoding="utf-8") as log:
        compiled = subprocess.run(["cmd.exe", "/d", "/c", str(command)],
                                  stdout=log, stderr=subprocess.STDOUT,
                                  timeout=60, cwd=repo)
    record["compile_exit"] = compiled.returncode
    compiled.check_returncode()
    with (root / "result.txt").open("w", encoding="utf-8") as log:
        result = subprocess.run([str(root / "fixture.exe"), "--expect-fixed"],
                                stdout=log, stderr=subprocess.STDOUT,
                                timeout=30, cwd=repo)
    record["test_exit"] = result.returncode
    result.check_returncode()
    rows = (root / "result.txt").read_text(encoding="utf-8")
    if "cases=24 strict=1 failures=0" not in rows:
        raise ValueError("missing complete assertion receipt")
    record["passed_cases"] = 24
finally:
    record["elapsed_seconds"] = time.monotonic() - started
    (root / "receipt.json").write_text(json.dumps(record, indent=2) + "\n",
                                       encoding="utf-8", newline="\n")
