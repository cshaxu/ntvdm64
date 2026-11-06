# T478 S27: DLC completion-event failure check

Parent: 811f07baa. Finding 59 is an original bug in both pinned source families.
DIV-881 changes only the second CreateEvent check in InitializeDlcWorkerThread
from DlcWorkerEvent to DlcWorkerCompletionEvent. The original valid-first-event
CloseHandle and FALSE return remain. No provider, protocol or recovery layer is
introduced.

## Focused evidence

The exact initializer is extracted by test/integration/verify-dlc-worker-event.ps1
and compiled with x86 MSVC. Event handles are real Win32 auto-reset events;
CreateThread is a controlled boundary that records its arguments and never runs
the actual DLC worker. First-event failure, second-event failure, thread failure
and successful setup are exercised. Success additionally checks initial event
state and completion auto-reset behavior. All fixture handles are retired.
Baseline has 35 checks and five failures: it attempts thread creation without
the second event and reports success. Current has 33 checks and zero failures;
the difference is the intentionally skipped thread callback on event failure.
The function differs from both originals only by the checked identifier.

## Original lifecycle limits

VrDlcInit first initializes buffer pools and the event-handler thread, then the
DLC worker. If the later worker setup fails, it reports NO_MEMORY but has no
rollback for the already-running event-handler thread. A later initialization
attempt may repeat this original setup. This is retained as an original partial
initialization/retry limitation; this correction does not claim transactional
rollback or make a stopped thread out of a live one. InitializeEventHandler's
own cleanup can call CloseHandle(NULL) after a failed event creation; its valid
previous events are still visited. No arbitrary invalid-handle tolerance is
added. The worker's own successful event/thread handles are process-lived.

DLC is an owner-approved modern-host capability exclusion recorded in the
T420 S20 operation matrix. Absence of SysWOW64/dlcapi.dll is current local
inventory evidence, not a proof against every possible externally supplied
provider. The selected loader returns COMMAND_CANCELLED_FAILURE before this
initializer when its provider is unavailable. No DLC provider is installed and
no actual adapter traffic is claimed. A controlled initializer test does not
establish end-to-end DLC support.

## Delivery

Focused receipts are in build/t478-s27. Product build, frozen-package verification,
Full gates and exact worktree assets/release publication remain required before
closure. No O:/winnt writes, guest/media changes or push are authorized.

The final package is build/t478-s27/package-final2, identity
c08075be-80dc-453c-b178-b0377ff9bb14. The earlier package-final was staged while
build completion was still pending and is not the acceptance package. Coherent
build has now exited zero. A relocation-masked comparison binds the complete
686-byte selected InitializeAdapterSupport function (including inlined worker
setup) to the final DLL at file offset 27328; a standalone initializer symbol
is not required to survive optimization. This is static evidence only.

The initial fresh DOS boundary probe exited before producing output with
launcher status 0x6BE and image identity unavailable. It overlapped the Full
run. No root cause is established and no concurrency explanation is claimed
as proven. The observation harness originally checked only result=exited;
it now also requires launcher exit zero. The failed attempt is retained at
build/t478-s27/guest-dlc and is not counted as DLC execution or acceptance.

Final Full passes 16/16 gates in 384050 ms. DOS4GW/NESticle is owner-excluded,
skipped rather than passed. Ten images plus manifest were published to this
worktree assets/release and hash-verified against package-final2.

The sequential same-package replay exits zero and prints DLC-REAL-RESULT= 07.
The original probe embeds NUL before its status; the console renderer displays
that character as a space. The new matcher initially rejected that valid output;
it now accepts whitespace before the exact 07. Existing replay artifacts prove
the result without rerunning product code after this parser-only correction.
This confirms the original unavailable-provider boundary, not live DLC support.
The first 0x6BE launch remains cause-undetermined. Replay details are retained
in guest-dlc-replay; the committed dlc-s27-results.json records their hashes.
