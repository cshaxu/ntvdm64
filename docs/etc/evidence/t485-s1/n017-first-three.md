# N017 first three declarations - S15 P8

Starting HEAD3ce595610. Fixed XP SP1 object commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
Raw three-way inputs and execution logs: build/t485-s15/n017.
[Per-file identities and census](n017-first-three-census.json).

## Whole-file changes and consumers

DIR positions1-3 of19 original N017 inventory files migrate together.
apinums.h and apiparam.h move from public/internal/ds/inc to XP's
public/internal/net/inc. Their XP bytes match their pinned OpenNT originals;
the prior local difference was CRLF. The existing product net include path
already handles their new home, so no forwarding header/build path is added.
Actual consumers are VDMREDIR vrremote/vrnetapi and XACTSRV xactsrvp.h.
The original packon/packoff ABI from N016 remains in use.

crypt.h remains at its XP DS path. Restore the complete XP file including
C++ extern-C linkage, session binding/signature declarations, random and
memory encryption declarations. These are original declarations, not added
implementations or newly selected product algorithms. VDMREDIR vrnetapi.c uses the password API definitions.
No previous local functional adaptation or excluded arm exists in these files;
all three final files are byte-exact to XP, including newline and EOF.
No guest source/binary changes or rebuilds.

## Original-relative metrics

| File | Before OpenNT nonerror points/lines | After XP |
| --- | ---: | ---: |
| apinums.h | 1/444 | 0/0 |
| apiparam.h | 1/2360 | 0/0 |
| crypt.h | 1/1210 | 0/0 |

All three prior local diffs are format-only against OpenNT; crypt also changes
the reference baseline to XP's expanded original. No #error/#line differences.
Global1315files(0),753raw exact(+3),758nonerror same(+3),557different(-3),
7744points(-3),172161lines(-4014).1215XP/100OpenNT. Paths move for two files;
no mirror is added or removed. These are mirror differences, not source LOC.

## Verification status

Cached full build and runtime8 dual publication passed (13images/26hashes).
RAP product objects11/11 and real guest NetAPI passed.
[Full24/24](n017-first-three-full.json) passed on this final package; no retry.
[Receipt](n017-first-three-result.json). Ten-file checkpoint resets to zero.
N017 now3/19 delivered,16remaining; overall52/159 and S15 4/14 unchanged.
Next debuglib.h. The existing Win95 exit1 boundary is retained.
The paired userenvp.h migrated earlier is not a new N017 inventory file.
