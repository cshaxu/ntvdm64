# M0 T333 S5 — Redirector network and assignment disposition

## Result

The complete Network/NetBIOS/DLC/assignment cohort has a source-first
disposition.  T333 does not invent a replacement BaseSrv, RAP transport,
NetBIOS completion engine, or DLC driver.  The row-level result is in
[`m0-t333-s5-redirector-network-assignment-disposition-ledger.tsv`](../operations/m0-t333-s5-redirector-network-assignment-disposition-ledger.tsv).

## Direct, source-shaped local cohort

- `57:1B VrGetCDNames` remains the original public NetAPI algorithm.  Its only
  unportable element was the packed `I_CDNames` guest pointer trio.  The new
  Redirector adapter decodes the original 12-byte 16:16 layout and writes each
  existing value via a bounded session lease (`MVDM-HOST-DIV-176`,
  `ADAPTER-REDIR-005`).
- `57:1C VrGetComputerName` and `57:1D VrGetUserName` retain their earlier
  direct public Win32/NetAPI paths and bounded copied-output adapters.
- `57:19`, `57:1E`, `57:1F`, and `57:22` already contain original explicit
  `ERROR_NOT_SUPPORTED` bodies.  They remain exact source behavior.

## Non-local boundaries

- `57:10..18`, `57:1A`, and `57:29..2E` depend on the original Xs/BaseSrv or
  RAP transaction/provider family.  They transfer as a coherent unit to the
  queued NetAPI/RAP Redirector network-boundary package.
- `57:25` and `57:31` require NetBIOS NCB completion plus monitor state;
  `57:27` requires the retired DLC driver contract.  The original dynamic
  failure branch remains the only admitted behavior until that same owner
  package can prove a finite public provider.
- `57:28` is a VDD/window/DLC event entry and transfers to the queued VDD
  package.  It is not a reason to add a Redirector-local VDD shim.

## Validation

The Redirector static libraries rebuild on x86 and x64.  The focused fixture
builds and exits successfully on both architectures, including the `I_CDNames`
composite copied-span test.  This verifies the boundary helper, not a live
network, NetBIOS or DLC claim.
