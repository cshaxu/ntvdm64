# T435 S2 — Independent Win1.01 INT33 Driver

Owner authorizes independent guest-driver implementation at
src/ADDON/Mouse Driver 101. S1 design commit7466a69b1 is pushed; S2 is active.
No original guest/core/media or host production code is changed. Installed
T434 ten-image package remains unchanged. Prototype is not published.

Independently authored mouse101.asm uses actual retained Win1.01 consumer
contract from [S1](m0-t435-s1-win101-mouse-audit.md), not copied original driver
implementation. NASM3.01 emits an isolated16-bit NE driver, no CRT/host link.

Build command:

    powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/build-win101-mouse-driver.ps1 -BuildRoot build/M0-T435/S2/r002-driver

r001 failed on missing NASM whitespace around modulo; corrected in r002.
r002 passes assembly and NE structure/three entrypoint checks. build.json
records assembler/source/output hashes and exact segment/entry layout.
This is build evidence only, not Windows loader or guest execution proof.

INT33 callback uses actual Win1.01 absolute-position bit15 plus provider
event bits0..4, normalized positions and two buttons. It queries current
geometry, does not replay cumulative SI/DI and changes no PIC/vector owner.
Enable/Disable use bounded4096-byte provider state and restore the prior
callback. Repeated Enable updates the Windows event pointer only.

Pending: executable mock/actual INT33 guest tests; Windows Setup/loader in
a separate build-owned installation; real movement/click/release/menu/exit
and repeat launch. Missing-provider, oversized-state/rollback, geometry,
repeated Enable/Disable and callback RETF remain untested. No runtime pass.
InPort defects and pre-existing Invalid handle error remain independent.

## Executable guest tests and implementation simplification

r004 mock prototype passed36 checks under the current T434 CCPU40 package
in r005-guest. The mock is a disposable COM INT33 provider and restores its
original vector before exit; it is not shipped. It executes the actual shared
driver routines, not a rewritten test copy; this is not NE loader proof.

r007-real against the unchanged real provider failed assertion10/full state
comparison with exit10, and the strict verifier rejected it. Source review
shows inherited SaveState writes checksum0 while RestoreState sums the cookie
and context and rejects mismatches. The matching SoftPC owner has the same
behavior, not a usable correction. No host/mirror fix or checksum patch is
introduced. The fixture remains strict.

The absolute bridge changes only callback and pointer hide count, so full
state snapshots were unnecessary. r008 removes the4096-byte buffer and
Save/Restore/ReadMotion calls. Enable exchanges callbacks and hides once;
Disable balances its hide and restores the old callback. No range, ratio,
position or motion counter changes are made. Repeated Enable/Disable do not
add hide/show operations. This supersedes the initial snapshot proposal above.

Build r008 then passes both current-source runs:

- r009-real: MOUSE101-REAL-11-PASS, target exit0; verifies actual provider
  registration/old callback and entire saved state equality after Disable.
- r010-mock: MOUSE101-MOCK-36-PASS, target exit0; missing-provider refusal,
  callback update, endpoints/normalization, button bits, disabled-provider
  input, no empty callback, balanced hide/show and repeated Disable.

Entrypoint is tests/observation/verify-win101-mouse-driver-guest.ps1 with
DriverBuild=build/M0-T435/S2/r008-driver,
PackageInput=build/M0-T434/S5/r037-runtime,
Observer=build/M0-T425/S9/r033/console-startup-observer.exe,
BuildRoot=the run above, Provider=Real or Mock. Each run uses a private copy,
private desktop and Z: removed in finally; the sealed package and original
Windows installation are untouched. The old W: product processes were pinned
and stopped under owner standing authority to release BaseSrv; W: was not
created/removed by these tests. Reports retain output and actual exit code.

Real Windows Setup/NE loading, mouse input IRQ to Windows, menu/drag/release,
normal exit and repeated start are still unverified. These tests do not close
S2, certify the new driver or replace real Windows acceptance.

## Original Setup route and diagnostic boundaries

Owner asks why a driver change would require reinstalling Windows. No
standalone in-place driver-reconfiguration mode has been proved. The original
Microsoft [KB Q28502](https://msarchive.pcjs.org/kb/Q28502/) specifically directs
Windows1.01 users to copy the updated MOUSE.DRV onto the install disk and
reinstall. The earlier response suggesting an established minimal driver-only
Setup mode was premature. Continue investigating the smallest original Setup
build/copy path, only in a disposable installation; do not patch WIN100.BIN
or overwrite O:/Windows. Original Mouse/WIN100 hashes remain unchanged.

Test-only win101_install_write_guard.cpp restricts disk mutations to the
build-owned copy; its private relink never enters publication. r012/r018 reuse
the exact baseline NTVDM formal link inputs plus the guard and the existing
project-pinned Detours source. Build metadata records inputs. This is diagnostic
instrumentation, not ordinary-product acceptance. r013 refused host TEMP and
Windows-directory scratch writes. r014/r015 physical long TEMP paths still
hit the original temporary-file initialization error. Z:/TMP resolves that
known path limitation without production changes. No Ignore was selected.

The realtime Console probe initially attached to the outer observer, not its
private-desktop child; consequently it read the wrong empty Console and CAF
missed the frontend. Original observer source confirms the CREATE_NEW_CONSOLE
child. The controller now pins that child's process/image and keeps its handle,
while the private desktop name remains derived from the outer parent's PID.
This is a test correction, not a product input/rendering regression finding.

r020-configure observes the real Setup welcome and sends C only after that
node. The next page asks for the destination directory, initially C:/WINDOWS;
no directory confirmation was sent and no original installation was updated.
The guard also refuses C:/WINDOWS/SYSTEM.INI mutation requests; these retain
the run as a non-pass until attributed. Their refusal is not silently waived.
No new-driver Windows load or mouse success has been achieved yet.

## Owner-authorized binary installation investigation

Owner now explicitly permits binary modification to install the independent
driver, asking to compare Setup's actual changes. Preserve originals and use
recoverable copies; no runtime hot patch or unrelated guest-core rewrite.
Source policy and active brief record this narrow superseding authority.

Read-only original Setup before/after images provide concrete layout evidence.
In both WIN100.BIN variants the MOUSE NE header begins8810h. Old two-segment
Mouse3239 has next DISPLAY at8DD0h; new three-segment Mouse3358 has DISPLAY
at8F60h. Embedded NE+8 replaces the original standalone checksum with the
next module paragraph (08DDh/08F6h). Embedded segment offsets become absolute
file paragraphs, alignment changes from standalone9 to4; nonresident names
move into the module block. Setup also changes segment minimum allocation.
Later modules move, and the EGA change independently affects later offsets,
so whole-image byte differences are not mouse-only attribution.

Potential simpler installation: the authored driver is smaller than the
current1872-byte MOUSE block. Replacing only that block while preserving its
next-module pointer and every other module offset may avoid global movement.
Must first validate loader handling, exports, allocations and any overlay
references; not yet an implementation or a runtime pass. New/old segment
counts differ, so plain same-offset raw DRV-file copying is not sufficient.

## Localized image installation and real Windows mouse evidence

install-mouse101.ps1 now validates the original ascending NE module chain and
zero terminal, bounds the MOUSE slot, rejects imports/relocations in the new
driver, embeds its header and two segments at16-byte boundaries, converts
segment locations to absolute image paragraphs and preserves the old next
module pointer. It checks every byte outside8810h..8F60h remains identical,
image length stays183216 and all original module offsets/names/links survive.
r024-image output SHA256 is
BA4141551E8B9F1433DF141F6540F4F3DDBAB102191BBF5DE1423E9DBD58FEA9.
Original O:/Windows input hashes remain unchanged. No runtime patch.

prepare-win101-image-test.ps1 creates a private Windows copy using the normal,
unmodified T434 NTVDM (hashCA8C7AC27A406ABF612927F4C485D58E413D0A686462FA4EEA7130F8ACB6F212)
and the localized image. It uses the original, already configured SETVER
device for WIN100.BIN/DOS3.30, hash
7D8B0765C302358117186D93283AB70D2D0DC72852757AFB4CFEECA69FD74653.
No invented DOS-version host provider. Configuration changes are private.

r025 captures the original startup logo. r026 capture-1 progresses to the
color MS-DOS Executive with a Windows arrow, no serial-device error dialog.
These are actual PrintWindow captures from the private frontend, not mock UI.

The existing private-mouse-input.c hook is compiled x64 against the exact
current NTCON lib headers. It invokes the production input sink on the actual
private Window thread. Controller corrected its SetThreadDesktop/HOOKCONTROL
boundary; initial r029 probe failure remains non-pass. This tests downstream
frontend -> worker -> INT33 -> new guest driver -> Windows, not physical
Raw Input, clipping or RDP behavior. No hook enters product publication.

r031 mouse-corner/file/down/up each gets the frontend FIFO acceptance marker;
more importantly the actual images show the arrow moving to File, its menu
opening on down, then closing on release. Thus results are not inferred just
from successful posting/acceptance. r032 opens Special and confirms its normal
End Session item. Images and input reports are under those build runs.
Normal End Session/return and repeated-start acceptance remain unfinished;
holding a menu until observer timeout is not normal guest exit. S2 stays open.

r033 reaches the real End Session confirmation dialog. r034/r035 move onto
OK and send down/up; the guest Windows display disappears and mode becomes3
(text). r035's readonly CCPU trace reports CS95EB/IP03C4 with three stable
samples. Exact20-byte opcode matching finds COMMAND.COM file offset2754h;
disassembly identifies its original BOP54:01/GetNext command boundary.
The remaining #32770 worker dialog is invisible in the window metadata;
black PrintWindow capture alone is not evidence of an active error box.
No visible Invalid handle dialog is established by this run.

However run16 still does not complete before the observer timeout. Thus
Windows guest exit/text-mode return is observed, but launcher receipt and
normal interactive DOS return are not yet accepted. Investigate record/
completion/parent restoration rather than equating BOP wait with success.
No cleanup is counted as guest exit. No lifecycle repair has been made.

## Normal-return failure localization

r036-batch-return uses the unchanged production NTVDM and the same patched
mouse module. End Session returns to BIOS text mode, but neither the batch's
MOUSE101-WINDOWS-RETURNED nor MOUSE101-DOS-RETURN-COMPLETE marker appears.
The retained timeout native stack, adjusted for actual image base00BE0000
against preferred00400000, places nt_block_event_thread at4707F4: its call
to DisplayErrorTerm after NtvdmConsoleUpdateText failed, source nt_event.c1494.
The visible dialog reports invalid handle. This occurs before ResetConsoleState
and before cmdGetNextCmd reaches GetNextVDMCommand to report the direct result.
Thus worker residency is not the explanation: the required WIN.COM receipt
has not been reached. No synthetic completion or forced worker exit is used.

The text publisher can return invalid handle when its cached resolved text
palette is absent and GetPaletteEntries cannot read the retired graphics
palette. This is a source candidate, not a proven runtime subcause; transport
failure must still be distinguished. r037 replacement-object and r039 Detours
diagnostic relinks compile but their r038/r040 observations fail during
environment setup, before the Windows frontier and without a palette trace.
r041-short-trace repeats with a short Z:/TMP trace path and fails at the same
startup frontier. These diagnostics are non-passes, never production artifacts
or evidence that the palette candidate is confirmed. Further localization must
use the unchanged image or first establish a diagnostic's equivalent startup.

r042 builds a read-only x86 palette-cache snapshot probe. Its private layout
is restricted by the runner to the exact unchanged T434 NTVDM SHA above and
the probe checks the selected PID's image path before reading. No process
write, suspension, input or production dependency is introduced. Runtime
palette evidence is still missing: r043 direct and r044 batch both encounter
the startup environment dialog with the unchanged NTVDM; original Windows
input hashes match r036. Therefore diagnostic relinking is not established
as the cause of that newer startup failure. r044 cleanup reported a transient
remaining process row; subsequent authoritative inventory found no live
product processes and Z: was removed. r045 initially refused an occupied
endpoint before starting; after that endpoint disappeared it ran and retained
the actual30-second timeout, not a pass. Its native stack (base00A60000,
cmdGetNextCmd preferred00659560/+6B0) identifies the failure after
GetNextVDMCommand(&VDMInfo), cmdmisc.c264, rather than environment conversion.
This is a separate startup/request failure to resolve before final-palette
sampling can establish the original normal-exit subcause. Neither repeated
attempts nor explicit cleanup prove normal completion.

## Confirmed palette cause and minimal adapter repair

r048 diagnostic hooks preserve actual I/O/GetNext/cache/GDI results. r049
reaches Windows; r050 exercises the complete mouse/End Session sequence.
The final failure is now observed directly: text-cache result0/error232
(ERROR_NO_DATA), followed by GetPaletteEntries(NULL) result0/error6. The
retired graphics palette is NULL and no text palette tick populated the copy.
This diagnoses project-added final-frame extraction, not an original guest
mouse defect or a reason to synthesize the completion receipt.

The repair calls existing original nt_graph.c::set_the_vlt through
mvdm_softpc_text_video_refresh_palette on the original video owner, only in
settled text mode and only when the copied palette is absent. That original
function resolves the real DAC/attribute registers and populates the existing
copied palette boundary. No duplicate mapping algorithm, default colour table,
transport change, suppressed error or OpenNT/MVDM mirror edit is introduced.
Both text-frame and text-configuration packers share the same helper; other
cache errors and genuinely unavailable palettes retain their failure rules.

r051-palette-repair clones the sealed formal cache and builds the affected
x86 /MT closure. softpc-text-video-test and console-text-producer-test pass,
including mode-change/graphics refusal, absent-palette recovery, real palette
content and retained failure cases. r052-repaired-return uses that uninstrumented
repair plus the authored Windows mouse driver. The real End Session down/up
sequence completes; subsequent output contains MOUSE101-WINDOWS-RETURNED,
MEM's conventional/extended-memory output, and MOUSE101-DOS-RETURN-COMPLETE.
This proves the native batch continuation across the Win.COM direct completion
boundary and subsequent real DOS execution, not merely observer cleanup.
The exact launcher result and remaining repeat/ordinary-package gates must
still be reviewed before S2 closure/publication.

r052 records result=exited/exit00000000 with uninstrumented repaired NTVDM
SHA EF533EE8C0DE26E33774039CE5D17B834E23B39B95F6C0A5934F1BAEC4388AD0.
r054 repeats actual completion and MEM but exploratory post-exit memory reading
races legitimate worker retirement and makes the script fail. This test
non-pass is retained. Strict acceptance now separates that diagnostic reading
from exited/zero plus ordered output assertions; r055 passes the strict gate.
Batch sources subsequently gain direct WIN.COM/MEM error propagation; the
prepared same-worker repeated scenario must validate that strengthened version.

r056-installer-tests passes localized installation and rejects bad MZ,
nonascending/cyclic module linkage, truncated image and imported addon.
Original WIN100.BIN/driver hashes stay identical; rejected inputs produce no
installed image. Earlier invocations supplied nonexistent fixture names and
failed before any case; neither is counted as a test pass.

r057 product runner fails before WOW execution because Windows PowerShell5.1
has no ProcessStartInfo.ArgumentList. r058 uses PowerShell7, the actual
prerequisite. Its Console17/Window17, three independently compared retained
WOW frontiers, RPC lifecycle and native GUI gates pass. The overall gate fails
at version negatives: APP0.0.434 does not match admitted T435. No publication
or overall-pass claim is made. APP is advanced to0.0.435 without protocol change
(RPC45/I/O25), and r060 builds the dependency-selected coherent ten-image
candidate for fresh strict-repeat and full product verification. Unchanged
original WOW32/VDMREDIR remain the baseline artifacts, not new source fixes.

r060-release completes all seven selected cache builds (x86 NTVDM/Hook32,
x64 service/Hook64/worker/frontend/launcher/monitor). release-manifest.json
records the ten-image candidate; application identity is0.0.435 and the
unchanged wire contract remains RPC45/I/O25. r061 does not start a product:
the observer rejects120000ms outside its existing allowed budget (exit68,
result=not-started). Repeat uses its admitted60000ms bound instead; no product
timeout or observer contract is weakened.

r062-repeat-release passes strict repeated execution in the new package.
REPEAT.BAT propagates WIN.COM/MEM nonzero results. A fresh FIRST.DONE marker is
written only after the first WIN.COM and actual MEM have completed successfully.
The test retains an actual NTVDM process handle and checks it is still alive
at second startup, preventing PID replacement from satisfying worker reuse.
Both cycles use the full real mouse sequence. Second-cycle captured File menu
opens on down and closes on up, with the Windows arrow visible. The final
report is result=exited/exit00000000 and ordered Windows-return/MEM/continuation
output. Private desktop images are actual PrintWindow data, inspected directly;
input FIFO acknowledgements alone do not establish UI behavior. Final package
full regression r063 is running; no publication/P closure claimed yet.

## Final acceptance and publication

r063 completes all three product groups (WOW frontiers, Console17, Window17)
but its RPC bootstrap fixture still embeds APP434 and is correctly rejected
by APP435 with1306. This is retained as a failed run. Rebuilding the actual
fixture targets fixes their identity, not a product retry or waived assertion.
An idle manual Ninja invocation is stopped by exact PID/path/creation identity;
the existing generated-cmd build style completes. The release recipe now also
builds its control fixtures, preventing this stale-cache mistake on replay.

r064-Control passes all11 remaining gates, including seven RPC cases, native
GUI, five version/interface negatives, strict DIR, modern EDIT return, cooked
return, rapid/interactive relaunch, isolation, retirement and nested Window
handoff. r063/r064 manifests contain identical ten product hashes. Publication
requires exactly the three successful r063 product gates plus all11 successful
r064 control gates; the earlier failed RPC entry remains in its raw record and
is never relabelled. This dependency-selected reuse preserves all14 assertions.

r066-real-final/r067-mock-final pass11 actual INT33 and36 guest-mock checks
using the final APP435 package. Current ASM source hash matches r008's build
manifest0999EA9631BBAE52DAFADFE0A6E4DD8B946DF8136A5D42869573A08C259CAA8E.
Callback/register safety, normalization, disabled/missing provider, repeated
enable/disable, old callback and balanced hide/show are covered by these tests;
real Windows loading/movement/menu down/up and same-worker repeat are covered
separately by r062, not inferred from mocks.

r065-publication deploys the exact accepted ten images to O:/winnt/system32;
published.json records hashes and accepted-gates.json records each of the14
gate sources. Recovery copies of the prior T434 ten images are retained in
r065/recovery. Original guest media/configuration are not overwritten.
r068-smoke actual deployed DOS VER, native32 echo and native64 echo each
produce their expected output with exit0; all ten deployed hashes still match.
Actual smoke logs are O:/winnt/Logs2/t435-s2-r068-smoke-{dos,native32,native64}.txt.

No new host process/component/channel or mirror change is shipped. Diagnostic
probes and private input hooks are test-only, excluded from publication. The
authored driver and recoverable image installer are delivered as source/build
artifacts, not installed over O:/Windows. Original Windows core/media hashes
remain preserved. Physical Raw Input/RDP capture, dragging/general Win1.01
application compatibility and inherited InPort/checksum defects are not claimed
as fixed; the driver accepts the owner-selected INT33 contract. S2 meets its
implementation/test/publication boundary; T435 awaits owner acceptance.

Production/source/test/evidence P is263118b33, pushed to main. Documentation
governance and staged diff checks pass; original mirror paths have no changes.
The final status stamp is a documentation-only follow-up, not another product
package or an additional runtime-capability claim.
