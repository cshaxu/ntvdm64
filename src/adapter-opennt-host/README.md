# adapter-opennt-host

This component is the package-private historical host-interface adapter for
`opennt-host`. It has one named subfamily for each accepted non-MVDM OpenNT
owner package that needs a private historical binding. BaseSrv/client VDM is
the first such package, not the component's permanent scope.

The current BaseSrv/client subfamily may preserve only the exact calling shapes
reached by its owning `opennt-host` package:

- client CSR capture, message dispatch and wait/retry transport;
- server CSR request/reply, process/thread lookup and bounded registration;
- event-pair, duplicated-handle and cleanup operations required by the
  original BaseSrv record lifecycle; and
- the original WOWEXEC registration notification shape when its WOW owner is
  explicitly admitted.

It must not receive BOP, DOS, WOW guest-frame, SoftPC/Bochs or generic MVDM
semantics. `mvdm-host` does not include or call this component directly.
Every exported binding must retain an OpenNT source spelling/ABI/failure row
in the shared host tracker and identify its `opennt-host` caller.

T412 S2 admits the finite BaseSrv/BaseClient subfamily described in the
[consolidated design](../../docs/etc/evidence/m0-t412-s1-design-closure.md).
`basesrv/include/base_client.h` now retains the four reached declarations from
OpenNT `public/sdk/inc/ntcsrdll.h`, without importing its CSR runtime:

| Binding | Original caller and contract | Current implementation status |
| --- | --- | --- |
| CsrAllocateCaptureBuffer | Original GetNextVDMCommand; allocate bounded capture or return NULL. | Test transport only; product binding pending. |
| CsrAllocateMessagePointer | Original GetNextVDMCommand; source-shaped capture allocation with aligned length and returned pointer. | Test transport only; product binding pending. |
| CsrFreeCaptureBuffer | Original GetNextVDMCommand; release captured storage at original cleanup sites. | Test transport only; product binding pending. |
| CsrClientCallServer | Original GetNextVDMCommand; dispatch captured request and preserve transport/service result distinction. | Focused test routes to actual original server; authenticated product transport pending. |

The test implementations in `tests/broker/original_server_lifecycle.c` are
not product providers. No command-selection, waiting/retry or completion policy
may move here from the original client/server bodies.

The same four declarations also serve the restored original ExitVDM,
SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification and
RegisterWowExec cohort. Their service opcodes dispatch to the corresponding
original BaseSrv routines; no per-opcode policy implementation is added here.
BaseUpdateVDMEntry is now another original client caller of that same dispatch
binding; its signature is declared for the forthcoming launcher composition.
