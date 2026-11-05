"""Check actual native classifier compilation and its ownership boundary."""
import argparse
import hashlib
from pathlib import Path
import re
import subprocess

SHARED = "src/common/base_srv/legacy_classifier_arch.h"
PRIVATE = "src/nthook32-dll/legacy_classifier_arch.h"


def inspect(root: Path, inject: bool = False) -> None:
    for graph_name in ("launcher", "hook64"):
        graph_dir = root / graph_name
        graph = (graph_dir / "build.ninja").read_text(encoding="utf-8")
        if PRIVATE in graph or graph.count(SHARED) != 1:
            raise RuntimeError(f"wrong classifier forced include: {graph_name}")
        matches = re.findall(
            r"(?m)^build ([^ :]+\.obj): cc [^\r\n]*src/non-mvdm/base/win32/client/vdm\.c[^\r\n]*\r?\n([^\r\n]*)",
            graph,
        )
        selected = [(obj, flags) for obj, flags in matches
                    if "OPENNT_BASE_CLIENT_CLASSIFIER" in flags]
        if len(selected) != 1 or SHARED not in selected[0][1]:
            raise RuntimeError(f"classifier binding not scoped: {graph_name}")
        obj = selected[0][0]
        deps = subprocess.run(["ninja", "-C", str(graph_dir), "-t", "deps", obj],
                              text=True, capture_output=True, check=True).stdout.replace("\\", "/")
        if inject and graph_name == "launcher":
            deps += "\n" + PRIVATE
        if not re.search(r"#deps \d+.*\(VALID\)", deps) or SHARED not in deps or PRIVATE in deps:
            raise RuntimeError(f"private classifier header or missing dependencies: {graph_name}")
        if graph_name == "launcher":
            edges = {}
            for line in graph.splitlines():
                if not line.startswith("build ") or ": " not in line:
                    continue
                left, right = line[6:].split(": ", 1)
                inputs = []
                for token in right.split()[1:]:
                    if token in ("|", "||"):
                        break
                    inputs.append(token)
                for output in left.split():
                    if output != "|":
                        edges[output] = inputs
            all_deps = subprocess.run(["ninja", "-C", str(graph_dir), "-t", "deps"],
                                      text=True, capture_output=True, check=True).stdout.replace("\\", "/")
            blocks = {}
            current = None
            for line in all_deps.splitlines():
                match = re.match(r"^([^:]+\.obj): #deps \d+.*\(VALID\)", line)
                if match:
                    current = match.group(1)
                    blocks[current] = []
                elif current and line.startswith(" "):
                    blocks[current].append(line)
            pending = ["run16.exe"]
            seen = set()
            while pending:
                target = pending.pop()
                if target in seen:
                    continue
                seen.add(target)
                pending.extend(edges.get(target, []))
                if target.endswith(".obj"):
                    if target not in blocks:
                        raise RuntimeError(f"missing launcher object dependencies: {target}")
                    if any("/src/nthook32-dll/" in p for p in blocks[target]):
                        raise RuntimeError(f"NTHook private header in launcher closure: {target}")
    digest = hashlib.sha256(Path(SHARED).read_bytes().replace(b"\r\n", b"\n")).hexdigest()
    # Git stores this project-owned adapter with LF; check the pinned original
    # content independently of checkout line-ending policy and branch HEAD.
    if digest != "142f700874be0689e4c0ba7a38ec0359236f02449308300d0965fc4212b499f9":
        raise RuntimeError("classifier compatibility header contents changed")
    print("PASS byte-identical scoped classifier header and complete launcher compiler closure")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("build_root", type=Path)
    parser.add_argument("--negative", action="store_true")
    args = parser.parse_args()
    if args.negative:
        try:
            inspect(args.build_root, True)
        except RuntimeError as error:
            if "private classifier header" not in str(error):
                raise
            print("PASS injected private-header negative control")
        else:
            raise SystemExit("negative control accepted")
    else:
        inspect(args.build_root)
