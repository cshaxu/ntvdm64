"""Freeze real product compile commands for a future instrumented build."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess


TARGETS = (
    ("worker", "build/.ninja/t476-s1-worker", "ntvdm.exe", "x86"),
    ("redirector", "build/.ninja/t476-s1-worker", "VDMREDIR.dll", "x86"),
    ("hook32", "build/.ninja/t476-s1-worker", "nthook32.dll", "x86"),
    ("wow32", "build/.ninja/t476-s1/wow32", "wow32.dll", "x86"),
    ("worker64", "build/.ninja/t476-s1/worker-native", "ntvwm.exe", "x64"),
    ("frontend", "build/.ninja/t476-s1/frontend", "ntcon.exe", "x64"),
    ("monitor", "build/.ninja/t476-s1/monitor", "ntmon.exe", "x64"),
    ("launcher", "build/.ninja/t476-s1/launcher", "run16.exe", "x64"),
    ("service", "build/.ninja/t476-s1/service", "ntsrv.exe", "x64"),
    ("hook64", "build/.ninja/t476-s1/hook64", "nthook64.dll", "x64"),
)


def ninja(graph, *args):
    return subprocess.check_output(["ninja", "-C", str(graph), "-t", *args], text=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("output must be fresh and under build/t476-s1")
    unique, graphs, targets = {}, {}, []
    for name, relative, target, arch in TARGETS:
        graph = root / relative
        ninja_file = graph / "build.ninja"
        graphs[relative] = hashlib.sha256(ninja_file.read_bytes()).hexdigest()
        commands = ninja(graph, "commands", target).splitlines()
        compile_commands = {line for line in commands if re.search(r"(?i)\bcl(?:\.exe)?\s", line)}
        compdb = json.loads(ninja(graph, "compdb"))
        selected = [entry for entry in compdb if entry["command"] in compile_commands]
        matched = {entry["command"] for entry in selected}
        missing = sorted(compile_commands - matched)
        if missing:
            raise SystemExit(f"unmatched compile commands for {name}: {len(missing)}")
        for entry in selected:
            key = (entry["directory"].lower(), entry["output"].lower())
            candidate = {"arch": arch, **entry}
            if key in unique and unique[key] != candidate:
                raise SystemExit(f"conflicting compile output: {key}")
            unique[key] = candidate
        targets.append({"name": name, "graph": relative, "target": target,
                        "compile_commands": len(selected)})
    rows = list(unique.values())
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({"graphs": graphs, "targets": targets,
                                  "unique_compile_commands": len(rows),
                                  "commands": rows}, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"targets": len(targets), "unique_compile_commands": len(rows)}))


if __name__ == "__main__":
    main()
