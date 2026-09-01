# M0 T341 — Original VDD owner-package recovery

## Purpose

Recover or explicitly retain the original MVDM VDD host-service family as one
source-owned package.  This is not permission to load a sample VDD, recreate
the kernel VDM product shell, or turn every historical VDD API into a new
adapter service.

## Source boundary

The owner begins with original `softpc.new/host/src/nt_vdd.c` and its VDD
declarations.  It includes only callers and same-package machine/monitor
interfaces reached from that code.  `vdd/samples/*` are reference products,
not host-runtime inputs; `ieuvddex` is a debugger extension package, not a
runtime VDD provider.  Any NT kernel, CSRSS, VDM monitor or device executor
dependency remains an explicit boundary to the appropriate adapter or owner
package.

## S plan

### S1 — Original source / ABI / failure admission

Inventory every original VDD public entry, static state, callback table,
caller, machine/monitor/public-host dependency and failure result.  Classify
each source form as directly selected original body, same-shaped existing
adapter, candidate original external package, exact-unavailable branch,
debugger/sample/tool-only, or hard non-user-mode boundary.

### S2 — Selected original local cohort recovery

Only after S1 chooses one complete bounded original cohort, retain its bodies
and bind existing same-shaped interfaces.  Preserve original argument layout,
registration order, teardown and failure semantics.  Do not invent a generic
VDD dispatcher or provide a fake device success result.

### S3 — Formal graph and local contract closure

Build the selected formal CPU40 graph, run focused owner-contract checks, and
record a single source-backed package closure or transfer.  A guest runtime
observation is optional evidence only and cannot choose a VDD leaf repair.

## Exit

Close only when every reached VDD source form has one original owner and
disposition, and any selected local cohort has source, ABI, failure and formal
build evidence.  A VDD sample build or a VDD-related link symbol alone is not
package closure.
