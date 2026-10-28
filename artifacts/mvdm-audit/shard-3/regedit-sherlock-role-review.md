# Shard 3: Win16 registration editor and Dr Watson inputs

The [observer](regedit-sherlock-observer.py) and
[observations](regedit-sherlock-observations.json) verify six current files,
available original edition hashes and both frozen catalogs, the actual
original build/resource recipes, and current ZIP/direct-path observations.
This is a read-only proposal; it neither modifies guests nor claims the
listed original inputs produced an installed program.

| File | Proposed | Reason and remaining boundary |
| --- | --- | --- |
| `regedit/regedit.ico` | excluded historical guest resource-input role | Original default-USA `regedit.rc` names `RegEdit.ico`; the REGEDT16 recipe compiles/attaches its resource. Actual RC search resolution and source-to-distributed image remain unproved. |
| `regedit/sdkreged.h` | excluded historical guest build-input role | Original makefile and `sdkreged.c`, `regmain.c`, `sdbase.c`, `virt.c` identify this declaration input. Guest registry operations and their host counterparts remain in scope. |
| `regedit/debug/REGEDIT.RES` | pending | Current-only binary with no frozen original/catalog identity; debug resource may be an output of the debug destination but its exact provenance and consumer are unproved. |
| `regedit/sdkreged.ico` | pending | Root icon differs from the named `RegEdit.ico` in the default resource script; other SDK/international/debug consumers need verification. |
| `sherlock/DRWATSON.EXE` | pending | Original guest executable matches both original editions, but no same-content or named DRWATSON member was observed in ZIP and neither direct `O:/winnt` nor `O:/winnt/system32` path exists. Its intended delivery and loader path remain unresolved. |
| `sherlock/watson.ico` | pending | Default USA resource explicitly names `doctor.ico`, not this icon. Alternate locale/build consumers remain unresolved. |

These exclusions refer only to historic build input roles. They do not remove
registry editing, crash diagnosis, or support for running guest applications
from scope. ZIP/direct-path observations are point-in-time and do not rule out
application directories or other search paths. The five cataloged original
files retain 100% source/input bytes, which is not a measure of functional
recovery; `debug/REGEDIT.RES` has no original comparator. No product or guest
image was built, changed, installed or executed.
