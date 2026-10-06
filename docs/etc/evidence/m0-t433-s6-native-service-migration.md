# T433 S6 — Native NTSRV implementation

## Scope and source ownership

Owner approves implementation after the [mirror audit](m0-t433-s6-native-service-mirror-audit.md).
One ntsrv.exe becomes AMD64. Original CCPU40/NTVDM, WOW32, VDMREDIR and Hook32
remain I386; run16, NTCON, NTVWM, NTMON and Hook64 retain their delivered AMD64
images. No new executable, helper, registry, protocol, scheduler or lifecycle
policy is introduced. APP0.0.433 / RPC41 / I/O25 remain unchanged.

The final service map selects the same35 translation units as the baseline.
Original complete bodies are srvvdm.c and RTL error.c; preexisting source-shaped
exports token-query and generated VDM-config fragments also remain selected.
No srvinit, RTL environment/time/x86 assembly or MVDM source enters the service.
Project server bindings and copied transport remain in their existing owners.

OPENNT-HOST-068 records exactly seven executable expression changes in one
original file: numeric PID/TID and wait-receipt conversions use ULONG_PTR;
parent bit-zero masking does not narrow its carrier. Fifteen project casts in
six service resource/registry units use the same native-width intermediate.
No original structure/header or execution/completion/cleanup algorithm changes.
Scoped NULL=0 applies only to native srvvdm compilation, retaining integer-zero
casts without eleven unnecessary literal edits. Narrowing diagnostics remain
errors, including /we4312; no warning suppression is used. x86 remains supported.

New-NativeWorkerNinja's Service mode selects the finite service closure and
architecture-local MIDL/server/config outputs. The formal graph imports an
explicit AMD64 producer with no I386 fallback. Original Windows subsystem and
mainCRTStartup are preserved. Packaging and verification require explicit
native-service provenance and machine identity. Version-negative peers now
compile at the selected service's ABI, not a hard-coded x86 ABI.

## Current build and focused results

Evidence is below build/M0-T433/S6; r001-r005 remain the admission audit.

| Evidence | Entrypoint/contract | Actual result |
| --- | --- | --- |
| r009-native-final | New-NativeWorkerNinja -Component Service, run-ninja ntsrv.exe and three existing service fixtures | Strict AMD64 compile/link; Windows subsystem. |
| r010-formal | New-T310OriginalSoftpcNinja -Architecture x86 -NativeService r009/ntsrv.exe; native import | AMD64 service hash matches producer; subsequent Ninja reports no work. |
| r009/r010 layout logs | native-service-layout-test.exe, same tracked test at both widths | Native CSR sizes, copied36/16/32-byte values and high numeric PID/TID/receipt/tag identities pass. |
| r015-native-fixtures | verify-service-fixtures.ps1 with native basesrv-service-reservation-test.exe | All29 original assertions pass;5915ms. Includes reentry, parent origin, I/O authority, cancellation/rundown, management and WOW completion. |
| r016-x86-fixtures | Same runner and assertions with rebuilt I386 fixture | All29 pass;9038ms. |
| r018-input-boundaries | native_service_build_inputs.ps1 | Native Windows import hash match; wrong machine, wrong subsystem and outside-build output rejected before copy. |
| r019-hook-unit | nthook-install-test.exe at each width, opposite-width native/GUI fixtures |147 assertions each; actual current Hook32/64, native handles/suspension and propagation retained. |
| r017-review | Final graph/map/source-selection ledger |35 selected units; AMD64 Windows PE; pinned upstream unchanged. |

Final producer ntsrv.exe SHA256:
05825EF39E4ADC5509806302A8385730C073F1F70D9EE0B4B5FC32C5C0027CF7.
The sealed candidate is r011-final-runtime, six AMD64/four I386 images.
Its other changed bytes are rebuilt NTVDM and Hook32, whose source expressions
remain x86-equivalent; unchanged images are pinned by the ten-image manifest.

## Retained non-pass attempts

The first generator attempt did not admit the existing absolute VDM-config
source dependency; the finite dependency is now validated and recorded.
r006/r007/r008 preserve an initial Console-subsystem native service and its
Full14 pass675372ms. It is not the final publication input; r009/r011 preserve
the original Windows-subsystem contract and receive their own product gates.

r013 native service29 stops at the native-worker snapshot assertion: the
fixture created its own AMD64 child but hard-coded kind2/Win32. A diagnostic
direct replay proves binding succeeds and the subsequent kind assertion fails.
Expected kind now follows the fixture's actual build width, including the GUI
fixture. All task/stack/image, receipt, exit and cleanup assertions remain;
no production change is needed. The initial attempted preprocessor placement
inside CHECK fails compile and is corrected to a constant-width expression.
The failed direct diagnostic leaves two suspended fixture children; exact
command/parent14960/creation-time-checked cleanup releases their image lock.
These failures and link logs remain evidence, not passing product runs.

## Product delivery status

Final r012 Full14 passes in438018ms. Console17 and Window17 pass all retained
output/exit/interaction assertions; WOW keeps the three independent accepted
frontiers. RPC includes actual x86 fixtures/native monitor against the AMD64
service, startup failures/timeouts, dead-worker receipts and snapshot control.
All five native-service version-negative variants reject both AMD64 run16 and
I386 NTVDM with1306 before delivery. GUI startup/wait/survival/management,
strict DIR, modern EDIT return, cooked CMD return, rapid relaunch/typeahead,
session isolation and frontend/worker retirement pass. Native target failure
still returns an infrastructure result such as1067, not a fabricated success.

r022-extra-real passes I386 and AMD64 CMD→DOS→CMD in both Console and Window:
actual input/output, parent-return marker and direct exit23. Both Hook origins
pass all ten actual legacy routes (bare/absolute COM/EXE, MEM, parent output
order, interactive input and explicit run16), then native CMD→WINMINE proves
the startup-only receipt and actual Mines UI frontier, not gameplay.

r020-publication validates final Full14 and the S5 deployed hashes, preserves
a coherent ten-image/notice/configuration backup, replaces only approved host
images/notice at O:/winnt/system32 and verifies every hash. Guest media and
configuration remain unchanged. r021 actual deployed smoke passes COMMAND0,
native VER0, nested MEM1 and direct MEM0, plus explicit System32/AMD64 and
SysWOW64/I386 CMD VER/output/exit0 through the published native service.
All ten image hashes match; test-created Z: is removed.

Source, diff, image-matched map/35-unit closure, strict compilation and
governance/relative-link reviews are complete. Commit/push bookkeeping follows
these measured gates. T433 remains open; S7/S8 are not admitted.

Known S5 rapid-interactive unclassified observation remains explicit debt, not
a claimed repair. Tests use owned observer Consoles/private desktops and only
Z: aliases, with cleanup. No physical RDP/manual gameplay acceptance is claimed.
WINMINE visible UI and independent SOL/WRITE frontiers, not invented gameplay
or universal arbitrary geometry/environment compatibility, remain the contract.
