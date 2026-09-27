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
