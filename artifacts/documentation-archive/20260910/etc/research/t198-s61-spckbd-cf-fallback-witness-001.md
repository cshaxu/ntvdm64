# M0 T198 S61: NTIO `BOP 5F` CF-Fallback Composition Witness

## Result

The active top-level bx-vdm composition now calls the existing exact
`spckbd` provider before generic BOP ingress.  This is necessary because the
generic registry correctly treats top-level `5F` as unavailable: the admitted
provider is narrower than a general `5F` service.

The provider accepts only the source-observed real-mode `AX=BEEF` handoff,
advances RIP by the three BOP bytes, and sets CF.  It does not interpret the
handoff table or add keyboard/PIC/8042/IRQ/host-input behavior.

## Clean source-built witness

The complete source-built MSVC x64 `/MT` run used the new build root
`artifacts/build/t198-s61-spckbd-cf-fallback-r1`.  Its legacy fixture returns
status `2` by design; its run log demonstrates that execution advances past
the `5F` BOP into OpenNT's `jc isk_int9` x86 fallback:

```text
next=66:83 terminal=1:8dc8:79c7
```

`66` is the next three-byte BOP selector.  `83` is the following guest opcode
and is not a BOP service number.

## Next frontier

OpenNT `spcemm.asm` identifies selector `66` as `InitSpcEmm`'s LIM/EMS
initialization probe.  The next task must audit its exact input and the
existing unavailable provider before deciding whether it is a contained
failure continuation.

