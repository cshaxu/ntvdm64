# T225 S36 — Startup-Machine Post-Return Component Audit

## Scope and evidence boundary

This is a read-only audit of the complete S25 startup-machine package after
the same-invocation S35 observation.  It adds no BOP route, provider,
guest/memory mutation, Bochs change, or trace run.

S35 proves only this order in identical Direct and Readonly runs:

1. `074A:0740` executes accepted selector `06h`;
2. the first subsequent recorded non-UD fault is `#GP(13)` at `0C41:2357`;
3. the mechanical access is a DS=`0C41`, offset/limit=`FFFF` word read.

It does not record the selector-06 frame contents, the guest `iret_com`
completion, or an intervening instruction-by-instruction state history.  It
therefore proves ordering, not causality.

## Original and current selector-06 contract

The original guest INT 06 vector is exactly `bop 06h; jmp iret_com` in
`src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm:1053-1055`; its IVT
installation is at lines 2168-2170.  The original host path is
`softpc.new/base/system/illegalp.c:79-134`, followed by
`softpc.new/base/system/unexp_nt.c:50-99`:

- `illegal_op_int` reads only the already-pushed real-mode return IP and CS
  from SS:ESP.  On IP wrap it writes the incremented saved CS, then always
  writes the incremented saved IP.
- `unexpected_int` performs PIC port sequencing and writes the BDA interrupt
  cause.  It does not write DS, a general register, guest code, or the live
  CS register.

The current source-derived provider preserves that fixed-width transcript in
`src/opennt/local/compat/host/opennt_int06_provider_v1.c:27-109`.  The
selected bx-vdm route in
`src/bx-vdm/bx_ntvdm_startup_machine_interrupt_v1.c:75-100` accepts only
real-mode `C4 C4 06`, preflights four saved-frame bytes plus BDA state,
executes typed RAM/PIC actions, and resumes at `fault_rip + 3`.  Control then
returns to the original guest `jmp iret_com`; bx-vdm does not execute IRET and
does not identify a DOS, COMMAND, or SoftPC semantic name to bx-core/mantle.

Thus the accepted provider has a narrowly possible influence only through the
saved return IP/CS and the documented PIC/BDA side effects.  It cannot
directly assign the observed DS=`0C41` tuple, alter general registers, or
write the instruction bytes at `0C41:2357`.  S35 does not capture enough
post-return state to prove or disprove an indirect control-flow relationship.

## Whole startup-machine package ledger

The S25 package contains all eight members below.  The sibling review matters:
no other member shares selector-06's four-byte hardware-frame prelude.

| Identity | Current source-defined effect relevant to continuation | Relation to S35 fault |
| --- | --- | --- |
| `02` | `unexpected_int` PIC/BDA common transcript; no saved-frame prelude. | Not observed as the final S35 record; cannot be silently equated with `06`. |
| `06` | Saved IP/CS update, then the `02` common transcript, then guest IRET. | Observed accepted immediately before the fault; temporal relation only. |
| `12` | Typed conventional-memory profile result. | No shared frame or DS mutation. |
| `15:AH=88` | Typed extended-memory result; other forms decline. | No shared frame or DS mutation. |
| `17` | Original absent-printer no-device continuation. | No shared frame or DS mutation. |
| `5F` | Bounded KIO/IRET table-state transaction. | No common path evidenced. |
| `66` | `BX=0` no-EMS continuation. | No shared frame or DS mutation. |
| `C8` | Checked headless INT 33 vector transaction. | No shared frame or DS mutation. |

The original/current routes and their source-recovery dispositions remain the
complete S25 map; this audit changes none of them.  Their whole-family and
native validations remain recorded in the S25 result, not re-run here.

## Corrected fault attribution

S35 additionally records COMMAND selector calls from the same CS segment:
`0C41:1B0E` (`54:02`), `0C41:1BFE` (`54:0D`), and `0C41:03A6` / `0C41:03CB`
(`54:0F`).  Consequently `0C41` is an active guest segment issuing COMMAND
plane calls in this execution.  It is **not** demonstrated to be the formal
NTDOS load segment.  The prior S29 algebraic mapping from an assumed NTDOS
load segment remains an explicitly conditional calculation only; it cannot
identify this live fault.

The controlled source-built COMMAND.COM has provenance but no exact `.map`,
`.lst`, or `.sym` binding offsets to source labels, as recorded by
`t145-s1-command-image-source-map-availability-001.md`.  Therefore neither
the exact function at `0C41:2357` nor a source-level command routine is
claimed.  The owner classification is narrowed from a generic/NTDOS candidate
to the **COMMAND guest-image/lifecycle plane candidate**, while exact routine
identity remains unproven.

## Result and next owner

No startup-machine repair is justified.  The selector-06 provider retains the
original frame/PIC/BDA contract and all package siblings have a distinct,
reviewed continuation contract.  The next owner is one whole COMMAND guest
image/lifecycle provenance package: source-built COMMAND load/segment
identity, bootstrap and initialization transitions, `54h` caller context,
and bounded fault-location evidence.  It must not become a singleton `54h`
implementation or a trace-led repair.
