# T228 S1 P24 — Bounded Post-IRET CLI Observation Result

## Result

P24 closes the diagnostic CLI lifecycle for P23's copied IRET record.  The
hash-locked Direct/Readonly pair reaches the retained one-million-tick budget
terminal in both modes, with byte-identical output.  Each run reports
`interrupt-returns count=0`.

This is a negative mechanical result: during the bounded execution no
real/V86 `IRET16` or `IRET32` reached the already-proven successful
post-`RSP_COMMIT` observation boundary.  It cannot prove that no interrupt was
entered or that no failed/redirected return was attempted; it does rule out a
bad state *after a successfully completed observed IRET* as the present cause.

## CLI boundary

The diagnostic build alone accepts `--observe-interrupt-returns`.  It configures
the copied mantle ring before the existing worker, clears it on setup/worker
failure, prints copied values after normal worker completion and clears it
again.  It neither pairs an entry with a return nor interprets vector, BOP,
DOS, OpenNT, address or guest-memory data.

A fresh default CLI incrementally rebuilt from the default P23 graph rejects
the option with normal usage exit `2`.  Thus the product CLI remains unarmed.

## Inputs and paired result

The run reused P22's exact four-image input manifest:
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.
The child environment was cleared and repopulated only with `SystemRoot`,
`ComSpec`, both retained startup-source variables and `OPAQUE_A` through
`OPAQUE_D` (1,000 characters each).  No image, provider, CPU or mutation
profile source changed.

| Mode | Exit | stdout SHA-256 | stderr SHA-256 |
| --- | ---: | --- | --- |
| Direct | 4 | `5b6a0b390b6481d79bbbb72d6cbde85a42bae6514c7dbe06804eaf71b50d46e9` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |
| Readonly | 4 | `5b6a0b390b6481d79bbbb72d6cbde85a42bae6514c7dbe06804eaf71b50d46e9` | `52de23b1a4fbd12fe3fd463888431e2b5a4b852502bad2dfff6a4b4b50025860` |

Diagnostic CLI SHA-256:
`893d3a78d474dc992b048411f02c94020fb38b095ea3b825372e1916c8786f93`.
Default CLI SHA-256:
`3edc3d8d9e3c0549644b41558dbb8d983f0e90321f01c559a827ef67cfcf8a4b`.

## Owner disposition

P24 selects no BOP, provider or CPU repair.  Combined with P22, it rejects
both a post-EndInit DEM reload repair and a completed-IRET-result repair.  The
unresolved work remains within the complete original guest COMMAND/NTDOS
transient/MCB and interrupt-delivery/control-state owner domain.  Any next
observation must be separately admitted at a selector-blind pre-completion
mechanical boundary; it may not become an INT-function or BOP leaf task.