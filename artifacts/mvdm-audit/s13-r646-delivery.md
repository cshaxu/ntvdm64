# S13: DBG guard mappings and distinct DEVL contexts

`demexp.h` retains its original DEVL branch: actual MSVC preprocessing
observed it selected by the DOS worker and not selected by WOW32. Its sole
conditional group and original branch body were reviewed in
[r644](s13-r644-demexp-review.json). The sole DPMI header DBG group was also
reviewed; its selected nondebug body matches the original.

Ten files received original diagnostic/continuation line mappings for 30
already excluded DBG arms. Other condition groups in nine files remain pending;
see [r645](s13-r645-dbg-guard-lines.json). Cached x86, x86-extra and x64
builds passed, along with 181 retained negative-guard controls and 30 isolated
MSVC DBG line controls.

Six product images are byte-identical to the previous release, and four differ
only in parsed PE timestamps: [comparison](s13-r646-build-image-compare.json).
Prior Full 17/17 evidence is reused per owner instruction, not rerun. All ten
components were published and hash-verified in both destinations. Guest files
were not modified; broader supported-context and remaining file review remain
open.
