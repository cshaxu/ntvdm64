# S10 P2 gvi.h whole-file review

Tenth ordered file; frozen points HXP-R-01074--01078. Fixed XP revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89 and original path
`Source/XPSP1/NT/base/mvdm/softpc.new/base/inc/gvi.h`.
The [metrics record](s10-p2-gvi-metrics.json) records hashes, dispositions and
line correspondence. Byte-sensitive non-error differences36/41 ->32/33:
four points/eight differing lines removed. Thirteen existing #error exclusions
and their original conditional structure are unchanged; no excluded body is
re-audited or restored. Original line endings and EOF remain unchanged.

01074/01075/01076/01078 were trailing spaces at original lines11/47/429/506.
Restore these exact original lines. The prior generic OEM-capacity evidence
was irrelevant to this header's whitespace and is replaced, not perpetuated.
These four frozen points are locally restored, pending P2 delivery.

01077 adds get_chain2_screen_ptr. gfx_updt.c uses it for text painting,
cursor attributes, split screens and scrolls; nt_cga.c uses it for painting
and scrolling. Callers use the existing doubled logical text offsets. The
macro preserves the low two bits and doubles the remaining offset, mapping
the four-byte logical text cell to the eight-byte physical plane cell used
by the BIOS scalar writers. Removing it cannot preserve the actual callers;
substituting get_screen_ptr reads the compact location, while scattering the
same expression into callers increases complexity. Retain this one expression
as an evidenced pending decision, not an approved permanent retention.

The following #line288 was stale OpenNT-era positioning: XP get_screen_base
is line491. Correct it to491. The original lines matched after projection
were checked:114 nonblank non-directive lines, zero logical-line mismatches.
All exclusion errors are byte-identical before/after. Existing #line anchors
remain included in the32/33 metric, not hidden as zero differences.

No executable expression changed. Cached full build, dual publication,
affected C-VID and real-package tests will verify the cumulative P2 batch.
Source review10/30 files,131/1060 points,929 remain. Four locally restored
points;127 evidenced residuals; one delivered file. Total267/62146 against
201/102863: +66/-40717. No P2 delivery claimed here. Next keymouse/keyba.c.
