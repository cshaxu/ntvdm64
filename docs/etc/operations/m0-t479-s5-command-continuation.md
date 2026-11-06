# T479 S5 Win3.1 command continuation implementation boundary

Baseline: 064c47fa6, clean before this source audit. The S4 bridge candidate
has actual Win3.1 wait/wake and lifetime evidence; it does not execute commands.
This record fixes the next implementation boundary, not runtime acceptance.

## Checked call chain and reusable owners

- `softpc.new/base/ccpu386/c_main.c` commits the decoded instruction pointer
  before BOP dispatch and invalidates its host instruction pointer afterward.
  Returning to the interpreter is the required unwind boundary; BOP FE and
  nested host_simulate are not substitutes.
- `softpc.new/host/src/nt_bop.c::MS_bop_4` loads the service byte at current
  CS:IP, calls CmdDispatch, then increments current IP as a 16-bit value.
  Services 8 and 10 select cmdExec and cmdExecComspec32 in cmddisp.c.
  A handled asynchronous entry must return before that original epilogue.
- `dos/command/cmdexec.c::cmdExec32` activates singleton state, blocks the
  event thread and waits in GetNextVDMCommand. It remains the ordinary path;
  wrapping it in another host thread does not make it per-VM.
- `command/source/mvdm_command_native_child.c::mvdm_command_capture_create`
  already makes an independent bounded copy without using that singleton.
  Preserve its command CR replacement and standard-handle tokens. Capture
  must complete in the actual VM's mapped CPU context before waiting.
- `mvdm_guest_location_acquire` passes segment*16+offset to the session lease;
  the actual backend calls c_sas_loads. In ccpusas4.c that is a **linear**
  byte read through c_sas_hw_at/bios_read_byte/xtrn2phy, not an unpaged physical
  alias. Reuse this chain in the current VM; never keep a lease across wait.
  This source fact is not a fault-injection or cross-page runtime proof.
- `command_process_compat.c::ntvdm_command_launch_prepare/start` owns copied
  command/environment/CWD/duplicated native handles and starts once. It does
  not own completion/lifecycle. `vm_host.h` requires its registration/context
  RPC calls off the CPU thread and supplies the authenticated capability pair.

## Entry and restoration contract

1. Only BOP 54:08/0A in a registered usable bridge can select the new path.
   Other services and the ordinary path keep their existing dispatch.
2. Enter the VMM callback through the original CCPU CALLF implementation,
   with original arguments untouched and wrapped IP+1 as the return address.
   No host request/token is created before VMM supplies the actual VM.
   Production admission must bypass MS_bop_4's subsequent +1 epilogue; the
   diagnostic interposition compensates its destination only for that epilogue.
3. VMM's Simulate_Far_Ret consumes the real CALLF frame and restores client
   CS:IP and SS:SP before request admission. This is deliberately a FAR call,
   superseding the earlier jump-and-token candidate; no synthetic frame or
   nested host_simulate is used. CCPU retains normal stack/page validation.
4. Begin binds the actual callback VM to the wake registry's generation, and
   returns a monotonic token plus one executor reference. VMM retains client
   state; the host stores no duplicate register/PC snapshot or unclaimed entry.
   The token stays on the VxD's stack, never in untrusted V86 registers.
5. Capture the calling VM's arguments before waiting, via its actual client
   frame and the VMM's documented CB_High_Linear mapping. Do not assume low
   linear addresses still select that VM while executing ring-zero VMM code.
   Reuse bounded copying/lease rules; no guest pointer survives scheduling.
6. Off-CPU preparation and launch use the exact VM frontend origin/context,
   copied directory/environment and resolved handles. NTSRV owns task-chain
   admission and terminal results. Neither the callback nor ICA lock may span
   RPC, host joins, process waits or frontend control.
7. Completion publishes through the existing claimed IRQ and exact VM
   generation. The callback consumes its own terminal result, applies the
   original CF/AL completion or reentry contract, then returns through VMM.
   Do not treat a bare host child exit as DOS reentry or final I/O acceptance.
8. Retirement removes admission and pending delivery before freeing state.
   A native completion must retain a request reference until its publishing
   call returns; stale VM/request generations cannot restore a new VM.

## Finite implementation and proof order

First implement the detached continuation and per-request ownership, retaining
ordinary command behavior. Prove actual guest caller recovery before connecting
launches: the witness must distinguish a diagnostic completion from actual
command execution and must never be linked into the released worker.

Then connect real capture/context/launch/terminal handling for both services,
with two VM commands completing in both orders. The VM frontend registration
requires real ntvdm_vm_io and input-event ownership: an empty provider, the
containing worker's upstream capability, or a direct unowned CreateProcess is
not an acceptable way to bypass that dependency. Implement the required
provider path before claiming actual command execution; S6 still owns full
47-operation, input, final-output, cancellation and nested acceptance.

Mandatory negative cases are failed capture/admission, repeated completion/consume,
VM retirement during each phase, stale generation, early completion before
wait, missing bridge and original non-bridge commands. Full register/flags/
stack and exact return IP are assertions, not inferred from a visible prompt.
No S5 runtime implementation or test has passed at this source-audit checkpoint.

The selected FAR entry is grounded in `ccpu386/call.c::CALLF`: its V86 branch
validates two stack items, pushes CS and IP using original stack/paging helpers,
loads CS and transfers IP. The private COMMAND callers at `tcode.asm`'s
`std_done` and `d16_run`, and `init.asm`'s `std_done`, execute INT 21h immediately
before these BOPs (with at most a subsequent PUSH ES). The four-byte FAR frame
uses the stack area already used by that interrupt, not command/STD_HANDLES
data at or above SP. This source proof covers these package-owned unprefixed
16-bit callers, not arbitrary malformed/prefixed third-party BOP sequences.
Production entry selection and current-VM capture still need integration proof.

## Win3.1 diagnostic checkpoint (2026-11-06 UTC)

[Structured evidence](../evidence/m0-t479-s5-continuation-diagnostic.json)
preserves each selected package, guest probe hashes, matrix result and VM
reports. This supersedes the audit-only checkpoint above, not S5 acceptance.

The new jump-entered callback restores the actual client's original CS:IP,
EAX and EDX and consumes a counted completion. Both BOP 54:08 and 54:0A run
in each of two real DOS VMs. The witness checks SP, all general registers and
selected arithmetic/DF/IF/CF flags after return. It also retains the 320
existing FAR-callback checks and blocked cross-VM notification checks.
The diagnostic host supplies AL=25h: it does **not** launch native commands.

The first full continuation run timed out on normal exit, although both DOS
reports contained two successful continuation checks and no errors. A control
using the same driver and diagnostic host without the new continuation cases
passed. A subsequent traced run passed, recording all four claim/consume
pairs, both VM retirements and system shutdown. The same traced-host binary
with trace output disabled also passed. These observations do not identify
or repair the initial timeout; it remains an S5 acceptance issue.

Trace output is optional, test-only and disabled by default. It is not a
production transport or a synchronization requirement. All runs used isolated
Win3.1 copies and verified the original installation unchanged. No Win95 run,
new default publication or S5 completion is claimed. The published S4 package
and candidate driver remain unchanged. Real command execution, delayed and
reordered completion, failure/reentry and retirement during command execution
remain required before the S5 delivery gate.

## Bound-request implementation checkpoint

The later `checked-continuation5` and `checked-continuation6` runs replaced the
test's fixed slots with a dynamic, ICA-serialized request registry; the latter
delays completion until both real VMs arrive, then a native thread completes
the second arrival first. Both runs exited normally with all four continuation
checks passing. These still exercised the earlier jump entry.

Review found that creating a token before the actual VMM callback left an
unbound interval on preemption/retirement. `checked-continuation7` proved the
FAR entry above with deferred completion, all four caller-state checks and
normal Win3.1 exit. Therefore the unbound prepare/claim/discard phases and
duplicate resume snapshot are removed, not retained as fallback. The host API
now begins only with an actual registered VM. Registry and executor references
permit consume/retire before the producer releases its reference; exact VM
retirement and whole-payload shutdown invalidate records under the same ICA
lock as wake removal. Late completion fails instead of targeting a reused VM.

This is still a diagnostic AL=25h executor. NTSRV launch/context, real command
capture through CB_High_Linear, the actual I/O provider and terminal handling
remain unconnected. The original exit timeout has not been attributed to the
entry design and is not claimed repaired. No production delivery is claimed.

`checked-continuation8` verifies the simplified known-VM-only API with the
same FAR driver and deferred native-thread executor: all four caller returns
and normal Windows exit passed; original installation hashes were unchanged.
The cached production worker build and the registered CTest target
`mvdm-vm-notification` passed (328 assertions, real registry/wake code with
unit-only PIC boundaries). There is no prepare/claim API in the retained
implementation. The failed notification-publication path must be handled by
the eventual lifecycle owner; it is not a retry or success path in this module.

## Win3.1 VM capture failure (continuation9/10)

The next candidate captures the actual VMM client frame and applies
CB_High_Linear to each VM's command, environment and standard-handle data.
The registered command-capture and continuation unit tests pass, but real
Win3.1 acceptance does not: both continuation9 and the traced continuation10
enter enhanced-mode Windows and terminate with ERROR_INVALID_ADDRESS (487).
The original installation's bytes were verified unchanged. Neither run is
passing or published evidence.

Continuation10 records actual VM 0x80797000, client frame 0x806b7f70 and
CB_High_Linear 0x84400000. Original CCPU xtrn2phy(frame, supervisor-read)
succeeds, mapping the frame to physical 0x00289f70; c_sas_memory_size is
0x01800000. The session backend in mvdm_softpc_guest_memory.c instead compares
the untranslated linear address against that physical RAM size, rejecting
the lease before reading it. This proves a host adapter boundary defect in
this new capture route, not an invalid original VMM frame or a guest defect.

Retained inputs/results are build/t479-s5-command/verify-continuation10.ps1,
continuation10.process/stdout/stderr/exit, continuation10-package and
checked-continuation10/continuation-trace.txt plus runtime/matrix-results.json.
The test-only trace reports translation without replacing the failed read.
The original xtrn2phy contract is non-faulting; c_sas_loads can instead enter
BIOS virtualization on translation failure. The repair must distinguish these
contracts, preserve access/dirty semantics and reject failed mappings without
leaking a lease or retaining a guest pointer across scheduling. Merely removing
the physical-size guard is not a validated repair. This remains S5 work;
Win95 is deferred until the Win3.1 command and I/O path works.

## Linear lease correction and actual capture checkpoint

The project-owned lease backend now checks every linear page with original
CCPU xtrn2phy before invoking the existing SAS copy. Nonpaged bounds stay
unchanged. Access flags exactly follow ccpusas4.c's bios_read/write helpers
(including their CPL rule and V86 accessed/dirty request); the physical RAM
bound applies to each translated span, not the original linear address.
This does not replace the CPU translator, publish a native backing pointer or
add mirror/guest differences. An unmapped/denied span fails the lease instead
of entering the SAS BIOS-virtualization fallback with an allocated bounce
buffer. Each lease preflights its whole span before writing; a larger chunked
copy is not newly claimed atomic. Mapping and copy are synchronous on the CPU
thread. No mapping survives scheduling.

Cached production worker build and three registered CTests pass: 328
notification/continuation assertions, 31 linear-memory assertions and 39
independent-capture assertions. The memory fixture checks cross-page
noncontiguous mappings, untranslated overflow, physical bounds, access flags,
absent pages, write rejection and lease reclamation after failed commit.
It substitutes CCPU services and therefore is not guest paging proof by itself.

Actual Win3.1 continuation11 passes with tracing; continuation12 reuses that
exact staged package with tracing disabled and also passes. Both prove two
concurrent VMs, 320 FAR-callback state checks, four BOP returns, independent
role-specific command/environment/standard-handle capture, native-thread
reverse-order completion and normal Windows exit. Original installation
hashes remain unchanged. The diagnostic completion is still AL=25h; no real
native command or complete I/O execution is claimed. The original earlier
exit-timeout observation remains unattributed, rather than silently closed.

These candidate results and exact input hashes extend the structured evidence
as `linearCapture`. Full ordinary-consumer regression, real command/context/I/O
integration, failure/reentry and retirement during real command execution
remain S5/S6 delivery work. No default publication, commit or S5 closure has
occurred at this checkpoint.

## Production client-frame capture and original service selection

`softpc/mvdm_vm_command.c` now owns the synchronous VMM frame conversion;
the diagnostic host invokes that production function instead of owning the
field extraction. The 72-byte Client_Reg_Struc is copied through the existing
lease, using the documented MS-DOS 6 VMM.INC layout. DS:SI, ES:0, SS:BP and
AL are taken from that frame, not native registers or the currently mapped
low VM. CB_High_Linear applies only to the captured VM's data locations.

Original `dos/command/cmdexec.c::cmdExec` selects direct execution when AH is
zero and its COMSPEC branch otherwise; `cmdExecComspec32` selects the
interactive service. The new capture preserves these explicit service values
for the existing Run16 launch adapter. It does not infer service from command
text. It retains host COMSPEC presence/size checks and cmdExec's combined-tail
length rejection, while the capture retains the original tail rather than
building and reparsing redundant COMSPEC text. COMSPEC failure destroys the
independent capture and returns ERROR_BAD_ENVIRONMENT. Direct execution does
not acquire that dependency. No session singleton or standard-handle override
is changed by this function.

Cached worker build and three registered tests pass, now including 63
independent-capture assertions (328 continuation and 31 memory assertions
unchanged). The added unit cases cover the actual frame field offsets, all
three service routes, absent COMSPEC, invalid service/frame, output ownership
and lease cleanup. `continuation13` uses this production capture in two real
Win3.1 VMs: six continuation cases (54:08 AH=0/AH=1 and 54:0A per VM), reverse
completion, 320 existing callback checks and normal Windows exit pass with
tracing disabled. The original installation is unchanged.

This connects production capture to the verified guest boundary, not the
default command dispatcher: command execution and the VM I/O provider remain
unconnected. Before launch, the per-VM directory, converted command and
resolved stream lifetimes must be supplied to the already delivered launch
owner. Original cmdCreateProcess reads the process-global `=drive:` variable;
sampling that later on a native thread would not prove the calling VM's
directory. Do not reuse that singleton/global state for concurrent commands.
No new default publication or S5 closure is claimed.

## Per-VM directory candidate and unresolved instance evidence

The next candidate obtains DOS AH=47 in the actual callback via VMM
Save_Client_State / Begin_Nest_V86_Exec / Exec_Int / End_Nest_Exec /
Restore_Client_State. Its save area is the full 108-byte Client_Reg_Struc,
including alternate nested registers (the capture reader only needs the first
72 bytes). A 64-byte paragraph-aligned GVDAInstance|GVDAZeroInit buffer is
allocated by VMM at Device_Init, not borrowed from COMMAND's stack. The BOP11
descriptor now contains frame, VM high-linear base and directory address;
all three are copied synchronously before wait. Host capture owns the resulting
drive-qualified directory. No host process current-directory mutation is added.
ABI references are the existing MS-DOS6 VMM.INC and read-only OpenNT-4.5
net/sockets/tcpcmd/dhcp/client/vxd/vxdfile.asm allocation/nested-call examples;
no original file body was imported or guest binary modified.

This candidate is **not accepted**. Initial continuation14 stopped during
linking with 0xc000026b; its process was confirmed absent before a retry.
The cached retry built and passed units, then Win3.1 entered its desktop but
the host diagnostic exited ERROR_INVALID_DATA. continuation15 traced the
same candidate: PIF-only directory setup did not give both VMs their declared
directories. continuation16 changed only the independently authored witness
to call DOS CHDIR explicitly. Its first pair captured distinct VA/VB paths,
but after the first reverse completion the second pair both returned VA.

continuation17 adds a direct guest INT21/AH47 read immediately before each
callback. Trace tags 0x204 (nested captured absolute directory) and 0x205
(direct DOS directory, without drive prefix) agree: initially VB/VA, then
VA/VA. Thus the wrong per-VM expectation is already visible through ordinary
guest DOS, before the new nested call. This does not yet prove an immutable
guest defect: the host's existing instance/state adaptation and VMM/DOSMGR
contract still require attribution. Do not relax the directory-isolation
assertion or publish this as full VM correctness. Original mscode.asm's
DOSMGR instance query explicitly leaves CDS/SFT/device instancing to DOSMGR;
that is the next source boundary to inspect, not authority to patch guest.

The current registered units pass 328 continuation, 31 linear-memory and 72
capture assertions, including invalid/unterminated directory, root path and
independent copied directory lifetime. They cannot override the actual
two-VM failures. Runs 14--17 and their immutable package/install-copy inputs
remain under build/t479-s5-command. No publication, source commit or S5 closure
has occurred; real commands/I/O and this directory gap remain unfinished.

continuation18 is a scheduling control using the same sealed host package and
candidate driver as 17. `-DirectoryYield` adds ordinary INT 2F/1680 yielding
for 18 BIOS ticks before each direct directory read, with immediate native
completion instead of paired deferred completion. This is test pacing only.
The directory assertion fails on the first BOP11 admission, before any BOP12
return or blocked command wait. Both direct and nested reads report VA.
The observer records ERROR_INVALID_DATA (13), not normal Windows exit.
Thus deferred command waiting is not necessary to reproduce this failure;
it does not prove that the complete bridge is uninvolved or attribute the
remaining DOSMGR/host instancing boundary. No original guest was changed.
The sealed trace, matrix, input identities and runner record are retained in
the diagnostic JSON. Win95 remains deferred while this Win3.1 path is resolved.

## Directory attribution: ordinary CDS versus NT NetCDS

Runs 19--21 resolve the first bounded directory failure. The test now reads
DOS AH52's original SysInitVars CDS pointer and count (offsets 16h and 21h).
Run19 records count=5, table=043D:0000 and role B reading VA, before its first
command wait. Drive O (index14) therefore does not index this table.

The original call chain is `path.asm::$CURRENT_DIR` ->
`macro.asm::GetVisDrv/GetCDSFromDrv`. For a drive >= CDSCount, the latter uses
one `DOSDATA:NetCDS`, reusing it without a host query when the drive letter
already matches. CHDIR in another VM can consequently change the directory
subsequently returned for that same drive. Original `sysinit1.asm` explicitly
ignores LASTDRIVE and allocates only SYSI_NUMIO CDS entries; LASTDRIVE=Z is not
a fix. `cmdmisc.c::cmdGetCurrentDir` supplies the process `=X:` environment
when the guest requests a resync; it is not a per-VM directory authority.

Original `dostab.asm::Instance_Table` does not cover NetCDS. The original
NTDOS.MAP places NetCDS at DOSDATA:13F3 (71 bytes), immediately before the
separately instanced exec_init_SP at 143A (24 bytes). The other instance spans
are 0022+2, 0032+4, 01FB+106h, 0302+1, 0089+1 and 008C+2. None covers NetCDS.
The guest's DOSMGR query explicitly delegates the separate CDS table's
instancing to DOSMGR. The special NT NetCDS is outside that table as well.
This is the inherited NT DOS / retail DOSMGR instance-data boundary, not a
missing command completion wait. No guest table/code is patched to hide it.

Run20 changes only the directory fixture to separate C: directories under the
user's Temp, adds explicit DOS default-drive selection, and retains every
capture/directory/register assertion. C (index2) is within the actual table.
It passes six immediate completions with ordinary guest yielding, distinct
VA/VB directories and normal Windows shutdown. Run21 reuses that sealed host
package with paired asynchronous reverse completion; all six returns,
320 callback identity/stack/register checks and normal Windows exit pass.
Both keep the original installation byte-identical. These controls establish
that original CDS instancing and the new command waiting path work for this
local CDS scope. They do not make O:'s failure a pass or prove other redirected
filesystem semantics. The O: failure remains a reproducible negative witness.

The immutable-guest limitation is recorded under the standing source-policy
disposition authority in TODO. S5 must still copy the actual calling guest's
directory at admission, never infer a desired directory or substitute host
process CWD. Independent host request snapshots remain required even where
original guest state itself is shared. Actual native launch, real stream
ownership, production BOP admission and full I/O are still unconnected.

## Screen snapshot candidate and loader control (runs 22--29)

The candidate driver obtains text geometry, cells and cursor through nested
guest BIOS INT 10 calls, restores the saved client state/cursor, and supplies
an owned snapshot to the diagnostic command consumer. It is not yet a live
VM I/O provider or production command admission.

Runs 22--25 fail before the strengthened DOS witness's entry marker. Run23
also fails with the older directory-only driver/package; the screen-reading
driver alone therefore does not explain the failure. Run26 restores the exact
2052-byte run21 witness and the older package and passes. Run27 combines that
frozen witness with the new screen driver and diagnostic consumer: six 80x25
snapshots, reversed paired completion and normal Windows exit pass. Its
explicit unseeded control does not prove distinct cell content between VMs.

Run28 uses the same frozen witness, only padded at EOF to 2244 bytes, and
fails before the first command callback. Run29 repeats that control with an
interposed lease translation diagnostic: it still fails, but records no
failed xtrn2phy preflight (trace operations 0x300/0x301). The proposed
accessed/dirty-preflight explanation is consequently unproved and has not
been applied as a repair. The original SAS bios_write_byte fallback and its
page-state semantics remain unchanged. Investigate loading before attributing
the size-sensitive failure to new witness instructions or screen handling.

Evidence is under build/t479-s5-command: continuation22--29 stdout/stderr,
process and exit records, checked-continuation*/continuation-trace.txt and
original.json. Run27 exits 0; runs28/29 exit 1. The original installation
identity check passes; these are isolated candidates, not published images.
Frozen witness SHA-256:
B7799C65A19B89306865154C12D7F837DD20FCF23FE3CE7BCCC76B3BA7D9861E.
Padded witness SHA-256:
3C376DF7AEBACD12FF1BED9970EF48AFB12996AFE883800BBF91C835321022B3.
Screen-driver SHA-256:
339DA0BBE4C743791BBA2E5AB02456202141062BF7CF6E2803EEEDC47463E3FB.
Win3.1 remains the sole current runtime priority; Win95 work stays deferred.

### Loading failure resolved: generated PIF memory allowance

Run30's diagnostic compilation did not run the guest: its copied SoftPC
compile flags omitted DEM's WIN_32/DEVL context. Run31 instead uses the actual
DEM Ninja rule, and interposes only demRead. Both VM loads request 0x830 bytes
and return exactly 0x830 with CF clear before retirement. No preflight failure
is recorded. Original msproc.asm Exec_Read_Com/OkRead explicitly rejects a COM
image which fills the available read extent; this is insufficient load space,
not lost bytes or a new BIOS-instruction failure.

The generated PIF has zero STDPIF maxmem/minmem and W386PIF30 maxmem/minmem.
The test now sets those documented fields to maximum 640 / minimum 128 KiB
and recomputes the standard checksum. Only newly generated fixture PIFs are
changed; product PIF defaults and original installations are untouched.
Run32 reuses run31's package and the exact failing padded image, changing only
the fixture memory allowance: six continuations and normal Windows exit pass.
Run33 uses the strengthened, freshly assembled witness with the same allowance:
both VM-specific A/B cells with attribute 1Eh, cursor (2,3), six screen captures,
three BOP routes per VM, reverse completion, full register checks and normal
Windows exit pass. Both runs terminate with exit 0. The diagnostic completion
checks snapshot content again after both guests have submitted, rather than
merely trusting successful BIOS reads. These remain diagnostic commands with
AL=25h, not real host execution or the full I/O provider.

Run31 remains the negative control. The transient memory-control runner used
for runs32/33 is frozen under build/t479-s5-command before making the proven
fixture correction the checked-in runner's default. No speculative paging
repair was made. Subsequent production integration still must satisfy the
original S5 launch/ownership/retirement requirements.

## In-progress guest-application handoff

The continuation now owns at most one copied I/O job with a monotonic ticket
and a borrowed acknowledgement event. Posting uses the existing exact-VM
wake/IRQ path; it creates no new cross-process transport. CPU-side extent
query and copy do not acknowledge application. An explicit matching VM/token/
ticket acknowledgement releases the off-CPU waiter; finish returns its error
or an explicit cancellation. Retirement wakes a pending waiter and returns
aborted. Terminal command completion rejects an unfinished I/O job. No wait,
guest pointer or RPC crosses the ICA lock. The executor reference owns the
event/job lifetime until the sole waiter finishes.

Cached x86 worker and three focused suites pass in io-build2 (exit 0). The
first io-build stopped in the strict unit compile on a missing HANDLE include
and MSVC's flexible-array warning; explicit Windows declaration and header-plus-
payload allocation resolved those, without disabling warnings. Tests cover
copy ownership, early-copy versus application, exact identity, stale ticket,
duplicate acknowledgement, guest error, cancellation, real host-thread wait
and retirement/reset. This is host handoff evidence only: driver job draining,
actual guest application and a production VM I/O consumer remain to connect.
No S5 publication or closure is claimed.

### Actual Win3.1 text-application bridge (runs34/35)

The command wait now consumes I/O and terminal wakes separately. The existing
BOP/IRQ path copies an exact ticketed job into temporary VMM heap; the resumed
calling VM applies text through nested BIOS INT10, preserving the complete
client state, then acknowledges. Terminal consumption cannot bypass that I/O
acknowledgement. The IRQ handler still only wakes VMM semaphores, never invokes
DOS/BIOS. No host wait or heap pointer crosses into CPU execution.

Run34 builds the driver and diagnostic host from the current source, using the
new I/O handoff rather than a write to a global VGA alias. Each VM initially
writes its own uppercase A/B and cursor (2,3). The host queues lowercase a/b,
attribute 2Fh and cursor (4,5), waits for the actual application acknowledgement,
then completes the command. The DOS witness reads back character, attribute
and cursor in its own VM after each of three returns. Both VMs, reversed
completion, 320 identity/register/stack/flags callbacks and normal Windows
shutdown pass; runner exit 0 and original installation hashes unchanged.

Run35 adds ownership of each temporary snapshot/application allocation to its
actual VMM control block. Normal release clears that pointer before HeapFree;
Destroy_VM also releases a still-owned allocation if its callback can no
longer return. The same six application/readback checks and normal exit pass
against the reused sealed run34 host package. Forced destruction during BIOS
application is not yet tested; normal exit does not prove that boundary.

These runs prove guest application for unchanged text geometry and text modes
0--3/7, not graphics, resize, full 47-operation routing, atomic visible redraw,
input or actual native child execution. BOP13/14 are currently dispatched by
the diagnostic host only. The production path remains unconnected and no S5
runtime delivery is claimed. Current producer/consumer sources are
test/integration/ntvdm64-386/host-continuation.c and dos-identity.asm, using
the production continuation owner and project-authored driver; evidence is
checked-continuation34/35, continuation34/35 process/stdout/stderr/exit records,
and apply-driver/apply-owned-driver build manifests under build/t479-s5-command.

Run36 reuses run35's exact driver and run34's host package, without deferred
pairing. Each independent completion runs on its own host callback thread;
all six guest application/readback and continuation checks, both live VM
identities and normal shutdown pass (exit 0). Thus the producer does not rely
on paired reverse ordering or wait on the CPU thread. This still does not
prove native program execution. Documentation governance passes in
application-governance (exit 0); git diff --check passes.

### Guest text encoding and model (run37)

Run37 completed with exit 0. The copied BIOS snapshot now includes the actual
DOS active code page (INT21/6601 BX) and BIOS font height (INT10/1130 CX),
using a 36-byte header. The diagnostic host uses frontend/vm_text rather than
editing snapshot bytes directly. Both live DOS VMs passed the six text
application/readback checks, reversed completion, 320 callback checks and
normal Windows exit. Original installation bytes remain unchanged. Evidence:
build/t479-s5-command/continuation37.{stdout,stderr,exit,process.json},
checked-continuation37 and continuation37-package. This is still diagnostic
dispatch, not actual native child execution or a published S5 package.

The formal frontend unit target mvdm-vm-text-test covers owned snapshot data,
snapshot/publication exclusion, exact candidate bytes, failed guest application
rollback, successful commit, invalid rectangles and unsupported operations.
The candidate rejects host-default code-page aliases and unmappable Unicode
instead of silently choosing the host OEM page or best-fit replacement.
DBCS remains explicitly unsupported by this partial model, not a disposition
of the complete I/O requirement. The model remains outside the product link
until its real provider is composed; CMake selects its formal unit target.
Cached x86 compilation with /W4 /WX and CTest passed all 31 assertions
(text-unit2, exit 0; LastTest.log reports 0.15 seconds). This is a focused
model test, not full product regression or native-command acceptance.

Standard-stream trace for the next integration: original COMMAND tcode.asm
do32BitPrompt obtains the three values through INT21/5303 and passes them at
SS:BP. Its Do16BitPrompt/d16_run path instead passes three FFFFFFFF sentinels,
meaning retain the selected default streams. Original host cmdexec.c performs
that fallback before CreateProcess. The asynchronous integration must preserve
both paths and duplicate resolved resources while valid; the diagnostic
11111111/22222222/33333333 values are not usable launch handles.

### Production application transport candidate

The bounded descriptor transfer previously in host-continuation.c is now owned
by softpc/mvdm_vm_application.c and selected by the real worker graph.
mvdm_vm_notification.c routes operations 13/14 only after its existing signature
and protected-ring checks. The integration interposer no longer implements
those operations: it reaches the production dispatcher. The same continuation
token, actual VM, exact application ticket and guest-memory copy routines remain
authoritative; no second queue or transport was added. Operations 11/12 and
actual native execution are still diagnostic-only and remain S5 gaps.

The expanded notification unit exercises those production cases, including
non-ring-0 and wrong-VM rejection, extent query, invalid target memory, exact
copy, lack of premature acknowledgement and explicit application failure.
Run38 uses the sealed run37 driver with the rebuilt current host, isolated
installation vm479bg and package continuation38-package. It completed with
exit 0: two real DOS VMs, six text application/readback cases, reverse
completion, 320 callback checks and normal Windows exit pass; original
installation bytes are unchanged. The cached production build and four unit
targets also passed (416 notification, 31 memory, 72 capture, 31 text assertions).
The indexed diagnostic JSON retains run37/38 process, input hash and matrix
records. These passes prove this application transport, not native execution.

Further standard-stream source attribution: original DOS misc.asm $SETDPB /
query_con implements INT21/5303 by resolving the DOS SFT and returning its
sf_NTHandle, rejecting DOS devices. COMMAND consequently passes the default
sentinel for those devices. Current worker console_client.c classifies its
CONIN$/CONOUT$ identity through CompareObjectHandles, including duplicates;
the identity is not a real Console buffer. command_launch_prepare already
copies that role while the session is bound and duplicates the resource.
The VM execution owner must invoke preparation in that bound epoch, then use
the VM's authenticated launch capabilities; async threads must not rediscover
the role through ambient session state or change the upstream frontend root.

### Production completion consumption candidate

Operation 12 now reaches the production notification dispatcher, using the
existing continuation owner. Its successful result maps completion.error,
exit_code and reentry to the driver's existing status/value/serial_low ABI.
Unfinished, wrong-VM, non-ring-0 and duplicate consumption remain errors;
failed consumption returns no stale exit or reentry value. The diagnostic
interposer only admits operation 11 and schedules its diagnostic executor;
it no longer implements operations 12--14. Run39 is the bounded real Win3.1
check for this change, with the unchanged run37 driver and a rebuilt host.
This still does not connect real native command admission/execution.
Run39 completed with exit 0: two actual VMs, all three service cases per VM,
text readback, reverse completion, 320 callback checks, normal Windows exit
and unchanged original installation pass. Cached worker build and the four
units pass (427 notification, 31 memory, 72 capture and 31 text assertions).
Exact run39 inputs/process/matrix are retained in the diagnostic JSON.

### Input observation prerequisite

Rechecked the original Microsoft VxD DDK chapter 21 and the pinned MS-DOS 6
VMM.INC (SHA-256 28BD6EDABA10434DFCAD26BFCDAB8DBD1A3CC28100CD6910C67397D8C05F92E9):
Hook_V86_Int_Chain is ordinal 41h and can observe reflected hardware interrupts;
returning carry set chains the original interrupt. The current command wait
uses ECX=0, while VMM.INC defines Block_Svc_Ints and Block_Enable_Ints. This
does not alone prove whether input can be serviced during that wait. The old
3.0 manual reserves Wait_Semaphore; it is not evidence overriding our actual
3.1 wait/wake tests or the later header contract.

Added a test-only static interrupt-observer.asm and optional isolated runner
attachment. It reports reflected vectors 8/9 with actual VM and CB status,
preserves registers/flags and always chains. It never reads the keyboard port,
consumes input or performs EOI. The diagnostic host alone recognizes its
operation 16; the production bridge still rejects it. No observer runtime pass
is claimed yet. Use it to calibrate reflected IRQ activity before selecting any
input consumer or changing production wait flags. VKD_Get_Msg_Key is for the
special message-mode queue and cannot stand in for normal DOS input.

Runs40--42 calibrated the observer with actual reflected vector 8 and normal
Win3.1 exit; original installations remain unchanged. Run40 had no deliberately
extended command interval. Run41 held both diagnostic completions for 500ms;
run42 reused that exact host package and observer, changing only the copied
test driver's command Wait_Semaphore ECX from 0 to 5. Both pass all six command
continuations, text readback and 320 callback checks. Production flags remain 0.

Run41 recorded one timer callback per DOS VM between admission and first
result query, but each CB status was 4002h, without the 400h blocked bit. Run42
recorded no timer callback in that interval. Neither run proves reflected
interrupt delivery while blocked; admission precedes entering the semaphore
and result query follows application work. No keyboard event was generated,
and no vector 9 was recorded. Do not promote ECX=5 based on these results.
The next discriminator must be a calibrated keyboard stimulus with explicit
wait-entry/exit observations, then actual input ownership and consumption.
Structured counts, trace hashes and run inputs are retained in the diagnostic
JSON. The modified run42 driver exists only in the task build directory.

Runs43--46 are failed keyboard-calibration attempts, not product acceptance.
The Win16 witness finds the exact VM identity A/B titles. Run45 records target
and active HWND both 150Ch; thus window-name lookup and activation succeeded.
The guest USER keybd_event calls still produced no observed vector 9 and neither
DOS BIOS read completed. Run46 adds OpenIcon before activation because the PIFs
are launched through WIN.INI load; it also fails the normal-exit assertion.
These results do not establish that guest USER synthesis reaches the enhanced
mode VKD hardware-input route, nor that semaphore flags are incorrect. Preserve
production wait flags and distinguish failed stimulus calibration from an input
ownership defect. The structured evidence retains all four failed matrices and
trace hashes. Win3.1 remains the exclusive implementation priority; Win95 work
is deferred until that path works.

Run47 introduces a separate test-only observer API that asks the diagnostic
host to call original nt_key_down_action/nt_key_up_action under ICA. The normal
key-code conversion, controller, PIC, VKD and BIOS remain in the path. This
early stimulus still failed: operation17 preceded the six command admissions.
Run48 reuses that exact host/observer package and waits for each DOS witness's
RDY file immediately before its BIOS read. It passes: A receives 1E61h, B
receives 3062h, with two reflected IRQ1 callbacks per VM, all at CB status
4002h. Both VMs pass six continuations total and 320 callbacks; Windows exits
normally and original installation hashes match. This proves calibrated
controller-to-BIOS delivery after readiness, not delivery while blocked or
input to an actual native child. No production wait flag or guest binary was
changed. The next input comparison must reuse this working stimulus and
observe the specific command-wait interval, not reuse the failed USER route.

Runs49--51 establish the missing wait boundary. In BlockedKeyboard mode A
publishes its live VM handle before command admission. The probe API enumerates
live VMM handles and verifies VMStat_Blocked before sending a physical-controller
make/break pair. A test-only event prevents host completion before this stimulus;
the existing 500ms observation interval follows it. Trace18 records blocked
status 4402h; trace19 is before any host I/O/completion wake. Run49 lacked19 and
is not sufficient for the timing claim. With flags0, run50 records both target
IRQ1 callbacks after19. With flags5, run51 records both before19. Both pass the
exact BIOS keys, six continuations, 320 callbacks, normal Windows shutdown and
original-installation hashes. Neither observation claims CB remains marked
blocked while VMM temporarily services the interrupt.

The candidate command wait now selects the original VMM.INC Block_Svc_Ints
and Block_Enable_Ints flags (1|4). It still resumes command work only on counted
semaphore wakes; the ordinary S4 wait API is unchanged. This is project add-on
code, no mirror or original guest diff. Native child input ownership remains
unconnected. The structured comparison retains exact packages, driver hashes,
trace ordering and negative flags0 control; it is not a complete S5 release.
The updated tracked source rebuild in interrupt-serving-driver52 produces
SHA-256 75BD594575FBDBCADDF46609E353BFF92D037203A8C58A3FA34B02E00C3088CD,
byte-identical to the run51 tested candidate. No duplicate runtime run is
required for that exact artifact under the owner's hash-reuse rule.

The application adapter now owns publication plus event-driven confirmation
and cancellation. The borrowed channel-stop, worker, retirement and broker
handles are deduplicated; an already signalled lifecycle handle prevents a
new publication. Otherwise the owned job waits on its exact acknowledgement
and those handles, without polling, a timeout or a second transport. Existing
finish_io decides whether actual acknowledgement won; cancellation cannot
manufacture success. This helper runs off CCPU/ICA and requires the caller's
executor reference throughout. The diagnostic executor now uses it rather
than its own unconditional wait sequence.

Cached x86 worker build and four focused tests pass: 574 notification, 31
guest-memory, 72 capture and 31 text assertions. New tests cancel an already
posted job through each of the four event slots, check actual application
failure, successful acknowledgement, duplicate handles, pre-cancellation and
invalid handles. These use unit PIC boundaries and are not real NTSRV-death
acceptance. The first build exposed a test signedness warning; the first run
also exposed the unit fixture's missing EOI between publications. Both are
fixed, and failed logs remain retained rather than labelled passing.

Run54 uses the rebuilt diagnostic host and byte-identical interrupt-serving
driver52. Two real guest VMs pass six command continuations, text application,
320 callbacks, precise BIOS keys and normal shutdown; original installation
hashes match. Two target IRQ1 callbacks occur before the first host wake.
Real native command admission/execution and complete frontend input dispatch
remain unconnected; this is a candidate output boundary, not S5 completion.

S5 supersedes S3's launch-capability acquisition order. Preparing the owned
command request no longer requires frontend/execution capabilities. It copies
source data, duplicates the resolved streams and records Console roles while
the original session/handles are valid, before an asynchronous registration or
context RPC. Start borrows the subsequently authenticated pair for its call;
the existing launch path duplicates it into the inherited child attachment.
The request no longer stores a redundant pair of capability handles. Missing
capabilities reject start without creating a child or consuming the request.
This changes no broker authentication, upstream association or ordinary
COMMAND launch path.

Cached worker/launch-fixture builds pass. The established S3 background runner
passes final launch56 capture and launch cases, including two parallel real
children after source closure and late capability acquisition, distinct output,
environment/CWD, one-shot start, installer/path failure and handle balance.
The earlier restricted direct invocation launch55 produced C0000008; the exact
same image passed the established runner. Retain that execution-environment
failure without claiming its precise exception site was traced. Product
acceptance uses the established runner, not that failed invocation. This tests
the launch seam with installer/path substitutes, not a live NTSRV VM chain.

Run57/58 closes two concrete publication-operation gaps identified in
ntvwm_presentation_capture: WINDOW_RECT and ATTRIBUTE are sent on every
publication, not just on a resize. The VM text model accepts its actual
full-grid viewport (or a zero relative adjustment), rejects unsupported
geometry, and keeps a frontend-owned default attribute seeded once from the
guest snapshot. Cursor motion and cell writes do not change that default.
Default changes are staged and commit only after guest acknowledgement;
application failure and explicit abort retain the prior default and grid.
No mirror, original guest or wire format changed. Resizing, non-byte attributes
and the remaining provider operations are not claimed implemented.

The cached x86 unit build passes 60 assertions including publication ordering,
commit/rollback, viewport negatives and default-versus-cell independence.
Run58 uses the established private-desktop runner and actual Win3.1 enhanced
mode/DOSONLY=1, with the diagnostic executor issuing the added operations.
Both DOS VMs pass six continuations, 320 callback/state checks, guest-applied
text, calibrated BIOS keys and normal exit; original installation hashes are
unchanged. These results do not prove production native launch or native input:
the executor still explicitly returns the diagnostic result. S5 remains open,
and this candidate has not been published as a completed product capability.

Runs59--62 examine the original keyboard interception contract. In
keymouse/keybd_io.c, keyboard_int reads port60, invokes INT15/AH=4F with CF
set, and if CF is cleared retains its K26/exit IRQ cleanup without BIOS
queue insertion. INT15 has a direct-default-handler shortcut, so the VMM
hook cannot be assumed reached merely from that source. The test observer
now chains INT15/4F and records actual Client_AL/flags/VM. Run59 proves A
1E/9E and B 30/B0 reach that hook; A reaches it before host command wake.
The existing exact BIOS keys, six continuations and normal shutdown pass.
This is observation, not input consumption or native delivery.

ConsumeKeyboard adds a test-only selected-target receipt and CF-clear branch,
plus a DOS witness that requires no duplicate BIOS key for A while B retains
its normal BIOS key. Runs60--62 fail before any keyboard stimulus: the first
B continuation exits ERROR_INVALID_ADDRESS in mvdm_vm_command_capture. Run62
records valid readable command echo B/CR, environment ROLE=B/double NUL and
all three standard-handle sentinels. It does not yet identify the failed
capture suboperation; readable bytes alone do not prove the write/lease or
other capture steps. Do not attribute this failure to INT15 consumption or
claim the consumption contract passed. The precise failure remains the next
diagnostic action; no product keyboard consumer or native input capability
has been published. All runs verified original installation hashes unchanged.

## Nested directory snapshot and consumed-key witness (runs63--69)

Runs63--66 narrow the prior capture failure to the 64-byte directory read,
with default drive C valid. Both the previous CB_High_Linear-adjusted address
and the raw GVDA allocator address fail non-faulting host translation; all
four read/write, user/supervisor combinations fail for the latter. Command,
environment, handles and client frame remain readable. Thus merely removing
the base addition is not a complete fix and is not retained as an explanation.

The candidate now takes the directory snapshot before End_Nest_Exec. Following
successful DOS AH=47 it calls VMM Map_Flat on Client_DS:Client_ESI, copies the
64 bytes into its callback-owned protected stack, then restores the full nested
client state. Host capture reads this independent stack snapshot. It neither
relaxes original CCPU translation nor retains an instanced guest pointer across
nested execution. The descriptor and its stack allocation are released on all
admission error/success paths. Original Microsoft DDK Map_Flat and GVDA contracts
were consulted at https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/ and
service ordinal 1Ch checked against the pinned MS-DOS 6 VMM.INC. Interface
research only; no original source body, guest or mirror was modified.

Run67 passes all four admissions reached and logs both consumed scan bytes,
but exits with ERROR_SHARING_VIOLATION (32). The diagnostic trace independently
opens a FILE_SHARE_READ append handle on both CPU and completion threads. A
single SRW lock now serializes that test-only open/write/close; temporary capture
return/byte/permission instrumentation is removed from the run68 host link.

Run68 passes with ConsumeKeyboard: actual VM A's 1E/9E reach INT15/4F before
host wake and are accepted once; its final BIOS buffer is empty. VM B still
reads 3062. Run69 passes without consumption and preserves A=1E61, B=3062.
Both runs cover two actual Win3.1 enhanced-mode DOSONLY=1 VMs, six command
continuations, 320 callback state checks, text application and normal Windows
exit. Original installation hashes remain unchanged. The candidate driver is
5039 bytes, SHA256 DF181C21AC1C3C4A30445E92046C44CCF607C4B728F695DA0C1A2ABB7BB1FAAC.
Structured evidence retains each failed and successful run separately.

This closes the bounded capture/input-consumption diagnostic gap, not S5.
The diagnostic executor still returns AL=25h. Production BOP11 admission,
real Run16/NTSRV execution, input queue delivery and terminal lifecycle
composition remain to be connected and tested. No candidate was published,
no original guest was rebuilt, and no Win95 test or implementation was added.

## Guest capture to owned launcher composition (runs70--72)

Source tracing cmdexec.c/cmdCreateProcess and cmdenv.c/cmdXformEnvironment shows
that a captured DOS command/environment cannot be passed unchanged to the ANSI
launcher. The new project-owned mvdm_command_launch.c composes the existing
capture view with original OEM-to-Unicode-to-ANSI RTL conversions and original
cmdXformEnvironment. It does not reimplement environment sorting, COMSPEC
selection or merge policy. The latter returns a bounded ANSI block; only its
actual double-NUL extent is passed to the already tested owned launcher.

Exactly UINT32_MAX preserves the caller-supplied default standard handle,
matching cmdCreateProcess's -1 case. Other values remain original native
handle tokens; the existing prepare API validates direction and duplicates
them before CPU admission ends. Neither process standard handles nor session
singleton capture state is changed. Conversion and transform allocations are
released on all exits; prepared launch data remains independently owned.
No mirror, original guest, wire format or lifecycle policy changed.

Run70 compiled both new sources but the unit link failed because nt.h maps the
status converter to the selected OpenNT symbol. No test passed in that run.
The unit now explicitly delegates that status-table boundary to host NTDLL;
it tests real RTL OEM conversion but substitutes environment/owned-launch
functions. It does not claim runtime coverage of original cmdenv.c. Run71
passes 34 composition assertions (non-ASCII conversion, source immutability,
-1/default selection, error propagation and malformed source/result blocks).
The task wrapper was also corrected to reject negative tool exit codes;
cmd.exe IF ERRORLEVEL 1 alone missed CMake's -1 link failure.

Run72 performs the cached production worker build, compiling the helper into
worker-command-bindings.lib, and passes all five selected suites: notification
574, guest-memory 31, capture 72, guest-launch composition 34, text 60 assertions.
Raw evidence is guest70/71/72.process/stdout/stderr/exit under the S5 temporary
root; structured hashes are retained in the diagnostic evidence JSON.
This preparation helper still needs the production BOP11 executor caller;
no actual Win3.1 host command completion is inferred from this unit result.

## Executor invalidation and actual publication requirements (run73)

A native executor cannot use a pending guest I/O job's acknowledgement event
as its lifetime signal: when it waits on a child process, there may be no I/O
job at all. Each continuation now owns a manual-reset invalidation event from
begin until its last reference is released. Exact VM-generation retirement,
bridge reset and final guest consumption signal it; host completion alone does
not. The executor borrows it only while holding its reference. It can therefore
include invalidation in its wait set and ask the existing NTSRV/frontend owner
to retire the chain. The event does not terminate a task or move lifecycle
policy out of NTSRV. Failed begin closes its newly allocated handle.

Cached production worker compilation and the notification suite pass in run73:
581 assertions. The retirement test's native thread now actually waits on a
duplicated invalidation event, rather than an independently signaled test event;
wrong generation, completion-before-consumption, reset and reference lifetimes
are also checked. This is host lifetime evidence, not a real NTSRV child run.

Call-chain audit of ntvwm_presentation_seed/capture/end confirms that real native
commands require more than WRITE_CELLS: seed requests snapshot, screen/cursor,
optional text configuration and handoff title; every capture sends a text VIDEO
frame (fonts, palette and serial) inside a publication; final release requires
PRESENT_BARRIER for that serial. Returning success while discarding VIDEO or
omitting its guest application is not an admissible implementation. The shared frontend-base video receiver now owns the validated staging
algorithm for reuse when the VM receiver is composed. Geometry/font application
and input records must remain explicit; no dummy success receiver was added.
Production BOP11 admission and executor/provider composition remain open.


## Shared video receiver (runs74--78)

Move NTCON's video receiver to common/frontend-base/video.c and video.h,
retaining every algorithm statement; comparison against HEAD after the sole
include-path substitution passes (line endings normalized for comparison).
The shared archive owns this implementation. NTCON retains display/commit
acknowledgement policy. No mirror or original guest source changes result.
The pure staged-frame test moves to test/common/frontend-base/video_test.c;
NTCON dispatch tests remain with their outer owner. Both production build
recipes and observation-script dependencies follow the shared archive.

Run74/75 failed before compilation due to incorrect CMake target names.
Run76 built x64 frontend but found a newly added Ninja test edge preceding its
link rule. The edge was moved after the existing rule; these failed attempts
remain retained, not passes. Run77 cached x64 frontend/x86 worker and four
receiver/presentation targets pass. Run78 rebuilds channel/service tests and
passes all three frontend-base suites, link ownership including negative
controls, Console dispatcher, text-frame 133 assertions, presentation 526,
and input-return 605 assertions. Console fixtures run with the established
private-desktop observer. Their production activation flag remains false:
these results do not prove guest command execution or end-to-end NTSRV I/O.

Evidence: build/t479-s5-command/video74--78 logs, process records and video78
observer reports. No S5 publication or closure; production BOP11 executor and
VM input/presentation composition remain open. Win95 remains deferred.


## Combined VM publication candidate

The VM text model now composes the shared VIDEO receiver. VIDEO_BEGIN/DATA
are accepted only within its publication and only for matching text geometry;
unsupported graphics/geometry changes remain errors. The single candidate
view contains both the BIOS cell snapshot and the complete owned VIDEO payload,
including palette/fonts/cursor, plus its serial. The application owner must
apply both before calling finish with success. The text-only diagnostic
explicitly rejects a VIDEO-bearing candidate instead of discarding it.

Incomplete or invalidated staging cannot commit cells. Application failure or
publication abort disposes the pending frame and cells while preserving the
last applied serial. PRESENT_BARRIER is an exact applied-serial check after
publication, not a render-thread wake or a new timeout/poll. It remains busy
while a publication is in flight. The original VIDEO validator is reused.
One extra flag retains the publication's VIDEO obligation when malformed DATA
causes the shared receiver to discard its pending allocation.

This is host candidate ownership/commit machinery. It does not yet teach the
guest applicator to install VIDEO fonts/palette, supply native input, or start
the production command executor. Those remain required, not waived. The model
is included in the production VM frontend archive; link selection alone does
not prove a runtime caller. Run79 passes 92 assertions before removing the
redundant text-only accessor. Final-interface build/regression evidence follows.


Run80 cached worker compilation and final single-accessor text model pass 91
assertions (one redundant legacy-accessor assertion removed with that API).
Run81 rebuilds the diagnostic worker against the current shared video archive
and runs the real Win3.1 enhanced DOSONLY=1 fixture: two concurrent DOS VMs,
320 callback identity/stack/register/flags checks, all six 54:08/0A diagnostic
continuations, text application and normal exit pass. Original O:/w31 bytes
are verified unchanged. This fixture sends only BIOS text snapshots, not VIDEO
fonts/palette; it therefore proves that existing guest continuation was not
broken, not that the new full-video guest application is complete. No S5
publication or closure is claimed.


## Copied application packet and BIOS application boundary (run82)

The candidate-to-guest boundary now has one owned serialization helper:
ntvdm_vm_text_copy_application. Text-only jobs retain their exact prior BIOS
header/cell bytes. VIDEO jobs append signature 0x31564d56, version 1, the fixed
1048-byte console_video_description and its complete payload. The fixed trailer
size is compile-time checked at 1056 bytes; total allocation is bounded by the
existing continuation transport maximum. No pointer/handle crosses the wire.
Copying does not commit or acknowledge. Incomplete VIDEO cannot be copied.
Tests prove the copy survives publisher-buffer modification and transaction
rollback, retains fonts/palette/cells exactly, and leaves text-only bytes intact.
Run82 cached worker compilation and 101 model assertions pass.

The existing Win3.1 diagnostic now uses that actual serializer for its text-only
application. It still does not generate VIDEO jobs. The current driver rejects
trailing data because its apply function requires the exact BIOS snapshot
extent: full VIDEO is therefore not yet accepted or silently ignored.

Source trace: selected ega_char_gen AL=00/10 delegates to load_font with
ES:BP, CX, DX, BL and BH; AL=03 selects the font map. Selected vga_set_palette
AL=10 supplies DH/CH/CL to set_dac; EGA palette AL=00 maps an attribute index.
These are emulated BIOS implementations, not permission to bypass the guest
VDD. The add-on must issue BIOS calls in the target VM's Begin_Nest_V86_Exec /
Exec_Int / End_Nest_Exec context and restore the full client state. Its existing
instanced Global_V86_Data_Area and Map_Flat pattern provides the pointer model
for a BIOS font transfer; direct host/global VGA mutation is not selected.
Actual font-map/palette application and its guest VDD behavior still need
implementation and measured acceptance. No claim is made from this source
trace that Win3.1 windowed VDD already accepts every VIDEO style.

Run83 rebuilds the diagnostic worker with the serializer and passes the real
Win3.1 enhanced DOSONLY=1 test: two DOS VMs, 320 callback state checks, six
54:08/0A diagnostic continuations, text application and normal exit. Original
installation bytes remain unchanged. This is text-only transport acceptance;
full VIDEO guest application and production native command execution remain
unconnected. Raw build/process/exit/observer evidence stays in the S5 temporary
root; hashes are retained in the diagnostic evidence JSON. Nothing is published.


## Guest VIDEO candidate and failed font readback (runs84--89)

The independently authored driver now validates the versioned trailer before
entering nested execution: complete extent, matching text geometry, bounded
font height, cursor shape, palette and cell style. It accepts plain pair cells
and triples with zero style. Underline, split cursor, out-of-viewport cursor,
font-height/geometry changes and non-text video return explicit errors; these
are incomplete provider capabilities, not approved reductions of S6.

The candidate expands its existing instanced, paragraph-aligned GVDA scratch
from 64 to 8192 bytes (maximum 256 glyphs x 32 rows), reused for directory and
BIOS font transfer. The callback copies packed font rows through Map_Flat in
the active nested VM, calls BIOS 1100 for banks 0/1 and 1103 for selection,
1000/1010 for attribute/DAC palette, then applies glyph cells/cursor through
the existing VM BIOS path. Eight-bit colors are converted to six-bit VGA DAC
components. Original guest and mirror sources are untouched. Wlink/NASM build84
passes. This is a candidate, not a published general-purpose implementation.

Run85 builds the diagnostic host and passes actual Win3.1 enhanced DOSONLY=1:
two VMs receive different palettes and read the expected DAC values through
BIOS 1015 after all six continuations. Callback state, text/cursor and normal
exit pass. That first test did NOT inspect font RAM.

Run86 adds a rejecting version-2 application before each valid version-1 job,
and a guest font-plane readback that saves/restores every touched sequencer/
graphics register and index latch. Host execution passes the exact error check
for malformed jobs and continues, but both DOS witnesses record three failed
continuation assertions. It is a FAILED run despite normal Windows exit.
Run87 retains exact diagnostic output: VM A stage=font, bank0 first byte=60
(0x3c expected), bank1 first byte=0, height=16, offset=0. This does not prove all
of bank0: the test stops at the first failing bank1 comparison.
Run88 actively selects both banks (attribute_font_select=1), and fails at the
same values; unused-bank selection alone is therefore not an explanation.
Run89 adds a read-only sample of the eight 8-KiB bank regions, retaining the
same assertion. Font mapping/installation root cause remains under diagnosis.
No passing font-application claim, S5 closure, release, or default deployment.

Run89 terminates with a failed runtime matrix: Windows starts, but the witness
never reaches normal exit within its existing budget. Original installation
hashes are unchanged. This broad eight-region diagnostic is removed from the
source probe; its exact compiled COM and logs remain in checked-continuation89.
The strict two-bank readback assertion remains, along with its stage/byte/offset
report. The timeout is not attributed to a product cause without further proof.
Runs86--88 remain failures; no assertion was relaxed to pass the candidate.
Remaining next investigation is the second-bank BIOS load / VM mapping boundary.

## Independent BIOS font controls (runs90--91)

Run90 uses the unchanged driver84 and diagnostic host88, adding an explicit
`-VideoFontControl` to the guest witness. After each continuation, the ordinary
DOS program fills its own buffer and invokes INT10/1100 for banks 0 and 1,
then executes the same strict device readback. This bypasses the driver's
nested font-transfer operation, not the original VM/BIOS. Both witnesses fail
all three font checks: A reports stage2/sample0=60/sample1=0/height16/offset0;
B reports stage2/sample0=102/sample1=0/height16/offset0. Windows starts and exits
normally and the original installation is byte-identical. Overall result FAIL.
Thus the observed failure is not unique to the new driver call site. It does
not yet establish a VDD limitation or rule out an observation/test defect.

Run91 assembles the same control loader/readback routines into a standalone
authored DOS COM, without Windows, and runs it through the established isolated
package/private-desktop observer. Its result is also FAIL, but different:
failure=1, sample0=0, sample1=0, height16, offset14. The preceding offsets passed
both-bank comparisons. This rules out treating the current evidence as proof
of an exclusively Win3.1 VDD problem. Audit the actual BIOS load arguments and
font readback path before changing production behavior or accepting a boundary.

Raw sources, runners, process records, logs, exit statuses and the standalone
result remain in build/t479-s5-command; durable hashes are in the diagnostic
JSON. These tests used isolated owned processes, no computer use, no original
guest changes and no mirror changes. No new product publication or S closure.

## Actual font entry and mapping (runs92--101)

The first build92 launch produced no compiler output and its process was gone;
the established modern-pwsh execution context then ran it. That build failed
because the copied nt_bop command omitted bios.c's target-local MONITOR flag.
Build93 instead reads that translation unit's exact Ninja flags and passes.
Only the diagnostic bios.obj renames entry symbols; original sources remain
unchanged. Run94 repeats the plain-DOS failure but records no AH11 calls through
the BOP10/42 wrappers. This is not evidence that font loading skips original C.

Run95 reads the actual DOS INT10 vector and its bytes: F000:ED80 starts
C4 C4 BE CF (mouse-video BOP followed by IRET). Original mouse_video_io calls
ega_video_io directly for AH11. Build96 adds that real entry observer. Run97
records AX1100/BX0E00 and AX1100/BX0E01, CX0100/DX0000: the ordinary test loaded
14 rows, then compared against 16 returned by the later BIOS1130 query.
Original low_set_mode writes ega_char_height from its mode table, explaining
the changed report. The original test conclusion at row14 was therefore not a
valid proof of lost requested rows. Run98 retains the actual requested height
and passes both banks, all 256 glyphs and all 14 requested rows. It does not
claim that BIOS geometry preservation passed, or resolve the Win3.1 failure.

Run99 records the unchanged Win3.1 candidate's true calls: AX1100/BX1000 and
BX1001, CX0100/DX0000, ES:BP0700:0000, followed by AX1103/BX0004. The font test
still fails at bank1 offset0, with normal Windows exit. Thus bank number,
height, count and transfer address reach the BIOS as intended.

Build100 uses the exact ega_vide Ninja flags and interposes c_sas_store only
in that diagnostic object. The observer performs the original store, then
records the first byte of each bank, xtrn2phy's read mapping and SAS readback.
Run101 fails the same strict font assertion and exits Windows normally. Its
repeated observations show A0000 mapping to per-VM RAM (0032C000 or003D0000),
with 66h or3Ch correctly read back; A4000 maps to000C3000 in both VMs and reads
zero immediately after writing the expected value. This establishes loss at
the write/mapping boundary, before subsequent display/VM retirement. It does
not prove why VDD chose that mapping or authorize a global physical VGA write.
The source ROM layout places C0000 upward in SAS_ROM, but the exact VDD policy
and supported font-map transition still require verification. No guest changes,
no mirror diff, no assertion relaxation, no production release or S5 closure.

Run sources, compiled probes, precise link scripts, PID/start/exit/logs and
traces are retained in the task temporary root and hashed in diagnostic JSON.
The diagnostic worker is distinct from the current published S4 baseline.

### Win3.1 priority: run102/103 permission control

The owner's latest direction keeps Win3.1 first and defers Win95. Build102
adds only read-only diagnostic observations to the existing font-store link
interposition. Run103 repeats the unchanged strict Win3.1 VIDEO witness using
the same candidate84 driver and a fresh isolated installation vm479cr.
The build exits zero; the font assertion still fails and Windows exits normally.

For each failed A4000 write the actual CPU reports CPL3 and VM=1. Both strict
supervisor-write (access5) and strict user-write (access7) xtrn2phy queries
succeed with physical C3000; immediate readback remains zero. Thus simply
changing the BIOS helper's access-request privilege bit would not reject this
observed mapping. No speculative CCPU change was made. This does not prove
that all page-protection behavior is correct or establish who chose C3000.

Read-only comparative VDD source was pinned to vbesvga.drv revision
622f38512555a42ed324af1aa62adfefbccd8f2a (VDDVBE directory), with per-file hashes
and original URLs in build/t479-s5-command/vdd-reference/provenance.json.
It is a modern derivative, not the exact installed V7VDD and not a build,
runtime or acceptance dependency. VDDINT's font-load notification and
VDDVMMEM's allocation/null-page paths supply investigation candidates only.
Their existence does not establish which path the installed V7VDD executes.
The remaining proof is the actual V7VDD mapping decision and the supported
font transition; no physical-memory bypass or guest alteration is admitted.

S5 production execution remains open. This diagnostic is neither native
command execution acceptance nor a reason to claim the S6 full-I/O contract
passed. No S5 publication, commit or closure accompanies run103.
### Actual guest stores and nested font candidates (runs104--109)

Run104 requests fFullScreen (PIF bit3, 2100Ah instead of21002h) in the same
isolated matrix. It does not exit Windows normally and is a failed control;
no conclusion about actual foreground ownership or full-screen support follows.

Run105 instead leaves the PIF windowed and enables the independent DOS MOV
control. After selecting virtual VGA planes, the probe writes each expected
byte to both fonts through ordinary guest instructions, then separately reads
all256x16 rows of both banks. Both actual VMs pass, with final offset8175,
samples60/60 and102/102, all320 callback-state assertions, all six diagnostic
continuations and normal Windows exit. It establishes this environment can
retain both fonts through that ordinary guest sequence. It is explicitly not
acceptance of the driver's earlier font application or real native execution.

A project-authored V86 helper was then tried inside the add-on's saved nested
execution. ABI uses original VMM Simulate_Far_Call48h and Resume_Exec85h,
verified against the already admitted ms-dos-6 VMM.INC and Microsoft's
Virtual Device Adaptation Guide nested-execution contract at
https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/ . No original guest
code or mirror is changed. The helper saves/restores touched VGA registers
and accesses normal virtual ports/pages, never host physical video memory.

Build106/run107 still fails strict readback: bank1 is correct up to the first
4KiB boundary, then zero at offset4096; Windows exits normally. Build108/run109
tries a single interleaved two-bank application with complete32-row storage,
using16384 bytes plus code in instanced GVDA. Run109 does not complete normal
Windows exit; the diagnostic host reports WRITE_FAULT, which currently merges
publication and model-finalization failures. This candidate is NOT accepted
or released. The next diagnostic preserves the exact underlying error. These
failures mean the original BIOS path cannot yet be declared the sole cause.

The pinned comparative VDDPHMEM.ASM was also read at revision
622f38512555a42ed324af1aa62adfefbccd8f2a, SHA256
817984D2E239F367D72CECEF8A8E966AC3E618F65DEE08DEFAD76CFCB0C2EB13.
It remains a modern derivative/reference only, not installed-V7VDD proof.
### Candidate rejection and bounded continuation (runs110--115)

Build110 preserves separate publication and model-commit errors instead of
collapsing them to WRITE_FAULT. Run111 proves the16KiB candidate's VM retires
before publication finishes with995/ERROR_OPERATION_ABORTED. Build112 adds
GC read-plane2 to match the successful DOS control; run113 has the same
retirement/cancellation result. Build114 returns to the smaller8KiB/per-bank
candidate with that read-plane setting; run115 exits Windows normally but
still fails bank1 at4096. Therefore read-plane selection alone is not a fix,
and the larger transfer is not a safe replacement.

All authored helper changes from runs106--115 were removed. The source was
reconstructed from the retained candidate84 NASM listing, required contiguous
source-line identity, and matched candidate84's recorded source SHA256 before
restoring src/addon/ntvdm64-386/ntvdm64.asm:
B5B01DF64AFDF5D1CEC8032CBC79D8B2DDA19278B889F176A6C0880E539965CA.
No unrelated driver changes were lost. Current driver source is exactly the
pre-experiment candidate, not the failed larger transfer. Original guest and
mirrors remain unchanged. The useful retained delta is the reproducible DOS
MOV control and precise error observations, not a speculative runtime patch.

The finite S5 production gaps are command admission/executor lifetime,
authenticated VM frontend binding, and a real channel provider completing
ordinary command I/O before continuation. The existing stage plan assigns the
complete font/graphics/input/Console and nesting contract to S6. The font
failure remains an explicit S6 requirement, not an accepted semantic reduction
or a passing S5 full-I/O claim. Avoid further unbounded font experiments before
connecting and exercising the S5 real-command chain. Win95 remains deferred.

### Win3.1 publication commit boundary (run116)

Added `mvdm_vm_application_commit` to the existing NTVDM application owner.
It copies the exact staged frame, waits through the existing continuation job
for actual guest acknowledgement, then commits or rolls back the text/video
model. No new transport, timeout, guest pointer or lifecycle authority is added.
The diagnostic caller now uses that boundary rather than separately committing
its model after publication. The malformed-version negative case remains.

Owned background run `build/t479-s5-command/owned-commit116.ps1`, PID 24192,
started 2026-11-06 05:34:24 local, exited 0. Cached MSVC x86 build and CTest
`^mvdm-vm-(notification|text)$` passed: 623 notification/application assertions
and 101 text-model assertions. The application cases now verify committed
cursor state after each of four cancellation sources, successful guest ack,
and rejected guest write, plus admission of the next publication after rollback.
These are host unit boundary tests, not real PIC or guest execution proof.
No new guest integration run or product publication occurred. Real native
launch/provider wiring and font application remain incomplete; S5 stays open.


### Off-CPU command execution owner (run117)

`mvdm_vm_command_execute` composes the existing authenticated VM launch-context
and prepared-launch owners. It borrows their lifetimes, closes both temporary
capabilities and the returned process/thread handles, and waits off CCPU for
Run16's terminal result or the continuation's invalidation. On invalidation
it asks the existing VM frontend registry/NTSRV retirement path to close the
chain. It never calls TerminateProcess, invents a timeout, writes guest state,
or acknowledges a continuation itself. A failed retirement remains an error;
already-completed concurrent retirement is distinguished by ERROR_NOT_FOUND.

Source-first disposition: original `dos/command/cmdexec.c::cmdCreateProcess`
waits for its child and reads the exit code. Its complete original translation
unit remains selected for the ordinary DOS path. Its global dwExitCode32,
Exe32ActiveCount, fBlock and process-wide cmdExec32 wait/reentry state cannot
be used for independent VMM VM continuations without serializing the payload.
The existing S3 capture/launch adapters retain the command/encoding/stream
contract; this S5 composition retains the signaled-process/result ordering
while binding cancellation to the actual continuation. No mirror intrusion
is needed for this helper. The new VM scheduling/lifecycle composition is the
already-admitted independent add-on/host bridge, not a replacement for the
ordinary original DOS command execution path.

The helper still needs the production BOP admission/provider owner. Its mere
presence in the build is not runtime integration or S5 completion evidence.


Run117 (owned PID 20760, 2026-11-06 05:39:53 local) exited 1 on
/WX C4389 in the test's expected-error comparison. Corrected the DWORD
comparison, without changing runtime behavior. Run118 (owned PID 26132,
05:41:31) reused the same cached target and exited 0: 81 executor assertions
passed in 0.47 seconds. Cases cover an already-invalidated VM, context denial,
invalidation during context acquisition, failed launch, ordinary completion
(including terminal code 259), invalidation after creation, failed retirement,
and concurrent retirement. Real fixture subprocesses wait/exit; broker and
launcher calls are substituted, explicitly not a live NTSRV VM-chain proof.
`git diff --check` passes. No guest/mirror changes or new publication.


### Owned callback snapshot composition (runs119--120)

Moved the addon callback descriptor/screen capture into
`mvdm_vm_command_snapshot`, beside the existing original COMMAND argument
capture composition. The five-word descriptor remains unchanged. The CPU
thread copies it, bounds and leases the screen, creates the owned text model,
releases the lease, then captures COMMAND data. Every failure returns both
outputs NULL and destroys a partially created model. The text owner now owns
screen geometry/codepage validation; the diagnostic no longer duplicates it
or retains a separate copied screen allocation. Its seed/cursor assertions
remain via the same production model read operations.

Run119, PID 27268, started 2026-11-06 05:45:48 local and exited 0. Cached
MSVC x86 builds and four CTest cases pass: notification/application 623,
executor 81, capture 92, text 101 assertions. New cases prove guest mutation
after capture cannot change the model, malformed extents and addresses fail,
and a later command-capture failure frees the already-created screen model.
Run120, PID 22672, started 05:47:19 and exited 0: the updated Win3.1 diagnostic
translation unit compiles with /W4 /WX. This is compile evidence only; no new
real guest diagnostic or end-to-end product run occurred.

No mirror/guest modifications, publication or closure. Production BOP11
admission, VM frontend/provider control lifetime and real input/video wiring
are still open; the new snapshot API has a diagnostic caller, not a completed
production entrypoint. Continue with that composition rather than treating
these unit passes as sufficient S5 acceptance.


### Wire publication and Win3.1 composition regression (runs121--124)

Added serving-thread `mvdm_vm_application_dispatch`: PUBLICATION_END uses the
exact guest-acknowledged commit boundary, operation failures populate the wire
reply without pretending a transport success means application success, and
unsupported operations remain explicit errors. Added text-channel disconnect
cleanup that discards uncommitted screen/video state and snapshot ownership
while retaining the last acknowledged model. These still require the complete
VM provider owner before production registration.

Run121 exited 1 on /WX C4389 in a test's uint32_t result comparison; normalized
the comparison to Boolean and reran. Run122 (PID 14728, 2026-11-06 05:50:25)
exited 0: cached four-case regression passed 629 notification/application,
81 executor, 92 capture and 108 text assertions. Run123 (PID 5188, 05:51:25)
exited 0: actual NTVDM worker cached build and diagnostic link succeeded.
Existing original-source/generated-code warnings remain; new helper units
compiled. No original-source changes were made by this group.

Run124 (PID 38420, 05:52:12) used the established medium-token private-desktop
runner, a fresh package and O:/tmp/vm479cz copied installation, unchanged
driver84, and Keyboard/BlockedKeyboard/Continuation/DeferredContinuation/
LocalDirectoryControl/TraceContinuation. It passed Win3.1 enhanced DOSONLY=1
startup/normal exit; two concurrent actual DOS VMs; 320 callback identity,
stack, register and flags checks; both 54:08 branches and 54:0A; original
O:/w31 byte identity. The diagnostic now uses the owned snapshot and wire
publication entrypoints. It still returns diagnostic AL=25h, not real native
command completion. VIDEO/font application was not selected and remains an
open failed contract, not a skipped success or semantic concession. No default
package publication, P commit or S5 closure is claimed.


### Native seed text-configuration contract (run125)

Source trace of `ntvwm_presentation_seed/read_configuration` confirms that
READ_TEXT_CONFIGURATION is mandatory even when a frontend has not yet
published a font. Added this operation to the VM text owner: initial absence
returns the protocol's ERROR_NOT_FOUND; an acknowledged text frame supplies
the exact style and first sixteen palette entries in bounded chunks with its
published serial. Stale revisions fail with ERROR_RETRY; invalid offsets and
missing buffers fail explicitly; an open publication is busy. The record is
derived from committed storage and introduces no separately maintained font
state. This does not capture the initial guest fonts or fix their application.

Run125 (owned PID 18680, 2026-11-06 05:56:20 local) exited 0. Cached x86 build
and all four selected tests pass: 629 notification/application, 81 executor,
92 capture, 122 text assertions. The new text cases read the complete record
using the native worker's revision/offset sequence and compare every byte,
then reject stale and malformed requests. No real NTVWM seed/command or new
guest run was executed in this batch. GET_HANDOFF_TITLE/input mode/provider
registration and production command admission remain open, not successful
fallbacks. No publication, commit or S5 closure is claimed.


### VM provider instance and input ownership (runs126--127)

Added `mvdm_vm_io`, composing the existing frontend callback ABI and actual
application dispatcher. Each instance owns its readiness event, bounded key
FIFO and copied ANSI handoff title; it borrows the model/continuation until
its registry callbacks and input producers have joined. It performs no RPC,
process termination, waits under the input lock, or guest-pointer retention.
Translated KEY_EVENT_RECORD input is copied into the existing wire shape.
Queue-full/retired errors mean not accepted, allowing the guest producer to
retain its event. Peek does not consume; normal reads remove exact FIFO
records and reset readiness only under the queue lock. Channel finish aborts
uncommitted screen state while retaining pending keys across channel handoff.

Native raw input mode and handoff title requests are implemented. Cooked modes,
receipt-bearing input return, mouse production and other unsupported operations
remain errors. This is a partial provider implementation, not a claim of S6
input semantics. Actual guest scan/character translation and provider
registration/control lifetime still require connection. The borrowed
continuation must outlive registry callbacks, even after its native command
has returned; releasing the executor reference early would be invalid.

Runs126 (PID 12568, 2026-11-06 06:02:37) and127 (PID 34788, 06:04:11, after
removing an empty error branch) exited 0. Cached four-test regressions pass:
943 notification/application/provider, 81 executor, 92 capture, 122 text
assertions. Tests cover copied titles, separate input queues/readiness,
peek/read, rejected returnable reads retaining keys, capacity rejection,
channel handoff preserving keys, native mode selection and cooked-mode refusal.
Existing publication cancellation/guest-ack cases now traverse the provider
callback itself and the real application/continuation code. No actual NTSRV
registration or new Win3.1 run was performed for this new provider yet.

Next integration boundary is a real registered VM frontend and native launch,
using this provider and S3 launch preparation, rather than another diagnostic
AL=25h completion. Its remaining operations must fail visibly; do not turn
unimplemented title publication, font application or input conversion into
successful stubs. Production BOP admission remains disabled/unconnected until
its lifetime and provider prerequisites are proved. No publication or closure.

## Real native chain integration (runs128--134)

The isolated `RealCommand` witness now composes the actual captured command,
VM frontend registration, NTSRV launch capabilities and Run16/NTVWM execution.
It asks CMD to return 37 for A and 38 for B through both 54:08 forms. This mode
does not test interactive 54:0A, graphics/fonts or the production BOP admission.
The ordinary three-continuation diagnostic remains available separately.

Cached build128 passed. Run129 exited 1: Win3.1 exited normally, but the actual
native execution returned 50 instead of the requested status. Build130 added
a diagnostic-only pass-through provider trace; run131 repeated the failure.
Its operation trace proves snapshot begin, screen/cursor/cells, handoff title,
snapshot end and native input mode all succeeded. The first fatal operation
was `CONSOLE_IO_PUBLISH_TITLE_A` (42), returning ERROR_NOT_SUPPORTED. Initial
READ_TEXT_CONFIGURATION returned ERROR_NOT_FOUND, which NTVWM already permits.
No command-success claim is made for either run; originals remained unchanged.

The provider now copies validated published titles into its channel-owned
handoff state, so subsequent carriers can read the actual caption rather than
the initial seed. Invalid/unterminated input leaves that state unchanged and
VM providers remain isolated. This does not yet update the Win3.1 window's
visible caption: guest caption rendering remains an unproved S6 boundary.
Run132 passed all four selected suites including those title ownership cases.
Build133/run134 are the next actual-chain verification, not a release gate.

Build133 exited 0. Run134 (PID 23904) exited 0: enhanced mode/DOSONLY1 normal
exit, distinct live VM identities, 320 callback checks, and actual 54:08 AH=0
and AH=1 execution in both VMs returned 37/38 as requested. Original installation
bytes remained unchanged. All provider publication and final frame barrier
operations completed. This proves actual execution/continuation, not the font
contents (the earlier font-readback failure remains open). Stdout SHA256 is
91184C5FEB2418BBFFC87D69CF807E158D13B7C3E9C78B896E7390EC0265C5E8;
operation trace SHA256 is
09CAAED87730A1F277EF91FFE5FA80B8A719BF2A25D20B1C600523544FAAFE94.

The next witness adds 54:0A with the approved interactive Run16 route. Its input is a
deterministic translated-key fixture fed through the real provider queue;
it is not guest physical-key translation evidence. No exit code substitution
or host UI automation is used. Complete guest input and visible captions remain
S6 requirements; production admission is still unconnected. No S5 release.

Build135 passed; run136 failed at actual unused-input return, operation21
PREPEND_KEYS, after publications completed. The fixture does not replace the
approved T443 S12 mapping of interactive service to `run16 command` with a new
COMSPEC interpretation. That route remains unchanged. Its pending native key
events must return through the VM frontend rather than fail at handoff.

The provider now validates the complete untagged key batch, reserves queue
capacity under the input lock, prepends it in FIFO order and signals readiness.
Malformed or oversized batches do not partly modify the queue. Receipt-bearing
input is still rejected because that separate ownership contract is not yet
implemented. Run137 passes all four focused suites, including returned-key
ordering, invalid second-record rollback and queue readiness. This does not
yet feed the returned keys back to the guest keyboard on final VM continuation.

Build138 passed. Run139 confirms PREPEND_KEYS and the final barrier now succeed,
but 54:0A returns 1, so the overall probe still fails. Source reconciliation
found a fixture defect: its guest environment contained only ROLE, while the
original cmdXformEnvironment deliberately imports only host COMSPEC and drive
directories, not host PATH. Consequently `run16 command` could not resolve the
packaged COMMAND.COM and reached the native shell fallback. The fixture now
supplies its own package PATH in the actual guest environment. This does not
alter the original transform or product search/routing. Its interactive input
is plain `exit`, expecting normal status 0, rather than CMD-specific `exit 37`.
Distinct 37/38 assertions remain on both real 54:08 cases. Build140/run141
verify the corrected fixture; neither failed predecessor counts as a pass.

Build140 exited 0; run141 exited 1. Both 54:08 variants still return distinct
37/38 correctly. The now-resolved interactive DOS route reaches its VM provider
and fails PREPARE_TEXT_REGION (46) and BARRIER (11), followed by launcher status
1067. Win3.1 itself exits normally and original bytes remain unchanged. Thus
the previous status1 was not the actual interactive contract. Do not accept
either error as an expected completion or substitute a native shell.

Next bounded implementation must cover the DOS carrier's actual preparation,
stream/cell publication, modes/codepage, input ownership and final flush as a
call-chain group, including the existing S6 full-I/O obligations. Do not chase
one unsupported request per probe or declare a successful no-op. The S5
entry/continuation path stays unpublished until its interactive prerequisite
works. Visible captions, exact guest fonts, keyboard translation and final
returned-key delivery remain explicitly unverified. Win95 remains deferred.

## Actual interactive chain: runs142--146

The grouped provider correction distinguishes explicit NTVWM publications from
implicit DOS cell/cursor/VIDEO transactions. Only a complete candidate posts
its exact guest application job; partial VIDEO remains private and rejects
interleaving. Actual guest acknowledgement commits the model, including VIDEO
cells/cursor, or failure rolls it back. A final barrier rejects pending work.
Exact current text geometry is supported; geometry changes remain unsupported.
Codepage comes from the captured guest state. OEM cell operations avoid an
unnecessary Unicode conversion. Output mode zero advertises no cooked/VT
processing. Returned copied guest keys use the existing zero-receipt form;
receipt-bearing batches remain rejected, not silently discarded.

Channel-local VIDEO serials reset on disconnect. The independent configuration
revision and committed font/cell state survive carrier handoff, so a new
carrier can start at serial1 without acknowledging an old channel's frame.
Tests cover explicit/implicit/chunked publication, cancellation, rollback,
interleaving, returned input and serial restart. Runs142/143 and final144 pass;
144 records 1382 notification/provider, 81 executor, 92 capture and 139 text
assertions. Build145 exits0. These focused suites are not full product gates.

Run146 exits0 with two actual Win3.1 VMs: both 54:08 forms return38/37 in each
pair; 54:0A through `run16 command` returns0 for both. All six execution and
retirement results are zero-error. 320 callback identity/stack/register/flags
checks and normal enhanced-mode DOSONLY1 exit pass. Original installation bytes
remain unchanged. Exact log/trace hashes and owned PIDs are in the diagnostic
JSON's realCommandWin31_142_146 record.

The interactive input is a deterministic `exit` key sequence through the real
provider queue, not physical guest-keyboard translation. Production BOP
admission remains unconnected. Exact font contents, visible guest captions,
mode changes, final returned-key delivery and complete guest I/O remain open;
this success does not supersede their earlier failures or prove S6. No S5
release, full-gate completion, commit or push is claimed. Continue with the
production entry/lifetime integration on Win3.1; Win95 remains deferred.

## Production executor ownership: runs147--150

`mvdm_vm_executor` now owns independently prepared command/text/provider jobs.
Submission copies no guest pointer and makes no RPC or wait; only a successful
threadpool admission exposes the continuation token. Failed admission abandons
that exact unexposed request and leaves launch/text with the caller. Concurrent
commands have separate VM registrations and execution contexts. Input selects
an exact request token. A second unfinished request for the same VM is rejected.

The worker creates this executor alongside its frontend registry. Authorized
worker shutdown first forbids admission, invalidates continuations, and joins
all command producers while the host registry still accepts NTSRV retirement.
Only then may it close/destroy the registry. No executor lock spans ICA, RPC or
native waits. Jobs leave input lookup before completion becomes visible, but
an independent active count retains the shutdown join until their last resource
and continuation reference is released. Unrecoverable retirement/publication
failure fails the process rather than freeing borrowed callbacks or reporting
success; those fatal boundaries are not fault-injection-tested here.

The diagnostic REAL_COMMAND path no longer owns its own payload, execution,
provider callbacks or completion thread. It submits to the worker's production
executor; only BOP admission/capture observation and translated `exit` stimulus
remain diagnostic. Fixed-result continuation probes remain separate.

Run147 failed a test-only /WX shadowed-name warning. Run148 corrects it and
passes all four suites (1511 notification/provider/executor assertions, 81
standalone execution, 92 capture, 139 text). New cases cover invalid VM, duplicate
admission, independent request keys, close rejecting submission/input, two
cancelled commands, the exact retirement/join barrier, caller-owned failure
data, normal completion and no outstanding token after cancellation.

Build149 exits0. Run150 exits0: actual enhanced-mode DOSONLY1 Win3.1, two VMs,
320 callback checks, both real54:08 forms and real54:0A, normal exit and unchanged
original installation. The production executor now carries this chain; the
ordinary BOP ingress is still not connected. Provider-stimulus input does not
prove physical guest input. Font application and complete I/O remain open.
Exact PIDs/log hashes are in productionExecutorWin31_147_150. No new mirror or
original guest changes occurred in this group. No S5 release/closure is claimed.

The next ingress group must register the addon's allocated FAR callback only
through the protected bridge, clear it on shutdown, select only54:08/0A in
actual software-V86 mode, and bypass MS_bop_4's original post-dispatch increment
only when CALLF has already installed the callback. BOP11 must independently
capture the actual VMM client descriptor and submit through this executor;
no marker register or probe-only callback address may select production work.
The ordinary non-bridge dispatcher remains unchanged.

## Production BOP ingress: runs151--152

The addon registers its VMM-allocated FAR callback with signed ring-zero
operation15 after notification initialization. Shutdown clears it. The small
MS_bop_4 seam (MVDM-HOST-DIV-871) selects only08/0A in actual software-V86 mode
with a registered callback, advances the original 16-bit returnIP once and
calls original CCPU CALLF. A handled entry bypasses the old +1 epilogue.
Absent bridge, other services and non-V86 callers retain original dispatch.
There is no diagnostic marker or caller-provided callback address in selection.

Production operation11 copies the VMM client descriptor and reads the original
service byte at restored CS:(IP-1) through the calling VM's high-linear mapping.
It captures independent arguments/text, prepares owned launch resources and
submits to the worker executor; every pre-submit failure retains/cleans the
correct owner and exposes no token. No RPC or process wait occurs on CCPU.
The remaining diagnostic wrapper supplies only observations and translated
interactive keys. REAL_COMMAND reaching its old test-only entry is now fatal,
so that fallback cannot falsely pass the production-ingress witness.

Build151 exits0, including five test suites:1511 notification/provider/executor,
81 execution,64 ingress selection/ownership,97 independent capture and139 text
assertions. The ingress unit covers no-driver/non-V86/other-service fallback,
registration failure/duplicate/reset,16-bit IP wrap, and capture/preparation/
admission cleanup. Capture tests distinguish VM mapping from ambient memory,
wrap the service offset, reject malformed service/extent and failed lease
release. Original CCPU behavior is proved separately by the real guest witness,
not by its mocked unit CALLF.

Run152 exits0: enhanced-mode DOSONLY1 Win3.1, two live DOS VMs,320 callback
identity/stack/register/flags checks, both actual54:08 forms returning37/38,
and actual54:0A returning0. Windows exits normally; original installation
bytes remain unchanged. Both addon and worker contain the production ingress.
Original COMMAND's complete interactive behavior is not inferred from these
guest-authored BOP witnesses; physical keyboard, fonts and complete I/O remain
open. Full product gates and S5 publication/commit/push remain outstanding.
All exact output, driver and trace hashes/PIDs are in productionIngressWin31_151_152.
Win95 work remains deferred.

## Full build and generation reuse: runs153--157

Complete cached build153 exposed a second, older Ninja consumer of the expanded
command_capture.c fixture. Its include/object closure was stale although the
CMake test passed. The test now uses the repository-qualified header and that
Ninja target links the actual command/location/text/video objects. Run155 passes
complete x86/x64 product targets plus14 shared tests per architecture; no old
images are substituted. This is build evidence, not the Full runtime gate.

The executor review found same-number VM reuse was still rejected while the
old generation's frontend was retiring. Run156 reproduces this with the real
executor, continuation, provider and threadpool; broker/launch boundaries are
mocked. A held retirement gate proves the old request is still alive. The new
generation submit fails at line670. That failure is retained, not waived.

The executor now compares immutable continuation generation as well as VM ID.
Same-generation duplication is still rejected. A new generation is admitted
without blocking CCPU, and its host thread waits on the existing condition
variable until older requests for that numeric VM finish frontend retirement.
Independent VM handles do not wait. After that barrier, invalidation is checked
before any new frontend registration. Old keys/tokens cannot select the new
provider. No new transport, polling, timeout or guest modification was added.

Run157 passes all four affected suites:1588 notification/provider/executor,
81 execution,97 capture and139 text assertions. The reuse case verifies no new
registration/execution before old retirement, distinct tokens, stale-key
rejection, new-key acceptance, old-token absence, normal new completion and
complete resource release. This does not substitute for real VMM reuse with a
native command in flight. Existing S4 real reuse and this host test prove their
respective boundaries, not that combined path.

Owned pipeline158 (PID20508) now builds the affected normal/diagnostic worker,
then runs real Win3.1 continuation159 and the established focused plusFull
product gates into r154-verification/r154-package. Its outcome is pending;
inspect the live owner and individual exit records before proceeding. No S5
publication, commit or closure is claimed.

## Win3.1 recheck159 and independent delivery gates160

Chain158 and continuation159 exit0. The actual Win3.1 witness again passes
both command services in two VMs,320 callback checks, normal exit and original
installation byte verification. Pipeline158 subsequently fails in release154:
the preceding fixture removed USERPROFILE from the shared PowerShell process,
so ccache rejected fixture compilation. This is retained as a failed run, not
a runtime pass. Independent owned release160 (PID1720) repeats focused and
Full gates in a fresh process. Its final outcome is still pending. Win95 is
deferred per owner priority; no S5 publication or closure is claimed.

## Completed gates160 and real command retirement161

Release160 exits0: focused tests and Full selected product gates pass with an
identical ten-image package. The external DOS4GW fixture is explicitly skipped,
not passed. No S5 publication has taken place.

Run161 uses the normal production worker, production BOP ingress and driver151.
VM A launches actual host CMD, which writes NATIVE.OK before blocking in pause.
VM B waits for that host-created file before requesting the existing test VxD
to retire blocked A. The late wake is rejected; the remaining VM and Win3.1
exit normally. Both reports and READY content pass, original O:/w31 hashes
remain unchanged. This is actual command-in-flight VM retirement evidence.
It does not prove native-process absence before fixture cleanup, physical keys,
full guest I/O or actual handle reuse during native execution. Those remaining
proof boundaries are not replaced by this result. Exact evidence hashes reside
in win31AndDelivery_158_161 in the diagnostic JSON.

## Real retirement and generation reuse163

Run162 failed before guest launch because a single NASM define was splatted as
characters. The fixture now retains an actual array. Run163 exits0 with the
normal production worker: native CMD starts and publishes READY, its blocked
VM is destroyed, late wake is rejected, then VMM reuses an actual previous VM
handle with a new generation. The new VM rejects both old wake forms, consumes
a valid completion and executes a second actual CMD returning37 before normal
Windows exit. Original installation hashes remain unchanged. Report error0 is
only written after checking that exit code. This closes real command execution
after actual reuse, but does not force overlap with unfinished old frontend
retirement; that ordering currently has host-unit evidence. Native process
absence before the outer fixture cleanup remains unproved. No S5 publication
or closure is implied. Exact hashes are in commandRetirementReuse_162_163.

## Pre-cleanup native retirement164

Run164 repeats actual native execution, forced VM destruction and new-generation
command execution successfully. The runtime matrix now optionally snapshots
CMD command lines before Stop-IsolatedPackageScope. No CMD containing this
fixture's unique NATIVE.OK path remains. The observer normal-return path does
not terminate its child and the runner does not enable MVDM_OBSERVER_OWN_JOB.
The fixture environment whitelist also excludes that variable. Thus the scope
cleanup did not manufacture this absence. This is pre-cleanup process-inventory
evidence, not a process-handle join proof. Both generation reports, normal
Windows exit and original installation hashes pass. Production images are
unchanged from release160; no product rebuild is needed for this test change.
Two actual commands in deliberately selected completion orders remain an S5
proof item; complete guest I/O remains S6. Win95 is deferred.

## Controlled native completion order165--167

The independent ordered-command.c fixture builds with MSVC /W4 /WX. It uses
per-fixture/per-round named events and one PID mapping. The first process
waits for the second to open its process handle; the second waits for actual
first-process exit and verifies its terminal code. No sleep, product polling
or runtime test hook is added. Guest ROLE and ROUND environment select each
independent participant; both original54:08 forms retain full callback checks.
Run166 exits0 with A before B, both VM results37/38,54:0A result0 from fixture
input,320 state checks, normal Windows exit and unchanged original installation.
Run167 (PID24052) is the reverse-order case; its outcome is pending. This test
controls native process termination order, not guest callback scheduling order.
No product images changed since the successful release160 gates.

Run167 subsequently exits0: B before A passes both54:08 forms,54:0A fixture
input,320 callback-state checks and normal Win3.1 exit. Original guest bytes
remain unchanged. Both controlled native orders now have actual Win3.1 proof.
Hash-bound evidence is nativeOrder165_167. Guest callback scheduling is not
forced; physical keyboard and complete I/O remain the S6 obligation.
