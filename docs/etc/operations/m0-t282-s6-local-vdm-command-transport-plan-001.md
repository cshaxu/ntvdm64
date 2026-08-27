# M0 T282 S6 — local VDM command transport

Recover only the one-session DOS portion of original
`GetNextVDMCommand(PVDMINFO)`: a copied producer record owned by `app`, a
bound-session request/response dispatch through `adapter-vdm-monitor`, and
originally visible buffer-size, re-entry and no-command outcomes. The adapter
must not retain native pointers or HANDLEs in `VDMINFO`; values are copied or
represented by the existing session-owned mapping path.

The historical CSR/BaseSrv transport, first-VDM query, cross-process broker,
WOW nonblocking branch and PIF provider stay excluded. Focused x86/x64 tests
must cover success, size negotiation, no command and unsupported mode.
