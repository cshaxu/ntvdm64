# T480 S4 registration and evidence reconciliation

## Contract and result

Owner authorizes S4 through closure after admission b754ee0ea: reconcile the
registrations and evidence affected by S2/S3, without repeating completed work,
changing guest or adding capability. This is a documentation/evidence delivery.
No product source, test fixture, build definition, release image, guest source
or immutable S1/S2/S3 receipt is modified. No new build/runtime pass is claimed.

The finite union is39 host C/H files:30 S2 files plus13 S3 files, with4 overlaps.
All current source hashes match their latest accepted stage. Both sealed
verification archives match their committed SHA-256 and entry count. The819
S2 consumer receipts preserve exact sets, command identities, emitted text and
logical locations; all6 final Base environment captures identify the final LF
source. The11 current release image hashes still match the tested S3 manifest.

## Corrected and retained claims

| Claim | Disposition | Current interpretation and evidence |
| --- | --- | --- |
| DIV-582 CPU40 selector account | Narrowed by successor | S3 restores only the C boundary before DpmiFaultHandler to original `#ifndef i386`. Actual command selection and object equivalence passed. Remaining header/carrier and CPU40 stack publication registrations stay. |
| DIV-869 compiler validation pending | Superseded for the measured restoration | Preserve the T476 snapshot, but add the S3 consumer/COFF result. This does not claim complete DPMI semantic validation. |
| OPENNT-HOST-014 environment subset | Retained, structure claim reconciled | Three original function structures are restored. Current project adapter path is ntvdm-exe/win32/mvdm_base_vdm_environment.c; old ntvdm/win32 path is historical. |
| OPENNT-HOST-065 pointer width | Retained | Original DWORD pointer truncation was not restored; native-width subtraction remains in the final source and six consumer captures. |
| RTL1970 implementation | Superseded | Original constant/macro/body replace local literals. The selected subset, existing NTDLL multiply declaration/provider and unsupported exclusions remain. OPENNT-HOST-005 still describes declarations, not this implementation. |
| S2-only whitespace registrations | Retained | Original whitespace recovery neither changes executable tokens nor retires behavioral bindings. Historical source hashes are S2-specific. |
| S3 expression/spelling registrations | Narrowed only for accepted fragments | Three expression and ten spelling/comment recoveries do not invalidate whole-file registrations. Scope follows S3 receipts, not registry filename matches. |
| S2 verifier on current checkout | Clarified | Its30 final-source checks intentionally bind b35effafd. Four later S3 edits cause expected mismatches on a current tree; do not weaken those assertions. |
| S3 environment intermediate hash | Corrected attribution | vdm-restoration.json is intermediate CRLF; line-ending-correction plus final-x86/x64 and environment-final-test identify delivered LF bytes. Immutable receipts remain unchanged. |
| First S3 Full failure | Retained non-pass | Scripted second run16 command lost opening characters; final unchanged test and12 immediate pairs passed. Intermittent cause remains unproved. |
|3,641 conservative S3 retentions | Retained limitation | Neither source registration, reachability nor runtime success proves every retained edit necessary or globally minimal. |

No additional product correction was established by this reconciliation.
Broader autonomous-implementation minimization remains the separate queued
proposal; S5 aggregate review is not admitted by this closure.

## Complete affected-file accounting

[Machine-readable ledger](../../../artifacts/mvdm-audit/t480-s4-reconciliation.json.gz)
contains each exact current/S2 hash, actual consumer list, inline registration
IDs and current registry line/text references. Filename matches provide context,
not semantic evidence; no missing registration is invented. align.h has no
filename hit in its registry, but its S2 source identity and compiler consumers
are independently checked. All rows retain existing behavioral registrations
except the bounded successor clarifications above.

| Host source | Stages | Disposition |
| --- | --- | --- |
| src/mvdm-host/dos/command/cmdenv.c | S2 | S2-whitespace-only |
| src/mvdm-host/dos/command/cmdexec.c | S2, S3 | proved-expression-or-spelling |
| src/mvdm-host/dos/command/cmdredir.c | S2 | S2-whitespace-only |
| src/mvdm-host/dos/dem/demfcb.c | S3 | proved-expression-or-spelling |
| src/mvdm-host/dpmi32/dpmi32.c | S2 | S2-whitespace-only |
| src/mvdm-host/dpmi32/dpmiint.c | S2, S3 | proved-expression-or-spelling |
| src/mvdm-host/dpmi32/dpmiselr.c | S3 | proved-expression-or-spelling |
| src/mvdm-host/softpc.new/base/bios/tape_io.c | S3 | proved-expression-or-spelling |
| src/mvdm-host/softpc.new/base/ccpu386/c_main.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/ccpu386/fpu.c | S3 | proved-expression-or-spelling |
| src/mvdm-host/softpc.new/base/comms/rs232_io.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/inc/xt.h | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/keymouse/keyba.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/keymouse/keybd_io.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/keymouse/mouse_io.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/system/ica.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/base/system/qevnt.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/inc/host_cpu.h | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/inc/insignia.h | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/inc/nt_inthk.h | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/src/nt_emm.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/src/nt_eoi.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/src/nt_error.c | S3 | proved-expression-or-spelling |
| src/mvdm-host/softpc.new/host/src/nt_event.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/src/nt_mouse.c | S2 | S2-whitespace-only |
| src/mvdm-host/softpc.new/host/src/nt_reset.c | S2, S3 | proved-expression-or-spelling |
| src/mvdm-host/softpc.new/host/src/nt_timer.c | S2 | S2-whitespace-only |
| src/mvdm-host/wow32/wow32.c | S2 | S2-whitespace-only |
| src/non-mvdm/base/ntos/rtl/time.c | S3 | original-rtl-time-owner |
| src/non-mvdm/base/win32/client/vdm.c | S3 | environment-structure-with-native-width-retained |
| src/non-mvdm/ds/netapi/rap/strucsiz.c | S3 | proved-expression-or-spelling |
| src/non-mvdm/ds/netapi/xactsrv/apimsg.c | S3 | proved-expression-or-spelling |
| src/non-mvdm/public/internal/base/inc/align.h | S2 | S2-whitespace-only |
| src/non-mvdm/public/sdk/inc/ntexapi.h | S2, S3 | proved-expression-or-spelling |
| src/non-mvdm/public/sdk/inc/ntregapi.h | S2 | S2-whitespace-only |
| src/non-mvdm/public/sdk/inc/ntrtl.h | S2 | S2-whitespace-only |
| src/non-mvdm/public/sdk/inc/ptypes32.h | S2 | S2-whitespace-only |
| src/non-mvdm/windows/core/ntuser/client/usercli.h | S2 | S2-whitespace-only |
| src/non-mvdm/windows/core/ntuser/kernel/userk.h | S2 | S2-whitespace-only |

## Reproduction and acceptance

Run the checked-in entrypoint in an owned background job:

`python test/build/verify-t480-s4-reconciliation.py --output build/t480-s4/<fresh-directory>`

Prerequisites: this repository's Git revisions b35effafd, ae65e175b and511f23fc0,
committed S1 build evidence, S2/S3 archives and current S3 release images. No
compiler, guest, network or external source tree is required. The verifier
rejects archive corruption, missing/duplicate consumers, source drift, mismatched
stage identities, failed recorded runs and changed published images. It checks
that this S has not changed product/guest/fixture files. Registry references
are regenerated from the final documentation; they are not a semantic gate.

The final run is reconcile-final, output reconcile-r2/reconciliation.json;
PID/start/deadline/exit and governance results are retained in
`artifacts/mvdm-audit/t480-s4-verification.zip`. Documentation governance checks
relative paths and anchors as well as packet structure; staged diff checks
include the new verifier. This local P does not republish unchanged artifacts,
write O:/winnt or push. Full runtime acceptance remains the S3 delivery, clearly
separate from this read-only evidence check.
