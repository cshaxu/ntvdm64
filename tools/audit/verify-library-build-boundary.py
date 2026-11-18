"""Verify A-library inputs in the generated, compiled production Ninja graphs."""
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GRAPHS = {
    "x86/worker": ["ntvdm.exe", "VCDEX.DLL", "VDMREDIR.dll", "nthook32.dll"],
    "x86/wow32": ["wow32.dll"],
    "x64/frontend": ["ntcon.exe"],
    "x64/service": ["ntsrv.exe"],
    "x64/launcher": ["run16.exe"],
    "x64/monitor": ["ntmon.exe"],
    "x64/worker": ["ntvwm.exe"],
    "x64/hook": ["nthook64.dll"],
}


def ninja(directory, tool, *args):
    return subprocess.check_output(
        ["ninja", "-C", directory, "-t", tool, *args],
        text=True, errors="replace",
    ).replace("\\", "/")


def verify(graph, products):
    arch, component = graph.split("/")
    directory = ROOT / "build/.ninja" / f"msvc-{arch}-release" / component
    commands = ninja(directory, "commands", *products)
    units = {}
    for line in commands.splitlines():
        match = re.search(r' /Fo("?)([^"]+?\.obj)\1\s+"?[^"]*/src/(lib-(?:nxvm|win32))/[^"]+\.c"?$', line, re.I)
        if not match:
            continue
        obj, owner = match.group(2, 3)
        units[obj] = owner
        roots = {(ROOT / "src").resolve(), (ROOT / "src" / owner).resolve()}
        for path in re.finditer(r'(?:^| )/I\s*(?:"([^"]+)"|(\S+))', line, re.I):
            if (directory / (path[1] or path[2])).resolve() not in roots:
                raise ValueError(f"{graph}/{obj}: unexpected include root {path[0]}")
        if re.search(r'(?:^| )/FI', line):
            raise ValueError(f"{graph}/{obj}: forced header")
    if not units:
        raise ValueError(f"{graph}: no A objects found")
    records = ninja(directory, "deps", *units)
    current = None
    valid = set()
    for line in records.splitlines():
        if not line.startswith(" "):
            current, _, state = line.partition(": #deps")
            if current in units and "(VALID)" in state:
                valid.add(current)
        elif current in units:
            header = Path(line.strip()).resolve()
            if header.is_relative_to(ROOT) and not header.is_relative_to(ROOT / "src" / units[current]):
                raise ValueError(f"{graph}/{current}: reads {header}")
    if valid != set(units):
        raise ValueError(f"{graph}: missing current compiler evidence for {set(units) - valid}")
    # Collect actual explicit and implicit prerequisites without following product links.
    prerequisites = ninja(directory, "inputs", *units)
    for line in prerequisites.splitlines():
        path = (directory / line.strip().strip('"')).resolve()
        if path.is_relative_to(ROOT) and not any(path.is_relative_to(ROOT / "src" / owner) for owner in set(units.values())):
            raise ValueError(f"{graph}: unexpected build prerequisite {path}")
    print(f"{graph}: {len(units)} library objects")


if __name__ == "__main__":
    for name, targets in GRAPHS.items():
        verify(name, targets)
