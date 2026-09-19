# T420 S35 XMS capability investigation

## Status and inputs

S35 remains open. These are real guest observations, not an XMS package
closure. The selected machine is MSVC x86 CCPU40; guest HIMEM is unchanged.
Its deployed SHA-256 is
`08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08`,
identical to `src/mvdm/dos/v86/dev/himem/himem.sys`.

The eight XMS manifest members include a header and the unselected RISC
backend `xmsmemr.c`; eight manifest members do not mean eight compiled C
files. Six now match the pinned `OpenNT/base/mvdm/xms.486` files bytewise:
`xms.c`, `xmsa20.c`, `xmsdisp.c`, `xmsmemr.c`, `xmsmisc.c`, `xmsumb.c`.
The last had only newline differences, restored mechanically after normalized
equality was checked. The two existing semantic deltas remain `xms.h`'s
callback parameter naming/order and `xmsblock.c`'s bounded guest-memory seam.

## PIF path lifetime finding and bounded correction

Original `config.c` calls `init_lim_configuration_data` before `sas_init`.
That function reads CONFIG through `GetPIFConfigFiles(TRUE, ...)`.
Later DOS `cmdGetConfigSys` calls `ExpandConfigFiles`, which asks for that
same path again. The original resolver frees and clears the override on the
first lookup, so the second read falls back to the default CONFIG. AUTOEXEC
has a separate pointer: an AUTOEXEC marker alone cannot prove CONFIG loaded.

The original owner and interface remain `nt_pif.c::GetPIFConfigFiles`.
Direct unchanged composition was tested and lost the override. A separate
adapter cannot retain the private path without duplicating its ownership.
The selected correction therefore retains the original strings across reads,
frees old strings before a new PIF selection, and leaves final reclamation to
the one-worker process lifetime. There is no new ABI, parser, allocator,
guest image, kernel provider or adapter policy. The change extends the
existing `MVDM-HOST-DIV-157` register: four changed executable lines, net zero
executable-line growth; explanatory comments are separate.

Pre-correction `s35-umb-r5.txt.console.txt` showed the AUTOEXEC marker but
not the expected INT15 reservation. Post-correction
`s35-umb-fixed-r1.txt.console.txt` showed the original HIMEM `/NUMHANDLES=17`
message, proving that CONFIG parameters now reached the driver. That reserved
INT15 configuration then timed out; it is not a passing result.

## Real guest coverage so far

All referenced logs are below `O:\winnt\logs`.

| Route | Result and exact evidence |
| --- | --- |
| Public INT 2F XMS entry; allocate, conventional/XMS moves, grow, forward overlap, lock/unlock, A20, free/reuse | `s35-xms-r2.txt.console.txt` emitted `S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK`, with process exit 0. |
| Odd move length, repeated unlock, repeated free, excessive allocation | The same guest asserts the original A7/AA/A2/A0 errors before the success marker. |
| UMB during CONFIG before DOS ownership | `s35-umb-driver-r1.txt.console.txt` emitted `S35_XMS_BOOT_UMB_ALLOC_RELEASE_DOUBLE_FREE_OK`; the test-only driver calls public HIMEM functions, then discards itself. |
| UMB after CONFIG | The same run emitted `S35_XMS_UMB_NO_FREE_BLOCKS`. Original `doskrnl/bios/sysinit1.asm::ConfigDone` unconditionally calls `AllocUMB`; post-boot absence is not proof that XMS allocation failed. |
| Default INT15 AH=88 | The same run returned AX=0 and emitted `S35_XMS_INT15_DEFAULT_ZERO_OK`, followed by the core success marker and exit 0. |
| `/INT15=128` without `/NUMHANDLES` | `s35-int15-reserve-r2.txt.console.txt` reached the boot UMB success marker but not AUTOEXEC; its report records timeout. Root cause is still under investigation. |
| Established product regression | `Verify-CommandExitStatus.ps1`, prefix `s35-config-regression-r1`, passed all 17 transcript-gated COMMAND/MEM/EDIT/native/guest-exit cases. |
| Reproducible XMS gate | `Verify-T420S35XmsGuest.ps1`, `guest-r7`, prefix `s35-xms-gate-r3`, passed direct, PIF profile, nested COMMAND and twice-in-one-batch cases. All four check actual guest markers and exit status. |

The formal graph is `build/M0-T420/S35/formal-x86-r1`. Its default library
build completed 469 edges; explicit `ntvdm.exe run16.exe basesrv.exe dtmgr.exe
VDMREDIR.dll` completed the remaining 58 edges. Those five artifacts supplied
the post-correction tests. Test fixture sources are `xms_capability.asm`,
`xms_umb_driver.asm` and `xms_profile_builder.c` under `tests/observation`.
`Build-T420S35XmsGuestTest.ps1` builds the default profile; `-ReserveInt15`
builds the retained failing reproducer. Neither option rewrites package media.

The first repeat harness never reached XMS: injected `tests\...` stopped at
the backslash and the observer reported failed input delivery. The successor
uses an original COMMAND batch instead of keyboard injection. Its first
input had LF-only lines, which old COMMAND interpreted as one command tail;
the build now explicitly emits DOS CRLF. Only `s35-xms-gate-r3` is the passing
four-case result. These two harness failures are not XMS failures and were
not counted as passes. The final source-format-only rebuild is recorded in
`formal-x86-r1/format-rebuild.stdout.log`; the final deployed package supplied
this four-case gate. This bounded S35 P1 delivery does not close S35.

## Remaining closure work

Resolve the reserved-INT15 startup failure without misclassifying it as
default-profile success. Finish the dispatch/caller and negative-case ledger,
repeat/task/worker teardown evidence, and source-manifest/hash review. Test
observer launcher exit is not proof that all worker resources have exited:
observed resident test workers were explicitly identified by parent PID and
terminated between isolated runs. No leak-free claim follows from that cleanup.
S35 must not close or advance to S36 on this record alone.
