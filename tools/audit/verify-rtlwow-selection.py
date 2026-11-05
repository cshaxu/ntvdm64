"""Observe four previously unverified rtl/wow.c arms in the WOW32 graph."""
import argparse
import hashlib
from pathlib import Path
import re
import struct
import subprocess


def text_sections(path):
    data = path.read_bytes()
    machine, count, _, _, _, optional, _ = struct.unpack_from("<HHIIIHH", data)
    if machine != 0x14C:
        raise ValueError("expected x86 COFF object")
    sections = []
    for index in range(count):
        offset = 20 + optional + index * 40
        name = data[offset:offset + 8].rstrip(b"\0")
        _, _, size, raw, reloc, _, reloc_count, _, _ = struct.unpack_from(
            "<IIIIIIHHI", data, offset + 8)
        if name.startswith(b".text"):
            sections.append((data[raw:raw + size], data[reloc:reloc + reloc_count * 10]))
    return sections


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    parser.add_argument("original", type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    output = args.output.resolve()
    if not output.is_relative_to((root / "build/t476-s1").resolve()) or output.exists():
        raise SystemExit("require a fresh directory under build/t476-s1")
    source = root / "src/non-mvdm/windows/core/ntuser/rtl/wow.c"
    graph = root / "build/.ninja/t476-s1/wow32"
    product_obj = graph / "obj/opennt-user/control-view-wow.obj"
    before = args.original.read_bytes()
    expected = "71e5b523bd44077ab4e86ffc6093544922c815c759bb753c32b4155e3206691d"
    if hashlib.sha256(before).hexdigest() != expected:
        raise SystemExit("original identity mismatch")
    text = source.read_text(encoding="utf-8")
    original = before.decode("utf-8").splitlines()
    for first, last in ((109, 126), (151, 168), (245, 245), (345, 345)):
        if "\n".join(original[first - 1:last]) not in text:
            raise SystemExit(f"restored source differs at original lines {first}-{last}")
    output.mkdir(parents=True)
    for line in (109, 151, 245, 345):
        marker = f"#line {line}\n"
        if text.count(marker) != 1:
            raise SystemExit(f"unexpected line anchor {line}")
        text = text.replace(marker, marker +
                            f'#pragma message("T476_RTLWOW_SELECTED_{line}")\n#line {line}\n')
    shadow = output / "rtlwow-shadow.c"
    shadow.write_text(text, encoding="utf-8")
    commands = subprocess.check_output(["ninja", "-C", str(graph), "-t", "commands",
                                        "obj/opennt-user/control-view-wow.obj"], text=True)
    command = commands.splitlines()[-1]
    source_path = source.as_posix()
    if source_path not in command:
        raise SystemExit("unexpected graph source command")
    command = command.replace(source_path, shadow.as_posix())
    replacement = f'/Fo"{output / "rtlwow-shadow.obj"}"'
    command, substitutions = re.subn(r"/Fo\S+", lambda _: replacement, command)
    if substitutions != 1:
        raise SystemExit("unexpected output argument")
    vs = r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
    runner = output / "run.cmd"
    runner.write_text(f'@echo off\ncall "{vs}" -arch=x86 -host_arch=x64 >nul\n'
                      f'if errorlevel 1 exit /b %errorlevel%\ncd /d "{graph}"\n'
                      f'{command}\nexit /b %errorlevel%\n', encoding="ascii")
    result = subprocess.run(["cmd", "/d", "/c", str(runner)], capture_output=True,
                            text=True, errors="replace", timeout=180)
    (output / "stdout.txt").write_text(result.stdout, encoding="utf-8")
    (output / "stderr.txt").write_text(result.stderr, encoding="utf-8")
    if result.returncode:
        raise SystemExit("shadow compile failed; inspect captured output")
    for line in (109, 151, 245, 345):
        if f"T476_RTLWOW_SELECTED_{line}" not in result.stdout + result.stderr:
            raise SystemExit(f"missing observed arm at {line}")
    actual = text_sections(product_obj)
    observed = text_sections(output / "rtlwow-shadow.obj")
    if not actual or actual != observed:
        raise SystemExit("text sections or relocation records differ")
    print(f"PASS four restored arms and {len(actual)} x86 text sections with relocations")


if __name__ == "__main__":
    main()
