# M0 T318 S2 P46 — Original configuration-media binding closure

## Defect

The P45 source audit exposed a real modern-host prerequisite rather than an
unimplemented BOP.  When no PIF supplies an override, original
`softpc.new/host/src/nt_pif.c::GetPIFConfigFiles` constructs
`GetSystemDirectory()\config.nt` or `autoexec.nt`.  Modern Windows normally
does not contain those NT4 files.  Original `cmdconf.c::cmdGetConfigSys` then
uses the bad result, displays its original error, and terminates the VDM.

The previously fixed runtime stage also omitted the two already-mirrored
original files.  This made the stage incomplete for the selected original
COMMAND configuration contract.

## Source-shaped repair

`nt_pif.c` retains its original PIF override, OEM expansion, ownership and
caller-provided-buffer branches.  Only its no-PIF default is now bound through
the existing `adapter-mvdm-host-out/softpc::mvdm_softpc_dos_find_file` entry,
which resolves the immutable session DOS root chosen by app.  The mirror
change is registered as `MVDM-HOST-DIV-157`.

The adapter returns an empty path when the selected media file is absent.
That preserves the original routine's documented possibility of returning a
bad name and leaves the original `cmdconf.c` error/`TerminateVDM` path in
control.  It does not add a configuration parser, an app loader, or a BOP
provider.

`tools/build/Stage-OriginalSoftpcRuntime.mjs` now stages byte-identical
original `src/mvdm-guest/bin86/{config.nt,autoexec.nt}` as
`dos/{config.nt,autoexec.nt}` alongside the existing NTIO, NTDOS and COMMAND
media.  This is package placement, not generated configuration content.

## Verification

The formal x86 one-time-MSVC Ninja launcher rebuilt exactly the affected
original `nt_pif.c`, rebuilt `original-softpc-host-roots.lib`, and relinked
`original-softpc-process.exe` successfully.  Existing historical source
warnings remain visible; none was suppressed or reclassified by this change.

The new immutable stage is
`build/M0-T318/S2/runtime-r32-config-media`.  Its manifest contains:

```
dos/NTIO.SYS
dos/NTDOS.SYS
dos/COMMAND.COM
dos/config.nt
dos/autoexec.nt
softpc/roms/*
```

One non-debug, console-owning eight-second observation ran the newly linked
product with the unchanged `-f -o --ordinary-child` profile.  Its manifest
SHA-256 is `f89ca7a098804fe9820346060685bc44900b859bdfe176a6830f93451d9e4721`;
the result was `timeout` with observer-owned `0x53504354`, no stdout and no
stderr.  This replaces the incomplete-media fixed stage as the formal runtime
baseline.  Future observations keep this media, firmware, arguments, console
ownership and timeout fixed, and may replace only a newly linked product EXE.

## Boundary

The timeout proves only that this configuration-media prerequisite no longer
causes the prior immediate process exit.  It is not proof of NTIO reachability,
NTDOS load, guest `EXEC`, or child return.  Those remain subject to the next
source-owned continuity cohort and the existing layered-observation rule.
