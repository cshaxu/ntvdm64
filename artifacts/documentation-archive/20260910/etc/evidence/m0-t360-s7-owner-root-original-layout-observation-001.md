# M0 T360 S7 — owner-root original-layout observation

## Question

Does the selected CPU40/x86 product reach the original COMMAND/Base VDM path
when the package uses the original SystemRoot shape directly below the
owner-designated short root?

## Fixed inputs

- Product: `build/M0-T359/S2/formal-x86/original-softpc-process.exe`
- Product SHA-256:
  `2a36708e39559472019527a16f14170a0b48b65d843f9d4d0a0906fee890b1e5`
- Runtime root: `O:\ntvdm64` (10 visible characters)
- Package manifest SHA-256:
  `d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`
- Fixed media manifest SHA-256:
  `43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`
- Immutable media: 16 selected entries at the root, `system32`, and `softpc`
  destinations.  The pre-existing `O:\ntvdm64\mvdm` subtree was retained and
  is not referenced by this manifest.

The complete original `shell=` value is 54 visible characters at this root,
including both source-defined occurrences of SystemRoot.  It is within the
63-character guest capacity.

## Procedure

One eight-second, console-owning observation used the frozen product and the
fixed root above.  The observer report is retained at
`O:\ntvdm64\observation-t360-s7.txt`.  No product, guest, firmware, or media
bytes were changed for this observation.

## Observation

The product did not reject the root layout.  It completed the observed
configuration/transient path, including BOP returns through `54:05`, `54:0C`,
`54:02`, `54:0D`, `54:0F`, `54:01`, `54:0E`, and `54:04`, then entered a second
`54:01` request.  The report records source-shaped successful DEM opens for
`O:\NTVDM64\SYSTEM32\HIMEM.SYS`, `COUNTRY.SYS`, `COMMAND.COM`, `REDIR.EXE`,
and `DOSX.EXE`.

The timed wait maps to the original owner chain:

```text
host_start_cpu -> c_cpu_simulate -> ccpu -> MS_bop_4 -> CmdDispatch
-> cmdGetNextCmd -> GetNextVDMCommand -> base_vdm_local_wait_for_command
```

The configuration trace records `MVDM-CONFIG-DONE al=00 cs=8E08 pass=02` with
the copied `O:\NTVDM64\SYSTEM32\COMMAND.COM` command.  The SAS store witness
also records the expected copied zero byte at guest linear address `0x914e6`.

## Interpretation

This is valid evidence for the original SystemRoot package layout and for the
T359 transient/configuration recovery boundary.  It is not evidence of DOS
EXEC, child-process execution, or interactive COMMAND completion.  The next
owner is the original Base VDM command-record delivery lifecycle, which is
admitted as M0 T358 S4.
