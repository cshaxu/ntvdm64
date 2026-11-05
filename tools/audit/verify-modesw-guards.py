"""Compile isolated original-i386 guards from the actual selected file."""
import argparse
from pathlib import Path
import re
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("require a fresh directory under build/t476-s1")
    output.mkdir(parents=True)
    source = (root / "src/mvdm-host/dpmi32/modesw.c").read_text(encoding="utf-8")
    guards = re.findall(
        r"(#line\s+\d+\s*\n#if defined\(i386\)\s*\n"
        r"#error DIVERGENCE MVDM-HOST-DIV-619[^\n]*\n#line\s+\d+\s*\n#else)",
        source,
    )
    if len(guards) != 2:
        raise SystemExit("expected exactly two source guards")
    vs = Path(r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat")
    for index, (guard, line) in enumerate(zip(guards, (50, 98)), 1):
        fixture = output / f"guard-{index}.c"
        fixture.write_text(guard + "\n#endif\n", encoding="ascii")
        runner = output / f"guard-{index}.cmd"
        runner.write_text(
            f'@echo off\ncall "{vs}" -arch=x86 -host_arch=x64 >nul\n'
            f'if errorlevel 1 exit /b %errorlevel%\n'
            f'cl /nologo /TC /E /Di386 "{fixture}"\nexit /b %errorlevel%\n',
            encoding="ascii",
        )
        result = subprocess.run(["cmd", "/d", "/c", str(runner)], capture_output=True,
                                timeout=120, text=True, errors="replace")
        (output / f"guard-{index}.stdout").write_text(result.stdout, encoding="utf-8")
        (output / f"guard-{index}.stderr").write_text(result.stderr, encoding="utf-8")
        diagnostic = f"({line}): fatal error C1189: #error:  DIVERGENCE MVDM-HOST-DIV-619"
        if result.returncode == 0 or diagnostic not in result.stderr:
            raise SystemExit(f"guard {index}: expected rejection at original line {line}")
        print(f"PASS guard {index}: rejected i386 at original line {line}")


if __name__ == "__main__":
    main()
