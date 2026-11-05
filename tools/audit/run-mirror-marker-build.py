"""Compile every frozen product context against the host-only marker shadow.

The resulting hits are observational. They must not drive exclusions until
every context passed and marker/preprocessor equivalence has been reviewed.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time


def environment(arch):
    vs = r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
    # Pass one raw Windows command line: list-to-command-line quoting adds
    # literal backslashes around the quoted Visual Studio path under cmd.exe.
    command = f'cmd /d /s /c "call "{vs}" -arch={arch} -host_arch=x64 >nul && set"'
    process = subprocess.run(command, capture_output=True, check=True)
    return dict(line.split("=", 1) for line in process.stdout.decode("mbcs", errors="replace").splitlines()
                if "=" in line and not line.startswith("="))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plan", type=Path)
    parser.add_argument("shadow", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--jobs", type=int, default=8)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    evidence = (root / "build/t476-s1").resolve()
    shadow, output = args.shadow.resolve(), args.output.resolve()
    if (not shadow.is_relative_to(evidence) or not output.is_relative_to(evidence)
            or output.exists() or not 1 <= args.jobs <= 16):
        raise SystemExit("require existing shadow and fresh output under build/t476-s1; jobs 1..16")
    if (shadow / "src/mvdm-guest").exists():
        raise SystemExit("guest mirror must not be included")
    plan = json.loads(args.plan.read_text(encoding="utf-8"))
    manifest = json.loads((shadow / "markers.json").read_text(encoding="utf-8"))
    if len(plan["commands"]) != plan["unique_compile_commands"]:
        raise SystemExit("compile plan count mismatch")
    for name, expected in plan["graphs"].items():
        if hashlib.sha256((root / name / "build.ninja").read_bytes()).hexdigest() != expected:
            raise SystemExit(f"build graph changed: {name}")
    for item in manifest["files"]:
        if not item["path"].startswith(("src/mvdm-host/", "src/non-mvdm/")) or not item["path"].lower().endswith((".c", ".h")):
            raise SystemExit("marker manifest contains non-host path")
        if hashlib.sha256((root / item["path"]).read_bytes()).hexdigest() != item["source_sha256"]:
            raise SystemExit(f'marker source changed: {item["path"]}')
    known = {item["marker"] for item in manifest["arms"]} | {item["marker"] for item in manifest["files"]}
    environments = {arch: environment(arch) for arch in ("x86", "x64")}
    compilers = {arch: shutil.which("cl.exe", path=env.get("PATH", env.get("Path", "")))
                 for arch, env in environments.items()}
    if not all(compilers.values()):
        raise SystemExit("MSVC compiler missing")
    output.mkdir(parents=True)
    (output / "objects").mkdir()
    old_src = root.as_posix() + "/src"
    new_src = (shadow / "src").as_posix()
    source_pattern = re.compile(r'[\\/]'.join(map(re.escape, old_src.split('/'))) + r'(?=[/\\"\s]|$)', re.I)
    marker_pattern = re.compile(rb"T476_(?:ARM_\d{4}_\d{5}|FILE_\d{4})")

    def compile_one(job):
        index, item = job
        command = item["command"]
        if re.search(r"(?i)(?<!\S)/(?:Fd|Fp|Yc|Yu)\S*", command):
            return {"index": index, "exit": -1, "error": "unisolated compiler side output"}
        command = source_pattern.sub(lambda _: new_src, command)
        object_file = output / "objects" / f"{index:05d}.obj"
        command, replacements = re.subn(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)',
                                        lambda _: f'/Fo"{object_file}"', command)
        if replacements != 1:
            return {"index": index, "exit": -1, "error": "unexpected object output"}
        command, replacements = re.subn(r"(?i)^cl(?:\.exe)?\b",
                                        lambda _: f'"{compilers[item["arch"]]}"', command, count=1)
        if replacements != 1:
            return {"index": index, "exit": -1, "error": "unexpected compiler command"}
        start = time.monotonic()
        try:
            result = subprocess.run(command, cwd=item["directory"], env=environments[item["arch"]],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=300)
        except subprocess.TimeoutExpired:
            return {"index": index, "exit": -1, "error": "compiler timeout"}
        combined = result.stdout + b"\n" + result.stderr
        markers = sorted(set(value.decode("ascii") for value in marker_pattern.findall(combined)))
        unknown = sorted(set(markers) - known)
        # /showIncludes must never resolve a host mirror header from the
        # uninstrumented tree; otherwise its missing marks are misleading.
        original_prefix = (root / "src").as_posix().lower().encode("utf-8")
        leaks = []
        for line in combined.split(b"\n"):
            normalized = line.replace(b"\\", b"/").lower()
            if b"including file:" in normalized and original_prefix in normalized:
                leaks.append(line.decode("mbcs", errors="replace"))
        return {"index": index, "arch": item["arch"], "source": item["file"],
                "object": item["output"], "exit": result.returncode,
                "elapsed_seconds": round(time.monotonic() - start, 3),
                "markers": markers, "unknown_markers": unknown, "mirror_leaks": leaks,
                "diagnostic_tail": combined[-10000:].decode("mbcs", errors="replace")
                if result.returncode or unknown or leaks else ""}

    results = []
    with (output / "contexts.jsonl").open("w", encoding="utf-8") as journal:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            for result in pool.map(compile_one, enumerate(plan["commands"])):
                results.append(result)
                journal.write(json.dumps(result) + "\n")
                journal.flush()
                if len(results) % 50 == 0:
                    print(f'{len(results)}/{len(plan["commands"])} contexts recorded', flush=True)
    failed = [row for row in results if row["exit"] or row.get("unknown_markers") or row.get("mirror_leaks")]
    hits = sorted({marker for row in results if not row["exit"] for marker in row.get("markers", [])})
    (output / "summary.json").write_text(json.dumps({"contexts": len(results), "failures": len(failed),
        "hits": hits, "note": "Do not use no-hit results before preprocessing-equivalence validation."}, indent=2) + "\n",
        encoding="utf-8")
    if failed:
        raise SystemExit(f"{len(failed)} compile contexts failed or leaked; inspect contexts.jsonl")
    print(f"PASS {len(results)} instrumented compile contexts; {len(hits)} distinct markers")


if __name__ == "__main__":
    main()
