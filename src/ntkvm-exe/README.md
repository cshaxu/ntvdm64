# ntkvm frontend executable owner

This owner contains character-session presentation: the copied DOS Console
dispatcher, screen/input channels and the frontend-owned native ConPTY backend.
S4 originally relocated the owner from run16-exe; S9 replaces its helper backend.

The formal graph links the service, presentation and ConPTY terminal state into
ntkvm.exe, not run16.exe. The ordinary launcher selects the independent
frontend through its authenticated client. There is no private helper role.
Object-cache paths retaining
the old run16 name do not identify their source or final executable owner.

`frontend-client.lib` now isolates native launch requests and copied launch
records from the service/renderer. Both the ordinary run16 link and the
client-only fixture exclude frontend service, input-pump and renderer objects.
The private request transfer implementation is shared by the two endpoints,
not duplicated. The client library contains bootstrap_client,
native_request_client, native_request_io and native_console_launch only.

BaseSrv retains authentication and original DOS/WOW records, ntvdm retains
guest execution, and run16 retains classification and direct-target waiting.
Do not create a parallel scheduler or duplicate these presentation providers.

`session_service.c` now owns the existing DOS-channel/native-request pump,
resource lists and joined teardown. It borrows the registered capability and
notification until close. The frontend process retains its single ConPTY
across native completion and intervening DOS tasks, not by launcher lifetime.
The backend retention predicate is not actual native Console occupancy.
Launcher completion, I/O failure and frontend-session closure remain
different events; native descendants are not recursively terminated.

S9 lifetime decision: keep HPCON until explicit frontend closure. Early
ReleasePseudoConsole and backend recreation have been removed from the
production backend; no observer process or Job supplies last-client detection.
Uncertain last-client retirement may conservatively retain the frontend.
Resource experiments opt into NTKVM_CONPTY_TEST_RELEASE to release admission
and verify that later attachment is refused. Release code/state/API lookup are
absent from the production build; startup does not require ReleasePseudoConsole.
Production target completion never releases the backend.
Shared DOS/native screen composition has real guest/native output coverage;
the complete exact-candidate gates and publication remain governed by the S9
ledger. Source placement alone is not a delivered migration claim.

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

## S9 ConPTY replacement

native_conpty owns the public pseudoconsole and byte-stream resource boundary;
native_terminal binds its output to the pinned libvterm UTF-8/VT state. These
are selected by the formal ntkvm graph. S9 published the verified replacement;
Status and its ledger retain exact hashes and explicit limitations. The old
helper implementation, protocol and dispatch have been removed, not retained as
a fallback. S10 removes its unused snapshot reader and second wait loop while
retaining visible Console presentation bindings. Native Window text uses
the same bitmap rasterizer as DOS, with default glyph data from the pinned
V7VGA ROM. Generate-FrontendFont.ps1 emits only its 256-by-14 bitmap slice
under build; DOS still supplies its actual current guest font banks. No GDI
font or general Unicode font subsystem is selected; missing glyphs become '?'.

libvterm-import.json pins the official MIT-licensed 0.3.3 archive and its nine
runtime C units plus header closure. lib/libvterm-0.3.3.tar.gz is the unchanged
source distribution, not a binary provider. The build materializes only the
selected closure under build in the formal Ninja generator (the focused
Build-FrontendTerminalLibrary.ps1 uses the same pinned source selection); no
download, external checkout, upstream test script or runtime dependency is
required. The source archive retains upstream notices; LICENSE.libvterm is
also visible beside it. None of the four existing shared library imports is
modified. Launcher and worker must not link terminal state or rendering.

native_terminal_screen.c compiles the original screen.c once and appends the
registered, frontend-local cell/cursor import. This private-layout build seam
does not modify upstream files but extends the compiled runtime; its source
and removal condition are recorded in the S9 ledger and import manifest.
It preserves the live parser, saved state and inactive screen. The focused
screen-import tests do not themselves prove DOS/native handoff wiring.
