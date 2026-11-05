"""Find uniquely matched original lines in actual product preprocessor output.

This conservative scan reports leads; unmatched and duplicate lines do not
become evidence of correctness or branch exclusion.
"""
import argparse
import collections
import csv
import hashlib
import json
from pathlib import Path
import re
import subprocess


CASES = (
    ("redir", "build/.ninja/t476-s1-worker", "VDMREDIR.dll",
     "src/mvdm-host/vdmredir/vrnmpipe.c", "x86"),
    ("wkman", "build/.ninja/t476-s1/wow32", "wow32.dll",
     "src/mvdm-host/wow32/wkman.c", "x86"),
    ("wow32", "build/.ninja/t476-s1/wow32", "wow32.dll",
     "src/mvdm-host/wow32/wow32.c", "x86"),
    ("wuman", "build/.ninja/t476-s1/wow32", "wow32.dll",
     "src/mvdm-host/wow32/wuman.c", "x86"),
    ("vdm32", "build/.ninja/t476-s1-worker", "nthook32.dll",
     "src/non-mvdm/base/win32/client/vdm.c", "x86"),
    ("vdm64", "build/.ninja/t476-s1/launcher", "run16.exe",
     "src/non-mvdm/base/win32/client/vdm.c", "x64"),
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    parser.add_argument("--reuse", action="store_true", help="rescan captured preprocessing output")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()):
        raise SystemExit("require a directory under build/t476-s1")
    if args.reuse:
        if not output.is_dir():
            raise SystemExit("reuse requires existing evidence")
        captured = {row["case"]: row for row in json.loads((output / "summary.json").read_text(encoding="utf-8"))}
    else:
        output.mkdir(parents=True, exist_ok=False)
    with (root / "artifacts/mvdm-audit/s13-r1314-file-audit.csv").open(
            encoding="utf-8-sig", newline="") as stream:
        census = {row["path"]: row for row in csv.DictReader(stream)}
    vs = r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
    records = []
    for name, tree, target, source, arch in CASES:
        graph = root / tree
        commands = set(subprocess.check_output(["ninja", "-C", str(graph), "-t", "commands", target],
                                               text=True).splitlines())
        compdb = json.loads(subprocess.check_output(["ninja", "-C", str(graph), "-t", "compdb"],
                                                    text=True))
        selected = [entry for entry in compdb if entry["command"] in commands and
                    entry["file"].replace("\\", "/").lower() == (root / source).as_posix().lower()]
        if not selected:
            raise SystemExit(f"no selected compile command: {name}")
        original = Path(census[source]["original"])
        raw = original.read_bytes()
        if hashlib.sha256(raw).hexdigest() != census[source]["original_sha256"].lower():
            raise SystemExit(f"original identity mismatch: {source}")
        # splitlines() also splits form feeds; MSVC counts physical LF lines.
        lines = raw.decode("utf-8", errors="replace").split("\n")
        if any(re.match(r"^\s*#\s*line\b", line) for line in lines):
            raise SystemExit(f"original contains line directives: {source}")
        originals = collections.defaultdict(list)
        for number, line in enumerate(lines, 1):
            text = line.strip()
            if len(text) > 8 and not text.startswith(("/", "*", "#")):
                originals[text].append(number)
        for index, entry in enumerate(selected, 1):
            stem = f"{name}-{index}"
            if args.reuse and (stem not in captured or captured[stem]["compiler_command"] != entry["command"]):
                raise SystemExit(f"captured command changed: {stem}")
            preprocessed = output / f"{stem}.txt"
            stderr = output / f"{stem}.stderr"
            command = re.sub(r"(?<!\S)/c(?=\s)", "/E", entry["command"], count=1)
            command = re.sub(r"\s+/Fo\S+", "", command).replace("/showIncludes", "")
            runner = output / f"{stem}.cmd"
            if not args.reuse:
                runner.write_text(f'@echo off\ncall "{vs}" -arch={arch} -host_arch=x64 >nul\n'
                                  f'if errorlevel 1 exit /b %errorlevel%\ncd /d "{graph}"\n'
                                  f'{command} > "{preprocessed}" 2> "{stderr}"\n'
                                  'exit /b %errorlevel%\n', encoding="ascii")
                run = subprocess.run(["cmd", "/d", "/c", str(runner)], timeout=180)
                if run.returncode:
                    raise SystemExit(f"preprocessing failed: {stem}, inspect {stderr}")
            current_file, logical = "", 0
            occurrences = collections.defaultdict(list)
            for text in preprocessed.read_text(encoding="utf-8", errors="replace").split("\n"):
                directive = re.match(r'^#line\s+(\d+)\s+"([^"]+)"', text)
                if directive:
                    logical = int(directive[1])
                    current_file = directive[2].replace("\\", "/").lower()
                    continue
                if current_file.endswith("/" + source.lower()):
                    original_lines = originals.get(text.strip(), [])
                    if len(original_lines) == 1:
                        occurrences[text.strip()].append((logical, original_lines[0]))
                logical += 1
            unique = [(text, values[0]) for text, values in occurrences.items() if len(values) == 1]
            mismatches = [{"actual": actual, "original": before, "text": text}
                          for text, (actual, before) in unique if actual != before]
            matches = len(unique)
            records.append({"case": stem, "source": source, "target": target,
                            "object": entry["output"], "matches": matches,
                            "mismatches": mismatches, "compiler_command": entry["command"]})
            print(f"{stem}: unique selected lines={matches} mismatches={len(mismatches)}", flush=True)
    summary = "summary-reuse.json" if args.reuse else "summary.json"
    (output / summary).write_text(json.dumps(records, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
