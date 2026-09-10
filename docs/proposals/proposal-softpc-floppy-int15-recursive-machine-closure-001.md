# M0 T319 — SoftPC FDC/INT15 Recursive Machine Closure

## Purpose

Recover the smallest original SoftPC machine cohort that currently prevents
the already-frozen NTDOS slice from continuing: `floppy.c::wait_int`, its
FDC/PIC interrupt completion, the original `int15`/`wait_int` guest-vector
table initialization, and recursive CPU40 `host_simulate()` return.

This is a machine prerequisite package, not a BOP, DEM, COMMAND, XMS or DOS
implementation package.  The authoritative queue identity is **Minimal
vertical-slice integration closure**.

## Source-first recovery order

1. Use the reached original SoftPC sources and retained firmware/table inputs.
2. Bind their historical interfaces through the existing MVDM-shaped machine
   interfaces only where direct composition cannot link.
3. Register a minimal divergence only if neither source nor a same-shaped
   binding can preserve the reached contract.
4. Do not invent a successful FDC wait, guest interrupt, or BOP result.

## S plan

### S1 — Reached recursive machine-contract freeze

Trace the original `floppy.c::wait_int` callers, the initialization and
validity conditions of `int15_seg:int15_off` and `wait_int_seg:wait_int_off`,
the FDC/PIC completion route, and CPU40 recursive-simulation return contract.
Classify each dependency as directly composable, same-shaped binding,
machine-source recovery, or explicit owner transfer.  No production behavior
changes occur in S1.

### S2 — Source-shaped reached-cohort composition

First compose the original generic `rom_init()` firmware-residency body
identified by S1: for the modern non-kernel product it replaces the retired
pre-resident-ROM assumption while retaining original resource lookup, SAS
copy and ROM-protection order.  The retained byte-exact firmware then becomes
resident before the original recursive FDC wait runs.  Only after that fixed
input is proven may this S inspect the next source-defined FDC/PIC/table/
recursive-run piece.  Preserve original call ordering and negative behavior;
do not expand unrelated disks, DMA, BIOS services, or devices.

### S3 — Fixed-container integration proof

Build the affected CPU40 product graph in x86 and x64.  Run the fixed,
non-debug x86 observation container and prove either a source-shaped return
past the former zero transfer or a narrower named original-machine owner
transfer.  The result is not an `EXEC` or child-completion claim.

## Exit

The former `floppy.c::wait_int -> host_simulate()` zero-transfer condition is
either eliminated through source-shaped original-machine composition, or its
remaining exact machine-owner prerequisite is proven and transferred.  Both
x86 and x64 formal graphs must compile/link.  No BOP route, guest DOS source,
app loader, or synthetic device success may be introduced.
