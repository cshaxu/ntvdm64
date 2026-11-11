# T482 DOS restart: bounded investigation

Baseline: 782854d6f; original Windows binaries and host publication unchanged.
Owner authorizes pure-host/addon attribution and minimal repair, not guest patches.

## Expected original behavior

Microsoft KB 138996 (archived original Microsoft article):
https://ftp.zx.net.nz/pub/mirror/ftp.microsoft.com/MISC/KB/en-us/138/996.HTM
The selected shutdown choice uses Exit to DOS.pif. With current configuration,
Windows leaves memory and starts real-mode DOS, processing DOSSTART.BAT; EXIT
returns to Windows. A custom configuration instead involves a boot/configuration
replacement cycle. These are distinct from normal shutdown or another DOS VM.
This source describes the intended contract, not what our guest actually does.

## Actual dialog discovery

Independent authored PE32 test/observation/win95_dos_restart_probe.c was built
with the established CRT-free Win95 probe recipe. No original guest was rebuilt.
An owned private-desktop run of an isolated O:/WINDOWS copy observed version
C0000004, Progman HWND 00A0, and opened the real shutdown dialog with WM_CLOSE.
Its radio options were 3800 (shutdown), 3801 (restart), and 3803 (restart into
MS-DOS); confirmation ID was 1. The diagnostic only identified the option in
this first run and exited; the outer observer timed out by design. This is
successful discovery, not successful DOS return, normal exit or acceptance.

Receipts: build/t482/dialog1/DOSRET.LOG and observer.txt; build/t482/job/dialog1.*.
The original installation hashes matched and O:/tmp/q82 was removed. The
next diagnostic explicitly enables selecting the witnessed radio and posting
its confirmation. No guessed ExitWindows flag is used as a product fix.

Root cause remains unproved. Scope next: record the actual DOS request outcome,
inspect its PIF/command owner and guest-to-host retirement, then establish a
normal-shutdown control before changing product semantics.

## Request and matched shutdown control

request1 selected/confirmed the observed DOS control; request2 retained an
ordinary invisible guest top-level window to log session-end messages. Both
posted the DOS request, but the outer observer timed out. In request2 the
welcome dialog remained visible and no QUERYENDSESSION/ENDSESSION appeared.
This alone is not proof of a deadlock or failed host exit.

The first normal control was inconclusive because its radio check failed.
control2 avoids clicking an already-selected radio and logs before/after state.
It observed QUERYENDSESSION, ENDSESSION(TRUE), then the real root exited 1,
matching the previously approved normal-shutdown boundary. Its original
installation hashes matched and O:/tmp/q86 was removed. The same diagnostic
binary is now repeated for DOS selection, to exclude selection-mechanism drift.
The current evidence localizes the distinction before ordinary session-end
notifications; it does not yet identify the failing owner or justify a fix.


request3 never opened the dialog and is inconclusive (probe posted too early).
request4 waits for the visible Explorer tray before opening the same dialog;
it confirmed 3803, posted IDOK and again retained the welcome dialog without
session-end notifications. This supports investigating the preparation seam,
not declaring host shutdown deadlocked. The original OpenNT shell ExitToDos
uses PifMgr_Open/Get/Set/Close before ShellExecuteEx; failed preparation beeps
and returns. Actual Win95 behavior at these calls is the next unproved point.


## PIF preparation and filename controls

pif1 calls the actual guest SHELL32 exports 9/10/13, checked against original
shell232.src and packed PROPPRG (658 bytes). Resource 0x10b3 resolves the
localized Exit-to-DOS PIF name. OpenProperties returns zero, last error 2.
This is a separate diagnostic invocation of the documented original sequence,
not a stack capture of Explorer's exact failing instruction.

pif2: direct CreateFile on that path also fails with error 2; same COMMAND and
OpenProperties flags 6 with DOSRET.PIF succeeds. GetProperties returns 658.
The nonzero successful handle is authoritative; successful OpenProperties
leaves a stale last-error 2, which is not treated as a failure.

pif3: DOSRET.TMP creates successfully; ASCII `Long file name.tmp`, leading-space
` LEAD.TMP`, and the localized name without its leading space all fail with 2.
Thus a Chinese-codepage-only explanation is insufficient.

pif4 precreates the localized PIF from COMMAND.PIF in the owned copy using the
host, before guest launch. Guest GetFileAttributes and OPEN_EXISTING still
fail with 2; short ASCII PIF creation/GetProperties succeeds. Merely shipping
a precreated localized PIF is therefore not a demonstrated remedy.

Original NTDOS mstable.asm ends ordinary extended DOS dispatch at 6Ch (optional
ROM functions 6Dh/6Eh), not a 71h LFN provider. This source fact alone does not
prove which Win95 IFS conversion rejected these actual requests. The first
confirmed boundary is guest filename access/PIF preparation. Attribution to
a precise filesystem provider still needs evidence before a product fix.
No product, original guest, installed Windows or released assets were changed.
Receipts: build/t482/pif1 through pif4, job/*.out/err/exit. Each runner verifies
the original installation hashes and removes only its owned temporary copy.


## Capability and original DEM boundary

pif5: actual guest GetVolumeInformation succeeds, reports maximum component
length 12 and flags 0 (filesystem name empty). It does not advertise LFN.
The attempted host-short-alias control is INVALID: GetShortPathName returned
its unchanged long name on this host volume. Its failure is not evidence that
a real 8.3 alias cannot be opened. Guest GetShortPathName also fails on the
preexisting long name. Do not count this invalid alias control as acceptance.

hostfile replaces only the DEM compilation unit in a diagnostic NTVDM, by
including unchanged demfile.c and wrapping CreateFileOem. It records only the
owned Q94 tree and preserves the real return and last error. It is independently
linked using the current worker Ninja commands; production objects/libraries
and publication are unchanged. The complete isolated package receives a fresh
identity and matching manifest before launch.

The same guest probe reports the long-path failures; host-files.log shows
successful DOSRET.TMP CREATE_NEW and DOSRET.PIF CREATE_ALWAYS/OPEN_EXISTING.
No long-path attempt (including a truncated variant) reaches that original DEM
CreateFileOem seam in the corresponding sequence. This excludes that host
file-create call and NTSRV shutdown as the immediate source of the reproduced
preparation failure. It does not identify the exact Win95 IFS internal function.

Conclusion: the current guest filesystem capability cannot access the long
localized PIF requested by the original Shell. Providing LFN is a capability
addition, not merely a correction to host retirement. This is NOT automatically
classified as an immutable original-guest defect: the selected DOS/provider
integration contributes the capability boundary. Do not use standing guest-bug
disposition to hide that distinction. A proper filesystem/provider solution
must be scoped and proven; a filename-specific bypass or patched Shell resource
is not a unified repair. Subsequent real-mode transition, DOS input and restart
remain unverified; PIF preparation success alone cannot close the objective.

Diagnostic receipts: build/t482/hostfile/host-files.log and DOSRET.LOG;
build/t482/dem-provider/build.cmd and ntvdm.map; job/dem-build2.*, hostfile.*.


## Downstream experiment and decision boundary

short1 uses the same actual PifMgr ABI to create DOSRET.PIF, sets the original
PRGINIT_REALMODE|PRGINIT_REALMODESILENT bits and the resolved COMMAND command,
then invokes ShellExecuteEx. This is an isolated diagnostic past the proven
filename obstacle, NOT a replacement of the real Explorer menu or a product fix.
Open/Get/Set and ShellExecuteEx succeed; QUERYENDSESSION and ENDSESSION(TRUE)
are observed. The outer runner times out; the Console snapshot is empty.
A usable DOS prompt, interactive commands and Windows restart are NOT proved.
short2 repeats with exact-image-bound CCPU/guest-memory capture; its result is
recorded separately rather than interpreting a process timeout as a deadlock.

The archived Microsoft Windows 95 Resource Kit explains the provider boundary:
https://techshelps.github.io/MSDN/DNWIN95/HTML/S6789.HTM
https://techshelps.github.io/MSDN/DNWIN95/HTML/S6556.HTM
IFS Manager dispatches to filesystem providers; real-mode network clients expose
short aliases whereas protected-mode clients can support LFN. These documents
support the architecture distinction, not identification of our exact rejecting
Win95 internal function. The current runtime's capability comes from the actual
volume API and matched file/DEM traces above.

A unified repair would provide LFN through the actual Win95 filesystem provider
contract, including names, directory enumeration, handles, attributes, errors and
shutdown. Implementing a couple of DEM calls cannot fix requests rejected before
DEM. Do not assume the separately queued XP NTVDM upgrade automatically supplies
that Win95 protected-mode provider. Do not rename the Shell resource, intercept
one PIF filename, fake volume capabilities or replace DOS return with host exit.

Owner scope choice requested: prepare a Win95 LFN-provider design for review,
or retain/transfer the functionality gap with this bounded investigation.
No approval or completion is inferred while that question is pending. Production
repair, build/publication/Full delivery and requested functional acceptance are
still outstanding; no changed runtime is currently eligible for publication.


short2 completed diagnostic cleanup with the original installation hashes intact.
The selected diagnostic ntvdm.exe hash is
F302D935BF60F65F42827A2C99F5A4E48E9E225835AE80D9B9AE026F90A33FD7.
Its exact map supplies SR RVA 41d2a0, IP RVA 41d420 and guest-base RVA 2efb48.
The terminal sample remains at CS 0028, base 0, limit FFFFFFFF, EIP C00034CD;
the guest is not shown to have reached a real-mode DOS prompt. This single
sample does not prove deadlock or explain the post-session-end continuation.
Owned O:/tmp/q96 was removed. The original menu remains unfixed.


## Reproduction entry and checkpoint limits

The maintained build-win95-menu-probe.ps1 now accepts `-Probe DosRestart` with a
fresh `-OutputRoot build/...`; default Menu preserves the existing source/name.
It uses the same CRT-free build recipe and adjusts only its independently
written test PE's metadata. DOSRET.EXE in isolated WIN.INI run= supports these
adjacent opt-ins: no tag identifies the dialog; DOSGO.TAG submits the actual
observed DOS selection; SHUTDOWN.TAG with DOSGO.TAG selects normal shutdown;
PIFTEST.TAG runs the filename controls; SHORTGO.TAG performs only the explicitly
labelled short-PIF downstream experiment. None is a product success fixture.
Use New-Win95ProbeCopy and isolated_package_cleanup with a fresh physical
package identity; the original source hash list must match after each run.
The DEM wrapper is a diagnostic-only full-TU replacement, never a product input.

This checkpoint contains admission, investigation and diagnostic sources only.
There is no product repair to publish; prior Full/publication is not reused to
claim the new feature passes. The request remains active pending scope review.


## Final diagnostic-source validation

The logger now explicitly preserves LastError, so logging cannot manufacture
or overwrite the measured API error. final-pif repeats the same failures with
that final source: long/leading-space/localized paths return error 2; short
DOSRET.TMP creation and PIF GetProperties succeed. It then selects the real
normal-shutdown radio, receives session-end notifications, and the root exits
naturally with the previously recorded code 1. Owned Q97 is removed and source
installation hashes match. This is a passing diagnostic/normal-exit control,
not a passing restart-to-DOS case.

Original SHELL32.DLL is version 4.00.950, SHA256
C7D234025511318F3F1A7002989E43AB0A1976C9BACD1022FC6436989D73EDD7.
The shared builder's default Menu and DosRestart selections both compile;
final source receipt is build/t482/shared-dos2/build.json. Documentation
governance and git diff --check pass. No production files, release assets,
original guest binaries or installed configurations are part of this checkpoint.
