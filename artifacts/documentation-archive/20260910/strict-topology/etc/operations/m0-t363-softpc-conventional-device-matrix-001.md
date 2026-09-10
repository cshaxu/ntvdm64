# M0 T363 — SoftPC conventional-device and DOS workload matrix

## Purpose

Recover the complete original CPU40 SoftPC conventional-device path exposed by
the frozen T362 `/C EXIT` observation before returning to COMMAND runtime
work.  This is a source-owner package, not a printer leaf patch: the first
reached `printer_io` frame must be evaluated together with its BIOS entry,
I/O-vector/port tables, BDA data, host-LPT binding, timer/ICA dependencies and
the selected app/session policy.

## S plan

1. **S1 — conventional-device owner ledger.** Map the original
   `c_IOVirtualised -> inb -> printer_io` stack and every immediate source
   edge, including ordinary BIOS INT 17, NTVDM selector 17, BDA port/timeout,
   IOS dispatch, host-LPT, timer/ICA and the staged configuration inputs.
   Classify exact source/package/adapter ownership and original failure
   contracts.  No production source change or second observation.
2. **S2 — bounded original device binding recovery.** Recover the entire
   immediate source-shaped missing binding cohort selected by S1.  Retain
   original device algorithms and use a same-shaped adapter only for a
   demonstrated unavailable modern boundary.  Local positive/negative tests
   must cover the source-defined device status and timeout direction.
3. **S3 — device cohort formal closure.** Formally link the selected CPU40/x86
   product and verify that all selected device units and the corresponding
   adapter contract are in one graph.  This is not guest execution proof.
4. **S4 — one frozen workload observation.** Run the unchanged T362
   console-owning `/C EXIT` container exactly once.  It may prove the expected
   device result/continuation or transfer one earlier complete original owner;
   it may not select a port-level patch.

## Boundaries

No COMMAND BOP result fabrication, guest/media mutation, virtual printer,
DOS EXEC, cmdExec32, BaseSrv/CSRSS, CPU30, Bochs, x64 guest-runtime claim,
WOW, Redirector, graphics feature or second T362 observation enters this
package.  Public Win32 printer APIs are not a substitute for the original
SoftPC LPT owner unless S1 proves that exact binding and its source failure
semantics.

## Completion

T363 closes only with complete source/binding ownership plus one frozen
device-workload result, or with one exact earlier source owner transferred
under the dependency-first rule.  It cannot claim a printed byte, DOS child,
COMMAND built-in, or interactive device support merely because a host-only
fixture or formal link passes.
