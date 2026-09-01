# M0 T336 S4 — Post-vector terminal attribution

## Result

The fixed, console-owning x86 observation is not waiting for an FDC result,
an INT15 completion, or a missing guest loader.  Its primary CCPU thread is
inside the original terminal path:

```
c_cpu_simulate -> ccpu -> MS_bop_0 -> DemDispatch -> demExitVDM
    -> RcErrorDialogBox -> ErrorDialogBox
```

The externally captured BOP marker is `50:3D`.  The original
`softpc.new/host/src/nt_bop.c::MS_bop_0` decodes that byte while IP points at
the SVC operand and dispatches it without adapter routing.  Original
`dos/dem/demdisp.c` maps `0x3D` to `demExitVDM`; original
`dos/dem/demmisc.c::demExitVDM` enters `RcErrorDialogBox` and then
`TerminateVDM`.

`demExitVDM`'s hard-coded `"config.nt"` dialog argument is historical
presentation text, not a reliable report of the failing guest file: its
documented DS:SI message input is not consumed by that function.  It must not
be used to diagnose a missing configuration file.

## Source boundary

The reached guest sources have two direct `SVC_DEMEXITVDM` sites:

- `dos/v86/doskrnl/bios/sysinit1.asm::stall`, reached after `comerr` from the
  initial command-file load/open/lseek/exec sequence; the selected name is
  `\\COMMAND.COM`.
- `dos/v86/cmd/command/init.asm`, reached only after COMMAND itself has
  started and rejected the expected DOS version.

The current observation records the terminal service but not guest CS:IP, so
it does not distinguish those two guest sites.  It does establish the exact
host terminal owner and rejects the previous FDC/INT15/wait attribution.

The surrounding original host contract exposes the next product boundary:

- `demgset.c::demGetBootDrive` obtains the host boot drive from the NT setup
  registry and otherwise returns `C:`.
- `demfile.c::demOpen` preserves the original direct `CreateFileOem` host-file
  access for a guest canonical path.
- The portable product's immutable DOS media is an executable-relative
  `dos/` directory.  `InitialiseDosEmulation` and `GetPIFConfigFiles` already
  resolve startup images/configuration from that directory, but neither
  original function makes it a drive root for the later guest `\\COMMAND.COM`
  access.

Thus the selected media has been loaded and is present, while its later
guest-visible drive-root projection is not yet an admitted product contract.
This is a host namespace/boot-media composition question, not a loader, BOP,
CCPU, FDC, BIOS, or guest-image repair.

## Evidence and limits

- The fixed stage contains byte-identified `NTIO.SYS`, `NTDOS.SYS`,
  `COMMAND.COM`, `config.nt`, `autoexec.nt`, and `HIMEM.SYS`.
- The process reaches original `50:11`, `54:05`, and the terminal `50:3D`;
  therefore it is beyond original image loading and COMMAND bootstrap setup.
- The timeout snapshot records the primary error-dialog wait, the original
  heartbeat worker (`Win32_host_timer -> HeartBeatThread`), and the original
  dialog worker.  It introduces no debugger, guest-memory read, product
  instrumentation, media rewrite, or runtime retry.

No implementation is selected here.  A successor package must explicitly
choose and prove the portable guest drive-root policy while preserving the
original DEM file/provider ownership and the project's host-mutation rules.
