# M0 T353 S1 — post-`SVC_CMDGETCONFIGSYS` owner admission

## Source result

The already-proven `54:0C` is not a command-delivery or workload boundary.
The original chain is:

```text
NTDOS sysinit1.asm::ProcessConfig
  -> CMDSVC SVC_GETCONFIGSYS (54:0C)
  -> MS_bop_4 -> CmdDispatch(12) -> cmdGetConfigSys
  -> ExpandConfigFiles(TRUE) -> GetPIFConfigFiles / config.nt
  -> temporary expanded CONFIG pathname copied to the original DS:DX buffer
  -> return through MS_bop_4 (original service-byte IP advance)
  -> ProcessConfig restores DS -> doconf
  -> later CONFIG device/load passes
```

`cmdGetConfigSys` is a `VOID` original provider.  It must either write an OEM
temporary-pathname into the BIOS caller's at-least-64-byte DS:DX output buffer
and return, or retain the original error-dialog plus `TerminateVDM` direction.
It does not itself execute a DOS workload.

## Current binding facts

- T352's durable CPU40/x86 observation proves both `54:0C` table return and
  the original `MS_bop_4` post-return `setIP(+1)`.
- The selected mirror keeps original `cmdconf.c` control flow.  Its limited
  `MVDM-HOST-DIV-158` uses the app-selected MVDM system-root input where NT4
  used its installed Windows directory; `MVDM-HOST-DIV-157` makes the same
  replacement for the no-PIF `config.nt`/`autoexec.nt` lookup in original
  `nt_pif.c`.  Both preserve the source buffer and fatal bad-file result.
- The configuration output remains an original short-lived guest DS:DX write
  through the CPU40/SAS mapping boundary.  No durable raw guest pointer is
  selected.

## Selected next cohort

The next complete owner cohort is **NTDOS CONFIG processing through the
original `ProcessConfig -> doconf` continuation**, including the immediately
required original DOS INT21 file/memory/parser edges and the already-selected
CPU40/SAS execution boundary.  It is not a `54:0C` replacement, a new DEM
leaf, nor an `EXEC`/graphics package.

S2 may only first audit that whole NTDOS continuation's immediate source
dependencies and select one bounded observation/implementation form.  It may
not alter CONFIG media, BOP return semantics, guest bytes, or machine behavior
without a new admission.

## Evidence boundaries

This is a static source and current-binding result.  It makes no new build or
runtime claim.  The complete per-edge record is in
[`m0-t353-s1-post-cmdgetconfigsys-owner-ledger.tsv`](../operations/m0-t353-s1-post-cmdgetconfigsys-owner-ledger.tsv).
