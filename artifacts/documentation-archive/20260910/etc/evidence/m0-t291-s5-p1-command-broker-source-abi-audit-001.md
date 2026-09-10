# M0 T291 S5 P1 — Command broker source/ABI audit

## Question

Which part of the missing NT4 BaseSrv command product can be recovered now
without treating all `GetNextVDMCommand` callers as one generic route?

## Inputs and procedure

The audit compared the original public `vdmapi.h` state bits and BaseSrv-to-
WOWEXEC notification comment with every selected COMMAND, WOW, config and V86
consumer. The resulting request/result and owner disposition is in the
[ledger](../operations/m0-t291-s5-p1-command-broker-source-abi-ledger-001.md).

## Superseded conclusion and successor

This audit correctly identifies the caller contracts, but its source-supply
conclusion was too narrow: it searched the selected MVDM mirror, not the
complete approved OpenNT source. The latter contains the original BaseSrv
server and client implementation under `base/win32`. The proposed authored
`broker` queue was not retained.

The successor [P2 source-package audit](m0-t291-s5-p2-basesrv-source-package-audit-001.md)
selects the original package and records its CSR/private-NT dependencies.
Only after that import may a same-shaped adapter or bounded broker record
replace an individual unavailable platform operation. CB-02 through CB-07
remain source-owned; their runtime admission is not implied by this correction.
