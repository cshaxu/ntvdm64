"""Select the C-owned coordination for caller unit tests; no CPU equivalence claim."""
import argparse
import hashlib
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("output")
parser.add_argument("--core-only", action="store_true")
a = parser.parse_args()
root = Path(__file__).resolve().parents[2]
source = root / "src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c"
raw = source.read_bytes()
text = raw.decode("latin1").replace("\r\n", "\n")
start = "int ccpu_guest_memory_acquire("
end = "#define MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK" if a.core_only else "#line 1960"
assert text.count(start) == text.count(end) == 1, "Review changed C source boundaries"
begin = text.index(start)
body = text[begin:text.index(end, begin)]
headers = ["guest_memory_access.h", "guest_location.h", "guest_memory_lease.h", "host_context.h"]
preamble = "".join('#include "' + (root / "src/compat/include" / h).as_posix() + '"\n' for h in headers)
preamble += "#include <stddef.h>\n#include <string.h>\n"
preamble += "int fixture_guest_read(void *, uint32_t, uint8_t *, uint32_t);\n"
preamble += "int fixture_guest_write(void *, uint32_t, const uint8_t *, uint32_t);\n"
preamble += "#define mvdm_softpc_guest_memory_read fixture_guest_read\n"
preamble += "#define mvdm_softpc_guest_memory_write fixture_guest_write\n"
preamble += '#line %d "%s"\n' % (text[:begin].count("\n") + 1, source.as_posix())
output = Path(a.output).resolve()
assert output.is_relative_to(root / "build"), "Fixture output must remain under build"
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(preamble + body, encoding="utf-8", newline="\n")
output.with_suffix(".sha256").write_text(hashlib.sha256(raw).hexdigest() + "\n")
