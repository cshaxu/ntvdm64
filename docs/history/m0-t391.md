# m0-t391 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t391-command-lifecycle-closure-20260906.md

# M0 T391 Closure — COMMAND lifecycle and x86 package

## Scope

T391 owned direct app-entry disposition and the original COMMAND child
lifecycle for the published Win32/x86 product. Its boundaries are retained in
the final audit: DOS and Win16 images enter the VDM; resolved native images
use public `CreateProcess`; unresolved text uses `COMSPEC`; and `54:08`
restarts the current product without classifying its tail.

## Evidence

- [S7 final matrix](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t391-s7-app-entry-command-final-matrix-001.md)
  records the real Console `command/c ver`, ordinary and nested
  `COMMAND.COM /c ver`, and interactive `COMMAND.COM`/`exit` rows.
- [S8 resource evidence](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t391-s8-three-rom-executable-resource-001.md)
  records exactly `bios1.rom`, `bios4.rom`, and `v7vga.rom` as executable
  resources, with `bios2.rom`, `profile.spc`, and `cmos.ram` excluded.
- [S9 final audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t391-s9-final-audit-closure-001.md)
  states that all T391 exit criteria are satisfied and explicitly transfers
  x64 recovery to T392.

## Closure

T391 is closed. It does not claim x64 execution or a DOSX/WOW bootstrap;
those remain separately owned work.

