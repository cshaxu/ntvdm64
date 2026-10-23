# T448 S11 — CAF and WOW startup investigation

Owner reports `run16 command` followed by CAF does not show its window, and
`run16 sol` shows neither WOWEXEC nor Solitaire. These are separate acceptance
targets; a failure before the COMMAND prompt does not prove a CAF failure.
No product or guest binary was changed or published during this investigation.

## Baseline and controls

All thirteen installed `O:/winnt/system32` release images match the release
manifest. The installed SOL and package SOL have SHA-256
`68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`;
installed COMMAND and package COMMAND have SHA-256
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
The installed CONFIG.NT and AUTOEXEC.NT differ from the test package, including
owner Windows 1.01 SETVER configuration. They were neither changed nor
executed by the isolated-package controls.

On the same S9 release package, `caf` with a controlled PATH confirms COMMAND
prompt, CAF visible-window ownership, natural return and echo. `sol-r3` uses
a private desktop and verifies a visible SOL window, stock draw, Deal and
normal close. The SOL test manifest-array bug and the diagnostic builder's
handling of Ninja implicit dependencies were corrected in test tooling; neither
is a product repair. Both test outcomes are controls, not proof the owner's
actual reported paths are repaired.

## Inherited-environment reproduction

On the active Default desktop, the S9 package with the controller's inherited
PATH reproduced SOL startup failure twice: no Solitaire or WOWEXEC window,
and the actual NTVDM illegal-instruction dialog identified real-mode
`05d8:213b`, bytes `63 68 61 69 6e`. Controlled short PATH on the same desktop
showed both WOWExecClass and Solitaire windows. First-half and second-half
PATH controls separately showed both windows. An independently constructed
single 1000-character path element also caused illegal instruction; 500 and
250 character elements and thirty 50-character elements showed the window.
These are bounded controls, not a general maximum or an approved truncation.
Raw private test output is in `build/t448-s11-regression` until scratch
disposition; values of inherited environment variables are not recorded here.

The controlled COMMAND CAF test passed; with inherited PATH its test instead
failed **before the prompt**, so it did not reach CAF and does not explain the
owner's specific post-prompt/window observation. The test package shares the
product broker endpoint; whether the owner's earlier launches coincided with
the prior isolated testing is unknown. No contention is asserted as cause.

## Original guest ownership and exact observed write

The pre-existing [TODO](../../states/TODO.md) and
[T447 source investigation](m0-t447-s3-control-source-audit.md#follow-up-environment-writer-attribution-and-class-collection-repair)
already identify the same immutable COMMAND `EndInit` limit; this investigation
independently reproduced it with today's environment. The diagnostic-only
owner wrapper logged initial environment calls without recording their values:
DS=05d8, ES=0683, 016f paragraphs allocated; DS:203c (`EnvSiz`) lies inside
that allocation. Before the real environment copy `EnvSiz=016f`; immediately
after the copy it is `464f`, while the segment's MCB and initial stack remain
otherwise consistent in this bounded observation. The shipped binary's original
instructions at 03c7 and 03cf store and compare DS:203c around the environment
service; the subsequent failure branch reaches the original INIT error path.
Current source `rdata.asm` and its `command.map` match these instruction
locations. The earlier host DIV331 WOW-kernel environment capacity check is a
different, later boundary; the WOW command-delivery diagnostic was not reached.

This is an original guest lifetime defect in this tested route, not proof of
a host out-of-bounds write. The standing policy prohibits guest patches, live
guest memory patches and host environment filtering as a workaround. The known
limitation remains tracked; no product fix or S11 closure is claimed.

## Deferred S12 observations

Before the owner's S11 insertion, the S12 investigation established that the
published MEM prints 4294940784 as largest executable size in one controlled
direct run. A disposable INT33 probe changed mouse position, saved and restored
state, and exited 1 with `MOUSE-STATE-FAIL`; the source saver writes zero while
the restore side compares a computed sum. These are initial results, not a
complete S12 attribution or disposition. The attempted nested BAT witness
printed `Bad command or file name`; it is invalid for nonreproduction and no
TODO was removed. The initial wrong-root runs timed out and are likewise not
product verdicts. Original guest binaries were not modified.

## S12 resumed: first bounded witnesses

The owner reports current startup testing normal and directs resuming S12.
This does not establish the cause of the earlier CAF observation or a product
fix. The following private-desktop tests use the S9 package, whose MEM,
COMMAND and NTVDM hashes match the installed O:/winnt copies respectively:

- MEM: `c4582b1e0738f3aaf89e17de0f940caa7e4b3d77a21fccd6ff91b7e075bdf6eb`
- COMMAND: `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`
- NTVDM: `3b0c432bd8ada2ae50a7e05a9270ca3a4dc33767f130303a38637e2c654a2f23`

An actual nested COMMAND executes a test BAT with `mem > S11MEM.TXT`, then
exits both shells. Both 1500 ms line spacing and zero inter-line delay display
the BAT's begin and end markers and produce the redirected MEM output file;
the observer reports an exited process with code 1, not a timeout. This is a
valid selected Console witness, unlike the earlier invalid invocation. The
owner directs closing this TODO on the valid nonreproduction. This does not
prove the cause of the historical timeout, nor independently verify the exact
historical BAT sequence or Window-mode route; that earlier evidence remains.

The same MEM guest directly returns zero and reports 655360 conventional
bytes but 4294940784 bytes as the largest executable program. In the nested
BAT output, the reported values are 4294920240 and 4294920208 on the paced and
continuous runs respectively. The source `membase.c` computes an unsigned
32-bit difference between `DOS_TopOfMemory` and the program PSP, then compares
it with DOS's largest free block; `mem.c` declares `DOS_TopOfMemory` as a zero
dummy and explicitly notes a missing library. This is a strong source-level
underflow hypothesis, not yet proof of the running binary's linked variable or
DOS-provider correctness. Direct original-DOS comparison remains outstanding.
An independent COM query on the same staged system returned PSP `067D`, PSP
allocation end `9FFF`, and INT 21h/48h failure's largest-available BX `0007`;
that COM holds its own large allocation, so BX is not MEM's value. The MEM
result `FFFF9870` mathematically equals `0 - 0679h * 16` modulo 2^32, four
paragraphs below the separate COM's PSP. This reinforces the underflow
hypothesis but does not establish MEM's actual PSP or internal variable.
An independent DOS test program subsequently installs a temporary INT 21h
observer, launches the unchanged MEM with DOS EXEC, captures the current PSP
on MEM's output call, restores the original vector, and reports `078E`.
In that same invocation MEM reports `4294936352` = `FFFF8720h`, exactly
`0 - 078Eh * 16` modulo 2^32. The test program is
`tests/observation/t448_mem_exec_probe.asm`; it does not patch guest images or
their code. The packaged MEM is byte-identical to the retained
`src/mvdm/dos/v86/cmd/mem/mem.exe`. Combined with the associated source's
explicit zero dummy, this directly supports the guest executable's underflow
path; the binary's private variable itself was not directly observed. It does
not by itself prove equivalent behavior on a separate original DOS system.

An independently authored real guest INT33 probe resets the mouse, queries
state size, saves after positioning, moves it, then restores the saved buffer.
It independently summed the saved bytes excluding the last byte: `26` versus
saved checksum `00`; position did not restore and the guest exited 1. The
original `mouse_save_state` writes its zero-initialized checksum without adding
the bytes, while `mouse_restore_state` compares the saved checksum against the
sum of cookie and context. This directly supports the already tracked mismatch
for this state, not a claim that every possible state fails: a coincidental
zero checksum can pass. No original host or guest code was changed.
The diagnostic control changes only the final checksum byte in the
independent test's own output buffer to the measured sum, calls the same
restore API again, and observes restored position (`CHECKSUM-CONTROL-PASS`).
The original unmodified state still fails, so the program exits 1. Probe:
`tests/observation/t448_mouse_checksum_probe.asm`. This diagnostic input is
not a guest patch, product workaround or repaired behavior.
Current comparative SoftPC `src/app-softpc/softpc.new/base/keymouse/mouse_io.c`
(SHA-256 `c42ee094f4ee9bb4db8da7492b7e333da92baa7b429f5409d3455ed68ef197a6`)
still initializes the saver checksum to zero and writes it without summation;
it does not provide a matching correction. This is a read-only comparison,
not an imported source dependency or proof about other upstream revisions.

### INT33 host correction proposal

The reached `mouse_save_state` and `mouse_restore_state` belong to the NTVDM
host's original SoftPC mouse provider, not to a DOS guest driver image. Under
the current source-policy original-host correction admission, compute the
same unsigned-byte sum over the saved `ISMMC` cookie and exact context bytes
in the saver, then write the low byte. Preserve state format and size, the
existing cookie, restore validation, context semantics and API. Compute over
the same bytes actually saved, including the existing MONITOR cursor handling;
do not introduce another state store, callback, guest modification or blanket
acceptance of invalid snapshots. Register the bounded mirror divergence and
source/recovery alternatives before implementation.

Verify real guest save/move/restore with multiple contexts and repeated
round trips; reject independently corrupted cookie, context and checksum
without changing live mouse state; cover size reporting, position and cursor
visibility, teardown and applicable NTVDM build branches. Verify the
previously accidental zero-sum case still passes. Snapshots persisted by an
older buggy implementation with zero checksum but a nonzero sum remain invalid
as they already are; silently accepting them is outside this proposal. Only a
test-buffer checksum control passed so far; no production correction, full
regression, publication or changed mirror diff is claimed.

### Expanded S12 source audit (in progress)

An isolated candidate NTVDM build containing DIV-357 was exercised through the
real DOS INT 33 save/move/restore path: `SUM=26 SAVED=26`,
`MOUSE-STATE-PASS`, observer exit 0; the updated probe also verifies rejection
of both a corrupted checksum and a corrupted cookie with a matching checksum.
The reproducible entry is `tests/observation/verify-mouse-state-checksum.ps1`.
The prior sealed baseline produced
`SUM=26 SAVED=00`, `MOUSE-STATE-FAIL`. The candidate stage was cleaned after
the witness. This verifies that case, not malformed-buffer handling or the
complete S12 regression; it has not been published.

The candidate DIV-359 installs 8042 command replies into the NTVDM output
register and marks them available without raising a keyboard interrupt. The
production-provider keyboard/PIC fixture completed 30 assertions with zero
failures, including C0 and 20/AA/AB/D0/E0 replies and actual PIC INTACK/EOI queue
advancement, including a direct response while a scan-code EOI is pending. It
does not establish every BIOS/device interaction.

The DIV-358 candidate was exercised with an authored DOS CRTC-register probe
and an instrumented build of the actual NTVDM publication path. Register 0Ah
read back `22h` after changing only the cursor-disable bit. Successive text
frames showed `(20,10, visible=1)`, `(20,10, visible=0)`, then
`(21,10, visible=1)` with the same scan-line shape; the guest and observer
exited successfully. The reproducible entry is
`tests/observation/verify-vga-cursor-disable.ps1`; it requires the test-only
console-wire observer worker and checks that the text cells remain unchanged.
This is a window-text witness, not a proof of every
graphics mode or frontend. An earlier attempt timed out before observation;
its test setup lacked the required initial 80-column geometry and is not a
product verdict.

The DEM fixture was relinked against the current worker fixture closure. In
the restricted execution environment it failed during `FileFindClose` after
the opened directory handle was observed live. The same diagnostic executable
outside that restricted environment completed all 50 instrumented assertions;
the uninstrumented current-source fixture then completed 47 assertions. It
observed `STATUS_NO_MORE_FILES` after deleting the remembered name. This
establishes an execution-environment difference for that fixture, not a new
production crash or an equivalent fix for the missing indexed reset service.
The original `OpenNT/base/ntos/vdm/vdm.c::VdmQueryDirectoryFile` builds a
kernel directory-control IRP containing both remembered name and index with
`SL_INDEX_SPECIFIED`; it also owns file-object references, buffers and I/O
completion. Modern public user-mode directory queries do not expose that
index input: Microsoft's
[NtQueryDirectoryFileEx contract](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfileex)
reserves indexed requests for a caller-generated kernel IRP, and its
[FILE_BOTH_DIR_INFORMATION documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_both_dir_information)
states that FileIndex is undefined on filesystems including NTFS. Keeping
all search handles indefinitely would defeat original handle/buffer limits;
guessing a successor by sorting names or snapshotting them would add different
mutation semantics. Neither is a demonstrated equivalent or has been adopted.
The deletion case and missing indexed service therefore remain open in S12.
An authored INT21 FindFirst/FindNext probe opened 142 searches, then examined
the first search. With buffer pressure, both deletion and no-deletion cases
returned the saved lookahead entry B but reported no more files on the next
call although C existed. With low pressure the deletion case continued to C.
The original `FileFindLast` can record end-of-directory while unconsumed
entries remain in a find buffer; `FileFindClose` then discards that buffer
under pressure while preserving the end status, and `SearchFile` terminates
instead of reopening. DIV-360 changes only that reclamation status when the
discarded buffer has entries and the status is a normal end. The real DOS
no-deletion pressure case then reaches C; a 54-assertion production-DEM
fixture also covers resource release and preservation of an empty-buffer end
status. The deleted-lookahead case still reports no more files after B and is
not claimed repaired. The repeatable real-DOS cases are in
`tests/observation/verify-dem-pressure.ps1` (`NoDelete`, `NoPressure`, and
`Deleted`); the last records an observed known limitation, not a repair pass.
Original index reset has not been supplied.

A complete isolated S9-based thirteen-image candidate with the rebuilt
NTVDM also passed the actual Win16 SOL stock draw, Deal, second stock draw,
window close and launcher-completion probe; the real command exit probe passed
direct MEM with exit 0 and a native command with exit 7. This is compatibility
evidence for the candidate, not full-package release or closure evidence.

The selected original VGA `vga_prts.c` writes CRTC register 0Ah only if the
five scan-line bits change; it also checks the old disable bit and then
`do_new_cursor` independently reconstructs visibility from geometry. A
bit-5-only write therefore needs both register preservation and visibility
calculation audited, not just a frontend override. A correction must preserve
shape changes, split cursors, scan-height changes, register readback and the
existing offscreen geometry rule. No candidate or passing guest witness yet.

The selected NTVDM keyboard `cmd_to_8042` C0 case marks a response valid
without assigning `input_port_val`; its NTVDM response branch invalidates
the prior cached output without transferring the computed response into
`output_contents`. On the current x86 Ninja host fixture, r01 reproduces C0
stale output while the EOI-owned slot and actual PIC INTACK/EOI checks pass
(18 assertions). This is a controlled provider witness, not a BIOS guest
acceptance. A safe candidate must initialize C0 and preserve non-interrupt
response, output status, pending scan code order and EOI ownership across all
controller response commands; no production candidate exists yet.

The DEM fast path requires an indexed `VdmQueryDir` operation that the modern
monitor binding does not supply. The current public Microsoft documentation
states that [`SL_INDEX_SPECIFIED`](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfileex)
requires a caller-generated kernel IRP, and
[directory FileIndex](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_both_dir_information)
is undefined for filesystems including NTFS. These facts do not prove a
user-mode equivalent or a deletion-safe cross-filesystem algorithm. Do not
assert that a simple scan or an `NtQueryDirectoryFileEx` flag preserves the
original contract. The existing DEM provider fixture was rebuilt against the
current Ninja closure; r06 exited `C0000008` before printing assertions, so it
is an invalid fresh reproduction, not a product or fixture pass. Earlier r01
through r05 failed for missing historical build inputs, stale link dependencies
or PowerShell host compatibility. The test runner now derives its extra
fixture closure from the actual Ninja generator; source ownership and the
fixture's invalid-handle failure still require investigation.

### S12 integrated candidate verification update

The preceding paragraphs record earlier investigation checkpoints, not the
current status. The four host-source candidates are DIV-357 through DIV-360,
with 33 inserted and 16 removed mirror lines in total. The current original DEM
provider fixture passes 54 assertions outside the restricted test sandbox;
the sandbox-only invalid-handle result is not classified as a product failure.
The VGA bit-only disable/re-enable is verified by guest register readback and
actual captured frames; the keyboard provider fixture covers all six response
commands and PIC ownership. The deleted-directory-anchor case remains open:
the buffered-status correction passes the no-deletion pressure control but
does not recover an anchor removed before reset.
The original `FileFindReset` itself warns that its name-scan fallback fails
if the remembered file is deleted. The original indexed fast path constructs
a kernel directory-query IRP; the published user-mode query interface does not
expose the equivalent indexed operation. Retaining all search handles or all
unconsumed directory records would replace the original bounded reclamation
policy with potentially unbounded resources, while guessing a lexical successor
is not the filesystem's enumeration order. None of those alternatives is
established as an exact, generally applicable replacement.

The S12 inventory also contains the original redirector username CX=0
underflow. Its actual-provider fixture explicitly demonstrates that a
capacity-checked call copies `user` despite zero declared capacity
(`redirector_netapi_provider_test.c`, `RETAINED_LIMIT`); it is a reproducible
host case, not merely an untested source suspicion. The older T430 gate was
superseded by the standing T447 owner clarification in source-policy.md:
source-proven original-host corrections may be implemented with a registered
minimal exception even without a matching SoftPC change. S12 therefore owns
this case; it is not an unreproducible S13 item or a guest defect. Baseline
`build/t448-s12-redir-baseline-r03` reproduces the unchecked write. DIV-361
uses the existing buffer-too-small status before any zero-capacity write;
`build/t448-s12-redir-fix-r01` passes 12 actual-provider cases, including
empty names, exact capacity, unchecked BX=0 and allocation cleanup. The real
VDMREDIR DLL builds; package-level verification follows below.

The full five-fix build `build/t448-s8/s12-build-r02` reports all 13 images
and 1,458 unchanged intermediates on its second build. Staging verified all
13 hashes (`build/t448-s8/s12-package-r02`), and that exact package passed
DOS17 in both Console and Window (`build/t448-s12-full-command-*-r02`),
WINMINE twelve-input gameplay/normal exit (`build/t448-s12-mine-r04`), SOL
deal/draw/close (`build/t448-s12-sol-r04`), the prior four-sample WRITE startup
frontier (`build/t448-s12-write-r02`), and CAF cmd/command/ntmon self-delete
with pre-Enter echo (`build/t448-s12-caf-r05`). WRITE editing and exit remain
unverified. Mine r02/r03 only failed to compile their probe due to a relative
compiler path and the wrong compiler architecture; neither ran the product.
All 13 tested r02 images were then published to both `assets/release` and
`O:/winnt/system32` and rechecked against their manifest. The previous complete
package is retained at `build/t448-s12-publication-backup-r02`; the publication
record is `build/t448-s12-publication-r02.json`. This is an incremental
checkpoint, not S12 closure or a fix for the deleted-anchor case.

An isolated coherent thirteen-image candidate, replacing only the S9 worker
with the final rebuilt worker, passed all 17 established DOS command cases in
each of Console and Window modes, including EDIT followed by MEM at the same
prompt (`build/t448-s12-full-command-console-r01` and
`build/t448-s12-full-command-window-r01`). It passed actual WINMINE interaction
(`build/t448-s12-mine-r01`) and SOL gameplay and normal close
(`build/t448-s12-sol-r03`). WRITE reached the prior four-sample startup
frontier (`build/t448-s12-write-r01`); editing, saving and normal exit remain
unverified, not claimed as passes. An unchanged Ninja build reports no work.
These are isolated candidate results; they do not establish publication,
deployment, all prior lifecycle regression gates, or S12 closure.

The candidate also passes the three real CAF in-session NTMON self-delete
cases (`cmd`, `command`, direct `ntmon`) and verifies active-buffer echo
before Enter (`build/t448-s12-caf-r04`). Earlier r02/r03 failures were not
valid product verdicts: their observer's unqualified `FindWindow` selected
the concurrently running NXVM's `LibKvmWindow`; r03 recorded its exact image.
The test observer now selects the visible window owned by the exact isolated
package's `ntcon.exe`, leaving NXVM untouched. The original S9 package also
passed its direct-NTMON control with the freshly built observer. This is a
test-observer ownership correction, not a production lifecycle change.
