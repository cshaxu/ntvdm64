# Interface migration marker

T424 S7 moves the current declaration/IDL owner to common/protocol and the
native packet codec API to common/codec. Local bootstrap/request client APIs
belong to run16-exe; worker client instance declarations belong to worker-base.
The transitional native_request_protocol.h and launcher transfer wrapper are
removed. Both protocols now have declarations in common; the old bootstrap/
native control-pipe DTOs are also removed. The internal service native pipe
test seams are deleted; copied-queue tests use a test-only translation of the
actual private service implementation, never a production transport/API.
The previous layout below is retained as provenance context, not current
file-location guidance.

Single declaration owner for product identity and cross-executable contracts.
console_io, console_mouse and console_video retain the existing copied KVM
request/input/frame layouts. service.idl declares the authenticated service
RPC used by launcher, workers, frontend and monitor; version.h owns the shared
application/protocol identity. Moving these files does not change wire layout.

vdm_protocol.h owns the copied VDM envelope, operation values, startup scalars
and payload spans. The service transport keeps encoder/validator functions and
pointer-bearing local encode inputs. vdm-protocol-test checks existing layouts
and malformed/short/overflow inputs; this is not NTVWM production acceptance.

frontend_protocol.h also owns bootstrap replies, native request/replies and
copied launch packets. The superseded native_console_protocol.h/private
control channel is removed; NTVWM uses console_io like NTVDM. native_launch.h
declares the shared launch codec/materializer and borrowed local string views;
those local API types are not serialized records.

worker_console_client.h declares the shared NTCON worker-client API and its
local, borrowed-handle request state. This is not a serialized wire record and
does not confer ownership of a Console, guest or process. The only implementation
is worker-base/console_client.c, linked by both worker backends; no code lives here.

No connection, transport, authentication, scheduling, rendering or lifecycle
implementation belongs here. RPC outputs are generated under build/ only.
service.acf declares shared context access for worker receive, completion,
frontend-channel acquisition and member sampling. Other calls retain exclusive context access, including Close;
service-side locks remain the implementation owner of mutable state. The ACF
is a generated-RPC input and is included in the broker source hash manifest.
Backend-private types remain in their executable owner. The original wrappers
include these declarations rather than repeat them. Copied frames/envelopes
carry fixed-width fields, never native pointers. Native launch resource slots
are checked against the authenticated sender and materialized as bounded
OS-managed attachments; they are not trusted sender-local handles.
