# OpenNT DPMI Startup/Table Source Mirror

This is the T257 S3 exact-copy staging manifest.  The sources below were
copied byte-for-byte by `tools/import/Sync-OpenNtDpmiStartupMirror.ps1` from
`src/opennt/base/mvdm/dpmi32/`.  They are not formal build members yet.

| OpenNT source | Mirror | Original role | Next composition boundary |
| --- | --- | --- | --- |
| `data.c` | `data.c` | shared DOSX/DPMI state | replace only historical pointer-valued state with a fixed-width session record. |
| `debug.c` | `debug.c` | diagnostic declarations | common header closure only. |
| `dpmi32.c` | `dpmi32.c` | dispatcher, DOSX initialisation, table/stack publication | bounded guest copy and session state; no `Sim32GetVDMPointer` export. |
| `dpmiselr.c` | `dpmiselr.c` | descriptor-table update policy | native Bochs descriptor authority; refuse old process-LDT imports. |
| `dpmidata.h`, `dpmi32p.h` | same | original data layout and context macros | `dpmi_session_shim` replaces unavailable NT4 context representation. |
| `precomp.h` | same | original compilation closure | replaced only when the source units are admitted to the modern build. |
| `xlathlp.h` | same | pointer/selector translation helpers | bounded guest-memory seam; no flat-address pointer mapping. |

The staged files preserve their original notices and order.  Before a C file
enters the formal Ninja source membership, any local change carries an inline
`DIVERGENCE:` rationale and names the shim declared in the T257 S2 ledger.
