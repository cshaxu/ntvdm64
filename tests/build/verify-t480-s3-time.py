"""Build and exercise the selected production RTL time object.

Invoke from an owned background process in a VS x86 developer environment:
python tests/build/verify-t480-s3-time.py --output build/t480-s3/time-replay
"""

import argparse
import gzip
import hashlib
import json
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = "src/non-mvdm/base/ntos/rtl/time.c"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    output = (ROOT / args.output).resolve()
    if not output.is_relative_to(ROOT / "build/t480-s3"):
        parser.error("output must be beneath build/t480-s3")
    output.mkdir(parents=True, exist_ok=False)
    evidence = json.loads(gzip.decompress(
        (ROOT / "artifacts/mvdm-audit/t480-s1-build-evidence.json.gz").read_bytes()
    ))
    source = next(row for row in evidence["files"] if row["path"] == SOURCE)
    if len(source["consumers"]) != 1:
        raise RuntimeError("Production time consumer set changed")
    command = evidence["commands"][source["consumers"][0]["object"]]
    if not command["graph"].startswith("msvc-x86"):
        raise RuntimeError("Expected x86 time consumer")
    obj = output / "time.obj"
    compile_command, count = re.subn(
        r'/Fo(?:"[^"]+"|\S+)', '/Fo"' + obj.as_posix() + '"',
        command["command"])
    if count != 1:
        raise RuntimeError("Expected exactly one object output option")
    steps = [
        ("production", compile_command, command["directory"]),
        ("fixture", ["cl.exe", "/nologo", "/TC", "/MT", "/W4",
                     "/Fetest.exe", "/Fotest.obj",
                     str(ROOT / "test/integration/rtl_time1970_restoration_test.c"),
                     "/link", str(obj), "ntdll.lib", "kernel32.lib"], output),
        ("runtime", [str(output / "test.exe")], output),
    ]
    records = []
    for name, invocation, cwd in steps:
        run = subprocess.run(invocation, cwd=cwd, shell=isinstance(invocation, str),
                             capture_output=True, timeout=180)
        (output / (name + ".log")).write_bytes(run.stdout + run.stderr)
        records.append({"step": name, "command": invocation,
                        "directory": str(cwd), "exit_code": run.returncode})
        (output / "result.json").write_text(json.dumps({
            "source": SOURCE,
            "source_sha256": hashlib.sha256((ROOT / SOURCE).read_bytes()).hexdigest(),
            "steps": records,
        }, indent=2))
        if run.returncode:
            raise RuntimeError(f"{name} failed: {run.returncode}")
    print("PASS original RTL time: seven calendar cases and output guards")


if __name__ == "__main__":
    main()
