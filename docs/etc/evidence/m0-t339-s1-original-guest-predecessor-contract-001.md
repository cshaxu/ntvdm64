# M0 T339 S1 — Original guest-predecessor contract

## Result

The reached `50:3D -> demExitVDM` host terminal has two direct original guest
callsites, and one of them has more than one predecessor family. The fixed
short-root observation proves that original configuration expansion ran, but
it does not select a callsite or predecessor family.
No product or guest change is admitted by this packet.

The companion [ledger](../operations/m0-t339-s1-guest-predecessor-contract-ledger.tsv)
is the row-level source of truth.

## Configuration is a normal startup path

`cmdGetConfigSys()` in the selected original
[`cmdconf.c`](../../../src/mvdm-host/dos/command/cmdconf.c) calls
`ExpandConfigFiles(TRUE)` unconditionally.  The latter asks the original
`GetPIFConfigFiles()` for a source: a PIF override when present, otherwise
the selected MVDM root's `config.nt`.  It then creates a temporary file that
NTDOS consumes as `CONFIG.SYS`.

This is not a “missing config file falls back to `cmdconf.c`” relationship.
`cmdconf.c` is the ordinary preprocessor for `config.nt`; a missing selected
source remains on the existing fatal path.

The same original routine filters a source `SHELL=` directive and writes its
own normalized line.  For the packaged contract it is:

```
shell=<mvdm-system-root>\System32\command.com /p <mvdm-system-root>\system32
```

The 38-character T338 root is below the original 64-byte short-root buffer,
so the observed run has crossed this configuration cohort.  It therefore
does not justify an invented root-level `COMMAND.COM` or a different guest
drive policy.

## Direct callsites and original predecessors

1. `sysinit1.asm` uses the configured `commnd` value to open and EXEC the
   initial command interpreter.  Any failure of that open/EXEC sequence
   falls through `comerr` to `stall`, which emits `SVC_DEMEXITVDM`.
   Its relevant inputs are the guest-visible temporary configuration shell
   line, DOS file service behavior, and the staged COMMAND image.
2. `command/init.asm` executes after COMMAND begins.  It asks DOS for the
   version (`INT 21h`, `GET_VERSION`) and compares it with
   `EXPECTED_VERSION`; an own-parent mismatch emits the same service.  The
   DOS implementation returns the current PDB version word, so the relevant
   inputs are the NTDOS-created PDB and the matched NTDOS/COMMAND binary
   pair—not a host BOP result.
3. `sysinit1.asm::stall` is not only the file/EXEC terminal. Its original
   `mem_err` predecessors in `sysinit1.asm`, `sysinit2.asm`, and
   `sysconf.asm` print the guest memory error and then jump to `stall`.
   They include command allocation/fit checks and CONFIG.SYS device-load or
   break-address allocation paths. These are a third source family and need
   a guest-memory/allocation contract before an initial COMMAND recovery can
   be selected.

The original memory input is also mapped. `sysinit1.asm` invokes BIOS BOP
`12h` before it establishes `memory_size`; the original SoftPC BIOS
`memory_size()` reads the guest BDA `MEMORY_VAR`, which original `reset.c`
initializes from `host_get_memory_size()` (the selected host header supplies
640 KiB). That machine/firmware contract constrains the initial-command fit
test and allocation limits. It remains an owner-specific candidate, not a
reason to invent a DEM service or alter guest media.

The selected normal `config.nt` also contains `dos=high, umb` and
`device=%SystemRoot%\system32\himem.sys`; the staged `himem.sys` exists.
Consequently SysConf's original device-load and break-address allocation paths
are source-valid. The current trace does not state whether either was taken,
so neither configuration removal nor a memory repair is selected yet.

The selected source definitions declare DOS version 5.00 in
`dos/v86/inc/versiona.inc`.  The current stage's `NTDOS.SYS`
(`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`) and
`COMMAND.COM`
(`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`) exactly
match the retained source-built inventory.  That rejects a mixed-artifact
version-pair as the selected S2 recovery cohort.  It does not locate the
actual callsite in the running guest.

## Existing observation and its limit

The unchanged short-root observation reached `54:05` before `50:3D`.
`msinit.asm` shows that `54:05` is NTDOS `SVC_CMDSETINFO` registration.  It
proves passage through that registration point, but it is not an execution
location record and cannot distinguish the two caller sites above.

`demExitVDM` also cannot distinguish them: the original host terminal has
already discarded the guest caller-specific reason.  Its hard-coded error
presentation is therefore not admissible evidence for a missing `config.nt`,
root drive or loader.

## S2 admission boundary

S1's static map is now complete. S2 may select one smallest original cohort
only after obtaining a non-mutating, source-shaped discriminator among the
staged NTDOS/COMMAND identity, guest file/EXEC, PDB-version, and
memory/allocation contracts. It may recover an existing original binding
through a same-shaped adapter, but may not introduce a BOP leaf, modify guest
media, invent a drive mapping, or synthesize a successful EXEC result.
