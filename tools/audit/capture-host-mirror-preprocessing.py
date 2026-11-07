"""Capture actual-command preprocessing for the finite T480 historical leads."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
SOURCES = (
    "src/mvdm-host/softpc.new/base/keymouse/keybd_io.c",
    "src/mvdm-host/softpc.new/base/ccpu386/fpu.c",
    "src/mvdm-host/softpc.new/base/video/egawrtm0.c",
    "src/mvdm-host/softpc.new/base/video/egwrtm12.c",
    "src/mvdm-host/dpmi32/dpmiint.c",
    "src/mvdm-host/wow32/wow32.c",
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("evidence", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    if not output.is_relative_to(ROOT / "build/t480-s1") or output.exists():
        raise SystemExit("require fresh T480 output directory")
    output.mkdir(parents=True)
    data = json.loads(args.evidence.read_text(encoding="utf-8"))
    records = []
    for source in SOURCES:
        rows = [(key, row) for key, row in data["commands"].items()
                if row["file"].replace("\\", "/").lower() == (ROOT / source).as_posix().lower()]
        if not rows:
            raise SystemExit("no compile owner: " + source)
        for key, row in rows:
            stem = str(len(records) + 1)
            command = re.sub(r"(?<!\S)/c(?=\s)", "/E", row["command"], count=1)
            command = re.sub(r'\s+/Fo(?:"[^"]+"|\S+)', "", command).replace("/showIncludes", "")
            stdout, stderr = output / (stem + ".i"), output / (stem + ".stderr")
            runner = output / (stem + ".cmd")
            arch = "x64" if "msvc-x64" in row["graph"] else "x86"
            runner.write_text('@echo off\ncall "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat" '
                              f'-arch={arch} -host_arch=x64 >nul\nif errorlevel 1 exit /b %errorlevel%\n'
                              f'cd /d "{row["directory"]}"\n{command} > "{stdout}" 2> "{stderr}"\nexit /b %errorlevel%\n', encoding="ascii")
            result = subprocess.run(["cmd", "/d", "/c", str(runner)], timeout=180)
            record = {"source": source, "object": key, "command": command,
                      "exit_code": result.returncode, "output": stdout.name,
                      "output_sha256": hashlib.sha256(stdout.read_bytes()).hexdigest()}
            records.append(record)
            print(json.dumps(record), flush=True)
            if result.returncode:
                raise SystemExit("preprocess failed: " + source)
    (output / "summary.json").write_text(json.dumps(records, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
