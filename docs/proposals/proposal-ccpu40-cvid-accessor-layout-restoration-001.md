# CCPU40 C-VID original accessor and layout restoration

## Origin and objective

Owner-directed successor to the transferred T412 S12 scope. Close the known
prefilled 80x5 C-VID crash through source-proven original accessor/layout
composition, preserving the S11 blank-gap repair and reducing unnecessary
mirror differences, overlays and autonomous providers. Do not presume that
CCPU instruction execution is faulty.

Baseline: [T412 closure](../history/m0-t412-broker-architecture-closure.md) and
[S11 evidence](../etc/evidence/m0-t412-s11-console-handoff.md). The three
published products remain APP_VERSION 0.0.412 / protocol 3 until a new verified
package is built. The old research patch and fault observations are retained,
not treated as an implemented fix or a mandatory design.

## Complete source-defined profile

Selected profile: Win32/x86, MSVC /MT, CCPU40 with C-VID external host accessors.
Original owners include softpc.new/base/cpu/src/evid/vglob.c, its identical
CCPU copy, generated C-VID tables, GDP headers and EvPtrs latch/selector logic.
Audit every selected consumer, not merely the faulting setter.

- All 38 getter/setter pairs; original latch access and three pointer selectors.
- Full and five-slot VideoVector layouts, GDP/VGAGlobals field offsets,
  signed and pointer call types, initialization and actual link-map ownership.
- Shared scratch-register/ring preservation and precise caller/thread/reentry
  ownership. The recorded corruption implicates accessor interference but does
  not yet prove its full scheduling cause.
- Mirror, overlay, adapter and build composition together. Prefer unchanged
  original translation units; then the smallest source-shaped binding. Record
  every retained exception and measure actual additions/removals separately.

CPU scalar, RMW/string, stack and fetch algorithms are not changed by this
dispatch/layout profile. A demonstrated need to alter them requires revised
scope, not an instruction-level fix selected from a trace.

## Sequential work

1. Audit and design: freeze source/product identities, trace all consumers and
   providers, establish the four-rung source-recovery ledger and complete ABI
   profile. Identify real removal candidates without promising a reduction
   before measurement. Review the saved candidate; replace it if inappropriate.
2. Restore and test the whole profile: compose original C accessors with the
   correct GDP/header selection; remove obsolete providers and unnecessary
   wrappers. Prove all fields/slots and scratch/ring preservation using real
   original bodies, including original latch/selector exceptions. Do not use
   an inert table fixture as proof of execution correctness.
3. Integration and delivery: fresh three-program x86 build; repeat prefilled
   80x5 stress with declared counts, COMMAND/EDIT/MEM, four ConPTY dimensions,
   live resize, mouse and the 17 command regressions. Preserve failure evidence.
   Publish verified products, record exact hashes and source footprint, commit,
   push and leave a clean worktree for owner confirmation.

## Gates and boundaries

Before the first new-task product build, advance shared APP_VERSION to the
new admitted T identifier; retain protocol 3 unless the wire contract changes.
Builds/tests stay below the new task's build root; only three formal EXEs go
to `O:\winnt`, observations only to `O:\winnt\logs`. Old T412 artifacts remain
comparison inputs, not relabelled new-task acceptance.

Completion requires the entire accessor profile and the previously failing
integration case to pass, with no regression of the S11 fix. A smaller passing
case, crash suppression or shifting the fault elsewhere is not completion.
Report measured mirror/overlay/autonomous deltas, including any necessary
increase, and unresolved external limits honestly.

No Broker redesign, generic CPU replacement, host mutation, full WRITE recovery
or x87 algorithm/layout restoration is included. The separate x87 candidate
remains next in its existing relative order. Actual Terminal/RDP visual
acceptance is distinct from automated ConPTY evidence.
