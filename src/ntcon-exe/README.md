# ntcon frontend executable owner

This owner contains visible Console/Window presentation and copied worker
screen/input channels. NTVDM and NTVWM use the same frame/input contracts.
NTVWM, not NTCON, owns the ordinary hidden Console and native targets.
The T423 S12 formal graph no longer selects ConPTY, its terminal parser or a private
helper. The duplicate native renderer has also been removed. Current delivery
and remaining acceptance gates are recorded in docs/states/CURRENT.md.

## Client and service separation

The formal graph links the service and presentation into
ntcon.exe, not run16.exe. The ordinary launcher selects the independent
frontend through its authenticated client. There is no private helper role.
Object-cache paths retaining
the old run16 name do not identify their source or final executable owner.

`frontend-client.lib` now isolates native launch requests and copied launch
records from the service/renderer. Both the ordinary run16 link and the
client-only fixture exclude frontend service, input-pump and renderer objects.
The launcher-owned client library contains bootstrap_client and
native_request_client under run16-exe. Common owns protocol declarations,
the copied packet codec and pipe transport; no launcher transfer wrapper remains.
The shared process primitive in run16-exe/native_launch.c is used by the
NTVWM target owner and launcher service bootstrap. Native GUI submission also
goes through NTSRV to NTVWM; it is not a frontend execution route.

The worker-side copied protocol client now belongs to common, not this
frontend component. Both workers link its ordered transport, validation,
frame/input codec and resource lifecycle. This owner retains the service,
renderer and frontend arbitration only; no private worker-client copy remains.

BaseSrv retains authentication and original DOS/WOW records, ntvdm retains
guest execution, and run16 retains submission and direct-result receipt waiting.
Do not create a parallel scheduler or duplicate these presentation providers.

`session_service.c` owns the shared worker-channel pump,
resource lists and joined teardown. It borrows the registered capability and
notification until close. NTVWM owns native request execution and reports real
Console membership separately from its resident worker process. The service
retirement barrier accounts for pending admission and DOS/native users.
Launcher completion, I/O failure and frontend-session closure remain
different events; native descendants are not recursively terminated.

The generated-link ownership check is
tests/component-integration/verify-frontend-link-ownership.ps1. It checks the
client archive and transitive executable link inputs, excluding order-only
build prerequisites. The current delivery and verification status belongs to
docs/states/CURRENT.md; component placement alone is not S4 acceptance or
permission to replace the published runtime package.

## Window library import

`nxvm-import.json` pins the exact Win32 dependency closure of the shared types,
base, kvm-base and kvm-window from SoftPC revision
3186afcd7b743e9797364c6a762eccd3b5576ca1, refreshed with owner approval on
2026-09-27 from the earlier nxvm pin. The selected files under `lib/` and
their MIT notice are unmodified. Only frontend presentation may consume this
library; launcher clients and worker objects must not acquire a link edge.
The upstream repository is not a build or runtime dependency. Its test library,
Console broker, application and Linux implementations are not imported.

The manifest's compile rows select fifteen translation units; the remaining
files are their recursive header closure and four component READMEs. Base
clock/process implementations are unnecessary for this selected closure.
`tests/component-integration/verify-frontend-window-library.ps1` checks pinned
bytes and builds the real Win32 leaf with a non-interactive contract fixture.
This initial library verification does not claim product Window wiring; the
S6 ledger tracks that remaining work.

INPUT_RESET releases only successfully delivered keys belonging to that source,
discards frontend-local dead-key composition, and retains the live source identity.
SOURCE_RETIRED alone permanently retires that identity. Native mouse capture
release remains owned by kvm-window, not by the NTVDM worker.

Logical text-region state is separate from the visible Console's physically
limited viewport. Backend handoffs use the logical region; only presentation
uses host window limits. NTCON routes native pointer motion/modifiers but does
not integrate its position or draw a native arrow. NTVWM composes its text
block cursor in the common frame; NTVDM retains its original guest cursor.
No worker depends on this component's private renderer or display dimensions.

T424 S12 unifies Console storage as logical_surface. Native buffer tiles,
viewport, cursor and text frame are staged under the per-channel publication
contract; only a complete validated publication replaces the committed grid
and renderer snapshot. Abort/EOF retains the prior snapshot and anti-replay
serial. DOS stream operations and complete VGA frames use this same storage.
Physical Console size clips upper-left projection only; unused visible cells
are blanked. Host cursor updates are applied even when unchanged in the
owner-requested diagnostic baseline; workers' repeated requests stay visible.
Current logical state seeds the next worker; no old-page/row-bias heuristic
or initial CMD snapshot is a source of truth. Host scrollback is not promised.

## Retired S9 backend evidence

T423 S9's native_conpty/native_terminal implementation is removed from its S12
production graph and source. Historical commits and the T423 S9/S12 evidence retain
its behavior and experiments; it is not an optional runtime backend. NTVWM
now packs native characters into the common text ABI. NTCON uses the same
text renderer for both workers, not a native bitmap-frame fallback;
font/palette transfer belongs to that shared ABI.

libvterm-import.json, its unchanged MIT source archive and license are retained
research provenance only. They have no formal production link edge. Historical
ConPTY/parser tests that require deleted providers are not S12 acceptance tests;
their retirement and replacement coverage are tracked in the S12 ledger.
