# S12 P9 process.c audit

Complete-file review against the pinned XP SP1 object; 65 frozen points.
Before:1point/1500lines, after:30points/125lines, excluding #error bodies.
Whole-file CRCRLF had hidden separate actual adaptations. LF restoration is
owner-approved even when block count increases. No original guest changed.

The 45 restored format/framework points are separate from 20 necessary
residual points. Partial framework restoration does not close mixed semantic
points R309/R310. These remain in pending decisions. All residual evidence
and minimality explanations are in points.csv, synchronized with pending.

SearchPath restores original FE_SB positions; actual product profile_1 and
OEM test profile do not define FE_SB. Two unselected bodies are #error,
matching the selected OEM file.c policy. The existing unused CreateProcess
#error remains. The file-part addition uses original DWORD pointer width:
this TU is x86, and the OEM prefix byte length replaces the WCHAR offset.
No new shim or duplicate implementation. GetEnvironmentVariable,
ExpandEnvironmentStrings and GetShortPathName retain single original owner
functions and finally cleanup. Exact capacity, 0/1/65535/65536 boundaries,
DBCS prefix, query growth and failed conversions have existing fixture evidence.

Cached full build PASS; actual product preprocessing has60unique original
line witnesses and zero drift. Dual publication26hashes PASS. Existing
family, failure, DBCS and expansion-failure fixtures PASS. Actual published
runtime10 DOS FCB PASS. Full not rerun; file20/runtime8 evidence retained.
Unrelated stale PIF/font aggregate fixture dependencies stay explicit for S13.

| Frozen point | Original line | Frozen scale | Result |
| --- | ---: | --- | --- |
| HXP-R-00294 | 140 | +1/-1 | residual; owner review |
| HXP-R-00295 | 149 | +2/-1 | restored |
| HXP-R-00296 | 151 | +1/-1 | restored |
| HXP-R-00297 | 160 | +2/-1 | restored |
| HXP-R-00298 | 162 | +2/-0 | restored |
| HXP-R-00299 | 163 | +0/-2 | restored |
| HXP-R-00300 | 172 | +1/-0 | restored |
| HXP-R-00301 | 173 | +1/-1 | restored |
| HXP-R-00302 | 175 | +1/-1 | restored |
| HXP-R-00303 | 183 | +1/-1 | restored |
| HXP-R-00304 | 186 | +1/-0 | restored |
| HXP-R-00305 | 187 | +1/-1 | restored |
| HXP-R-00306 | 189 | +1/-1 | restored |
| HXP-R-00307 | 191 | +4/-1 | residual; owner review |
| HXP-R-00308 | 195 | +1/-1 | restored |
| HXP-R-00309 | 197 | +8/-10 | residual; owner review |
| HXP-R-00310 | 208 | +21/-20 | residual; owner review |
| HXP-R-00311 | 232 | +0/-3 | restored |
| HXP-R-00312 | 236 | +4/-0 | residual; owner review |
| HXP-R-00313 | 237 | +1/-1 | restored |
| HXP-R-00314 | 240 | +1/-1 | restored |
| HXP-R-00315 | 244 | +1/-1 | restored |
| HXP-R-00316 | 594 | +2/-1 | restored |
| HXP-R-00317 | 596 | +2/-0 | restored |
| HXP-R-00318 | 597 | +0/-2 | restored |
| HXP-R-00319 | 604 | +1/-1 | restored |
| HXP-R-00320 | 606 | +9/-1 | residual; owner review |
| HXP-R-00321 | 608 | +1/-1 | restored |
| HXP-R-00322 | 611 | +1/-0 | residual; owner review |
| HXP-R-00323 | 612 | +1/-1 | restored |
| HXP-R-00324 | 614 | +1/-0 | residual; owner review |
| HXP-R-00325 | 618 | +5/-0 | residual; owner review |
| HXP-R-00326 | 619 | +1/-1 | residual; owner review |
| HXP-R-00327 | 627 | +2/-1 | residual; owner review |
| HXP-R-00328 | 630 | +4/-1 | residual; owner review |
| HXP-R-00329 | 635 | +8/-0 | residual; owner review |
| HXP-R-00330 | 637 | +1/-0 | restored |
| HXP-R-00331 | 638 | +1/-2 | restored |
| HXP-R-00332 | 642 | +1/-1 | restored |
| HXP-R-00333 | 646 | +1/-1 | restored |
| HXP-R-00334 | 650 | +1/-0 | restored |
| HXP-R-00335 | 651 | +0/-1 | restored |
| HXP-R-00336 | 653 | +1/-1 | restored |
| HXP-R-00337 | 766 | +3/-3 | restored |
| HXP-R-00338 | 776 | +1/-1 | restored |
| HXP-R-00339 | 781 | +2/-1 | restored |
| HXP-R-00340 | 783 | +2/-0 | restored |
| HXP-R-00341 | 784 | +0/-2 | restored |
| HXP-R-00342 | 791 | +1/-1 | restored |
| HXP-R-00343 | 795 | +9/-7 | residual; owner review |
| HXP-R-00344 | 805 | +2/-1 | restored |
| HXP-R-00345 | 807 | +2/-0 | residual; owner review |
| HXP-R-00346 | 808 | +1/-3 | residual; owner review |
| HXP-R-00347 | 814 | +4/-0 | residual; owner review |
| HXP-R-00348 | 815 | +1/-1 | restored |
| HXP-R-00349 | 817 | +1/-2 | restored |
| HXP-R-00350 | 821 | +1/-1 | restored |
| HXP-R-00351 | 825 | +1/-1 | restored |
| HXP-R-00352 | 829 | +1/-0 | restored |
| HXP-R-00353 | 830 | +0/-1 | restored |
| HXP-R-00354 | 832 | +1/-1 | restored |
| HXP-R-00355 | 851 | +6/-6 | residual; owner review |
| HXP-R-00356 | 858 | +3/-3 | restored |
| HXP-R-00357 | 862 | +44/-8 | residual; owner review |
| HXP-R-00358 | 871 | +4/-37 | residual; owner review |
