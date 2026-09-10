# M0 T400 S10 — WRITE selector-stability disposition

> Superseded diagnostic design: the fixed-selector premise below was tested
> and rejected. The temporary source/build additions named there have been
> removed. The retained conclusion is at the end of this record.

## Original diagnostic-only design

S8 proves that selector `0x172c` uniquely identifies immutable WRITE NE
logical segment 5, whose map assigns `FInitMemory` to offset zero.  The
normal product graph does not define `MVDM_WRITE_SEGMENT_DIAGNOSTIC`.

Only the separately generated x86 graph
`build/M0-T400/S10/write-segment-diagnostic-x86` defines it.  In that graph,
the original CPU40 post-decode loop records at most 128 scalar
`CS=0x172c` IP/opcode entries.  The app captures/removes the report path
before COMMAND environment construction and `cmddisp` restores it only after
the original environment-copy service returns.  No guest media or decoded
instruction/result changes.

## Object evidence

The diagnostic graph was generated with `-Architecture x86` and
`-WriteSegmentDiagnostic`.  These changed-object targets compiled:

| Target | Result |
| --- | --- |
| `obj/ccpu/c_main.obj` | produced, 231256 bytes |
| `obj/adapter-softpc/mvdm_softpc_machine_observation.obj` | produced, 49564 bytes |
| `obj/app/report_configuration.obj` | produced, 4743 bytes |
| `obj/command/cmddisp.obj` | produced, 3425 bytes |

The full isolated graph currently encounters a Ninja wrapper/lock scheduling
issue after sequential object work; this is not treated as a source or runtime
result.

## Final disposition

The isolated x86 stage retained the selected immutable `WRITE.EXE` hash
`08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B`.
The read-only NE/LDT observer uniquely matched logical segment 5 as `0x172c`
only in a later modal state; an earlier isolated run observed a different
candidate selector. Reusing either selector in a separate run produced no
valid WRITE instruction identity. LDT selector allocation is therefore not a
stable cross-run early-execution identity.

The temporary `MVDM_WRITE_SEGMENT_DIAGNOSTIC` compilation switch, CCPU
reporter, app report-path capture and generator parameter were all removed.
No normal product image, normal stage, or guest media changed. This result
does not say that `FInitMemory` was skipped and cannot identify a branch.
T397's later correction withdraws the selector/IP-derived final-`HAllocate`
and local-heap conclusions. The retained result is narrower: no stable
cross-run selector identity exists for this diagnostic design, so it cannot
select a WRITE branch, recovery owner, or DPMI/XMS, CPU, x64, guest, USER/GDI
or synthetic-memory repair.
