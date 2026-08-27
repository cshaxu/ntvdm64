# M0 T290 S1 — Redirector source, ABI and dependency audit

S1 freezes the current source truth before any `57:xx` provider is enabled.
The current `mvdm-host/vdmredir` mirror contains all twenty-five selected
paths. Twenty-four are byte-identical to the selected canonical OpenNT source;
`vrmslot.c` has only the registered `MVDM-HOST-DIV-021` identity-boundary
change. This supersedes older evidence that treated the historical provider
bodies as absent.

The original `VrDispatchTable` directly defines exactly fifty selectors,
`57:00` through `57:31`.  The [service ledger](../operations/m0-t290-s1-redirector-service-abi-ledger.tsv)
maps every one to its real original C entrypoint and recovery owner group.
The separate [path/interface ledger](../operations/m0-t290-s1-redirector-path-interface-ledger.tsv)
classifies the full fifteen-body, six-declaration and four-build-form package.

The auditable recovery order is now fixed:

1. lifecycle, local broker and synchronous named-pipe group;
2. mailslot and asynchronous completion group;
3. NetAPI, NetBIOS, DLC/window and mode group;
4. whole-family reconciliation and only then BOP ingress/integration.

`adapter-mvdm-host-out/redir` is the single external-API boundary.  It may
provide same-shaped historical calls through public Win32 and session records,
but it cannot become a second Redirector provider.  `session` remains the
owner of native handles, completion records and the guest-memory mapping
manager; `adapter-bochs` may provide only selector-blind mechanical completion
delivery.  The existing `cmdredir.c` and DEM pipe callers are recorded as
consumers, not authority for a duplicate COMMAND broker.

S1 enables no selector and makes no runtime claim.  Its passing audit checks
both canonical hashes, all dispatcher entries and every ledger source path.
