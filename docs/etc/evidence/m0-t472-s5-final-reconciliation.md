# M0 T472 S5 — Final response/lifecycle reconciliation

## Reconciliation result

S1 through S4 establish one owner for every selected response route.

| Route | Owner | Evidence |
| --- | --- | --- |
| DOS fatal Abort | Original `TerminateVDM` to `ExitVDM`; NTSRV authorizes, but does not replace, the exit. | [S2 DOS terminal proof](m0-t472-s2-dos-fatal-terminal-proof.md) |
| Retry, Ignore and Edit | Immediate original PIF, floppy, allocation or DPMI caller. | [S3 nonfatal ledger](m0-t472-s3-nonfatal-response-ledger.md) |
| WOW hard error | Original `W32HungAppNotifyThread` and its caller tail; public adapter only preserves the response slot. | [S4 reconciliation](m0-t472-s4-wow-hard-error-reconciliation.md) |

No T472 stage added a response broker, second terminator, private-server emulation, broker ABI, guest change, or production source modification.

## Identity and verification

`git diff --name-only 0d97d4ff..HEAD -- ':!docs' ':!tests'` is empty: the candidate changed only its ledgers and current-state records. The ten files in `build/t467-s3/package-r2/SYSTEM32` are SHA-256 equal to their installed counterparts in `O:\winnt\system32`. Documentation governance and whitespace integrity pass at closure.

No fresh full product run is asserted for S5 because there is no runtime source, build-selection or package change to validate. The retained T464 decoder fixture and Full gate, plus T471's current-package acceptance, remain the relevant executable evidence. S4's attempted additional fixture supplied no receipt and is deliberately not counted.

## Closure

T472 closes the finite error-dialog response/termination semantics candidate. The unavailable private hard-error UI stays an explicit boundary, not an unfinished local implementation. The next ordered candidate owns whole-MVDM source/build/runtime closure; it does not inherit any unclassified T472 response or lifecycle defect.
