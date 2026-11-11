# T481 S16 Win3.1 mouse transition investigation

## Reproducer and baseline

S15 was accepted by the owner. S16 uses its accepted launcher and unchanged
published host package 092da729-caf9-4a89-8f2e-305e368faacf. The owner clarified
the minimal sequence: Enhanced mode, both addons enabled, start a windowed DOS
prompt, enter O:/WINNT/EDIT.COM interactively, then Alt+Enter to fullscreen.
Either DOSONLY choice can exhibit the defect; CAF is not a prerequisite.
Native EDIT.EXE is the requested comparison. The older graphical-return
double-pointer/menu failure remains related evidence, not a proved same cause.

The owner's subsequent exact six-step witness is DOSONLY=1 with both addons:
open the fullscreen DOS prompt in Console, CAF into KVM Window, Alt+Enter back
to the guest's windowed DOS prompt, run EDIT.COM there, then Alt+Enter into DOS
fullscreen while remaining in KVM Window. Movement and clicks still work;
the reported failure is cursor appearance. Preserve this presentation history.
The prior direct-fullscreen and windowed-first probes are separate controls.

## Initial observations, 2026-11-10

Private-desktop probes use owned installation copies and package identities,
not the owner's live desktop. Original guest binaries remain unchanged.

- The first custom PIF failed with insufficient conventional memory. Reusing
  the existing fixture's 128 KiB minimum / 640 KiB maximum fields corrected
  the probe. This was not a product mouse failure.
- COMMAND /c EDIT.COM produced one NTVDM in the observed run. That case does
  not establish coverage of interactive COMMAND's cmdExec launch path.
- Interactive input produced two NTVDM processes. A fullscreen-start case
  followed by CAF showed a normal text block; it did not reproduce the strips.
- The disposable edit08 probe completed three graphics/text round trips.
  Captured text frames differed from the initial frame at at most two cells;
  the captured final graphical frame showed the editor and a normal arrow.
  This starts fullscreen before the first editor launch, so it is not the
  owner's clarified windowed-first acceptance sequence.
- edit03/edit04 did not deliver their fullscreen readiness handshake. They
  are incomplete probes, not passes. edit05/edit06 rejected malformed probe
  input arguments before launch and are likewise not product evidence.

Raw observations reside in build/t481-s16. Each runtime runner stops and joins
its owned package and removes its marked O:/tmp installation/stage on exit.
The diagnostic intentionally holds the editor; its final timeout is not
normal-exit acceptance. Read-only mouse snapshots first compare the mapped
image's .text/.rdata/.data hashes with the corresponding link-map image.

## Retained owner scene and candidate, 2026-11-10

Read-only capture of the owner's NTCON 8148 reproduced the supplied screenshot:
two vertical character strips at approximately columns 0 and 40. The owning
Win3.1 NTVDM was 33848; nested EDIT.COM ran in NTVDM 21896. The published
NTVDM SHA-256 was
`0FBEFB8F20420C23F7615C7C01B9B5A3BD1655530B2C32342ACCB27A26A91447`.
Before reading state, the probe checked `.text`, `.rdata` and `.data` against
the image corresponding to its map. It did not write process memory or send
input to the owner's window. Captures are under build/t481-s16/live-owner.

The outer worker had BIOS mode 3, last BIOS-set emulated mode 103 (0x67),
`jap_mouse=1`, and actual EGA display state 0xC0 (text bit 0x80 set).
The nested worker had BIOS/emulated mode 3 and `jap_mouse=0`. The original
`mouse_io_interrupt` predicate treated the stale BIOS-set mode mismatch as
graphics-backed text. The software painter selected by the project's DIV-318
binding therefore wrote an EGA graphics cursor into current text planes.
Its 80-byte scanline stride explains the separated character strips; the
input delivery path need not fail for this rendering defect to occur.

DIV-899 adds the original `is_it_text()` hardware predicate to that existing
decision. It retains the fake-text path when VGA really is in graphics mode,
the original cursor algorithms and BIOS mode bookkeeping, with no new state,
command-specific exception or guest change. This is not a claim that the
original NT4 runtime shipped the same project software-painter binding.

The focused test extracts the real INT33 entry and exercises eight BIOS,
last-mode and actual-display combinations. The frozen pre-change source fails
the captured-state case; the candidate passes all eight. The existing EGA
cursor geometry regression also passes. These are unit controls, not proof
of the complete Win3.1 interaction.

Cached x86/x64 builds and all component links completed. The initial combined
runner exceeded CMD's environment command-line limit after repeated VsDevCmd
initialization; rerunning x64 in a fresh background environment completed.
No product workaround was added for this runner issue.

The private-desktop edit18-fixed run used the clarified DOSONLY=1 six-step
sequence. Its final rendered surface contains a single text mouse cell and
no strips. The outer worker retains BIOS mode 3 / last mode 103 / actual text
0xC0, but correctly selects `jap_mouse=0`. Its diagnostic controller holds
EDIT open and ends by owned cleanup: this establishes rendering/state evidence,
not normal shutdown. Synthetic private-desktop mouse delivery does not prove
the physical RDP input route.

## Pending

DOSONLY=0 transition, affected configuration/runtime regressions, Full gates,
coherent publication and commit/push remain pending. The graphical-return
double-pointer/menu report and native EDIT.EXE comparison remain to be checked.
No S16 completion or owner acceptance is claimed.

## Source-only transition review

The owner requested source review rather than further reproduction. No new
runtime probe is required for this review. These findings distinguish a
demonstrated rendering defect from an unproved reverse-transition cause.

The original bios.c BOP wrappers select NIDDB before INT33, mouse IRQ entry,
IRQ return, video interception and the enhanced mouse callback. The project
direct route callback also selects that original instance. Cursor position,
show count, saved background and cursor_EM_disabled are original per-VM
MM_INSTANCE_DATA; jap_mouse is instead a file-global derived cache updated
only at INT33. IRQ movement/return, enhanced callback and route changes can
reach cursor_display without that refresh. Do not introduce an independent
shim rendering state machine to compensate for this mismatch.

Original enhanced callback AX=2 removes the displayed cursor before setting
cursor_EM_disabled; AX=3 clears the flag before redisplay. AX=1 processes
motion/buttons and reaches mouse_update. Thus disabling painting is not
equivalent to suppressing input callbacks. A lingering block alone does not
prove the cause of lost clicks. Likewise, cursor_undisplay's disabled guard
is not independently proved erroneous: the intended AX=2 order erases first.

A separate configuration discrepancy exists: the inspected O:/w31/SYSTEM.INI
has mouse=*vmd but no device=*vmd. The established win-multitask/APPLY.CMD
explicitly normalizes that Win3.1 entry to device=*vmd; the insignia-386
contract explains why its INT2F query/BOP BF callback requires VMD to load.
The newer win31-setup/WIN.CMD only toggles the two authored drivers, omitting
that normalization. The setup-generation fixture starts with device=*vmd
already present, so it cannot detect this omission. Some dedicated Win3.1
mouse tests add the entry themselves, which must not substitute for testing
the shipped launcher. This is a source-proven missing setup transformation,
not yet proof that the reported reverse-transition click failure has this
sole cause. Do not change Win95 configuration rules by analogy.

Recommended bounded design: preserve original per-VM ownership and enhanced
callback ordering; fix actual-display classification at its original owner;
restore the existing Win3.1 VMD configuration contract in the shipped launcher;
and cover missing/wrong/already-correct entries in its tests. Inspect all
rendering entries for stale derived state before selecting a final cache
change. Never recompute the new painter and use it blindly to erase a saved
old background. Retain original text/graphics algorithms and callback semantics;
do not add guest-application checks, VM managers, retries or mode caches.

## Final derived-state repair and early publication

The final revision replaces the original file-global jap_mouse variable with
one side-effect-free expression at its existing uses, and replaces the INT33
assignment with its divergence explanation. This is two added/two removed
mirror lines, net zero, preserving physical line count and original CRLF.
No shim or addon source changes are involved. Original display, undisplay,
text-shape selection and EGA coordinate conversion use the same expression;
the latter still captures its scaling decision locally for paired restoration.
Original enhanced callback erase-before-disable / enable-before-draw ordering,
per-VM background state and original painting algorithms remain unchanged.

The new unit test extracts the actual production definition and INT33 entry.
It covers eight mode combinations plus graphics->text->graphics changes with
no intervening INT33 call. The frozen original source fails the captured text
case; the final source passes, as does the existing real-body EGA geometry
test. Commands run under the x86 MSVC environment in the owned background job:

```text
node test/mvdm-host/softpc.new/base/keymouse/verify-mouse-display-mode.cjs build/t481-s16/unit2-before build/t481-s16/mouse-before.c
node test/mvdm-host/softpc.new/base/keymouse/verify-mouse-display-mode.cjs build/t481-s16/unit2-fixed
node test/mvdm-host/softpc.new/base/keymouse/verify-mouse-ega-restore.cjs build/t481-s16/unit2-geometry
```

The first command is an expected negative, not a passing runtime test.
Cached x86/x64 builds completed; the final x86 rebuild/relink completed after
preserving comment line placement. Nine unchanged host images have identical
non-resource sections to the prior installed package. nthook32 differs there
only in its IMAGE_DEBUG_DIRECTORY timestamp (file offset 134228); NTVDM has
the intended compiled repair. All 11 receive the coherent new package identity.

The owner explicitly requested publication before further regression so manual
and isolated testing can proceed concurrently. Package
ac091ee4-c735-4934-afc0-1c1861fecd5d was published by owned background job 2984
to assets/release and O:/winnt/system32, with all 11 manifest hashes verified.
The existing W31 driver hashes and SYSTEM.INI were preserved by publication.
The earlier owner-authorized manual mouse=*vmd -> device=*vmd configuration
change did not by itself fix the reported corruption. No VMD-only fix claim
is made, and no setup script change is included in this delivery.

The prior one-predicate candidate passed all 16 selected Full gates. Its first
14-case matrix had 13 passes and one enhanced driver-on DOSONLY=0 timeout after
the command receipt. A same-frozen-source single-case control subsequently
passed both the published baseline and that candidate; the initial timeout
is retained, not relabelled a pass or assigned a proved cause. Two preliminary
subset harness attempts failed before running due to extraction/path mistakes;
these are not product outcomes. Final-revision tests use the original whole
matrix and a static installation copy, followed by Full. Results remain pending
in job/verify-final.out; historical candidate passes do not qualify this revision.

Final matrix update: runtime-final contains 14/14 passing records, each with
normalExit=true, and all six scheduling assertions pass. The source was frozen
with the existing copy helper before owner testing continued. Both O:/tmp/q31
and O:/tmp/s16sealed were removed through the ownership-aware helper.
The final original whole-matrix runner exited 0. Full remains in progress.

An additional negative uses the retained first candidate's actual INT33 body:
all mode-entry cases pass, but the new no-INT33 transition assertion rejects
its stale cache. The final derived expression passes that assertion. This
distinguishes the final repair from merely adding a guard at INT33; it does
not claim a newly reproduced physical-input session.

## Owner feedback after publication

The owner confirms the coloured-strip failure is fixed. Movement/clicks and
the fullscreen text cursor now work, but each fullscreen->window transition
leaves another cursor-sized coloured cell that survives further transitions.
This is an unresolved background-restoration issue, not full S16 acceptance.
Source review identifies that cursor_undisplay dispatches using current mode
and software_text_cursor_undisplay recomputes the address using current
VID_ADDR and current text geometry, while storing only the old position and
cell value. These are concrete dependencies to audit against transition order;
the owner's residue has not yet been causally assigned to one of them.
Do not add screen repaint, application exceptions or another renderer to
conceal it. Source review continues without new reproduction attempts.

Final Full failed Window-17 nested-mem: CAF reported
result=fail stage=find-kvm error=203, window-key-message-count=0 and
scripted-console-input=failed; timeout exit 0x53504354 is not expected guest
result 1. An identical Full repeat refused to start while the owner's formal
NTSRV 18460 was live. Preserve that owner acceptance session. No final Full
pass or completed commit/push is claimed; earlier Full passes apply only to
the earlier candidate.
## Residue: restoration ordering audit

The current installed WIN386.EXE still matches the previously extracted retail
VMD image: SHA256 c67e667e25e91c65efb5acddec437da8b586037da7de502228ca8fbd806cf3f8.
The retained build/t481-s3/vmd074 object-1 bytes show the focus path at 0x8c9:
0x8d2 exchanges the selected VM, then 0x8e1 and 0x8ed call 0x3fd for the old
and new VMs. At 0x444 that routine uses VMM Call_Priority_VM_Event (0x14),
with callback 0x484 and ECX=3. The callback selects AX=2 or AX=3 at
0x4ba..0x4d0, then calls the registered far callback through services 0x4c
and 0x85. The admitted local vmm.inc identifies ECX bits 1/2 as waiting for
STI and outside the critical section. Therefore a focus notification is not
proof of a synchronous pre-video-change erase. Disassembly after INT20's
inline service dword must be decoded at instruction boundaries; the raw
listing's interpretation of service data as instructions is not evidence.
This proves the notification mechanism, not its ordering against the VDD
screen save in the owner's failed transition.

NIDDB swap_NIDDB only updates master instance pointers. BIOS BOP BF selects
that instance before mouse_EM_callback. Adding arbitrary cursor erasure to
NIDDB switching would mix global video ownership with VM state selection and
is not justified by this audit.

An isolated compiled check (build/t481-s16/restore-order, background PID 41980)
extracts the current production software text display/undisplay bodies using
the existing unit runner. The existing control passes. A second fixture proves:
erase before VID_ADDR changes restores the painted cell without touching the
new page; erase after VID_ADDR changes leaves the painted old cell and writes
its background over a different cell on the new page. No guest was launched,
no live scene was sampled, and no production source was changed. This is a
function-level ordering witness, not a reproduction of the owner's Win3.1
transition or proof that VID_ADDR actually changes in it.

The derived jap_mouse predicate fixes choosing the current painter, but it
also remains used by cursor_undisplay. A text-painted save area followed by
a graphics state with a BIOS/emulated-mode mismatch selects the EGA restore
instead of the text restore. The existing save-area boolean does not identify
the painter that produced the saved background. Before implementing a repair,
resolve the VDD screen-save ordering and virtual video address ownership;
saving an old painter/address alone must not be claimed to remove a cursor
already copied into the guest's saved window surface. S16 remains open.
### Actual restore dispatch witness and VM-memory distinction

The current text-cursor bodies and actual cursor_undisplay body were extracted
into build/t481-s16/restore-order/dispatch.c + dispatch.h. Background PID 29048
compiled and ran both controls successfully. With BIOS mode 3, last emulated
mode 0x67 and hardware text mode, display/undisplay restores the original cell.
Changing only hardware state to graphics before undisplay invokes the EGA
restorer, consumes save_area_in_use, and leaves the text cell painted. The
fixture stubs the EGA restorer to record selection; it does not pretend to
validate physical EGA writes or to reproduce the integrated guest session.
This establishes a dispatch defect, separately from the earlier page-offset
ordering witness. No product code was changed in this investigation step.

A relevant correction to the address hypothesis: sas_storew -> c_sas_storew
-> bios_write_word is a linear, paging-aware write. When paging is active it
uses xtrn2phy and, on a failed direct translation, the original biosDoInst
virtualization path. Therefore text restoration must retain this guest-VM
linear-memory path, not be replaced by a raw physical framebuffer write.
EGA_graphics_cursor_undisplay instead writes EGA_planes directly and marks
the affected video range. Selecting it for a text save area not only chooses
the wrong representation, but abandons the correct VM-memory owner.

The installed V7VDD.386 matches the retained analysis SHA256
909a5e6c313daf4de4559171e211ef9ae7c5f15d10194f091a91be8a352f0275. Its focus
handler at object-1:0xa0 handles EDX=0/0xa and exchanges its selected VM at
0xab, independently of the VMD focus path (EDX=0/0xc). No proof of their
cross-driver callback/snapshot ordering is claimed from those entry points.

Bounded repair direction: attach the text background's original linear
address/type to its existing per-VM save-area ownership, and restore through
the text function even when the physical adapter now displays Windows graphics.
Do not add a second input/renderer/VM state machine, force an early VMD call,
change VMM scheduling or bypass SAS paging. Text address provenance is save-area
metadata, not another cache of the current video mode. Every graphics painter
must retire that text tag when it owns the save area. The expected control is
that text->graphics-state restoration still restores the original text cell;
normal text motion, graphics save/restore and repeated VM transitions must
remain covered. Actual integrated residual-free transitions and the final
Full gate remain unverified; this is not a publication/closure claim.
## Unpublished saved-background candidate

DIV-900 now records a painter kind and text linear address in original
MM_INSTANCE_DATA. The existing save_area_in_use remains the sole validity
flag. All four software painters establish their kind; restoration consumes
that flag once and dispatches to the matching original function. This removes
the current-mode reclassification, including the reverse graphics->text case.
The first text-only draft was rejected during self-review because it left the
reverse direction using current-mode dispatch. It was never built into or
published as a product package. The unified candidate replaces that draft.
The original hardware text cursor has no saved pixel area and needs no kind.
No original guest, addon or launcher was changed.

Background unit job 25760: pre-residue production source fails at the exact
text-to-graphics restoration assertion (line 41); current candidate passes
text motion/masking, original-page restoration after a page change, duplicate
restore suppression, and EGA/VGA/CGA destination checks. Graphics destination
checks use stubs and are not visual validation. Existing extracted EGA geometry
and eight display-mode cases also pass. The EGA fixture now additionally
checks that the actual painter writes its provenance. No live reproduction
was performed. Current assets/release and O:/winnt still contain the previous
published package, not this new source candidate. Cached product build,
coherent delivery and runtime acceptance remain outstanding.
The first cached-build wrapper (PID 6728) terminated with exit 1 during CMake
regeneration: nested VsDevCmd initializations exceeded CMD's input length.
Focused controls had already passed; no product compiler result was reached.
The replacement owned wrapper PID 11580 uses separate compiler environments
and a task-local PATH containing resolved build tools. Its result is
job/residue-build2.result.json; inspect the live PID before any retry.
No current published image was changed. Before packaging, append the new
MM_INSTANCE_DATA metadata after cursor_EM_disabled, preserving all existing
field offsets for retained state probes; do not edit a source/header while
the current build is reading it. Original NIDDB size-based allocation/copy
must continue to include the appended metadata. This is pending self-review,
not an additional runtime state machine or a claim of build completion.
## Saved-background candidate build and publication

Cached x86/x64 build PID 11580 completed with exit 0. Self-review then moved
the new metadata after cursor_EM_disabled: all pre-existing MM_INSTANCE_DATA
field offsets are retained. NIDDB allocates using sizeof(MM_INSTANCE_DATA),
and INT33 save/restore serializes MOUSE_CONTEXT, not this host-only extension.
The final-layout cached build/staging PID 9800 completed with exit 0. The
inherited wrapper scope string mentions focused units; those unit results
belong to the preceding owned unit jobs, not a second execution in build3.

Publication PID 20852 completed with exit 0. Complete package
2fef3b20-3b5c-4466-b701-feb9ac9101c9 is hash-verified at both assets/release and
O:/winnt/system32. NTVDM SHA256 is
1E2C65FAD721A319E6ED38B30376FD698C286D5F0EFECB4B11ACF77756E617AD.
The owner had requested early coherent publication for parallel manual testing.
There were no formal product processes at the publication check; this helper
refuses publication if one starts rather than terminating an owner session.
O:/w31/PATCH's two addon hashes match released assets; SYSTEM.INI is unchanged.
No guest media or winnt.zip was changed. This is still a candidate, not S16
closure or an assertion of integrated residual-free operation.

Owned verification PID 24648 runs verify-residue.ps1 against candidate-residue:
existing whole Win3.1 generated-profile matrix, then Full against stamped
cache-residue. Outputs are runtime-residue and full-residue. Inspect this PID
and its recorded runtime/full exit files before any restart. The previous
revision's Window-17 failure remains in the record; new results must actually
pass before replacing its unresolved qualification status. Commit/push pending.
## Owner acceptance and optional atomic-transition assessment

The owner manually accepted package 2fef3b20-3b5c-4466-b701-feb9ac9101c9:
residue no longer persists/accumulates and mouse movement/clicks are normal.
The owner clarified that erasure is visibly completed after the switch, not
that every intermediate transition frame is free of the old block. Record
that distinction; do not claim flicker-free atomic transitions.

The owner then requested an assessment of erasing together with the switch,
explicitly allowing omission if inconvenient or contrary to original semantics.
The original mouse_EM_callback AX=2 already orders erase before disable, and
AX=3 orders enable before draw. The retail VMD requests a priority VM event
with Wait_For_STI and Wait_Not_Crit; the callback is not an unconditional
synchronous pre-VDD-screen-save hook. Its original eligibility conditions
must not be bypassed. The authored INSIGNIA companion exposes the original
callback and instance ownership; it does not own display focus or a second
mouse scheduler. BIOS mouse_video_io has a pre-mode-change erase path, but
VDD direct register restoration need not go through that BIOS entry.

The clean boundary remains: guest VMD owns cursor visibility transitions,
original per-VM mouse state owns restoration, and NTCON presents supplied
frames. NTCON route edges describe Console/KVM Window presentation, not guest
Win3.1 DOS-window/fullscreen transitions. Inferring a guest transaction from
Alt+Enter, hardware mode bits or CAF could select the wrong VM or miss other
switch methods. A guarantee of no visible intermediate block would require
an actual guest transition begin/complete contract and either coordinated
pre-save cleanup or holding presentation until cleanup acknowledgement.
No such existing contract was found in this chain. Adding it for this visual
transient would add cross-layer state and is not recommended. No timing,
sleep, forced guest callback or frame-suppression patch was implemented.
This is a bounded architecture assessment, not a claim that original Windows
must always visibly delay or that every possible upstream ordering was proved.

## Final verification and closure

This section supersedes the pending statements in the chronological record.
The owner accepted both repairs and agreed not to add an atomic switch barrier.
Final cached x86/x64 build/stage and coherent 11-image publication succeeded.
Owned verification PID 24648 completed: runtime-residue 14/14 cases and
full-residue 16/16 selected gates; both exit files are zero. Full explicitly
skipped DOS4GW-NESTICLE because its external fixture was unavailable. The
prior Window-17 failure is retained as an earlier run, not a final failure or
a claimed independently diagnosed defect. No original guest was rebuilt.

The final mirror delta against the S15 baseline is +43/-86 (net -43 lines)
across mouse_io.h and mouse_io.c, including DIV-899 and DIV-900. Host-only
metadata is appended after the original instance fields. Existing original
painters/restorers and the single save_area_in_use validity flag are reused.
No addon, frontend barrier, second cursor scheduler or app-name case was added.

Setup review correction: the earlier inference that mouse=*vmd fails to load
VMD was not established. Microsoft's original KB Q88543 explicitly lists
[386enh] mouse=*vmd as the normal Windows 3.1 configuration:
https://jeffpar.github.io/kbarchive/kb/088/Q88543/
The unpublished normalization draft and its associated test/README edits were
withdrawn. Its additional setup-final run passed, including another 14-case
matrix, but that is not used as proof that normalization is necessary. The
accepted S15 launcher is preserved byte-for-byte. The earlier owner-authorized
O:/w31 device=*vmd setting remains unchanged; its successful manual test does
not prove the original mouse= setting invalid. No new setup normalization is
part of this closure. Repository setup contract/round-trip/runner checks are
rerun against the restored launcher and recorded separately.

Persistent residue is fixed; visible intermediate cleanup is an accepted
boundary, not claimed atomic rendering. Win95-specific coverage remains
outside this S. T481 remains open; no subsequent S is admitted here.

Final closure job PID 8088 completed with exit 0. The restored S15 launcher
passed six-profile generation, CMD apply/repeat/unapply and startup/exit-code
checks. Setup/run16 were shims in these script checks; the separately recorded
14-case runtime matrix supplies real runtime evidence. Documentation governance,
relative links and diff whitespace checks passed. All 11 hashes match both
publication destinations, and O:/w31/PATCH/WIN.CMD equals the repository file.
