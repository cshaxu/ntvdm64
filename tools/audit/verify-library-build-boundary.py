"""Verify A-library and B-compat inputs in the generated, compiled production Ninja graphs."""
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


# These two original USER units use mirror-owned data layouts. The exception
# is limited to the WOW32 build of the named source, not to the component.
WOW_USER_EXCEPTIONS = {"wow_dialog_lookup.c", "wow_object_ownership.c"}


def allowed_roots(owner):
    roots = {ROOT / "src" / owner}
    if owner == "compat":
        roots.update(ROOT / "src" / library for library in ("lib-nxvm", "lib-win32"))
    return roots


def verify(graph, products):
    arch, component = graph.split("/")
    directory = ROOT / "build/.ninja" / f"msvc-{arch}-release" / component
    commands = ninja(directory, "commands", *products)
    units = {}
    for line in commands.splitlines():
        match = re.search(r' /Fo("?)([^"]+?\.obj)\1\s+"?[^"]*/src/(lib-(?:nxvm|win32)|compat)/([^"]+\.c)"?$', line, re.I)
        if not match:
            continue
        obj, owner, source = match.group(2, 3, 4)
        if graph == "x86/wow32" and owner == "compat" and source in WOW_USER_EXCEPTIONS:
            continue
        units[obj] = owner
        roots = {ROOT / "src"} | allowed_roots(owner)
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
            if header.is_relative_to(ROOT) and not any(header.is_relative_to(root) for root in allowed_roots(units[current])):
                raise ValueError(f"{graph}/{current}: reads {header}")
    if valid != set(units):
        raise ValueError(f"{graph}: missing current compiler evidence for {set(units) - valid}")
    # Collect actual explicit and implicit prerequisites without following product links.
    for owner in set(units.values()):
        objects = [obj for obj, component in units.items() if component == owner]
        for line in ninja(directory, "inputs", *objects).splitlines():
            path = (directory / line.strip().strip('"')).resolve()
            if path.is_relative_to(ROOT) and not any(path.is_relative_to(root) for root in allowed_roots(owner)):
                raise ValueError(f"{graph}/{owner}: unexpected build prerequisite {path}")
    print(f"{graph}: {len(units)} A/B objects checked")


if __name__ == "__main__":
    for name, targets in GRAPHS.items():
        verify(name, targets)
