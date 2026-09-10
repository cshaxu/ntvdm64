# M0 T394 S8 — service observation and ABI extraction

Date: 2026-09-06

`error_abi.h` now has one definition owner under `opennt-abi`; the former
SoftPC path is only a transition include for existing force-include build
islands. COMMAND observation bodies and private report helpers moved to
`basesrv`; DEM observation bodies and private path helper moved to `redir`.
Each new unit directly compiled, and the remaining host aggregate compiled on
x86 and x64 after the extraction.

BOP ingress/return remains at its original host dispatcher boundary because it
spans DEM and COMMAND selectors; exception and main-return remain product
container probes. Neither is a generic monitor/logging concern. These are
explicit S8 retained boundaries, not unclassified service bodies; S9 audits
their single-body state and removes only forwarding paths.
