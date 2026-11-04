"""Check every object in the selected DLL link closures, including archives."""
import argparse
from pathlib import Path
import re
import subprocess


def inspect(graph_root: Path, inject: bool) -> int:
    edges = {}
    rules = {}
    for line in (graph_root / "build.ninja").read_text(encoding="utf-8").splitlines():
        if not line.startswith("build ") or ": " not in line:
            continue
        left, right = line[6:].split(": ", 1)
        tokens = right.split()
        inputs = []
        for token in tokens[1:]:
            if token in ("|", "||"):
                break
            inputs.append(token)
        for output in left.split():
            if output != "|":
                edges[output] = inputs
                rules[output] = tokens[0]
    result = subprocess.run(["ninja", "-C", str(graph_root), "-t", "deps"],
                            capture_output=True, text=True, check=True)
    deps = {}
    current = None
    for line in result.stdout.splitlines():
        match = re.match(r"^([^:]+\.obj): #deps \d+.*\(VALID\)", line)
        if match:
            current = match.group(1)
            deps[current] = []
        elif current and line.startswith(" "):
            deps[current].append(line.replace("\\", "/"))
    if inject:
        deps.setdefault("obj/nthook32/entry.obj", []).append(
            "src/ntvdm-exe/softpc/include/mvdm_softpc_symbol_compat.h")
    total = 0
    for target in ("nthook32.dll", "VDMREDIR.dll"):
        stack = [target]
        seen = set()
        while stack:
            item = stack.pop()
            if item in seen:
                continue
            seen.add(item)
            # This is a PE runtime import, explicitly outside this S; never
            # mistake its executable producer for a static source closure.
            if item == "ntvdm.lib":
                continue
            stack.extend(edges.get(item, []))
            if not item.endswith(".obj"):
                continue
            if item not in deps:
                if rules.get(item) == "asm_x86":
                    continue
                raise RuntimeError(f"missing valid compiler dependencies: {target}: {item}")
            total += 1
            for path in deps[item]:
                match = re.search(
                    r"(?:^|/)src/(run16-exe|ntsrv-exe|ntcon-exe|ntmon-exe|"
                    r"ntvdm-exe|ntvwm-exe|wow32-dll|vdmredir-dll|nthook32-dll|"
                    r"worker-base)/", path)
                own = "nthook32-dll" if target == "nthook32.dll" else "vdmredir-dll"
                if match and match.group(1) != own:
                    raise RuntimeError(f"{target}: {item}: {path}")
    print(f"PASS {total} DLL closure objects, including all archive candidates")
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("graph_root", type=Path)
    parser.add_argument("--negative", action="store_true")
    args = parser.parse_args()
    if args.negative:
        try:
            inspect(args.graph_root, True)
        except RuntimeError as error:
            if "obj/nthook32/entry.obj" not in str(error):
                raise
            print("PASS injected private-header negative control")
        else:
            raise SystemExit("negative control was not rejected")
    else:
        inspect(args.graph_root, False)
