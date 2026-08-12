# BYOB Bootstrap Resource Ownership

## Decision

The fixed NT4 EN-US BYOB profile supplies immutable identities and transient
byte access.  It does not own historical DOS transitions.  The first-cut
resource authority is:

| Role | Identity owner | Byte access owner | Placement/execution owner |
| --- | --- | --- | --- |
| `NTIO.SYS` | BYOB profile selection | profile resource loader | backend entry transaction before first instruction |
| `NTDOS.SYS` | BYOB profile selection | historical `demLoadDos` resource bridge | original `SVC_DEMLOADDOS`, requested by NTIO with live `DI` |
| `COMMAND.COM` | BYOB profile selection | contained normal DEM file path | original NTDOS open/EXEC flow |
| owned smoke command | profile/session command policy | contained normal DEM file path | original `COMMAND.COM /C` flow |

This preserves the source-evidenced chain:

```text
profile validates identities
  -> initial entry loads NTIO only
  -> NTIO invokes SVC_DEMLOADDOS
  -> original demLoadDos obtains selected NTDOS bytes and applies its live DI
  -> NTDOS opens/executes COMMAND.COM
  -> COMMAND.COM opens the owned smoke command
```

## Existing Evidence

`byob_profile_validate_file_select` accepts exactly the required `ntio`,
`ntdos`, and `command` descriptors only after every selected component is
opened and SHA-256/size verified.  The resulting `byob_profile_selection`
contains file name, hash, and size only; it contains neither path nor bytes.

`byob_image_load_exact` may reopen one selected component under the explicitly
chosen root, reject reparse points, verify the selected identity, and return a
transient owned buffer.  Callers must release it after the immediate handoff;
there is no guest-image cache, system directory fallback, current-directory
lookup, or diagnostic byte dump.

The historical source is decisive for NTDOS.  `demmisc.c:152-207` documents
that NTIO invokes `demLoadDos`, provides the live `DI` load segment, and that
the function owns the NTDOS read.  The historical use of `GetSystemDirectory`
and mutable `pszDefaultDOSDirectory` is a retired host policy, not a permitted
modern file-selection rule.  A later narrow resource bridge may satisfy that
one original request from the selected NTDOS descriptor, but it may not preload
NTDOS or substitute another SVC handler.

`file.asm:212-219` and the M22/M25 records establish that COMMAND is opened
later through NTDOS and DEM.  It must not be converted into a preload or an
out-of-band host process.

## Retired Planning Fixtures

`src/nt4_ntdos_load_plan.[ch]` and
`src/nt4_bootstrap_transaction.[ch]` are default-disabled owned fixtures.
They demonstrate only source-derived range arithmetic for synthetic byte
buffers.  Their NTDOS preload representation is **not** an executable startup
plan and must not be connected to `ntdos64-run`, a backend, mantle, or a
future runtime.  In particular, the two-preload transaction is incompatible
with the required historical `SVC_DEMLOADDOS` ownership.

Their value remains bounded trace evidence: a backend that receives a live
`DI` from the original chain will eventually require full-range placement
validation.  That validation belongs at the `demLoadDos` resource bridge, not
at initial machine construction.

## Modern Resource Bridge Constraints

The eventual bridge for each role must:

1. start from a copied descriptor selected by successful BYOB validation;
2. reopen only beneath the selected BYOB root and recheck size/hash before
   exposing bytes;
3. retain no component path or bytes after the immediate historical operation;
4. report role, length, hash prefix only when producing diagnostics;
5. reject a role request that is absent, mismatched, oversized, or no longer
   rooted, without falling back to a Windows system location;
6. leave guest memory untouched until the backend-specific entry/write
   transaction has fully preflighted the requested ordinary-RAM range.

## Outcome

M30 is complete as a resource-ownership record.  The next high-ROI work is to
design the one original `demLoadDos` resource bridge against the selected NTDOS
descriptor.  It must combine M29's guest-write transaction with the historical
live `DI` load request, but remain default-disabled until a backend reaches
that original SVC.
