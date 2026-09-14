# adapter-opennt-host

BaseClient's base_config binding supplies caller-owned immutable package paths
to original BaseGetVdmConfigInfo. Run16 must validate files and convert paths to
ANSI without loss before initialization and own binding lifetime on its thread.
The original 256-byte configuration envelope is retained and rejects oversized
inputs. No registry access, image classification, process launch or arbitrary
command parser lives here. The worker-path-end helper replaces only the original
system32 pathname assumption; these configuration records never cross RPC.

The BaseClient capture-memory binding is declaration-only in base_capture.h:
the process supplies CsrPortHeap as a local heap, initializes it before calls
and destroys it only after requests drain. All three memory routines now come
from original opennt-host/base/ntdll/csrutil.c, replacing test-authored bodies.
Original pointer-offset tables contain local addresses and never form the RPC
wire format. Counts/spans are supplied by the original bounded callers, not
remote peers; the future transport validates copied message fields separately.

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

T412 S2 adds `basesrv/include/base_classifier.h` and
`basesrv/source/base_classifier_path.c` for original BaseClient classification.
The header retains original nturtl/ntmmapi structures and status constants,
provides a private GetBinaryTypeW symbol and selects native NtOpenFile instead
of the DOS-path DEM adapter. This is a run16 client dependency, not broker
classification policy. The only new executable path binding calls native
RtlDosPathNameToNtPathName_U with its optional relative result omitted and
clears the original relative record, selecting the original full-path branch.
Native relative output writes 16 bytes and can return a nonempty name without
a directory handle, unlike original NT4's 12-byte borrowed-directory contract.
No parser, handle table, queue or current-directory policy is added. Original
curdir.c depends on native PEB/current-directory ownership and is not imported.
The original body therefore composes at the finite-binding recovery rung;
no mirror-body intrusion or new classifier algorithm is required. Focused
positive/negative proof and remaining limits are recorded in the
[S2 evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

Original BaseCheckVDM now consumes the existing capture/dispatch declarations.
Its private PPEB spelling binds only the existing support carrier; standard
streams and startup flags are public-API fields, not host PEB layout guesses.
`basesrv/source/base_client_process.c` implements only CONSOLE_HANDLE's modern
capability query via GetConsoleMode, preserving LastError. The original
conroute.h low-two-bit pseudo-handle test cannot identify modern Console
handles. Command/stream routing policy stays in the original function; no
native handle enters a broker record. The header maps historical swprintf to
the CRT's _swprintf signature, avoiding ISO's extra count parameter. The
original hotkey-to-reserved-string branch is tested with an exact result.

`basesrv/include/base_server.h` is the finite native declaration binding for
the unchanged srvvdm.c owner. Its declaration shapes were audited through the
S1 compile probe and now belong to this package, not a tests/ include. They
cover event/token/loader/duplication APIs, source-shaped CSR record types and
the request-thread accessor. OpenNtBaseServerRequestThread must return the
currently authenticated bound request context; the focused fixture supplies
its own process, while production authentication/registration is still pending.
The header supplies no CSR runtime, command policy or success stubs. Keeping
original CSR declaration records does not admit their historical transport.

`base_interactive.h`/`base_interactive.c` bind the single private predicate
below original winsrv `_UserTestTokenForInteractive`. The original kernel
TestForInteractiveUser compares against grpwinstaList's first windowstation
user, an unavailable global NT4 service contract. The admitted standalone
scope is captured from the broker's real process token on a visible process
windowstation; an immutable caller-owned scope is bound per dispatch thread
and restored/unbound afterward. Only equal AuthenticationId succeeds; absent,
invalid and mismatched scope fails closed. No wire LUID may initialize it.
Original token inspection and BaseSrv SYSTEM impersonation decisions stay in
their original owners. App must capture/bind this scope and install the linked
original helper before serving shared-WOW requests; product wiring is pending.
