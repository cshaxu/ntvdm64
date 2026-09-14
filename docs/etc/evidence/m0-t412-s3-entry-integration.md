# T412 S3 entry integration

## Launcher process-support dependency

S2's classifier test supplied its own NtCurrentPeb, current TEB and process-heap
functions. That proved the original classification algorithm but left a real
run16 composition dependency unverified. The S3 test removes those definitions
and links the selected adapter-mvdm-host-out/win32/source/opennt_support_rtl.c.
No new process-support implementation is introduced.

The test checks that the provider initializes ImageBaseAddress from the actual
module. Native classification/path services remain selected through the formal
opennt-base-client and opennt-base-bindings archives. Link-map checks require
NtCurrentPeb, opennt_support_current_teb and RtlProcessHeap from support.obj,
not fixture.obj. The support TU is compiled with /Gy so unused unrelated
helpers are not pulled into this focused link. This is not yet the complete
run16 product link or an audit of every helper in that translation unit.

Verify-BrokerOriginalClassifier.mjs now writes under
build/M0-T412/S3/original-classifier and records the process-support selection.
With OPENNT_BROKER_OWNER_BUILD pointing to S2/formal-owners, the actual original
classifier plus real support pass MEM/COMMAND/EDIT DOS, WRITE Win16, absolute
and relative PE, DLL rejection, extension handling and missing-file error tests.
No guest program is executed or modified.

The three BaseInit suffix strings remain fixture-owned initialization, retaining
the original baseinit.c values from lines 270 onward and basedll.h declarations.
Run16 must compose that initialization rather than import the entire Base DLL
initialization shell. Other pending product dependencies include command-tail
handling, actual broker discovery, original launch calls and error propagation.
Removing test-owned process providers is not three-program acceptance and does
not permit deleting the old product entry before its replacement is verified.

## Actual launcher entry checkpoint

The subsequent `src/app/run16_entry.c` composition now supplies those three
suffix values and calls the same original classifier archive. It does not
implement a second image parser. Classification belongs to run16; BaseSrv
receives the classified request, and ntvdm remains the original-shaped worker.
Original BaseClient launch-update calls belong on the launcher side, with
original BaseSrv state updates on the service side, not a worker CLI policy.

The formal generator's explicit x86 `run16.exe` target links the original
classifier and finite path binding, actual process support and original RTL.
Its Console-subsystem CRT wWinMain entry supplies the unmodified command tail.
CommandLineToArgvW decodes the target only; CreateProcess receives a mutable
copy of the original tail. The full historical CreateProcess/CSR product shell
is an excluded dependency; this app-owned native launch uses the public API
under the approved CLI exception, not a copied replacement kernel algorithm.

Commands, from the repository root:

```text
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot O:/repos.hobby/ntvdm64/build/M0-T412/S3/product -NodeExecutable O:/.nvm/versions/node/v22.22.1/bin/node.exe -ParallelJobs 4
cmd /d /c build\M0-T412\S3\product\run-ninja-parallel.cmd run16.exe
node tools/audit/Verify-Run16Entry.mjs
```

Build artifact: build/M0-T412/S3/product/run16.exe, 157,184 bytes,
SHA-256 `7becc9c20ebeb1d6122393e5bee4c6ce579ff3af5741c3b5a5dc71c1232f1d52`.
The fixture builds its own native child beneath S3/entry-tests. Absolute and
relative quoted paths both preserve the exact UTF-16 command tail, including
spaces, empty arguments and backslashes; the child checks inherited directory
and environment, relays stdin to stderr and returns 23. Empty invocation returns
87, and a missing file returns 2. The map selects the original classifier and
excludes the old local VDM provider/machine shell. No CPU is executed in these
native-entry tests; this is not a CCPU or guest regression result.

MEM returns 50 with an explicit unfinished-dispatch diagnostic. This is a
temporary S3 implementation gap, not accepted DOS behavior or the final product
contract. Broker discovery/dispatch, worker creation/registration, DOS/WOW
delivery and original completion/rollback remain required. No staged or runtime
EXE was replaced, and neither S3 nor T412 is closed by this checkpoint.

## Server request-context binding

S3 removes the lifecycle fixture's constant OpenNtBaseServerRequestThread
provider. Original OpenNT public/sdk/inc/ntcsrsrv.h lines 175--176 define
CSR_SERVER_QUERYCLIENTTHREAD as a lookup of NT4's private TEB CsrClientThread
field. That field is not a usable standalone binding on modern Windows;
recreating the CSR thread runtime is excluded. The smallest retained-interface
binding is base_request.c in adapter-opennt-host/basesrv: a borrowed thread-local
original CSR_THREAD pointer and explicit bind/restore. srvvdm.c is unchanged.

This is a dispatch mechanic, not authentication or a replacement registry.
RPC dispatch must authenticate and obtain a registered live context before
binding, then restore it on every exit before freeing that context. No native
pointer enters wire records. Unbound lookup returns NULL, never a manufactured
default caller; original service entry must not run in that state.

`node tools/audit/Verify-BrokerOriginalLifecycle.mjs` passes with outputs under
S3/original-lifecycle, preserving S2 evidence. The map selects request.obj;
tests check empty initial TLS, nested restoration, independent enqueue-thread
binding and explicit teardown, together with all original lifecycle checks.
Process records and dispatch remain fixture-owned: this is not actual broker
command delivery or worker-death registration. Formal owner composition now
also includes this binding, without changing the deployed executable.
The formal `opennt-broker-owners` target in S3/product builds successfully;
rerunning the verifier with OPENNT_BROKER_OWNER_BUILD set to that directory
also passes and selects opennt-base-bindings:request.obj. Existing original
source compiler warnings remain; this is not a warning-free build claim.

## Remaining fixture-to-product dependencies

The actual lifecycle link map still used fixture-provided NtCurrentPeb,
current TEB and RtlProcessHeap. Those three definitions and their private
records are now removed. The test links existing opennt_support_rtl.c with
/Gy, checks real module/PID/heap values and asserts the three map owners.
With OPENNT_BROKER_OWNER_BUILD=S3/product, all original lifecycle tests pass,
including asynchronous retry and generation cleanup. No original service
algorithm or production support function was changed to obtain this result.

Console association remains explicitly fixture-local (value 1). Existing
process support initially uses stdout as ConsoleHandle; stdout can be a pipe
and is not cross-process Console identity. The test overrides that local field,
not a real Console registration. Product membership must use the admitted
Console query/association design. The result records this limitation and the
selected process-support source hash.

Remaining fixture providers are CsrClientCallServer and
CsrLockProcessByClientId/CsrUnlockProcess. Product counterparts require real
authenticated transport and registered-process lookup with pinned lifetime.
Original base/subsys/csr/server/process.c lines 943 onward acquires the process
lock and references the match; unlock dereferences and releases the lock.
A PID lookup returning an unpinned pointer cannot preserve that contract.
The full CSR runtime remains excluded. The nullable UserNotifyProcessCreate
symbol retains the already approved absent-hook disposition, not emulated
success. These tests do not yet establish a production broker or S3 closure.
