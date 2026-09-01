# M0 T342 — Debugger / BDE / FAX owner-package recovery

## Purpose

Recover or explicitly retain the original MVDM debugger, BDE and FAX
owner packages as complete source-owned surfaces.  This task first separates
runtime owners from debugger extensions, samples and retired NT4 shells; it
does not enable a debugger, FAX VDD or host UI merely because their source is
mirrored.

## S plan

### S1 — Original source / ABI / failure admission

Inventory the original `dbg`, `bde`, `vdmdbg`, `vdmexts`, `fax` and directly
reached host debugger files.  Map public entrypoints, state, callbacks,
callers, existing build selection, debugger/sample/tool disposition and every
machine, BaseSrv, monitor, VDD, Win32 or kernel boundary.

### S2 — Selected original local cohort recovery

Only after S1 selects one complete, bounded original cohort, retain its
original bodies and bind existing same-shaped interfaces.  Preserve argument
layout, callback order, error/termination behavior and mapping boundaries.
No generic debugger broker, fake event, FAX service or VDD sample may be
created.

### S3 — Formal graph and local contract closure

Build the affected formal CPU40 x86/x64 graphs and run focused source-shaped
contract checks.  A runtime observation can only verify a completed package;
it cannot select a debugger/FAX leaf.

## Exit

Close only when every reached source form has one owner/disposition, any
selected local cohort has source/ABI/failure/formal-build evidence, and every
monitor/kernel/CSRSS or sample branch has a named transfer rather than an
implicit stub.
