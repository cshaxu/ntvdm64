"""Compare real and marker-shadow preprocessor output for every product context."""
import argparse
import concurrent.futures
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("plan", type=Path)
    parser.add_argument("shadow", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--indexes", help="comma-separated original plan indexes for targeted retries")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    evidence = (root / "build/t476-s1").resolve()
    shadow, output = args.shadow.resolve(), args.output.resolve()
    if not shadow.is_relative_to(evidence) or not output.is_relative_to(evidence) or output.exists():
        raise SystemExit("require existing shadow and fresh output under build/t476-s1")
    if (shadow / "src/mvdm-guest").exists() or not 1 <= args.jobs <= 16:
        raise SystemExit("guest must not be included; jobs must be 1..16")
    spec = importlib.util.spec_from_file_location("marker_runner", root / "tools/audit/run-mirror-marker-build.py")
    runner = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(runner)
    plan = json.loads(args.plan.read_text(encoding="utf-8"))
    for name, expected in plan["graphs"].items():
        if hashlib.sha256((root / name / "build.ninja").read_bytes()).hexdigest() != expected:
            raise SystemExit(f"build graph changed: {name}")
    environments = {arch: runner.environment(arch) for arch in ("x86", "x64")}
    compilers = {arch: shutil.which("cl.exe", path=env.get("PATH", env.get("Path", "")))
                 for arch, env in environments.items()}
    old_src, new_src = root.as_posix() + "/src", (shadow / "src").as_posix()
    source_pattern = re.compile(r'[\\/]'.join(map(re.escape, old_src.split('/'))) + r'(?=[/\\"\s]|$)', re.I)
    path_patterns = [re.compile(re.escape(value.replace("/", sep).encode()), re.I)
                     for value in (old_src, new_src) for sep in ("/", "\\")]
    marker = re.compile(rb'^\s*#pragma\s+message\("T476_(?:ARM_\d{4}_\d{5}|FILE_\d{4})"\)\s*$')

    def normalize(data):
        lines = []
        for line in data.splitlines():
            if not line.strip() or marker.fullmatch(line):
                continue
            for pattern in path_patterns:
                line = pattern.sub(b"<T476_SOURCE_ROOT>", line)
            lines.append(line)
        return b"\n".join(lines)

    def check(job):
        index, item = job
        command = item["command"]
        command, fo = re.subn(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)', '', command)
        command, includes = re.subn(r'(?i)(?<!\S)/showIncludes\b', '', command)
        command, compile_flag = re.subn(r'(?i)(?<!\S)/c(?=\s)', '/EP', command)
        command, compiler = re.subn(r'(?i)^cl(?:\.exe)?\b', lambda _: f'"{compilers[item["arch"]]}"', command, count=1)
        if (fo, includes, compile_flag, compiler) != (1, 1, 1, 1):
            return {"index": index, "error": "unexpected compiler flags"}
        other = source_pattern.sub(lambda _: new_src, command)
        results = []
        for cmd in (command, other):
            try:
                results.append(subprocess.run(cmd, cwd=item["directory"], env=environments[item["arch"]],
                                              stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=300))
            except subprocess.TimeoutExpired:
                return {"index": index, "error": "preprocessor timeout"}
        a, b = results
        if a.returncode or b.returncode:
            return {"index": index, "error": "preprocessor failed", "exit": [a.returncode, b.returncode],
                    "diagnostic": (a.stderr + b.stderr)[-5000:].decode("mbcs", errors="replace")}
        x, y = normalize(a.stdout), normalize(b.stdout)
        result = {"index": index, "source": item["file"], "equal": x == y,
                  "base_sha256": hashlib.sha256(x).hexdigest(), "marker_sha256": hashlib.sha256(y).hexdigest()}
        if x != y:
            xs, ys = x.splitlines(), y.splitlines()
            first = next((i for i, pair in enumerate(zip(xs, ys)) if pair[0] != pair[1]), min(len(xs), len(ys)))
            result["first_difference"] = {"line": first, "base": xs[first:first + 2][0:2], "marker": ys[first:first + 2][0:2]}
            result["first_difference"] = {k: [v.decode("mbcs", errors="replace") for v in value] if isinstance(value, list) else value
                                          for k, value in result["first_difference"].items()}
        return result

    output.mkdir(parents=True)
    jobs = list(enumerate(plan["commands"]))
    if args.indexes:
        selected = {int(index) for index in args.indexes.split(",")}
        jobs = [(index, item) for index, item in jobs if index in selected]
        if len(jobs) != len(selected):
            raise SystemExit("invalid plan index")
    results = []
    with (output / "contexts.jsonl").open("w", encoding="utf-8") as journal:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            for result in pool.map(check, jobs):
                results.append(result)
                journal.write(json.dumps(result) + "\n")
                journal.flush()
                if len(results) % 50 == 0:
                    print(f'{len(results)}/{len(jobs)} preprocess contexts checked', flush=True)
    failures = [r for r in results if not r.get("equal")]
    (output / "summary.json").write_text(json.dumps({"contexts": len(results), "failures": len(failures)}, indent=2) + "\n", encoding="utf-8")
    if failures:
        raise SystemExit(f"{len(failures)} preprocess contexts differ or failed")
    print(f"PASS {len(results)} preprocess contexts")


if __name__ == "__main__":
    main()
