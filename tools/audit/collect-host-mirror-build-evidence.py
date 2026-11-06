"""Bind the T480 host-only census to actual built product Ninja dependencies.

This does not infer branch activity or function retention from file selection.
No guest source is opened. Generated commands/deps are retained as evidence.
"""
import argparse
import collections
import csv
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess


ROOT = Path(__file__).resolve().parents[2]
TARGETS = [
    ("msvc-x86-release/worker", "ntvdm.exe"),
    ("msvc-x86-release/worker", "VCDEX.DLL"),
    ("msvc-x86-release/worker", "VDMREDIR.dll"),
    ("msvc-x86-release/worker", "nthook32.dll"),
    ("msvc-x86-release/wow32", "wow32.dll"),
    ("msvc-x64-release/worker", "ntvwm.exe"),
    ("msvc-x64-release/frontend", "ntcon.exe"),
    ("msvc-x64-release/monitor", "ntmon.exe"),
    ("msvc-x64-release/launcher", "run16.exe"),
    ("msvc-x64-release/service", "ntsrv.exe"),
    ("msvc-x64-release/hook", "nthook64.dll"),
]


def norm(value):
    return str(value).replace("\\", "/").lower()


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def ninja(graph, *args):
    return subprocess.check_output(["ninja", "-C", str(graph), "-t", *args],
                                   text=True, errors="replace")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    if not output.is_relative_to(ROOT / "build/t480-s1") or output.exists():
        raise SystemExit("require fresh output below build/t480-s1")
    output.mkdir(parents=True)
    with (ROOT / "artifacts/mvdm-audit/t480-s1-files.csv").open(newline="", encoding="utf-8") as stream:
        files = {row["path"]: row for row in csv.DictReader(stream)}
    lookup = {norm(ROOT / name): name for name in files}
    consumers = collections.defaultdict(list)
    commands, graphs, targets = {}, {}, []
    for relative, target in TARGETS:
        graph = ROOT / "build/.ninja" / relative
        if relative not in graphs:
            rawdeps = ninja(graph, "deps")
            (output / (relative.replace("/", "-") + "-deps.txt")).write_text(rawdeps, encoding="utf-8")
            deps, current = {}, None
            for line in rawdeps.splitlines():
                if not line.startswith(" "):
                    match = re.match(r"^(.*?):\s+#deps\s+\d+.*\((VALID|STALE)\)", line)
                    current = norm(match[1]) if match else None
                    if match:
                        deps[current] = {"state": match[2], "paths": []}
                elif current:
                    deps[current]["paths"].append(norm(os.path.abspath(graph / line.strip())))
            graphs[relative] = {"sha256": sha(graph / "build.ninja"),
                                "deps": deps, "compdb": json.loads(ninja(graph, "compdb"))}
        data = graphs[relative]
        closure = ninja(graph, "commands", target)
        (output / (target + "-commands.txt")).write_text(closure, encoding="utf-8")
        selected = {line for line in closure.splitlines() if re.search(r"(?i)\bcl(?:\.exe)?\s", line)}
        # Build closure includes import-library producers. Those producers are
        # not statically linked into their DLL consumer. Follow only operands
        # of the final link and librarian commands for actual link inputs.
        archives = {}
        final_link = []
        for command in closure.splitlines():
            tool = re.search(r'(?i)\b(?:link|lib)(?:\.exe)?\s', command)
            if not tool:
                continue
            # cmd /s /c wraps the entire NTVDM link command in one quote pair;
            # that pair is not one linker argument.
            command = command[tool.start():].rstrip('"')
            out = re.search(r'(?i)/out:("[^"]+"|\S+)', command)
            if not out:
                continue
            operands = [norm(v.strip('"')) for v in re.findall(r'"[^"]*"|\S+', command)
                        if not v.startswith('/') and v.strip('"').lower().endswith(('.obj', '.lib'))]
            name = norm(out[1].strip('"'))
            if re.search(r'(?i)\blib(?:\.exe)?\s', command):
                archives[name] = operands
            if name == norm(target):
                final_link = operands
        if not final_link:
            raise SystemExit("no final link operands: " + target)
        link_objects = {v: "direct-object" for v in final_link if v.endswith('.obj')}
        for operand in final_link:
            for member in archives.get(operand, []):
                if member.endswith('.obj'):
                    link_objects.setdefault(member, "archive-member:" + operand)
        rows = [row for row in data["compdb"] if row["command"] in selected]
        if selected - {row["command"] for row in rows}:
            raise SystemExit("unmatched compile commands: " + target)
        for row in rows:
            key = relative + "/" + norm(row["output"])
            if not (graph / row["output"]).is_file():
                raise SystemExit("missing built object: " + key)
            if key not in commands:
                dep = data["deps"].get(norm(row["output"]))
                if not dep or dep["state"] != "VALID":
                    raise SystemExit("missing/stale compiler dependencies: " + key)
                commands[key] = {**row, "graph": relative, "targets": [], "build_targets": [], "link_roles": {},
                                 "object_sha256": sha(graph / row["output"])}
                source = norm(row["file"])
                if not Path(row["file"]).is_absolute():
                    source = norm((graph / row["file"]).resolve())
                for dependency in set(dep["paths"] + [source]):
                    if dependency in lookup:
                        consumers[lookup[dependency]].append({"object": key,
                            "role": "translation-unit" if dependency == source else "included-input"})
            commands[key]["build_targets"].append(target)
            if norm(row["output"]) in link_objects:
                commands[key]["targets"].append(target)
                commands[key]["link_roles"][target] = link_objects[norm(row["output"])]
        targets.append({"target": target, "graph": relative, "compile_count": len(rows),
                        "image_sha256": sha(graph / target),
                        "link_operands": final_link,
                        "link_query": ninja(graph, "query", target),
                        "maps": {p.name: sha(p) for p in graph.glob("*.map")}})
    records = []
    for name, row in files.items():
        if sha(ROOT / name) != row["current_sha256"]:
            raise SystemExit("changed mirror identity: " + name)
        refs = consumers[name]
        records.append({**row, "connection": "product-input" if refs else "no-observed-product-consumer",
                        "consumers": refs,
                        "products": sorted({t for ref in refs for t in commands[ref["object"]]["targets"]})})
    result = {"head": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
              "targets": targets, "graphs": {k: v["sha256"] for k, v in graphs.items()},
              "commands": commands, "files": records,
              "limitations": ["File selection is not individual branch execution or linker symbol retention.",
                               "No product consumer is not proof of safe restoration; preserve exclusion policy and check test consumers."]}
    # The inherited census uses str.splitlines, which also splits form feeds.
    # Keep its stable IDs and add physical LF coordinates rather than silently
    # interpreting its sequence indices as compiler source line numbers.
    hunks = json.loads((ROOT / "artifacts/mvdm-audit/t480-s1-hunks.json").read_text())
    source_cache = {}
    classifications = []
    for hunk in hunks:
        name = hunk["path"]
        if name not in source_cache:
            pair = []
            for path in (Path(files[name]["original"]), ROOT / name):
                text = path.read_bytes().decode("latin-1").replace("\r\n", "\n").replace("\r", "\n")
                chunks = text.splitlines(keepends=True)
                physical, line = [], 1
                for chunk in chunks:
                    physical.append(line)
                    line += chunk.count("\n")
                physical.append(line)
                pair.append((chunks, physical))
            source_cache[name] = pair
        before, after = source_cache[name]
        oi, ci = hunk["original_start"] - 1, hunk["current_start"] - 1
        old = "".join(before[0][oi:oi+hunk["original_count"]])
        new = "".join(after[0][ci:ci+hunk["current_count"]])
        if hashlib.sha256(old.encode("latin-1")).hexdigest() != hunk["original_text_sha256"] or hashlib.sha256(new.encode("latin-1")).hexdigest() != hunk["current_text_sha256"]:
            raise SystemExit("hunk content identity mismatch: " + hunk["id"])
        without_line = re.sub(r"(?m)^[ \t]*#[ \t]*line\b[^\n]*(?:\n|$)", "", new)
        if not old and not without_line.strip():
            category = "diagnostic-line-or-blank-insertion"
        elif hunk["has_error"]:
            category = "contains-existing-exclusion-review-mixed-content"
        elif [v.strip() for v in old.splitlines() if v.strip()] == [v.strip() for v in without_line.splitlines() if v.strip()]:
            category = "whitespace-restoration-lead"
        elif consumers[name]:
            category = "product-input-substantive-review"
        else:
            category = "no-product-consumer-substantive-review"
        classifications.append({"id": hunk["id"], "path": name, "category": category,
                                "original_physical_line": before[1][oi],
                                "current_physical_line": after[1][ci]})
    result["hunks"] = classifications
    result["hunk_categories"] = dict(collections.Counter(h["category"] for h in classifications))
    (output / "evidence.json").write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"targets": len(targets), "compile_commands": len(commands),
                      "files": dict(collections.Counter(row["connection"] for row in records))}))


if __name__ == "__main__":
    main()
