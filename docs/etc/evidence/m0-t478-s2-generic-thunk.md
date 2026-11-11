# T478 S2 — original generic thunk recovery

## Question and inputs

Recover finding 33 under the owner-admitted S2 without globally enabling i386
or changing guest. Inputs are the current wkgthunk.c and callpr32.asm and the
[historical ABI/guest experiments](../../../artifacts/mvdm-audit/shard-0/wow-generic-thunk.md).

## Procedure and observations

Initial source inspection confirms three excluded i386 regions in wkgthunk.c;
only the third is the generic-call helper. The checked-in original helper
copies dynamic arguments and restores ESP but writes EBX without saving it.
Historical tests reproduced both the portable-call mismatch and the original
helper EBX issue. These historical results are not current acceptance.

## Implementation direction and remaining proof

Select only the generic-call branch, compile the original host assembly from
source with a provenance-controlled macro include, and establish a minimal
ABI-preserving boundary. Review exception unwinding before choosing a wrapper
versus a registered minimal helper correction. Do not directly link the
known EBX-clobbering helper as a compliant callable. Current build integration,
focused tests, real guest regression and publication are pending; no product
source has yet changed in this S.

## Initial implementation and focused verification

The generic-call branch is restored under WOW_X86_GENERIC_CALL only. The
original assembly saves/restores EBX and shifts its parameter offsets by four
bytes; no wrapper or second call algorithm is introduced. Macro input
public/sdk/inc/callconv.inc is copied byte-exact from OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, SHA-256
044E0712A074C7FD61E1A4CA04FAC1FF860F028D289C92E320B01CC9C7690BB5.
It has no further includes and supplies compile-time procedure macros only;
its original copyright notice is retained. OPENNT-HOST-295 registers this
finite dependency. MVDM-HOST-DIV-674/675 register caller and ABI changes.
The alternative wrapper adds a second assembly frame unnecessarily; the
minimal two-instruction preservation plus offset correction retains original
argument copying and stack-reset semantics.

The independently compiled original register probe returned result 14 and
EBX 12345678. The tracked wow_generic_helper_test.c then passed cdecl/stdcall,
nesting, zero arguments, ESP/EBX preservation and propagated SEH exception:
errors=0, exception=1. These are host helper tests only, not product or guest
acceptance. Reproduction uses build/t478-s2/helper-test.cmd with x86 MSVC /O2
/MT and freshly assembled source; no prebuilt original object is linked.

Initial CMake compiler detection failed with C1902 PDB manager mismatch.
The retry uses native Ninja, CMAKE_TRY_COMPILE_CONFIGURATION=Release and
CMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded; compiler detection now passes.
Formal product compilation and expanded thunk/guest acceptance are pending.
No publication or commit has occurred.

## Current product build and thunk matrix

The x86 worker target (533 steps) and WOW32 target (238 steps) completed with
exit 0. The actual generated WOW32 object and assembled helper were linked
into test/observation/wow_generic_thunk_test.c, adapted from the retained
historical chain matrix without its obsolete EBX wrapper and with sufficient
32-argument fixture storage. All 12 checks passed: zero/one argument, four
source/destination convention flag combinations against both cdecl and
stdcall targets, 32 arguments and SEH propagation. Output is retained in
build/t478-s2/thunk-test.log (errors=0). The helper sentinel/nesting tests
passed separately. Documentation governance passed during this run.

Real guest integration, full product regression, coherent two-destination
publication and commit are not yet done. S2 remains active and is not closed.

## Real guest verification and package preparation

Both independently authored Win16 runs (normal and intentionally cdecl native
callee advertised by the guest as stdcall) completed three GEN1 records.
Each recorded zero errors, 3 Pascal callbacks, 1 cdecl callback and 1 nested
native entry after execution; the final record confirmed 3 resource releases.
The same NE probe was used; only the native test calling convention changed.
The retained runner is test/integration/verify-wow-generic-call.ps1, using
explicit build root, compiler driver and observer arguments. No original guest
was modified. Full product gate is still pending.

All ten product images have now been rebuilt/staged. The initial old released
WOW startup baseline failed independently; a source-baseline WOW32 rebuilt
from HEAD wkgthunk.c using the same surrounding product objects passed
WINMINE/SOL/WRITE window observations, as did the candidate. This establishes
an independent current-build old-call-path baseline instead of claiming the
old release asset failure as a candidate regression. The first Control run
passed RPC and GUI cases but its version-negative builder was pointed at the
staging directory; its missing service.h failure was fixed by selecting the
actual native-service build tree. Full's first attempt rejected 19 missing
immutable files from the installed-media copy. Only the isolated test copies
were completed from assets/winnt.zip, byte-verified against the manifest;
63 immutable files now pass. No O:/winnt write occurred.

The owner supersedes previous publication scope: only this worktree's
assets/release may be published; no O:/winnt publication and no push.
Mirror comparison checks both bytes and normalized newlines: callconv.inc is
byte-exact, wkgthunk.c and callpr32.asm retain registered semantic changes.
No format-only original divergence was introduced. Private i386 load-library
branches remain excluded. The portable C alternative remains original source,
not a second autonomous implementation; the production graph selects only
the recovered helper route.

## S2 closure and local delivery

Owner authorizes S2 closure after implementation, build, testing, local release
and commit, with no installed-runtime write and no push. The final Full run
passed all 16 selected gates in 419088 ms on one identical ten-image package
(build/t478-s2/verify-Full-r3/timings.json). DOS4GW/NESticle remains the prior
owner exclusion. The preceding attempts are retained as failed harness/media
preparation, not counted as passes. The final run includes strict DIR,
modern EDIT return, cooked return, rapid and interactive relaunch, session
isolation, retirement and nested I/O handoff as well as WOW, DOS and RPC gates.

The final retained generic-call runner passed its mismatch case with the
completed immutable test package. All three records have zero errors;
post-call counts are 3 Pascal, 1 cdecl and 1 nested native entry, followed by
3 releases. The original normal-convention run passed the same assertions.
The real exception-sibling test additionally passed: original exception Close,
no guest continuation, exactly-once retirement in the same worker and surviving
WINMINE gameplay (build/t478-s2/guest-exception/sibling-result.txt).

Published only to this worktree assets/release; every image equals the tested
staging manifest, with prior exact files backed up under the task build root.
The original O:/winnt tree was a read-only media input, never a publication
target. No guest source or original image bytes changed. There is no push.
Finding 33 is resolved by this S; broader S1 inventory limitations and other
S proposals remain separate. The next proposed issue is S3/finding 20, which
is not admitted by S2 closure. Deferred S1 planning documents are included in
the local S2 P1 delivery rather than discarded.

| Released image | SHA-256 |
| --- | --- |
| run16.exe | 1BBF2FD31C4D3D58655D878CB9C8A04E466FD159DAEEB97A8C2F6F4B7B997C1C |
| ntsrv.exe | 875E4DC67C3B0B21C8EABECE360E2D47A6D0266C0A1B9FB233032E867B9EB10D |
| ntcon.exe | 33C5E0DBF385C3A03B03C03855114CF893B5437CBFAFE9174210B6B6514120A6 |
| ntvdm.exe | DD31593D477F01F1844F7E3958B8697B99038993F6FC99EA7FBCB41BB9F4C22C |
| ntvwm.exe | 7A45202240813E32ACE42C0688F91789885011A01A3079CC940353A542190E98 |
| ntmon.exe | F7A5BC88E28DE8FD8361EF17EC0F3BE5FE3D97892C475369C3574E47C3C2FBD5 |
| WOW32.DLL | 03318EF720E7C6106E9006A47EED6DAF0EB5D098ABAE0ADF74C97802168B1817 |
| VDMREDIR.DLL | 5FC5F0CD3A3E57C4542C393855AA152B5A62F3709591BD84492CF66FE5B2C60C |
| nthook32.dll | 9FE1804EEF0D5BE8D37AC12CC7C8CDD9DA195E7DAF856163A7F677BA0B3E034C |
| nthook64.dll | 08DF0579880109A4D9FE61EFFE4DE58B952D921823DD8E54B2EB9519444E55B6 |

Final review: the original helper appears in the linked map as
_WK32ICallProc32MakeCall@12 from callpr32.obj and the caller as
_wow_original_WK32ICallProc32@4 from wkgthunk.obj. No new autonomous
implementation replaces the original call algorithm. The minimal EBX repair
adds preservation and adjusts only its three argument offsets. The actual
product objects pass the host matrix. Documentation governance and diff checks
are required immediately before committing this closure.

Local delivery P1 is commit ca910d6a1. A follow-up P2 records that identity
and removes a trailing blank line in the retained test script; no production
source, executable or test behavior changes after the verified publication.
