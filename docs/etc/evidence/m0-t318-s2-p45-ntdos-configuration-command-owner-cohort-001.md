# M0 T318 S2 P45 — NTDOS configuration-to-COMMAND owner cohort

## Purpose

Identify the first original COMMAND-host crossing selected by the NTDOS
post-load configuration sequence.  This avoids treating a later fixed-run
result as evidence that an arbitrary BOP needs a new provider.

## Original guest route

After the original `NTIO.SYS` `init` transfers to `sysinit`, the selected
NTDOS `doskrnl/bios/sysinit1.asm::ProcessConfig` prepares a guest `DS:DX`
file-name buffer and emits:

```asm
CMDSVC SVC_GETCONFIGSYS
```

`inc/CMDSVC.INC` defines `CMDSVC` as `BOP BOP_CMD` followed by one service
byte, and defines `SVC_GETCONFIGSYS` as `12`.  Thus this is the original
`54h:0Ch` COMMAND service; it is neither a DEM `50h` service nor an
app-created loader path.

## Original host owner and contract

`softpc.new/host/src/nt_bop.c::MS_bop_4` reads the service byte at guest
`CS:IP`, calls `CmdDispatch(Command)`, then advances the original guest IP by
one byte.  `dos/command/cmddisp.c` preserves the indexed original table and
places `cmdGetConfigSys` at service 12.

The selected provider is the original `dos/command/cmdconf.c::cmdGetConfigSys`:
it expands the configuration file, converts the generated temporary name to
OEM, and copies that name to the guest-provided `DS:DX` buffer.  Its source
failure path is also explicit: the original error dialog path calls
`TerminateVDM`; it is not a successful empty configuration response.

This puts the first configuration crossing in an already-defined owner
package:

```
NTDOS sysinit ProcessConfig
  -> BOP 54h / service 0Ch
  -> MS_bop_4
  -> CmdDispatch
  -> cmdGetConfigSys / ExpandConfigFiles
```

The same early cohort can subsequently invoke the original console-init
service (`54h:09h`) while parsing configuration directives.  It must be
diagnosed as part of the COMMAND/console owner package, not by changing the
observer, media selection, or BOP ingress.

## Selected build and next boundary

Both selected formal CPU40 graphs include the original `cmddisp.c`,
`cmdconf.c`, `cmdmisc.c`, and `cmdexec.c` translation units.  Consequently the
configuration service has a normal original code/link owner in both x86 and
x64 rows; this record makes no claim that a run reached it or that temporary
configuration processing succeeded on the modern host.

The remaining startup question is the source-shaped behavior of this original
COMMAND configuration/console cohort under the fixed, console-owning runtime
container.  A new observation is admissible only after a real source change
to that cohort produces a new product EXE.  Until then the existing fixed
container is retained unchanged.
