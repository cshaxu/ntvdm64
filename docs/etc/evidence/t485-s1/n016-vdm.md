# N016 VDM declarations and published template - S15 P6

Starting HEAD db476ed02. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`; originals read as Git bytes.
Raw inputs and execution evidence: `build/t485-s15/n016-part3`.
This batch handles DIR positions 12-15 of 21, together with the actual
consumer ABI test and the build-time storage validator.

## Implementation and residual evidence

XP vdm.h replaces inline interrupt/fault tables with pointers. The original
vdmtib.inc and C declarations migrate together; original WOW fastwow.asm
consumes these offsets. The existing test now verifies offsets 4,8,0xc,
0x2d8,0x5a4,0x634 and full x86 size 0x674 at compile time.
The product storage validator previously required more than 0xfc0 bytes,
a threshold from OpenNT's inline tables. It now requires the complete XP
size while retaining unique definition, real storage, map owner, overlap
and startup binding checks. No runtime algorithm is replaced.

vdm.h retains two original excluded arms: _VDMNTOS_ kernel-only declarations
and DBG. Two active #line resets retain original diagnostics. Two existing
product architecture guards use _X86_ because this CCPU build does not
define i386: restoring those guards verbatim caused missing VDM_TIB and
VDM_PRINTER_INFO types in the real product build. The first i386 guard for
IRETBOP constants stays original; no global macro or new branch is added.
NTSYSCALLAPI is omitted at the existing local NtVdmControl declaration:
its SDK definition imports a kernel service, whereas the B provider retains
this standalone host's unavailable-service ABI and leaves caller data intact.
The test checks every XP service through VdmPMCliControl, not the removed
OpenNT VdmQueryVdmProcess enumerator. These are all residual functional bindings.
Old PC98 and disabled declarations absent from XP are not reintroduced.

vdmapi.h and vdmtib.inc are XP byte-exact. winbase.w moves to its original
XP base/published directory and is byte-exact; it has no current production
build consumer. No original guest binary is changed or rebuilt.

## Original-relative metrics

Before uses each registered OpenNT object; after uses XP. LF restoration
exposes the five real differences formerly joined in a whole-file block.
No source-file line-count reduction is substituted for mirror metrics.

| File | Before nonerror points/lines | After | Excluded arms |
| --- | ---: | ---: | ---: |
| vdm.h | 1/967 | 5/8 | 2 |
| vdmapi.h | 1/282 | 0/0 | 0 |
| vdmtib.inc | 4/306 | 0/0 | 0 |
| base/published/winbase.w | 0/0 | 0/0 | 0 |

[Census](n016-vdm-census.json): 1319 non-guest files; raw exact 750 (+2),
nonerror same 755 (+2), different 564 (-2), points 7751 (-1),
lines 190572 (-1547). 1210 XP / 109 OpenNT originals. The template relocation
changes the path and baseline, not the number of files.

## Verification status

XP TIB layout and NtVdmControl ABI test passed. Initial product compile exposed
architecture guard mismatch; corrected compilation/link then exposed the
obsolete storage-size gate. Both failures are retained in batch logs.
The final cached build and runtime6 dual publication passed (13 images,
26 matching hashes). XP TIB/NtVdmControl ABI and real guest NetAPI passed.
[Full24/24](n016-vdm-full.json) passed on this final package; no Full retry.
[Delivery receipt](n016-vdm-result.json). Ten-file checkpoint resets to zero.
N016 now has15/21 files verified for delivery; six remain. Overall51/159
directories and S15 3/14 are unchanged. The approved Win95 exit1 boundary
remains; no newly claimed normal-exit repair.
