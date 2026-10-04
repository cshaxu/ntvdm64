# T428 S3 authenticated parent text-I/O resume

Owner approved moving run16's Console-member routing decision into existing
NTSRV relationships. Baseline is S2 6a8934f6b, its tested/published system32
package and APP0.0.427/RPC38/I/O25. Other-session planning is preserved.

## Source audit and implementation

Original `src/opennt-host/base/win32/server/srvvdm.c` owns DOS records,
completion events and exit codes. The original `GetNextVDMCommand`/
`RETURN_ON_NO_COMMAND` result remains the DOS resume decision; the existing
project seam in NTSRV `base_service.c` authorizes I/O only after that result.
No original mirror execution, scheduler, block/resume or cleanup is moved or
modified. These original units cannot route the project's separate native
worker/frontend components: the smallest seam is their existing service-owned
authenticated context plus existing zero-payload native I/O resume admission.
No original translation-unit replacement, external intrusion, new protocol,
helper, registry or scheduling policy is introduced.

The prior context keyed only root/Console, so a native and DOS origin could
alias; run16 compensated using physical Console PID enumeration and native
worker selection. The existing Console context now also identifies its
origin worker by registered generation. Native command delivery mints the
context for its admitted worker; NTVDM's existing context acquisition records
itself. Binding copies this authenticated origin into the existing launcher
connection, not a caller-selected PID or ancestry observation.

`service_acquire_console_context()` shares the old context lookup/create/
duplicate/rollback logic, now keyed by root/Console/origin. It runs under the
existing service lock. Resource ownership and root-rundown deletion are
unchanged. Stored generations are identities, not pointers or retained tasks.

After exact DOS receipt consumption, run16 sends the existing resume request
without enumerating members or selecting a worker. NTSRV checks its own
completion flag and authenticated origin: DOS origin uses original resume
unchanged; live native origin must belong to this root and retain active native
work. Only that registered parent is selected for the existing I/O admission
and startup acknowledgment. No payload means no new native task/receipt.
Successful resume consumes the completion flag; premature, foreign, duplicate,
missing or failed origins cannot grant an unrelated worker ownership.

Final DOS publication/input return still precedes original completion. Native
reacquisition still passes the existing route release/close barrier,
GetNextNativeCommand, `begin_io()` and startup result. Outer root Console
restoration still follows its independent return/restored acknowledgment.
Run16's two local membership checks for temporary Console cleanup/WOW detach
remain; they do not decide parent routing.

## Verification record

All new evidence is below `build/M0-T428/S3`; incremental x86 `/MT` CCPU40
cache is `build/M0-T427/S2/r001`. r001 build/rebuild logs retain affected
launcher, service and dependent frontend links. No copied DTO/IDL/I/O format
changed, so protocol versions are unchanged. Candidate runtime is r003.

| Contract | Exact entry/evidence | Result |
| --- | --- | --- |
| Origin and completion/negative checks | `--parent-resume-origin` in production-linked service fixture; r006 | Passed, including exact public resume consumption and duplicate rejection, wrong root, failed/missing origin, native selection and DOS no-op. Real pinned fixture processes; no global RPC endpoint. |
| Retained service behavior | `tests/observation/verify-service-fixtures.ps1`, r006 | 23 cases passed, 4442ms. Native-command capability aliasing still asserted against worker-issued origin; old launcher-origin expectation corrected, not dropped. |
| Product matrix | `Invoke-ProductVerification.ps1 -Suite Product`, r004 | Passed Console17 (66956ms), Window17 (75194ms), three retained WOW frontiers (67399ms); total 215377ms. |
| Native-to-DOS-to-native I/O | `verify-broker-io-handoff.ps1 -Case nested-console` and `-Case nested-window`, r012 | Both passed: fresh DOS banner, real MEM output, executed parent-return marker and exit23; Window additionally verifies CAF. |
| Coherent publication | r008 `publish.ps1`, tested r004 manifest, r008 recovery/published manifests | Eight files published to O:/winnt/system32; hashes equal r003 candidate. Guest media/configuration untouched. |
| Published ordinary smoke | r009 `published-smoke.ps1`; O:/winnt/Logs2/t428-s3-published-* | Empty, native-zero, MEM, EDIT passed in both Console and Window. Subsequent explicit eight-row manifest iteration confirmed all hashes. |

r002's native-command assertion failed because it compared delivery with the
old launcher-issued locator; r005/r006 keep the object-alias and restricted
access assertions with the correct worker-issued locator. The first new
origin fixture attempt used duplicate process registrations, correctly
rejected; it now uses two real private suspended child processes.
These failed attempts are retained and not counted passed.

The nested I/O probe now resolves current system32 versus sealed flat layouts
through the existing test-only binary-root helper and validates exact physical
and Z: alias cleanup identities. It retains actual DOS/MEM/parent output,
exit23, CAF, package hashes and input-delivery assertions.

r007/r010/r011 probe attempts failed and are retained: the old probe assumed
flat guest files and package-root working directory; after correcting system32
paths, its fixed-delay first snapshot preceded the DOS banner. r012 uses the
existing observer's full-echo/fresh-prompt consumption milestones, retaining
all output and result assertions, rather than accepting submission as input
consumption. No product behavior or observer implementation was changed.

The r009 wrapper's final hash loop initially treated the parsed JSON array as
one row under Windows PowerShell and failed after all eight smoke cases had
passed. The manifest was assigned to a variable and iterated explicitly;
that separate read-only check passed all eight hashes. This is not a failed
product case or a retry-to-pass claim. Raw case reports are retained.
Governance, relative links, diff checks and original-mirror no-change checks
passed. Final review retains original DOS result/cleanup ownership, service
lock boundaries and the existing native startup/I/O barrier.

## Remaining boundary

Common management shutdown, exclusive native residency and root-worker
association/reuse consolidation remain later T428 stages. Physical RDP/focus
is owner-waived/unobserved, not passed. No host scrollback guarantee or
descendant observation is added. S3's runtime/publication gates are complete;
the reviewed P is recorded in CURRENT. This does not claim whole-T closure.
