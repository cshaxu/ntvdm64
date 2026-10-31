"""Check actual product compiler commands for SoftPC profile leakage."""
from pathlib import Path
import re
import subprocess


ROOT = Path(__file__).resolve().parents[2]
FLAGS = {"CCPU", "CPU_40_STYLE", "NEW_CPU", "C_VID", "SPC386", "SIM32", "V7VGA", "PROD", "NTVDM", "ANSI"}
SOURCE_FLAGS = FLAGS | {"CPU_PRIVATE"}
ORIGINAL_ROOTS = ("src/mvdm/", "src/opennt-host/", "src/opennt-abi/source/")


def main():
    checked = 0
    failures = []
    scanned_sources = set()
    targets = [("msvc-x86-release", "worker", "ntvdm.exe"),
               ("msvc-x86-release", "wow32", "wow32.dll")]
    targets += [("msvc-x64-release", tree, target) for tree, target in
                (("worker", "ntvwm.exe"), ("frontend", "ntcon.exe"),
                 ("monitor", "ntmon.exe"), ("launcher", "run16.exe"),
                 ("service", "ntsrv.exe"), ("hook", "nthook64.dll"))]
    for architecture, tree, target in targets:
        build = ROOT / "build/.ninja" / architecture / tree
        output = subprocess.run(["ninja", "-C", str(build), "-t", "commands", target],
                                check=True, capture_output=True, text=True).stdout
        for command in output.splitlines():
            if "cl.exe " not in command:
                continue
            match = re.search(r'("[^"]+\.(?:c|cpp)"|\S+\.(?:c|cpp))\s*$',
                              command, re.IGNORECASE)
            if not match:
                raise ValueError(f"unrecognized compile command: {command}")
            source = match.group(1).strip('"').replace("\\", "/").lower()
            relative = source.split("/src/", 1)[-1] if "/src/" in source else ""
            original = bool(relative) and ("src/" + relative).startswith(ORIGINAL_ROOTS)
            original |= source.endswith("/worker/generated/base_vdm_config.c")
            if original:
                continue
            checked += 1
            defined = set(re.findall(r"(?:^|\s)/D([A-Za-z_][A-Za-z_0-9]*)", command))
            leaked = sorted(defined & FLAGS)
            if architecture == "msvc-x86-release" and "WIN32" in defined:
                leaked.append("WIN32")
            if leaked:
                failures.append(f"{architecture}/{tree}: {source}: {', '.join(leaked)}")
            if relative and relative not in scanned_sources and "/detours/" not in relative:
                scanned_sources.add(relative)
                source_text = (ROOT / "src" / relative).read_text(encoding="utf-8", errors="replace")
                for line_number, line in enumerate(source_text.splitlines(), 1):
                    if re.match(r"\s*#\s*(?:if|ifdef|ifndef|elif|define)\b", line):
                        used = sorted(set(re.findall(r"\b[A-Za-z_][A-Za-z_0-9]*\b", line)) & SOURCE_FLAGS)
                        if used:
                            failures.append(f"{source}:{line_number}: {', '.join(used)}")
    if failures:
        raise SystemExit("SoftPC profile leaked into authored code:\n" + "\n".join(failures))
    if not checked:
        raise SystemExit("No authored compile commands inspected")
    print(f"PASS: {checked} authored/generated product commands without SoftPC profile flags")


if __name__ == "__main__":
    main()
