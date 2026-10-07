# T436 S2 Windows 3.1 mouse-driver boundary

Owner accepts S1's unstable enhanced execution and admits the independent
mouse driver, followed by S3 installer patches. This record covers S2 only;
startup-only research is not resumed and no new runtime capability is claimed.

## Initial ABI audit

Input O:/win31/SYSTEM/MOUSE.DRV, Mouse8.20 SHA256
B4F0B9DE8043071FA2B82B16373BF5188BBE8162C3EF24C8C3D1B01F86B96B07.
`tests/observation/audit-win31-mouse.py --driver <input> --output
build/M0-T436/S2/r001-abi/retail-mouse.json` reads movable/fixed NE export
bundles, segments, imports and names. It does not write the input, copy code
or prove execution. NE expected Windows version030A, automatic data segment2,
exports Inquire1/Enable2/Disable3/MouseGetIntVect4, GetSetMouseData5,
CplApplet6/PowerEventProc7 and named WEP8; original imports KERNEL/SYSTEM.
The Win101 audit intentionally rejects movable bundles and is not weakened.

## Provenance and design choices

OpenNT WOW mouse.asm/def retain public entry shapes but implement WOW thunks;
they cannot serve as a real NT DOS guest driver. Original DPMI dxintr.asm
PMIntrMouse converts protected function12 callbacks to a real-mode reflector,
but function20 exchange is explicitly not translated. Do not assume its raw
returned address is a protected selector. The selected independent driver
therefore uses public DPMI0300/0303/0304 for raw INT33 and callback ownership;
retail DOSX/VMM support remains a runtime gate, not inferred from OpenNT source.

Win101's authored endpoint normalization and reversible callback/pointer
ownership are reusable concepts. Its real-mode NE callback and code-segment
mutable state are not reusable blindly in protected Win3.1. Keep mutable state
in driver data, preserve the original INT33 provider, and avoid a second INT71
consumer or hardware-vector/PIC programming. No host/mirror/protocol changes.

Microsoft [DDK adaptation guide](https://www.pcjs.org/documents/books/mspl13/win/w3ddkadp/)
defines the fourteen-byte MOUSEINFO and the driver entry/event contract.
Microsoft [Q74572](https://jeffpar.github.io/kbarchive/kb/074/Q74572/) explains
that the default Windows mouse driver does not consume the installed DOS
driver automatically, and VMD arbitrates hardware rather than acting as the
Windows driver. Both are primary Microsoft texts preserved by archives.

## Remaining gates

Implementation/build and exact NE/export/relocation checks; callback allocation,
real/protected stack return, old callback restoration and failure rollback;
normal/missing-provider/repeated-enable/disable tests; actual standard-mode
arrow and click/show-hide/exit/restart; enhanced-mode interaction when it starts.
Inherited enhanced startup failures remain distinct and never count as mouse
passes. S3 remains planned, not active; final installer files are not delivered.

## First independent driver build — not runtime acceptance

mouse31.asm implements fixed/resident protected code and separate writable
data, raw DPMI0300 INT33 calls, allocated DPMI0303 callback and0304 release,
prior callback exchange/restoration, balanced DOS-pointer hide, normalized
Windows events and named WEP cleanup. It claims no physical IRQ. Cleanup
does not free a thunk if restoring the provider callback fails. DPMI's real
register image consumes the provider's far-return stack words before the
protected callback returns IRET; this is still a runtime verification gate.

build-win31-mouse-driver.ps1 with explicit Python and BuildRoot
build/M0-T436/S2/r003-driver-build assembles through NASM and audits the
generated NE: Win030A, flags8201, automatic data2, fixed/preloaded code/data,
exports1/2/3/4/8, named WEP8 and no imports. PASS construction/structure only.
Driver SHA2565BC9D9931C358839976F13AFF97F200475C12A28EE7EDCCDF89067B5B25538F4.
No new driver is deployed yet. Mock/provider tests, optional-export consumers,
actual Win3.1 loading, arrow/clicks and exit/restart remain open. There is no
host-image change. S1 research/design checkpoint42fd2c883 is pushed.

## Guest mock and first actual load attempts

r004 builds win31_mouse_driver_guest.asm against the same authored routines.
r005 runs it through published run16/NTSRV/NTVDM x86 CCPU40: real exit0 and
fresh Console marker M31MOCK:PASS50 checks. Fifty assertions cover repeated
Enable/Disable, event-procedure replacement, normalization/endpoints/buttons,
DPMI far-return image adjustment, callback/pointer restoration and refusal/
rollback. Persistent restoration failure keeps the thunk allocated rather
than freeing a referenced callback. This is mock-DPMI evidence, not actual
protected transition or Windows interaction. The build now emits this COM;
verify-win31-mouse-mock.ps1 retains strict completion/output assertions.

r006 stages reversible SYSTEM.INI selection of the new driver only in the
E386 copy plus separate /S and /3 PIFs. The selector targets only [boot], not
the similarly named [boot.description] field; original retail MOUSE.DRV is
unchanged. r007 rebuilds the test-only private frontend mouse sink tools.
No Z:/mapping, production helper or host binary replacement is used.

r008's actual /S candidate stays at logo, exits1, and never reaches desktop
readiness; no mouse events are posted. r009's same-PIF original-driver control
also remains at logo and times out. These do not identify a driver root cause
or prove a standard-mode mouse pass. The constructor/load/callback boundary
still needs precise verification.

Original newexe.inc distinguishes NEPROT0008 from application-type bits.
r010 corrects the authored NE flags to8309 (library/single data, Windows API,
protected-only), not the earlier misleading8201 comment. The export prolog's
8CD890 sequence matches the loader's source-defined DS patch recognition;
NASM's alternate MOV BP,SP encoding is not the recognition predicate.
Driver r010 SHA25682A85B6CEC416B87E2E396621F903C21CB6B492F60C09CEF7C1F86798ADD093A.
r012 rebuilds corrected driver plus mock and passes structural checks.

r011 selects r010 for the existing /3 PIF. Its fresh BOOTLOG records LoadStart/
LoadSuccess for the exact PATCH/MOUSE31.DRV and Mouse initialization completed;
the run then exitsC0000005 before verified desktop/mouse consumption. Loading
success is not input acceptance. The private sink runner requires a fresh
known desktop and records pixel changes/images separately from injection
acknowledgement; no click pass is fabricated from either alone.

After these failed actual trials, E386 SYSTEM.INI is restored to r006's exact
pre-test hash22743418C5F6B4A9F9E417BB7F997EE190FFA974C03B4A3AA6E3E1BC5B52D11A.
The new driver remains an experimental file, not the default selection or
owner hand-test release. Original installation/default profiles and ten host
images remain unchanged. S2 stays open; real DPMI/provider and Windows arrow,
click/show-hide/normal-return/restart gates remain incomplete.

## Proven selector error corrected and standard mouse interaction

r013/r014 executes the authored routines as a16-bit client of real NT DOSX
and INT33, without Windows. It enters protected mode then faults at
CS01BF:IP0258, the exact MOV ES,[real_callback+2] instruction. DPMI0303
returns a real segment, not a protected selector. The earlier real-mode mock
could not expose this invalid selector load. This is a project driver defect,
not an inherited guest/CPU defect.

mouse_call now accepts/returns numeric real ES in SI and the real register
image only. It never loads that address into protected ES; protected ES is
preserved and used solely for the DPMI image pointer. Exchange/registration/
restoration all use the numeric form. r015 builds the corrected driver.
r016 actually executes the allocated real->protected callback via DPMI0301,
including nested real INT33 geometry query, far-return adjustment and free;
exit0 and M31PM:ENTER / M31PM:PASS16 checks prove this real NT DOSX route.
This is not proof of the retail Win3.1 DPMI host. r017 repeats the50-case mock,
including output/completion assertions: PASS.

r018 actual /S reaches Program Manager with a Windows arrow after correction.
The old whole-screen readiness hash included the absence of an arrow and
therefore never posted input. The runner now optionally hashes the fresh
Program Manager title rectangle80,52,530,65, expected SHA256
A836889B241814E8597EACC1438BCABCB79BC28B1F66EFD8390FB5F6706F52E5.
Full-frame changes remain separately compared for subsequent input evidence;
the title crop is not confused with that full-frame hash.

r019 repeats /S with the exact corrected driver and performs actual downstream
input. Reviewed mouse-file-1.bmp shows the Windows arrow over File;
mouse-down-1.bmp shows the File menu open; mouse-up-1.bmp shows the released
menu state with New selected and Exit Windows present. Corner/movement/down/
up each produce fresh image changes. Actual visual content, not the sink
acknowledgement alone, establishes the movement/menu result. These are
private input-sink observations, not physical Raw Input/RDP acceptance.
Windows is deliberately left live until the bounded observer timeout, so
these cases are not normal guest exit or cleanup passes. Finally restores
the prior experimental INI. Enhanced input, actual normal return/restart and
mode-specific provider cleanup remain open; S2 is not closed or published.

## Enhanced interaction and normal-return boundary

r020 uses the identical corrected driver for /3 and the fresh title-region
readiness gate. Reviewed mouse-up-1.bmp shows the File menu open with New
highlighted after the movement/down/up sequence. mouse-down-0.bmp captured
the earlier arrow-over-File state, so it is not mislabelled as completed menu
painting. Enhanced-mode movement/click consumption is observed when startup
succeeds. The60-second cleanup remains a timeout, not normal shutdown.

r022 exercises File/Exit Windows by mouse and captures the actual guest
Exit Windows confirmation dialog in surface-9.bmp. r023 gates the Enter key
on its exact title/body rectangle179,185,461,214, SHA256
B8AD95452D1BB2F0E92B49743EF45A721946B090AD187DC7B815C53EC99AC8A5.
It posts confirmation only after the fresh matching image. Then NTVDM reports
ERROR_INVALID_PARAMETER87 and the observer eventually times out: no normal
return pass is claimed.

The published-image map/disassembly resolves the blocked CPU stack:
cmdGetNextCmd -> nt_block_event_thread -> DisplayErrorTerm. The return address
maps to4707F4 and the embedded source line1494: the project-added final
NtvdmConsoleUpdateText call failed. This is after guest exit into the DOS
command-completion/handoff path, not the earlier MOUSE31 selector GP.
The exact deeper transport/frontend failure is not yet localized. Host code
changes are outside the admitted add-on-only ABI surface; owner approval is
requested before expanding S2 to repair that blocking adapter path. No
original algorithm, guest or protocol change is made. Finally restores the
experimental INI; driver and installer delivery remain incomplete.

## Read-only host-boundary review while scope approval is pending

The deeper candidate is NTCON frontend_session.c's hidden-buffer preparation,
not the validated wire text decoder: decoder rejection paths normally return
INVALID_DATA/INVALID_STATE, whereas prepare_text_frame and clone_grid propagate
native Console API errors. Both form SetConsoleWindowInfo rectangles from the
incoming buffer extent rather than the current physical viewport extent.
Thus a valid logical40/50-row frame can request enlargement of a newly created
hidden buffer's viewport beyond its physical/font limit and receive87.
Logical viewport authority is already separately stored in logical_window;
storage-buffer window enlargement is not required to preserve that contract.

This is a source-supported repair candidate, not a proved failing API yet.
No host file is modified and no new reproduction is started while approval
is pending. If approved, use a focused real Console fixture to isolate the
failing preparation step; preserve buffer/cell/cursor and logical-window
semantics, avoid physical viewport growth during hidden-buffer shrink/prep,
then rerun actual guest return and applicable host release gates. Do not fix
by ignoring87, truncating logical frames, retrying or forcing window repaint.

## Confirmed failing API — owner-requested deterministic diagnosis

Owner requests “你调查一下，给我一个确定性的答案”. r024/r025 build an
AMD64 test-only observer linked with the existing private input hook. It
activates only inside ntcon.exe on the owned NTVDMConsoleTest desktop. It
records failed original Console calls, original arguments/LastError and
pre-call native geometry/font, preserving call result and error. The test
DLL stays pinned through return; it is not a product image, new production
component or guest patch. No production source/file is changed.

r026 repeats the actual /S mouse-driven exit and fresh Exit Windows dialog
confirmation. The observer records:

```text
api=ATTACHED pid=7432
api=SetConsoleWindowInfo error=87 caller-rva=5f04 request=0,0,79,49 info-valid=1 buffer=80,50 viewport=0,0,79,39 maximum=80,40 font-valid=1 font=7,16
```

The request is an80x50 viewport; actual storage already80x50, actual viewport
80x40 and reported maximum80x40. Windows rejects the viewport enlargement,
not the storage capacity. The matching published image SHA256
A7916B09F4DBF6838F239406FE2F670E3D186D5415DA8DD4EE2EC7C6576A6F64
matches r060-release/frontend/ntcon.exe and its map. The formal directory's
different image/map is not used. RVA5F04 is the instruction immediately
after the SetConsoleWindowInfo call at5EFE within prepare_text_frame
(function RVA5BB0), confirming that exact branch, not clone_grid or cursor
restoration. Binary disassembly and current source agree.

This confirms the failure mechanism for the reproduced exit: hidden-grid
preparation unnecessarily equates buffer extent with its native viewport,
then propagates87 to final NTVDM publication. It does not establish that only
Win3.1 can trigger it; another50-row publication under the same native limit
can reach that path. The underlying frame/data need not be shortened to40.
Fix scope should preserve50-row storage/logical_window and avoid enlargement
of the hidden native viewport; clone_grid deserves the same bounded review,
but was not the failing call in this reproduction.

r026 reproduces the error dialog and timeout, not a normal-return pass.
Finally restores the pre-test experimental INI/environment; all published
EXEs/defaults/original installation remain unchanged. Diagnosis is complete
for this API/parameter failure. Production repair is not performed or claimed.

## Owner-approved hidden storage/viewport repair

Owner approves implementation after the original OpenNT resizeWindow review.
The source reference is mvdm/softpc.new/host/src/nt_graph.c windowSize /
resizeWindow: shrink an obstructing viewport before resizing storage, query
and bound any presentation enlargement by dwMaximumWindowSize. The original
body owns sc/global guest presentation and cannot directly compose into the
backend-neutral frontend. NTCON's existing private logical_surface is inactive
storage, not an original active Console window. The smallest binding retains
the prerequisite shrink and complete buffer allocation but omits unnecessary
viewport enlargement altogether. No mirror, guest, protocol or new state.
Remaining buffer/write failures still abort and release the candidate; they
are not ignored or retried. The original debug-only viewport assertion is not
used to hide a failed storage transaction.

r029 explicitly starts an80x25 viewport, observes now_height25 during graphics,
and returns normally with25-row storage. r030 starts120x30, selects80x28, then
after confirmed guest exit reads BIOS mode3/columns80/rows50/font8 and simulated
VGA screenHeightRaw399/charHeight8/now_height50. Thus400/8 produces50 rows;
the frame is not invented by NTCON. The exact guest instruction choosing that
font is not traced. r030 reproduces the80x50 viewport request/error87 at the
same production boundary, whereas r029 disproves unconditional50-row exit.

frontend_session.c now shares prepare_grid_size between clone_grid and
prepare_text_frame. Only a viewport that would exceed the requested buffer
is shrunk, using its existing extent rather than the entire buffer extent.
Storage stays complete and logical_window remains independent. Production
diff is21 added/15 removed lines (net6), with no persistent state or retry.

Build: New-NativeWorkerNinja.ps1 -Component Frontend -BuildRoot
build/M0-T436/S2/r031-viewport-repair -ReferenceGraph
build/M0-T435/S2/r060-release/formal/build.ninja, followed by its run-ninja.cmd
for ntcon and affected fixtures. MSVC AMD64 /MT, matching current published
frontend architecture; unchanged CCPU40 NTVDM remains x86. New graph selects68
source edges. Sealed baseline inputs/products are not overwritten.

build-frame-codepage-test.ps1 -Cache build/M0-T436/S2/r031-viewport-repair
-BuildRoot build/M0-T436/S2/r034-scripted-frame-fixture -Environment
build/M0-T436/S2/r031-viewport-repair/msvc-x64.cmd reproduces the real importer
fixture:41504 checks, zero failures, including50/50/25/50 complete cells,
attributes, last-row cursor, legal native viewport, repeated clone and rollback.
The33-check production text-handoff fixture passes. Private channel lifetime
and injected frame/projection failure fixtures pass with stable handle counts;
video and Window-library tests pass. The threshold fixture first shrinks its
inherited viewport before narrowing its buffer; all threshold/cell/cursor
assertions remain. This fixes test initialization, not its acceptance criteria.

Publication preserves a complete ten-image recovery set under r031/recovery.
All nine unchanged published hashes match T435 r060; only NTCON changes to
3927A308EB9F46CE0262176CF96D66C26F91DC9080E5C803C3606A06ACA91DF1.
r033 uses that actual O:/winnt package, the corrected mouse driver and fresh
Program Manager/Exit Windows pixel gates. Initial viewport120x30; native API
observation confirms successful80x50 storage with no failed viewport request.
Actual launcher exit0, no error dialog/timeout; normal /S return passes this
replay. Experimental SYSTEM.INI is restored to22743418C5F6B4A9F9E417BB7F997EE190FFA974C03B4A3AA6E3E1BC5B52D11A.
No /3 stability, physical/RDP mouse or full S2 closure is claimed by this repair.
r035 serial published-package regression passes all17 Console and17 Window
cases with unchanged exit/output/ordered handoff assertions. Console case-body
total100763ms, Window109625ms; these are summed case bodies, not wall-clock
performance promises. No subst or additional drive mapping is used.

The first WOW run uses absent O:/winnt/system32 sample EXEs and exits1 with
file-not-found; this is retained as invalid application evidence, not passed.
The actual installed O:/win31 samples have different hashes from the accepted
baseline. Therefore three exact r060 baseline originals are copied into the
existing O:/winnt/tests location, not the installation directory. The observer's
optional GuestRoot explicitly selects that location while retaining default
system32 behavior, process ownership, inspection and timeout assertions.
wow-baseline-media repeats the accepted WINMINE visible application, SOL dialog
and WRITE dialog frontiers, compared with r063-product-regression UTF16 class/
title evidence and live-worker samples. These bounded observations end by
timeout as before: they are frontier non-regression, not normal exit, gameplay
or SOL/WRITE functionality passes. NTVDM/WOW32/VDMREDIR/GUI control inputs remain
byte-identical; no WOW repair is implied. The temporary media copies are removed
after hash-checked cleanup. Complete ten-image published/assets equality and
documentation governance/diff checks pass. This is an intermediate host repair
delivery; the remaining independent mouse-driver candidate stays unclosed.

## Compiled add-on release and tools-owned Setup entrypoint

Owner adds compiled MOUSE101.DRV / MOUSE31.DRV to assets/release, with a
separate addon-manifest.json (the ten-host-image manifest is unchanged).
Stage-MouseDriverRelease.ps1 validates source and output hashes against each
build before copying. r038/r039 rebuild both NE artifacts; Win101 SHA256
DC01039B0FC2B7E9244F31A42A4AAA9ED10613291557B4B6E2695ECE1BA5666E
equals the accepted win101-setup.zip PATCH/MOUSE.DRV byte-for-byte. Win31
SHA256D6BA5380A2EDCDB33E0C36850FB6BDD0542D78EC03E982DBD5114A462B0A2F1A
and source22D0A16DC701CA058BFC4357A155460C06187CBB0DDB6A3805735067CC19FCB9
match r015/r016/r017/r033's tested driver. Win31 is explicitly candidate;
normal reentry/enhanced cleanup remain open. Duplicate build/readme lines are
removed, not a driver semantic change.

Owner subsequently relocates all Win101 installer tooling to
tools/win101-setup; future Win31 setup is tools/win31-setup at S3. Independent
driver sources stay src/addon. git mv preserves modified/untracked files;
root derivation, component-test callers and current design/plan references are
updated. Historical run paths remain historical evidence, not live callers.
The owner explicitly authorizes deletion of src/interface/README.md, the
obsolete declaration-owner marker displaced by common/protocol.

apply-setup.cmd invokes the adjacent script, prompts for original media and
leaves output visible. apply-setup.ps1 reads the release driver/manifest,
preserves original root files, validates accepted/owned PATCH identities and
refuses unknown modified files before writes. Its PIF/SETVER support comes
from the accepted existing ZIP; no installer compilation or build path is
required. The advanced package entrypoint uses the same implementation, not
a second PATCH assembler. Installed PATCH keeps MOUSE.DRV alongside profiles;
actual Win101 still uses the original-Setup-embedded module. Existing plain
run16/PATH, pause, result, WORK cleanup and package independence remain.

verify-win101-release-setup.ps1 r040 correctly rejects a64-byte profile path.
r41 exposes a test expectation that omitted the existing call prefix; that
assertion is corrected to the exact call run16 adjacent-PIF command, not to
permit a hardcoded launcher. r42/r48/r50 pass unchanged-original-media,
repeat-apply, unknown-file preservation, modified-file refusal, corrupted
release-driver refusal and installed-PATCH identity/independence. These use
inert installed-file sentinels and never execute original Setup or Windows.
r043/r049/r051/r055 pass complete packaging with no WORK or root-media changes.

The old launch-profile fixture reveals a real Get-Command ambiguity when two
PATH run16 applications exist: concatenating their Source values is not a
valid executable. run-setup now selects the first matching application.
r054 passes all existing PIF/destination/three-refusal/result/WORK-cleanup
assertions with a PATH launcher fixture. r053 passes localized Win101 image
installation, immutable inputs and four malformed-image refusals through the
moved tool, without modifying the original O:/win101 image.

r055 refreshes assets/win101-setup.zip from the accepted original media with
current authored PATCH payloads, retaining a recoverable previous archive.
Every original root-file hash is unchanged, and no WORK is shipped. This is
package preparation, not new original-Setup/guest-execution acceptance.

New provider replays are retained as non-passes: r044 fails before the probe
on the known long TEMP path; r045/r047 with short O: TEMP fail at application
environment setup, before mouse-test execution. A restricted PATH containing
only O: and read-only C: system directories does not resolve it. No retry is
counted as a pass or used to claim driver correctness. Default CONFIG.NT and
AUTOEXEC.NT and the published NTVDM/NTCON hashes remain exact; attribution of
this new runtime prerequisite failure is not established. Prior source/binary-
identical provider results remain evidence, not successful new replays. The
driver stays candidate and S2 remains open. S3 is not admitted by this tooling
delivery. Task file writes/temp stay on O:, C: tooling is read-only, E:/X:/Y:
are excluded, and no subst is used.
