# T412 S1 consolidated design and exit review

S1 concludes audit/design, not the T or the three-program product. Implementation
is the already owner-approved sequential S2--S6. The working plan remains the
single stage plan and the broker proposal remains the single proposal.

## Evidence and removal scope

- The [owner audit](m0-t412-s1-broker-owner-audit.md) accounts for D01--D07/D10,
  original owners, actual selected providers and mirror/overlay boundaries.
  Its 1,349-line local-provider envelope is replacement scope, not net deletion.
  Server mirror diff is zero; client +136/-3242 includes genuine package crops.
  There is no directly removable BaseSrv private overlay. Mixed COMMAND hooks
  and async captures require individual integration proof before retirement.
- [RESET evidence](m0-t412-s1-reset-consumer-repair.md) and
  [Console cell evidence](m0-t412-s1-console-cell-regression.md) disposition the
  two diagnosed prerequisites. The published repaired baseline is retained;
  EDIT snapshot/watchdog evidence is not keyboard/mouse or normal-exit proof.
- [Full original compilation](m0-t412-s1-original-server-compilation.md),
  [resource attachments](m0-t412-s1-rpc-resource-feasibility.md), and
  [Console membership](m0-t412-s1-console-membership.md) prove the bounded
  mechanisms described there. They do not prove product integration.

## Selected process and interface design

| Phase | Owner and decision | Failure/verification owner |
| --- | --- | --- |
| Positional entry | run16 preserves target/tail, cwd, environment and stream aliases. Select original GetBinaryTypeW, using the modern native image-section result; no autonomous NE parser. Native PE uses public CreateProcess. | S2 malformed/OS2/DOS/Win16/PE/error classification matrix; S3 quoting/search/creation errors. |
| Service connection | Authenticated local RPC, per user/session/logon scope. Named singleton/startup coordination and protocol major version distinguish a live compatible broker from a stale endpoint. | S3 concurrent start, wrong identity/version, bounded connect failure. |
| Original request | Validate lengths/overflow/type/IDs and capture local original message fields; execute original CheckVDM/UpdateVDMEntry and DOS/WOW record branches. | S2 source-body tests; no new task-selection table. |
| Missing worker | Original no-worker result owns selection. A bounded launch reservation pins that record while run16 creates/registers ntvdm. Process creation alone is not readiness. | S3 abort/timeout/duplicate registration, exact original undo branch. |
| Resources | Synchronous recipient acknowledgement at original duplication sites, typed OS attachments, role-specific receipts and original release calls. | S2/S3 alias, partial failure, cancellation, receipt revocation and peer loss. |
| Acquisition | Original BaseClient captures/copies/waits/retries; original server owns capacity and consumption. DOS wait and shared-WOW nonblocking polling remain distinct. | S2 short buffers, zero-length/empty queues, retry clears exit code; S5 WOW delivery. |
| Execute/reenter | ntvdm has original worker entry with consistent OS command line/SoftPC argv. COMMAND BOP 54:08 invokes run16; original permcom/reentry remains the guest owner. | S4 real programs, native/DOS child, streams/EOF and return. |
| Complete/retire | Original records own completion/errors/waits. Product retirement starts only after no tasks, reservations, active guest/reentry or outstanding resource work remain. | S5 full 60-second idle and active-interactive survival, races and abnormal exits. |

Transport carries an explicit version, operation, request ID, registered
generation, lengths and copied bytes. Reject malformed/unrecognized input
before service dispatch. Exact wire structs belong to S3 implementation, not
a second source-shaped CSR wire ABI. Request IDs correlate replies; they do
not authorize replay of a command after an indeterminate broker failure.
Client disconnect and launch-reservation expiry invoke original cleanup with
the recorded creation state; they never silently replay work.

One broker owns one original service-global first-VDM flag. Serialize its
query/clear with service dispatch; restart creates a fresh instance. Preserve
the existing non-mutating AddSystemFiles disposition. No host boot-file writes.

## Console and WOW boundaries

Use the [Console design](m0-t412-s1-console-membership.md): registered live
identities, original DosSesId/Console association, and a bounded owned internal
run16 query helper. No persistent broker attachment or client-claimed HWND
identity. Receipt endpoints do not transfer Console pseudo-handles.

WOWEXEC registration remains driven by original WOW32. Preserve original
PostMessageA/GetWindowThreadProcessId loading and PID/TID/sequence validation.
Bind CSR sequence/lookup to authenticated registered live process generations;
do not enumerate arbitrary processes or manufacture a CSR subsystem.

Preserve the original interactive-token query/AuthenticationId logic. Its
unavailable global first-windowstation predicate becomes the broker's admitted
interactive session/logon predicate. Original SYSTEM-impersonation handling
must query the authenticated effective token, not assume equal user SID is
enough. S2/S5 test both positive and rejected identities; no success stub.

UserNotifyProcessCreate is separate from WOWEXEC delivery. The original
nullable callback guard remains absent in this standalone CLI composition:
the product does not expose NT4 USER's native CreateProcess/WinExec pseudo
process or WaitForInputIdle registration service. Do not claim those external
system APIs are emulated or install an empty callback reporting success.
run16 waits for the original task completion event, not USER input-idle.
S5 must verify no required broker command path depends on that omitted hook;
an observed dependency reopens the design rather than being waived. Full
guest WRITE recovery remains its existing WOW16 owner, not broker acceptance.

## Entry and publication contract

Final public entry is `run16.exe <binary> [arguments]`; no required diagnostic
flags. BOP helper use is an internal authenticated product route, not a general
shell interpreter. Host-shell-consumed redirection uses inherited streams;
a literal `>` in the target tail is literal; guest COMMAND redirection remains
guest-owned. Batch/shell grammar uses the existing original/delegated host-shell
boundary when required, never a newly authored command parser.

S3 changes the formal build/staging authorities together to exactly three x86
products: build/output/{run16,basesrv,ntvdm}.exe, deployed side by side under
O:/ntvdm64. No fourth ntvdm32 compatibility provider is planned. Archive the
previous deployed baseline under build before replacing it; leave guest media
in their existing manifest paths and all runtime logs under O:/ntvdm64/logs.
Until that verified S3 migration, current ntvdm32-only publication remains in
force; S2 must not rename or overwrite it with an unverified probe.

## Exit and implementation gates

The mandatory S1 rows have source-backed owners, selected finite designs,
removal scope and explicit executable migration. Remaining listed tests are
implementation acceptance gates, not unspecified design choices or claimed
passes. S1 is closed at this design boundary. S2 must now restore/select the
coherent original server/client lifecycle, using focused tests of actual
original bodies. A fixture transport may bind unavailable mechanics during
S2, but cannot implement the policy or count as three-process acceptance.
T412 remains open until all S2--S6 requirements are verified and delivered.
