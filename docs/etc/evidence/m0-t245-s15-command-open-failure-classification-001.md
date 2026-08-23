# M0 T245 S15 — permanent COMMAND open-failure classification

## Question

Which original `sysinit1.asm:comerr` incoming path produces the retained
`50:3D` terminal after the source-built `commnd` observation?

## Existing diagnostic surface

No production source was changed.  The repository already contains the
registered, default-off `bx_ntvdm_software_interrupt_observation_v1` record.
The formal graph was generated with only
`-SoftwareInterruptDiagnostic`; it records bounded real-mode interrupt state
and does not expose BOP, DOS or COMMAND vocabulary to bx-core/bx-mantle.

The original source has two relevant paths:

1. `sysinit1.asm` opens `DS:DX=commnd` with `AX=3D00h`; carry transfers to
   `comerr`.
2. Only after that open/size setup succeeds does it issue `AX=4B00h` for the
   permanent command `EXEC`; carry also transfers to `comerr`.

## Verification

Formal MSVC x64 `/MT` graph:

```text
build/M0-T245-S15/formal-r1
```

The existing `t228-s1-software-interrupt-observation-fixture.exe` passed with
the diagnostic switch.  A single Direct source-built native run then retained
the final 64 interrupt records.  Its decisive record is:

```text
vector=21 mode=1 cs=8e08 eip=0000093b ax=3d00 bx=1208
cx=0002 dx=3466 ds=8e08 es=0bf2 ss=8e08 sp=0604
```

`DS:DX` is exactly the S14-observed `commnd` address, and `AX=3D00h` is the
original DOS open request.  No subsequent `AX=4B00h` appears before the
`comerr → stall → 50:3D` terminal.

## Conclusion

The current blocker is the guest permanent-COMMAND **open** path, not
`EXEC`, PSP/parent-return, COMMAND host lifecycle, or Bochs execution.
The observed token is relative (`BUILD\\OUTPUT\\DOS\\COMMAND.COM`) because
the CLI passed its relative `--dos-root` string unchanged into the startup
root descriptor.  A following bounded T245 correction may canonicalize that
CLI bundle-root descriptor before the existing original `cmdconf → sysconf`
route; it must not create a virtual drive or invent a file-service result.
