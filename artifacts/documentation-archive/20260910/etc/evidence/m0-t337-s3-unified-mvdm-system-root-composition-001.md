# M0 T337 S3 — Unified MVDM system-root composition

## Question

Can the selected original DOS, COMMAND, WOW-facing system-media and SoftPC
resource paths share one executable-relative MVDM root without changing guest
drive semantics or modifying guest bytes?

## Change

- Replaced the split session `dos_media_root` / `win16_media_root` state with
  one immutable `mvdm_system_root`.
- App now derives `<exe>\\mvdm` and `<exe>\\mvdm\\softpc`; the latter is the
  original SoftPC external-resource root, not a ROM-resource container.
- Renamed the project-owned same-shaped lookup helpers to
  `mvdm_softpc_system_*`; selected original callers retain their algorithms
  and use only that binding at their existing registered divergences.
- Staging now produces `mvdm/`, `mvdm/system32/` and `mvdm/softpc/`. DOS and
  Win16 source mirrors remain distinct and no guest binary is changed.

## Verification

1. Node 22 staged the existing formal x86 product into
   `build/M0-T337/S3/package-contract-r3`. The manifest contains selected
   `NTIO.SYS`, `NTDOS.SYS`, `config.nt`, `autoexec.nt`, `COMMAND.COM`, the
   `mvdm/system32` cohort, and all selected SoftPC firmware/resource files
   under `mvdm/softpc`.
2. Fresh generated `CPU_40_STYLE` formal graphs compiled and linked
   `original-softpc-process.exe` for x86 and x64 under
   `build/M0-T337/S3/formal-{x86,x64}`. The first x86 invocation exposed only
   a generated-runner environment mistake (`cl.exe` absent); the supplied
   `run-ninja-parallel.cmd` launcher then built normally. No source or link
   error is attributed to the system-root change.
3. A source sweep found no remaining live production or current-test reference
   to either removed split-root API or to `softpc/roms`.

## Boundary

The root is session-local host media selection for imported MVDM code only.
It does not intercept guest `C:`, change ordinary DEM host-file conversion,
replace host `%SystemRoot%`, create a virtual disk, or claim guest execution
continuity. The selected package currently carries the DOS system cohort; the
same root is the required destination for selected WOW system media as that
runtime cohort is admitted.
