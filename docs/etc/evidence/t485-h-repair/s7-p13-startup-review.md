# S7 P13 startup file review — local, not delivered

## Scope and result

Complete-file review of `src/mvdm-host/softpc.new/obj.vdm/ntvdm.c` against XP
blob `c31ff248800686e36a844a8b2e7c65e26158c9fa`. Non-error differences fall from
6 points / 18 lines to 5 points / 17 lines. Only a surplus blank after
`#line 334` is removed; `#line 250` restores diagnostics after the omitted
kernel handshake. Original DBG/non-PROD exclusions remain unchanged. The
project-added MONITOR rejection is counted in full, not masked as upstream.

## All six original points

| Point | Conclusion and exact evidence |
| --- | --- |
| HXP-R-04045 | Retain one canonical shadow-registry include. Its macros route original NtOpenKey/NtEnumerateValueKey/NtClose calls to the same package registry owner. Removing it changes lookup to native HKLM and bypasses the package overlay. See compat/include/mvdm_shadow_registry.h. |
| HXP-R-04046 | Retain INT-pointer host_main declaration matching base/support/main.c. A by-value declaration conflicts with the actual provider and passes an integer where the provider stores an address. |
| HXP-R-04047 | Retain the split mvdm_softpc_prepare entry. F already owns main and calls prepare, NtvdmActivateLifecycle, then softpc_run_initialized under its exception boundary. Restoring main here creates a duplicate product entry and removes the called preparation symbol. Outer main owns argc throughout execution. pargc is stored by the original support owner; no active dereference was found, so that storage alone is not claimed as an observed dangling-pointer crash. |
| HXP-R-04048 | Retain NTAPI matching ntrtl.h DbgBreakPoint declaration. The x86 product uses /Gd; dropping it changes the definition calling convention, not formatting. |
| HXP-R-04049 | Retain removal of the packed kernel version/size handshake. compat/mvdm_vdm_tib.c binds teb->Vdm to monitor->tib before run_softpc; original CpuEnvInit would replace that live pointer with a tick-count/size integer. Preserve the explicit unsupported MONITOR rejection. No replacement TIB or registry algorithm is introduced. |
| HXP-R-04050 | Restore the surplus blank after #line 334. It was both an unnecessary diff and an off-by-one diagnostic mapping. |

## Independent counter-review

The entry split is necessary for the existing initialization/activation/execution
sequence and single main owner; this review does not rely solely on an unused
pargc consumer. Registry include necessity is a package lookup semantic boundary,
not merely an existing adapter dependency. The monitor rejection is new relative
to XP and remains charged as 3 added / 7 removed lines. Original kernel handshake
cannot initialize this process-local real-TIB contract. No new functional path.

## Verification and delivery state

Exact non-error measurement is in [startup diff](s7-p13-startup-diff.json).
Source review and git diff --check passed. No compile, publication or runtime
claim is made. This nonfunctional file change accumulates into the next complete
file batch as authorized; P13 is not delivered. S7 remains 17/20 delivered,
18/20 locally reviewed, two not yet reviewed. Delivered reduction stays
865 points / 7669 lines; including local work it is 866 points / 7670 lines.

## wkgthunk.c whole-file review

HXP-R-04196 and HXP-R-04197 remain 2 points / 4 lines. The exact product
flags in tools/build/wow32/CMakeLists.txt select WOW_X86_GENERIC_CALL;
units.cmake links i386/callpr32.asm. That original assembly copies exactly
cbArgs bytes and restores ESP after the call, supporting native stack cleanup.
The unselected RISC C body calls fixed 4/8/32 argument groups and shares the
same symbol. Restoring both guards would select that body alongside the actual
assembly provider. Globally defining i386 would also enable the unrelated
native FPU-save blocks, which do not save the emulated CCPU x87 state.
Keep the two narrow guards; do not add another native-call implementation.

Correct the obsolete OpenNT diagnostic anchors to XP 229/237 and 316/322;
add 416/457 around the excluded RISC body. All selected function bodies and
unselected WX86 originals remain unchanged. A full-file normalized comparison
confirms the same two guard differences only. No runtime claim. S7 now has
19/20 source-reviewed, 17/20 delivered; wow32.c is the last unreviewed file.

## wow32.c in-progress correction

Whole-file entry: 18 frozen non-error points / 103 lines. HXP-R-04367 and
HXP-R-04368 had incorrectly called W32TryCall a required original XP body.
Direct XP comparison shows only W32FilterException under NO_W32TRYCALL and
no W32TryCall else arm. Repository production-symbol search and the current
WOW32 product map show no caller or retained W32TryCall symbol. Removed the
old else implementation, keeping the original rejected NO_W32TRYCALL arm and
its XP continuation at line 1347. This is a local semantic reduction, not a
completed whole-file or delivered claim. The remaining initialization,
registry, GDI and thunk exception contracts must be completed before building.
No cumulative reduction for this unfinished file is claimed yet.

### wow32.c initialization and exception review continuation

The current published O:/WINNT tree has no NTVDM.REG at either checked root.
The actual shadow initializer explicitly permits missing registry files;
open_section returns ERROR_FILE_NOT_FOUND for a missing optional WOW key and
ERROR_NOT_ENOUGH_MEMORY if allocating an admitted key fails. The old W32Init
fallback suppressed both. The candidate reuses cb to capture open status,
continues only for ERROR_FILE_NOT_FOUND and otherwise returns FALSE. Subsequent
queries reset cb as originally; NULL queries select existing defaults. This
is a semantic correction requiring targeted final-code verification.

Restored original DEBUG_MEMLEAK declaration framework, original whitespace and
trailing comments; corrected stale XP diagnostic anchors in DEBUG blocks and
after dispatch additions. NT LPC include supplies PORT_MESSAGE used directly
by ntcsrmsg.h. GDI original shared-table query remains omitted: walias.h uses
mvdm_wow_gdi_handle_to_native/from_native/retire, not host kernel table layout.
Package paths are owned by compat/system_root.c, avoiding native OS system
paths. Exit uses W32ExitWowProcess to preserve authorization, ExitVDM and exit.

Exception necessity is supported by the existing complete-TU
wow_dispatch_exception_fixture.c: pre-setup failure, nested callback frames,
original W32Exception Close/Ignore, and debugger-first-chance propagation.
Reuse test/observation/verify-wow-dispatch-exception.ps1 for this final candidate;
no claim that it has run yet. No build starts until whole-file review and
related test preparation are complete.


## Complete source review and P13 delivery launch

Final wow32.c measurement: 18/103 -> 15/69 (minus 3 points / 34 lines),
[all remaining hunks](s7-p13-wow32-diff.json) retained as S7-P13-W01..W15.
Together with ntvdm.c 6/18 -> 5/17 and wkgthunk.c 2/4 unchanged, this batch
reduces 4 points / 35 lines. S7 has 20/20 locally reviewed, 17/20 delivered.
Delivered reduction remains 865/7669; local inclusive is 869/7704.

Whole-file source review is complete. Background PID 10924 runs
build/t485-s7/p13/deliver-final16.ps1: cached full build, dual thirteen-image
publication with hashes, actual selected W32Dispatch/W32Exception fixture,
real WINMINE, three guest desktop cases including strict enhanced Alt+Enter
roundtrips, and NESticle graphics. This is pending evidence, not a pass.

### Actual delivery checkpoint

Runtime16 cached full build and thirteen-image publication to assets/release
and O:/WINNT/SYSTEM32 passed, including hashes. Documentation governance passed.
The dispatch fixture initially failed compilation because its pre-XP FindPWC
function conflicts with the XP macro and WWSTATE_ICLASSISSET no longer exists.
Those obsolete fixture definitions were removed. XP-only inactive initialization
link dependencies are bound to the existing failure stub; interpreted thunk and
common-dialog branches explicitly fail if reached, preserving the test scope.

The updated fixture passes phase 0, then fails phase 1 before stack establishment.
This is NOT a passed gate. Existing exception observer with the fixture-only
--diagnose-policy switch captured the first fault at image RVA 0x13a0, read of
0xf2, in W32Exception; ordinary unhandled recursion later yields access violation
or stack overflow. The exact cause remains under investigation. Production
exception policy is unchanged. See build/t485-s7/p13/final16/diagnose/debug.txt.
Product runtime gates now run separately using the same published runtime16;
no repeated product build or publication. P13 remains uncommitted/unpushed.
### Final runtime16 verification

The phase-1 failure was a fixture ABI mismatch: the production TU uses FE_SB,
but the fixture wrote TD.htask16 at offset 0x48 instead of the selected 0x4c.
The fixture now uses FE_SB too. The subsequent callback failure was another
pre-XP fixture assumption: XP MarkWOWProc/UnMarkWOWProc encodes two high selector
bits. The fixture supplies the original macro-encoded procedure and verifies
the exact decoded 0x12370056 selector/offset. No product exception workaround
was added and no cleanup assertion was removed.

Final dispatch-final5 passes eight dispatch phases, debugger first-chance
propagation and five hard-error entry/return/exception cases. Its map gate
rejects retained unrelated initialization owners. Diagnostic failure records
remain in build/t485-s7/p13/final16; temporary diagnostic policy was removed.

The same published runtime16 passes real WINMINE, Win3.1 standard and enhanced
desktops, Win95 desktop, strict enhanced same-window Alt+Enter roundtrips and
real DOS4GW/NESticle menu/Escape graphics. Win95 retains the approved
knownExitOne boundary, not a normal-exit claim. Evidence: final16/{wow-result,
targeted-result,dispatch-result,publish-result,build-result}.txt and case logs.
The registry failure branch is source-reviewed; allocation-failure injection
is not claimed. No Full run is claimed for P13; S13 retains the Full gate.

All twenty S7 files are final-verified. Batch reduction is 4 points/35 lines;
cumulative reduction including the P10 coupled header is 869/7704. This record
and its registers are included in P13; commit/push supplies delivery identity.