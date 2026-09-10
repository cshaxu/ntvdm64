# T225 S22 P3 INT 06 Machine-Boundary Correction

## Correction

The active S22 observation previously called the repeated `C4 C4 06` at
`074A:0740` an "unclassified" BOP.  That wording described the generic
adapter ingress result, but it was not an ownership conclusion.  Original
OpenNT source and the retained machine-boundary evidence identify it as a
SoftPC pseudo-ROM machine BOP, not an OpenNT host-service selector.

In particular, `src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm` defines
the original INT 06 vector as `bop 06h` followed by `jmp iret_com`.
`src/opennt/base/mvdm/softpc.new/base/bios/bios.c` maps BIOS table entry `06h`
to `illegal_op_int`; it separately maps `56h` to `MS_bop_6`.  The latter is
the debugger dispatcher in `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`.
Selector `06h` must therefore not be reclassified as debugger, DEM, COMMAND,
or any adapter host-service family.

## Original Contract and Owner

`src/opennt/base/mvdm/softpc.new/base/system/illegalp.c:illegal_op_int` reads
the CPU-built INT 06 fault frame at `SS:ESP`, reports under the historical
host policy, advances the saved IP by one byte (with the original `FFFFh`
carry rule), and invokes original `unexpected_int`.  The following guest
`iret_com` consumes that changed frame.  Bochs owns #UD delivery, real-mode
frame construction, and stack protection; the original SoftPC machine handler
owns the frame-side semantic action.

This confirms the existing retained owner record
[t177-s9-illegal-op-vector-owner-audit-001.md](t177-s9-illegal-op-vector-owner-audit-001.md)
and the vector-installation reconciliation
[t179-s2-int06-vector-installation-reconciliation-001.md](t179-s2-int06-vector-installation-reconciliation-001.md).
The Direct/Readonly profiles have no different disposition for this guest
machine path.

## Current Formal Composition

The formal Ninja manifest includes the selector-blind
`src/bx-vdm/bx_ntvdm_machine_bop_facade_v1.c`, but it does not include the
legacy `bx_ntvdm_machine_composition_v2.c`, `illegalp.c`, or `unexp_nt.c`
machine-handler island.  Consequently S22's source-built native executable
can observe the source-defined vector yet has no linked owner that consumes
selector `06h`; the repeated vector/stack path is consistent with that omitted
machine composition.  This is a bx-mantle/machine-composition closure gap,
not a failure of the BOP provider table.

## Recovery Ledger

| Recovery rung | Decision |
| --- | --- |
| Original source | Retain `illegal_op_int` and `unexpected_int` as the semantic reference; do not substitute a debugger or DOS handler. |
| Existing composition | The legacy isolated machine island is evidence only.  It cannot be silently restored because it carries historical callback/port/RAM/report ABI assumptions and is outside the formal Ninja closure. |
| Source-derived shim | Not admitted in S22.  A future bx-mantle package must first establish a typed opaque machine-mechanics ABI and prove the original handler's complete object/link closure. |
| New behavior or Bochs intrusion | Rejected.  No selector recognition belongs in Bochs; no adapter provider, device, firmware, guest-byte change, or altered #UD rule follows from this correction. |

## Effect on S22

S22 P1 still proves native NTIO -> NTDOS -> COMMAND reach, and P2 still
corrects the original `54:0F` BX contract.  They do not prove progress past
this machine boundary.  The next work is not a singleton BOP patch or a new
trace run: it is a separately admitted bx-mantle whole-machine-handler
source/ABI/link closure, after S22 records the corrected boundary.