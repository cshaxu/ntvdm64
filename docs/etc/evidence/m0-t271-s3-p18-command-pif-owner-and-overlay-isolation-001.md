# M0 T271 S3 P18 — COMMAND PIF parser owner and overlay isolation

## Decision

`softpc.new/host/src/nt_pif.c` remains an exact OpenNT source file in the
`opennt-bop` COMMAND owner-family mirror.  It is not a candidate for an
`opennt-host` move in the current component graph.

The conclusion follows the reached original source relationship rather than
the source directory's historical name:

- OpenNT `dos/command/cmdpif.c` consumes `GetPIFData(&pfdata, ...)` using its
  COMMAND `cmdpif.h` `PIF_DATA` layout.
- OpenNT `softpc.new/host/src/nt_pif.c` maintains the parser-global `pfdata`
  and its `GetPIFData` body reads that global in addition to its parameter.
- OpenNT `softpc.new/host/inc/nt_pif.h` has a distinct `PIF_DATA` layout
  (`fgprio` is present there but absent from COMMAND's record).  A public host
  provider ABI would therefore either change an original caller layout or
  make `opennt-host` depend back on COMMAND definitions.

The parser source remains byte-for-byte OpenNT.  The required modern
translation-unit binding is project-authored and is now located at
`opennt-bop-overlay/command/opennt_pif_composition.c`.  Its matching mirror
file is the sole `BOP-DIV-032` include boundary.  No other component includes
the overlay.

## Retained source and bounded modern seams

- `nt_pif.c` retains parser order, PIF defaults, file reads, PIF extension
  precedence, allocation/retry decision and CONFIG/AUTOEXEC selection.
- `adapter-win32` continues to provide only unavailable OEM path/environment/
  console-title spellings and the shared source-shaped dialog result.
- The two layout records do not cross a public component ABI.  The private
  bridge is confined to the source owner's translation unit.

## Verification

Outside the sandbox, the formal graph was regenerated at
`build/M0-T271-S3/r003` and Ninja rebuilt
`bin/t234-s2-command-pif-direct-fixture.exe` successfully.  Running it
returned zero and reported:

```text
T234 S2 original OpenNT PIF parser: title, command, directory, target,
close-on-exit, and early CONFIG/AUTOEXEC selection verified
```

This proves the original parser and its bounded COMMAND layout bridge.  It
does not claim the separate historical PIF console editor, WOW product shell
or any unadmitted machine policy.
