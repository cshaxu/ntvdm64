# M0 T282 S11 — COMMAND OEM support composition evidence

## Question

Can the original OEM helper bodies needed by COMMAND environment/current-
directory behavior run through the existing adapter-win32 layer without a new
environment rewrite?

## Inputs

- Exact `opennt-mvdm-support/oemuni/process.c` and `oemuni/file.c`.
- Exact `opennt-mvdm-support/inc/oemuni.h` declarations.
- `adapter-win32/source/opennt_support_rtl.c` and its same-shaped headers.

## Procedure

The formal Ninja graph compiles both complete original translation units,
links their reached helpers with `opennt_support_rtl.c` and public
`kernel32.lib`/`gdi32.lib`, then invokes original `GetEnvironmentVariableOem`
and `GetCurrentDirectoryOem` through a read-only host query fixture.

The x64 and x86 `/MT` runs both report:

```text
PASS: original OEM environment/current-directory helpers
```

`dumpbin` review of both original objects found that every reached RTL/TEB/PEB
import (`Rtl*String`, heap, current-directory, PEB lock and
`RtlNtStatusToDosError`) is already supplied by `opennt_support_rtl.c`; the
remaining API imports are public Kernel32/GDI32 entries.  `RemoveFontResourceW`
requires `gdi32.lib`; adding that library selection preserves the original
source body and adds no adapter.

## Interpretation

The selected OEM helper algorithms are directly composable original source,
not source-derived replacements.  They are ready to serve the COMMAND group
once its three S9 guest spans are routed through bounded adapter-softpc leases.

## Limits

The complete historical files retain existing warnings in unrelated branches,
including pointer-width casts in file-search/font helpers.  Those branches are
not entered by this read-only COMMAND environment/current-directory slice and
must receive their own tracker disposition before activation.  This S enables
no BOP route and changes no COMMAND mirror file.
