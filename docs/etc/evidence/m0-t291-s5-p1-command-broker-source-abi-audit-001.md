# M0 T291 S5 P1 — Command broker source/ABI audit

## Question

Which part of the missing NT4 BaseSrv command product can be recovered now
without treating all `GetNextVDMCommand` callers as one generic route?

## Inputs and procedure

The audit compared the original public `vdmapi.h` state bits and BaseSrv-to-
WOWEXEC notification comment with every selected COMMAND, WOW, config and V86
consumer. The resulting request/result and owner disposition is in the
[ledger](../operations/m0-t291-s5-p1-command-broker-source-abi-ledger-001.md).

## Conclusion

The selected mirror has client evidence but no BaseSrv implementation to copy.
The smallest permitted authored seam is therefore a `broker`-owned copied
record queue/wake capability. It is constrained by the original callers:
bounded `VDMINFO` payload data, capacity-retry reporting, no-command result,
re-entry count and producer/consumer ordering. It does not receive a caller
pointer or replace the monitor command entry.

The one-session DOS forms CB-02 through CB-04 are ready for a focused P2.
WOW, PIF, first-VDM and `ExitVDM` forms stay explicit unavailable/deferred;
they need their source package owners rather than an invented fallback.
