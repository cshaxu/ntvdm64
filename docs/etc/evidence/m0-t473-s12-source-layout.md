# Source-layout rename evidence

S12 moves the selected non-MVDM original host and ABI source cohorts into
`src/non-mvdm/host` and `src/non-mvdm/abi`, and shared project-owned ABI
bindings into `src/compat`. Component-private adapters stay with their owner.
The original S12 NTCON handoff moves to S13 without a completion claim.

The two original cohorts have three nonidentical same-relative-path files:
`public/sdk/inc/ntexapi.h`, `ntrtl.h`, and `nturtl.h`. Both copies and the
previous selected include order are preserved. The 383 tracked original
host/ABI files were compared to the pre-move Git blobs byte for byte: only the host README and the include paths in `vdm.c` and
`chartran.c` differ. MVDM host files `cmdenv.c` and `cmdexec.c` each update
existing include paths, recorded as MVDM-HOST-DIV-494; no guest binaries were
changed. These path-only edits are registered in the mirror READMEs.

The x86 and x64 cached builds passed, as did the check of 428 authored product
commands for SoftPC profile macros. A same-package-identity comparison against
the previous release found two images byte-identical, seven different only in
PE timestamps and WOW32 differing in `.text` and data/relocation bytes. The
WOW32 bytes are not asserted equivalent merely because source paths changed;
the complete staged candidate passed all 16 Full gates and all four Win3.1
Standard/386 by DOSONLY combinations; the 68 original and isolated guest
binaries retained their hashes. The candidate was published and all ten image
hashes were verified at each destination. After restoring six original host
files to their exact Git-blob line endings, both cached incremental builds
passed; same-package-identity comparison found four images byte-identical and
six differing only in PE link timestamps, so the already tested release was
retained without repeating tests. [Results](../../../artifacts/mvdm-audit/source-layout-rename/).
The DOS4GW known issue remains in TODO and its optional fixture is not silently
claimed as passing by the normal Full run.
