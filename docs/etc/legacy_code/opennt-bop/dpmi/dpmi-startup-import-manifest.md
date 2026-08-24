# OpenNT DPMI Startup/Table Source Mirror

This is the T257 S3 exact-copy staging manifest.  The sources below were
initially copied byte-for-byte by `tools/import/Sync-OpenNtDpmiStartupMirror.ps1`
from `src/opennt/base/mvdm/dpmi32/`.  A mirror remains outside the formal
build until its smallest named shim has been reviewed.

| OpenNT source | Mirror | Original role | Next composition boundary |
| --- | --- | --- | --- |
| `data.c` | `data.c` | shared DOSX/DPMI state | replace only historical pointer-valued state with a fixed-width session record. |
| `debug.c` | `debug.c` | diagnostic declarations | common header closure only. |
| `dpmi32.c` | `dpmi32.c` | dispatcher, DOSX initialisation, table/stack publication | **formal bx-vdm member (T257 S3)** through `dpmi_startup_source_shim`; bounded guest copy/session state, no `Sim32GetVDMPointer` export. |
| `dpmiselr.c` | `dpmiselr.c` | descriptor-table update policy | **formal bx-vdm member (T257 S3)** through `dpmi_descriptor_source_shim`; original descriptor-limit repair/order retained, bounded ES copy and guest-table write replace process-LDT imports; no descriptor cache. |
| `dpmidata.h`, `dpmi32p.h` | same | original data layout and context macros | `dpmi_session_shim` replaces unavailable NT4 context representation. |
| `precomp.h` | same | original compilation closure | replaced only when the source units are admitted to the modern build. |
| `xlathlp.h` | same | pointer/selector translation helpers | bounded guest-memory seam; no flat-address pointer mapping. |

The staged files preserve their original notices and order.  Before a C file
enters the formal Ninja source membership, any local change carries an inline
`DIVERGENCE:` rationale and names the shim declared in the T257 S2 ledger.
`dpmi32.c` and `dpmiselr.c` are the admitted units. `dpmi32.c` retains the
25-slot original table and its three startup/table bodies, while its four old
NT4 pointer/product boundaries are routed only through
`dpmi_startup_source_shim` and `dpmi_startup_session_shim`. `dpmiselr.c`
retains the selected 486 descriptor-update sequence through the named bounded
descriptor seam; it does not activate a `53:xx` route.
