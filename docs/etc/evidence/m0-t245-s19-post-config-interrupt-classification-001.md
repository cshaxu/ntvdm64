# M0 T245 S19 — post-CONFIG interrupt classification

## Question

After S16 canonicalized the admitted bundle root and S18 proved successful
opening of COMMAND's temporary SCS configuration file, which original
`sysinit1.asm:comerr` incoming path retains the `50:3D demExitVDM` terminal?

## Existing diagnostic boundary

S19 changes no production source.  It reuses the already registered,
default-off `bx_ntvdm_software_interrupt_observation_v1` ring in a disposable
formal Ninja graph built with `-SoftwareInterruptDiagnostic`.  The observer
records only mechanical real-mode interrupt state; it has no BOP, DOS or
COMMAND selector knowledge.

## Verification

The focused `t228-s1-software-interrupt-observation-fixture.exe` passes from
`build/M0-T245-S19/formal-r2`.  One bounded, source-built Direct run with a
one-million-instruction budget produces a full 64-record circular history and
ends at the existing controlled guest terminal.

The decisive records, in original `sysinit1.asm` order, are:

```text
INT 21h AX=4900h       ; release the probe allocation
INT 21h AX=4A36h      ; resize probe allocation
INT 21h AX=480Dh/4808h ; largest/free command-memory calculations
INT 21h AX=49F2h      ; release the second probe allocation
INT 21h AX=3D00h DS:DX=8E08:3466 ; open commnd
```

There is no later `INT 21h AX=4202h` (seek), `3E00h` (close), or `4B00h`
(EXEC) in the retained post-open history.  The run then follows the original
`comerr → badfil → stall → 50:3D` terminal.  `8E08:3466` is the source-built
`sysinit2.asm:commnd` storage previously observed by S14/S16.

## Conclusion

S16 fixed the former *relative* root descriptor, but it did not make the
guest `commnd` open succeed.  The remaining owner is the guest DOS-name to
Direct-host namespace boundary used by this permanent-command open; it is not
`cmdExec32`, `54:08/0A/0B`, PSP return, or a Bochs defect.  No repair is made
by this observation packet: a later source-first admission must map the
original permanent-command pathname through the existing Direct DOS file
contract without inventing a virtual boot volume or a successful file result.
