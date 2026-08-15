# T202 S6 COMMAND typed child lifecycle

## Question

Can the connected COMMAND execution services replace the historical NT4
worker-thread/broker shell with a bounded direct-profile child transaction
while preserving the original terminal result contract?

## Source basis and replacement

`cmdExec32` starts a child, waits for `GetNextVDMCommand`, and—when no next
DOS command is available—clears CF and returns the child exit byte in AL.
The CLI composition has no NT4 broker queue.  The source-derived replacement
therefore uses that terminal branch directly: a typed session transaction
launches, waits, closes all private resources, then resumes with CF clear and
the low exit/error byte in AL (preserving AH).

The transaction is admitted only after:

1. the shared COMMAND `HOST_GLOBAL` capability resolves direct;
2. `54:08` has copied and validated the original stderr/stdout/stdin
   `STD_HANDLES` token record from `SS:BP`; or `54:0A` has the separately
   copied host processor input;
3. command-tail/environment guest data is copied through checked real-mode
   ranges; and
4. the provider has no retained guest address.

Private handles are duplicated inheritable solely for one `STARTUPINFO` with
`STARTF_USESTDHANDLES`; they are closed before the BOP result returns.  The
provider never transports a handle/PID, calls `SetStdHandle`, creates the
historical temporary-pipe files, starts VDM pipe workers, or calls a
VDM/CCPU/VDD broker.

## Verification

The bound MSVC x64 `/MT` COMMAND fixture obtains all three opaque tokens,
writes the original packed record to guest RAM, then invokes `54:08` with a
copied `cmd.exe /c exit 7` tail and double-NUL environment.  It proves one
private launch and completion with `CF=0`, preserved AH and `AL=7`.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t202-s6-command-child-lifecycle-r2 `
  -Fixture boot-namespace
```

The manifest records `linkExitCode: 0`, `runExitCode: 0`, and `passed: true`.

## Limits and follow-up

This closes the direct terminal child transaction, not all historical child
composition.  A future admitted host command that supplies a new DOS command
requires a separate typed re-entry queue; this provider must not fabricate
one.  The non-direct profiles, malformed tail/environment, invalid stream
record, absent processor and unsupported queue form continue through the
existing explicit deferred/error paths.  Native trace remains integration
evidence only and is not run by this change.
