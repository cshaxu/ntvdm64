# N016 packing and SMB declarations - S15 P5

Starting HEAD06de88cd5. XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Raw original and prior bytes: build/t485-s15/n016-part2; complete three-way
inputs: build/t485-s15/n016. This batch handles DIR positions6-11 of21.

## Implementation

packoff.h/packon.h move to XP public/sdk/inc byte-exact. Their pack()/pack(1)
contract is unchanged; vdmredir, apiparam and xsdef16 retain original includes.
tstr.h moves to XP SDK; existing C++/LM20 exclusions each replace one line,
so no #line is necessary. No string implementation or caller changes.
SMB headers retain their original existing debug/alignment exclusions.
XP smbmacro.h supplies ULONG_PTR arithmetic; smbtypes.h keeps original new
INCLUDE_SMB_IFMODIFIED declarations. No extra exclusion or runtime branch.
smbgtpt.h is unchanged upstream; restore raw LF and remove19 redundant line
resets adjacent to another conditional boundary. Remaining17 resets map the
next active original declaration. smbmacro retains1, smbtypes3 line resets.
All remaining nonerror differences are these21 #line directives.
No original guest edits or rebuilds; no architecture boundary changes.

## Metrics

Before uses registered OpenNT; after uses XP. Restored raw LF exposes separate
#line points previously joined into whole-file CRLF differences. This is the
owner-approved formatting increase, not added functional adaptation.

| File | Before points/lines | Final points/lines |
| --- | ---: | ---: |
| src/non-mvdm/public/sdk/inc/packoff.h | 1/68 | 0/0 |
| src/non-mvdm/public/sdk/inc/packon.h | 1/64 | 0/0 |
| src/non-mvdm/public/internal/base/inc/smbgtpt.h | 1/1538 | 17/17 |
| src/non-mvdm/public/internal/base/inc/smbmacro.h | 1/472 | 1/1 |
| src/non-mvdm/public/internal/base/inc/smbtypes.h | 1/738 | 3/3 |
| src/non-mvdm/public/sdk/inc/tstr.h | 1/599 | 0/0 |

| Global metric | Before | Final | Delta |
| --- | ---: | ---: | ---: |
| files | 1319 | 1319 | +0 |
| byte_identical | 746 | 748 | +2 |
| nonerror_identical | 750 | 753 | +3 |
| nonerror_different | 569 | 566 | -3 |
| nonerror_points | 7737 | 7752 | +15 |
| nonerror_lines | 195577 | 192119 | -3458 |

1206XP/113OpenNT. [Census delta](n016-packing-smb-census.json).

## Validation

Initial cached full build passed. Code review then removed19 metadata-only
line resets; final cached rebuild also passed. Runtime5 dual publication
verified13images/26hashes. RAP product objects passed11checks, both existing
redirector lifecycle fixtures exited0, and real guest NetAPI passed.
[Delivery receipt](n016-packing-smb-result.json). Six files since P4 Full24/24;
Full is not due and was not repeated. N016 now has11/21 files delivered;
the directory remains open. No original guest modification/rebuild.
