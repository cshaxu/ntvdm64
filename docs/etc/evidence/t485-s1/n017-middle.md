# N017 middle ten declarations - S15 P9

Starting HEAD19deb2b88. Pinned XP SP1
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`; raw Git bytes.
Three-way originals: build/t485-s15/n017; current execution evidence:
build/t485-s15/n017-middle. [Sources](n017-middle-sources.json),
[census](n017-middle-census.json).

## Scope and decisions

DIR positions4-13 of19: debuglib, netdebug, netlib, netlibnt, prefix,
rap, rapdebug, rapgtpt, remdef, remtypes. Preserve every original declaration
outside existing rejected arms. All current exclusion keys match XP's
original conditions; no new conditional framework or compatibility shim.

Debug declarations move to XP ds/netapi/inc, prefix to ds/inc, RAP/rem
public interfaces to public/internal/net/inc. Existing worker profiles and
two standalone compilation units add the original ds/inc and ds/netapi/inc
search paths. No forwarding files or caller include rewrite. The standalone RAP fixture
compiler also switches its sole rap.h search path from DS to the XP Net owner. Consumers:
Netlib packstr/time/memalloc/ntstatus/svcinfo/xlatesvc, RAP convert/parmnum,
XACTSRV and VDMREDIR. Host and guest ownership do not change.

XP netlib contains the already-used NetpAllocateEnumBufferEx declaration;
restore it at its original position, eliminating the extra declaration diff.
Original wide-string pointer types match the previously migrated packstr
implementation. XP netlibnt removes old string/product helpers which have
no production callers. XP netdebug adds stdarg and narrows obsolete debug
interfaces inside existing rejected DBG arms. debuglib drops old ERRNO tag.
Existing RAP pointer helpers and Ex conversion declarations are XP originals,
not new host adaptations; rap/rapgtpt already had XP baseline and are not
counted again as newly imported files. Original unused _WIN64 branches stay.
No guest binaries changed/rebuilt, no new algorithm or callback path.

## Per-file nonerror results

| File | Before points/lines | Final points/lines |
| --- | ---: | ---: |
| debuglib.h | 1/310 | 2/2 |
| netdebug.h | 1/652 | 16/16 |
| netlib.h | 5/18 | 3/3 |
| netlibnt.h | 1/232 | 0/0 |
| prefix.h | 1/126 | 0/0 |
| rap.h | 1/1 | 1/1 |
| rapdebug.h | 1/124 | 1/1 |
| rapgtpt.h | 0/0 | 0/0 |
| remdef.h | 1/2486 | 0/0 |
| remtypes.h | 1/352 | 0/0 |

All remaining differences are diagnostic #line resets, with original conditions
and #error bodies retained. One-line replacements require no reset; resets
immediately followed by conditional boundaries are removed. LF restoration
exposes individual line-map points previously hidden by whole-file CRLF diffs;
this owner-approved format cleanup adds no behavior. Global1315files(0),
756raw exact(+3),762nonerror same(+4),553different(-4),7754points(+10),
167883lines(-4278).1223XP/92OpenNT. Eight new XP baselines, two already-XP paths;
no mirror addition/deletion. Path movement is separate from textual reduction.

## Verification status

Whole batch and original include search paths were complete before build.
Final cached full build, runtime9 dual13-image26hash publication, RAP11/11,
real guest NetAPI and [Full24/24](n017-middle-full.json) passed; no Full retry.
Initial RAP standalone compilation used its obsolete DS include path and failed.
The fixture path was corrected to XP Net, then this failed targeted test retried.
No product change/rebuild or successful gate repeat was needed; the failure
log remains rap-first-build-failed.log and delivery-first-rap-error.txt.
[Delivery receipt](n017-middle-result.json). Ten-file checkpoint resets to zero.
N017 now13/19 delivered,6remaining; next rxp.h. Overall52/159 directories and
S15 4/14 remain. Existing approved Win95 exit1 boundary is unchanged.
