"""Wire selected caller fixtures to current C coordination with test address-space I/O."""
import argparse
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("graph")
p.add_argument("--core-only", action="store_true")
p.add_argument("--target")
a = p.parse_args()
root = Path(__file__).resolve().parents[2]
graph = Path(a.graph).resolve()
assert graph.is_relative_to(root / "build")
text = graph.read_text(encoding="utf-8-sig").split("\n# Guest memory caller fixture wiring\n")[0]
text = text.split("\nrule extract_guest_memory_fixture\n")[0]
text = text.replace(" obj/guest-memory-coordination.obj", "").replace(" obj/guest-memory-location.obj", "").replace(" obj/guest-memory-admission.obj", "")
targets = a.target.split(",") if a.target else []
def ninja(path): return path.as_posix().replace(":", "$:")
source = root / "src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c"
extract = root / "tools/build/extract-guest-memory-fixture.py"
output = graph.parent / "guest-memory-coordination.c"
# Original source changes invalidate the generated unit, rather than reusing a snapshot.
addition = "\n# Guest memory caller fixture wiring\nrule extract_guest_memory_fixture\n  command = python \"%s\" \"%s\"%s\n" % (extract.as_posix(), output.as_posix(), " --core-only" if a.core_only else "")
addition += "build %s: extract_guest_memory_fixture %s %s\n" % (ninja(output), ninja(source), ninja(extract))
flags = "flags" if "\nflags = " in text else "cflags"
addition += "build obj/guest-memory-coordination.obj: cc %s\n  %s = $%s /Gy\n" % (ninja(output), flags, flags)
scalar = not a.core_only and "src/compat/guest_location.c" not in text
if scalar:
    addition += "build obj/guest-memory-location.obj: cc %s\n" % ninja(root / "src/compat/guest_location.c")
admission = "src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c" not in text
if admission:
    addition += "build obj/guest-memory-admission.obj: cc %s\n  %s = $%s /I \"%s\"\n" % (ninja(root / "src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c"), flags, flags, (root / "src/ntvdm-exe/softpc/include").as_posix())
lines = []
matched = 0
for line in text.splitlines():
    if line.startswith("build ") and ".exe: " in line and (not targets or any(line.startswith("build " + target + ":") for target in targets)):
        line += " obj/guest-memory-coordination.obj"
        if scalar: line += " obj/guest-memory-location.obj"
        if admission: line += " obj/guest-memory-admission.obj"
        matched += 1
    if not a.target and "link " in line and "/nologo" in line and "/OPT:REF" not in line.upper():
        line = line.replace("/nologo", "/nologo /OPT:REF")
    lines.append(line)
assert matched, "No selected caller fixture link"
graph.write_text("\n".join(lines) + "\n" + addition, encoding="utf-8", newline="\n")
