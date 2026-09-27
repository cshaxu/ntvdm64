# Frontend executable owner

This owner contains character-session presentation: the copied DOS Console
dispatcher, screen/input channels and native hidden Console backend/helper.
The source was relocated from run16-exe without algorithm changes during S4.

The formal graph links the service, presentation and private helper into
frontend.exe, not run16.exe. The ordinary launcher selects the independent
frontend through its authenticated client. The helper is a private role of
frontend.exe, not another product executable. Object-cache paths retaining
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
notification until close. The frontend process uses actual native Console
membership and original DOS record usage to retire, not launcher lifetime.
Launcher completion, helper failure and frontend-session closure remain
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
