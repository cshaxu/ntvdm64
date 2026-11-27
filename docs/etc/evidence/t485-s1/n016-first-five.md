# N016 first five files - S15 P4

Question: migrate the first five DIR-ordered Base declaration files to XP,
including required original paths and ABI providers. Starting HEADe72a83c2e;
XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Complete21-file three-way input snapshots are in build/t485-s15/n016.
This checkpoint does not close N016;16 original files remain.

## Decisions and implementation

- align.h: XP public/sdk/inc original replaces base/inc. Retain existing
AMD64 exclusion only; preserve new original ALPHA/IA64 branches. One #line
executes after the complete architecture conditional, not in an inactive arm.
- conroute.h: XP public/internal/windows/inc original replaces base/inc.
Preserve all original declarations and pointer-width Console handle check.
Existing B GetConsoleInputWaitHandle declaration/definition now use WINAPI,
matching XP APIENTRY; no queue, duplicate provider or mirror ABI workaround.
- debugfmt.h: XP SDK path, including original FORMAT_POINTER.
- nettypes.h: XP SDK path, complete original declarations.
- ntddvdm.h: original path, XP raw bytes (normalized prior content identical).

All four latter headers are XP byte-exact. Original includes keep their
spelling. The first real cached build found native-family missing windows/inc
for conroute.h; add that original search directory beside base/inc in the
five existing native owner profiles. No forwarding header is introduced.
No original guest modification/rebuild.

## Per-file nonerror metrics

| File | Prior OpenNT points/lines | XP final points/lines |
| --- | ---: | ---: |
| src/non-mvdm/public/sdk/inc/align.h | 3/5 | 1/1 |
| src/non-mvdm/public/internal/windows/inc/conroute.h | 1/168 | 0/0 |
| src/non-mvdm/public/sdk/inc/debugfmt.h | 1/152 | 0/0 |
| src/non-mvdm/public/sdk/inc/nettypes.h | 1/162 | 0/0 |
| src/non-mvdm/public/internal/base/inc/ntddvdm.h | 1/88 | 0/0 |

## Whole non-guest metrics

| Metric | Before | Final | Delta |
| --- | ---: | ---: | ---: |
| files | 1319 | 1319 | +0 |
| byte_identical | 742 | 746 | +4 |
| nonerror_identical | 746 | 750 | +4 |
| nonerror_different | 573 | 569 | -4 |
| nonerror_points | 7743 | 7737 | -6 |
| nonerror_lines | 196151 | 195577 | -574 |

1200 XP /119 OpenNT baselines. [Census](n016-first-five-census.json).

## Verification

Cached full build passed after the missing XP directory correction. Runtime4
was dual-published (13images/26matchinghashes); real guest NetAPI and
[Full24/24](n016-first-five-full.json) passed on the final package.
[Delivery receipt](n016-first-five-result.json). This closes the ten-file
checkpoint (N015five plus N016five), not N016's entire directory.
Independent arm-level projection confirms align.h excludes only the AMD64
body, leaving the original ALPHA/IA64 branches visible in the comparison.
No full-suite retry. Existing approved Win95 exit1 boundary is retained.

## Remaining directory research

winss.h has real consumers, but XP declares the service IDs in the already
imported ntcsrmsg.h. Follow the original declaration ownership before removing
the obsolete file. VDM TIB declaration layout and other generation fragments
still require complete consumer review. These remaining files are unchanged.
