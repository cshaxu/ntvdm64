# OpenNT BOP Entry Contract Research

## Question

What exact control-flow and operand-consumption contract must a future
adapter-owned BOP recognizer preserve before any DOS, WOW, DEM, or other
OpenNT service is connected?

## Evidence

The original OpenNT macro in `base/mvdm/inc/bop.h` defines a normal BOP as
three bytes: `C4 C4 <number>` (`BOP_SIZE == 3`). The pinned SoftPC CCPU
decoder at `base/mvdm/softpc.new/base/ccpu386/c_main.c` recognizes that pattern
in the `LES` (`0xC4`) path. It reads the BOP number, advances the Intel IP past
the entire instruction, and then invokes `bop(number)`.

That advance is part of the original contract; it is not a convenience detail.
For ordinary BOPs the owner starts at the byte immediately following the BOP.
The DOS owner (`MS_bop_0` in `host/src/nt_bop.c`) demonstrates the consequence:
it reads one command byte at current `CS:IP`, dispatches it through `DemDispatch`,
then increments IP by one. Other owners may define different trailing-byte or
register conventions.

`BOP_SWITCHTOREALMODE` and `BOP_UNSIMULATE` are special values in the original
header. The CCPU code gives `0xFE` a separate fast/unsimulate path. This record
does not claim that the first adapter profile may implement either value.

## Required Future Boundary

A future BOP recognition request must, at minimum, distinguish:

- the fault address where the candidate bytes begin;
- the post-instruction continuation address (`fault + 3` only after exact
  recognition and in the applicable decode mode);
- the fact that trailing service bytes remain unconsumed; and
- the original OpenNT owner selected by an evidence-backed number, not by a
  new adapter service table.

The adapter may carry copied scalar machine state only when a reached original
owner proves it needs that state. It must not hand `BX_CPU_C`, `BX_MEM_C`, guest
mapping pointers, or an OpenNT function pointer across the boundary. Guest
memory reads/writes remain separate, bounded operations under the memory-seam
rules; recognition itself must not silently parse service-private operands.

## Current Decision

Do not add BOP recognition, register snapshots, guest-memory copying, service
dispatch, or a `fault_rip + 3` resume rule yet. The verified generic `#UD`
bridge remains pass-through by default. `BX-UD-001` is merely the mechanical
trap point that can carry a future evidence-backed request.

The first admission must name one original owner and prove its exact input,
output, trailing-byte, IP, and error/termination behavior. Candidate names in
`bop.h` are inventory evidence, not authorization to implement a dispatcher.

## Implication For Bochs Intrusions

If exact byte recognition cannot be performed wholly by the Bochs-side
mechanical wrapper using a registered, scalar-only adapter request, a new
Bochs intrusion must be proposed in the exception register before code changes.
It must be generic (no BOP number or OpenNT name), default-off, and retain
native `#UD` behavior for every non-admitted case. This is an allowed but
exceptional route under the owner's 2026-08-10 authorization; it does not
authorize changes to decode semantics, CPU execution, memory ownership,
devices, or firmware.
