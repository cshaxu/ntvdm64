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
        left, right = line[6:].replace("\\", "/").replace("$:", ":").split(": ", 1)
        tokens = right.split()
        parts = re.split(r"\s+\|\|?\s+", right[len(tokens[0]):].strip())
        inputs = parts[0].split()
        if tokens[0] == "CUSTOM_COMMAND" and re.search(r"\s\|\s", right):
            inputs += [x for x in parts[1].split() if x.endswith((".obj", ".lib"))]
        outputs = left.split(" | ", 1)[0].split()
        image = any(x.lower().endswith((".exe", ".dll")) for x in outputs)
        for output in outputs:
            edges[output] = [] if image and output.endswith(".lib") else inputs
            rules[output] = tokens[0]
    result = subprocess.run(["ninja", "-C", str(graph_root), "-t", "deps"],
                            capture_output=True, text=True, check=True)
    deps = {}
    current = None
    for line in result.stdout.replace("\\", "/").splitlines():
        match = re.match(r"^([^:]+\.obj): #deps \d+.*\(VALID\)", line)
        if match:
            current = match.group(1)
            deps[current] = []
        elif not line.startswith(" "):
            current = None
        elif current:
            deps[current].append(line.replace("\\", "/"))
    if inject:
        candidates = [obj for obj, inputs in edges.items() if obj in deps and
                      any(x.endswith("/src/nthook-dll/entry.c") for x in inputs)]
        if len(candidates) != 1:
            raise RuntimeError("negative control requires one compiled hook entry")
        deps[candidates[0]].append("src/ntvdm-exe/softpc/include/mvdm_shadow_registry.h")
    total = 0
    for target in ("nthook32.dll", "VDMREDIR.dll"):
        if target not in edges:
            raise RuntimeError(f"missing product target: {target}")
        count = 0
        stack = [target]
        seen = set()
        while stack:
            item = stack.pop()
            if item in seen:
                continue
            seen.add(item)
            stack.extend(edges.get(item, []))
            # Native graphs may copy an object; validate its compiler producer.
            if not item.endswith(".obj") or not any(
                    re.search(r"\.(c|cpp|cxx|asm)$", x) for x in edges.get(item, [])):
                continue
            if item not in deps:
                if "ASM_MASM" in rules.get(item, "") or rules.get(item) == "asm_x86":
                    continue
                raise RuntimeError(f"missing valid compiler dependencies: {target}: {item}")
            total += 1
            count += 1
            for path in deps[item]:
                match = re.search(
                    r"(?:^|/)src/(run16-exe|ntsrv-exe|ntcon-exe|ntmon-exe|"
                    r"ntvdm-exe|ntvwm-exe|wow32-dll|vdmredir-dll|nthook-dll|"
                    r"worker-base)/", path)
                own = "nthook-dll" if target == "nthook32.dll" else "vdmredir-dll"
                if match and match.group(1) != own:
                    raise RuntimeError(f"{target}: {item}: {path}")
        if not count:
            raise RuntimeError(f"no compiler records checked: {target}")
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
            if "mvdm_shadow_registry.h" not in str(error):
                raise
            print("PASS injected private-header negative control")
        else:
            raise SystemExit("negative control was not rejected")
    else:
        inspect(args.graph_root, False)
