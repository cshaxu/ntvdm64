"""Verify A-library and B-compat inputs in the generated, compiled production Ninja graphs."""
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GRAPHS = {
    "x86/worker": ["ntvdm.exe", "VCDEX.DLL", "VDMREDIR.dll", "nthook32.dll"],
    "x86/wow32": ["wow32.dll"],
    "x64/worker-reference-x86": ["ntvdm.exe"],
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


# S19/S20: no A/B unit is exempt from the inward compile boundary.


def allowed_roots(owner, source=""):
    roots = {ROOT / "src" / owner}
    if owner == "compat":
        roots.add(ROOT / "src/compat/include")
        roots.update(ROOT / "src" / library for library in ("lib-nxvm", "lib-win32"))
    return roots


# No A/B unit may consume generated C-private layout declarations.
def generated_inputs(directory, owner, source):
    return set()


def verify(graph, products):
    arch, component = graph.split("/")
    directory = ROOT / "build/.ninja" / f"msvc-{arch}-release" / component
    commands = ninja(directory, "commands", *products)
    units = {}
    for line in commands.splitlines():
        match = re.search(r'\s+"?[^" ]*/src/(lib-(?:nxvm|win32)|compat)/([^" ]+\.c)"?$', line, re.I)
        if not match:
            continue
        output = re.search(r'(?:^|\s)[/-]Fo(?:"([^"]+)"|(\S+))', line, re.I)
        if not output:
            raise ValueError(f"{graph}: compiler output missing: {line}")
        obj = output[1] or output[2]
        owner, source = match.group(1, 2)
        units[obj] = (owner, source)
        roots = {ROOT / "src"} | allowed_roots(owner, source)
        for path in re.finditer(r'(?:^| )[/-]I\s*(?:"([^"]+)"|(\S+))', line, re.I):
            if not any((directory / (path[1] or path[2])).resolve() == r for r in roots) and not any(
                    (directory / (path[1] or path[2])).resolve() == h.parent
                    for h in generated_inputs(directory, owner, source)):
                raise ValueError(f"{graph}/{obj}: unexpected include root {path[0]}")
        if re.search(r'(?:^| )[/-]FI', line):
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
            header = (directory / line.strip()).resolve()
            if (header.is_relative_to(ROOT) and header not in generated_inputs(directory, *units[current])
                    and not any(header.is_relative_to(root) for root in allowed_roots(*units[current]))):
                raise ValueError(f"{graph}/{current}: reads {header}")
    if valid != set(units):
        raise ValueError(f"{graph}: missing current compiler evidence for {set(units) - valid}")
    # Collect actual explicit and implicit prerequisites without following product links.
    for roots in {frozenset(allowed_roots(*identity) | generated_inputs(directory, *identity)) for identity in units.values()}:
        objects = [obj for obj, identity in units.items()
                   if frozenset(allowed_roots(*identity) | generated_inputs(directory, *identity)) == roots]
        for line in ninja(directory, "inputs", *objects).splitlines():
            # CMake's empty object-order phony node names its working directory.
            # It is not a source/header; all actual file prerequisites remain checked.
            if line.strip() == ".":
                continue
            path = (directory / line.strip().strip('"')).resolve()
            if path.is_relative_to(ROOT) and not any(path.is_relative_to(root) for root in roots):
                raise ValueError(f"{graph}: unexpected build prerequisite {path}")
    print(f"{graph}: {len(units)} A/B objects checked")


if __name__ == "__main__":
    for name, targets in GRAPHS.items():
        verify(name, targets)
