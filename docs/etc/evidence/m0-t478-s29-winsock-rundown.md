# T478 S29: Winsock task rundown closure

Parent: 5a075005f. Product repair, verification and worktree-only publication.

The pre-S29 project-added WWS32CancelAsyncRequestsForTask detached all task
records, calls WSACancelAsyncRequest outside the list lock and frees native
buffers regardless of the return. Explicit guest cancellation already frees
only on provider success; it is not the same path.

Microsoft's WSACancelAsyncRequest contract distinguishes successful cancellation
from errors. WSAEALREADY/WSAEINVAL mean no operation remains in progress; other
errors (including WSAEINPROGRESS) do not establish this. Source:
https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacancelasyncrequest

The exact task-rundown body is extracted and compiled by
verify-wow-winsock-rundown.ps1. A controlled provider accepts or rejects cancel;
allocations are quarantined so premature release is recorded without inducing
an actual use-after-free. Baseline has ten checks and two failures when cancel
is rejected with WSAEINPROGRESS and provider ownership remains. Other-task
preservation and successful-cancel release pass. This is a controlled contract
counterexample, not a reproduced native DNS race. Inputs and output are at
build/t478-s29/baseline.

Candidate repair stays in the existing request list: mark retired-task records,
suppress their guest conversion/post, retain native write destinations while
cancellation is unresolved, and release only on provider completion or known
terminal cancellation. A cancellation/completion handshake must prevent freeing
a context while the cancel caller still owns it, including reentrant completion.
All six constructors must initialize the state; all three completion categories
must share retirement handling. Do not introduce another request registry,
callback worker, retry or handle-wrap redesign. If a failed provider never
signals completion, retained native ownership must be recorded as a limitation,
not hidden behind a timeout free.

The following sections record the implemented repair and its verification.
No O:/winnt publication or push is authorized.

## Implemented ownership repair

DIV-882 initializes retirement state in all six original constructors and
uses the existing list/critical section. Rundown marks every task record before
calling the provider outside the lock. A pending-cancel bit prevents a reentrant
completion from freeing the caller's context; that completion instead transfers
release responsibility back to the cancel caller. Other retired records can
complete and retire independently. Successful cancellation, WSAEALREADY and
WSAEINVAL are terminal; other errors retain the native destination until a
completion. Repeated rundown does not retry. All three original completion
categories use the same lookup and suppress conversion/post for retired tasks.

Explicit guest cancellation remains unchanged. Task cleanup clears its TLS
before rundown, so the already-existing initialized-thread check rejects a
new explicit guest cancellation after retirement. Ordinary live completion
still removes and returns the original context for conversion and delivery.
No generic cross-thread guest-conversion guarantee is added: completion that
already claimed a live context retains the existing WOW execution contract.

The original source has no replacement task-retirement implementation to
restore: this corrects T467's project-added cleanup, retaining its original
provider/list/converter owners. No guest input or source package is imported.
If cancellation is nonterminal and the provider never delivers completion,
the native allocation remains until process teardown. This is an explicit
bounded ownership limitation, not a new timeout/free or retry mechanism.

## Focused evidence

`test/integration/verify-wow-winsock-rundown.ps1` extracts the exact current
context declaration and two changed bodies. `focused2/run.txt` reports
323 checks, zero failures: six cancellation results with and without reentrant
completion; sibling completion during cancellation; other-task preservation;
repeated rundown; late/stale completion; ordinary live completion. Buffer and
context release counters detect early/double release using quarantined
allocations. These are controlled provider cases, not native DNS fault injection.

The first updated fixture compile failed because its release-spy function name
collided with a production local variable. Renaming only that test spy resolved
it; `focused/build.txt` retains the failure. A restricted initial launcher
returned PID 28428 but created no test output or surviving process; the reused
desktop-capable hidden pwsh path ran PID 20736 (compile failure), then 25764
(passing test). This launch failure is not a product result.

Coherent x86/x64 build PID 16288 completed with exit 0. Selected static-image
verification identifies the full 260-byte rundown and 152-byte completion
lookup bodies (relocations masked) in package
`7434fc0b-10e5-4081-a37f-b33031436b9e`. This is selection evidence, separate
from execution. The retained observer is
`artifacts/mvdm-audit/shard-1/winsock-s29-observer.py`.

Sequential real-guest/Full supervisor PID 24820 uses the existing private-
desktop runner, immutable Winsock guest DLL and independent localhost NE
probe; no external peer or O:/winnt write is involved. Logs and exact hashes
remain under `build/t478-s29`. Final gate and publication results follow below.

Earlier S23 RPC 1817, S24 concurrent Registry writer exit 33 and S27 startup
0x6BE diagnostics remain outside this fix; passing later runs do not establish
their root causes. DOS4GW/NESticle remain owner-excluded, not passing claims.

## Final verification and delivery

The exact package above passed all 16 Full gates in 374,881 ms. Real Win16
localhost completion delivered exactly once and freed its procedure; explicit
cancel returned success with zero completion; task loss reached phase 3 with
zero errors and no guest completion. All three existing runners returned zero.
The ten tested images and manifest were published to this worktree's
assets/release with SHA-256 equality; previous files are preserved in
build/t478-s29/release-before. Version 0.0.478 / protocol 67 remain unchanged.
No O:/winnt publication and no push occurred.

The durable [result receipt](../../../artifacts/mvdm-audit/shard-1/winsock-s29-results.json)
contains source/object/image identities, fixture results, real guest records,
all Full gate timings and the publication result. S29 resolves finding 65's
project-introduced premature native-buffer release within the stated provider
contract; unbounded provider failure and original handle wrap are not claimed
fixed. The broader T478 inventory reconciliation remains open.
