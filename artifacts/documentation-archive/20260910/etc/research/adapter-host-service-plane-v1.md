# Adapter Host-Service Plane v1

## First enabled contract

The first enabled plane contract is source-derived `DEM:11` (the historical
NTDOS load request). It runs only after the shared catalogue has identified
`C4 C4 50 11` and produces a common transaction, not a direct memory write.

Inputs are the captured CPU state, the validated session resource identities,
and same-island staged payloads. The NTDOS payload must have the exact byte
count already admitted for the NTDOS resource. The destination is computed as
the historical real-mode `DI:0000` linear address, `low16(EDI) << 4`. The
admitted v1 contract therefore requires a copied real-mode CPU state;
protected and v8086 snapshots remain unhandled rather than reinterpreted.

The prepared transaction uses the shared physical-range preflight. A resource
length mismatch, invalid session/state, or aperture overrun rejects the whole
request with no mutation. Every other currently catalogued service returns
`UNHANDLED`; it is not quietly emulated or treated as success.

## Deliberate limits

This plane implementation does not call original `demLoadDos`, use a host
path/HANDLE, open a file, write Bochs RAM, advance IP, alter registers, or
stop a session. Those actions require the pending common commit/result seam.
It is a profile service handler built on the common catalogue and transaction
contracts, not a BOP special case.

## Verification

The UCRT C11 fixture verifies the source-shaped `DI=08b0` destination
`0008:b000`, the exact NTDOS payload length, generic unhandled behavior,
resource-length rejection, aperture rejection, protected-mode pass-through,
and null-result rejection.
