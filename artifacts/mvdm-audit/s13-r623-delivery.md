# S13: real-mode and ANSI diagnostic line reconciliation

Ten source files were changed: the existing rejected real-mode-limit arms in
six CCPU sources have original diagnostic and selected-branch line mappings;
four further C/H sources have original line mappings for already rejected
non-ANSI arms. The r621 and r622 records identify the exact changes. Other
conditional groups in these files remain pending review.

The cached x86, x86-extra and x64 product builds passed. Thirteen isolated
MSVC negative branch witnesses reported the intended original line numbers.
Eight images were byte-identical to the previous release and NTVDM and
nthook32 differed only in PE timestamps; see
[image comparison](s13-r623-build-image-compare.json). The previously passed
Full 17/17 is reused under the owner's instruction; it was not rerun here.
All ten images were published and hash-verified in both release destinations.

The filesystem census was corrected independently: 1087 existing C/H files
are present under the two mirror roots, including 30 absent from the prior
1057-file observed build/dependency list. Their usage is not yet decided.
See [census](s13-r624-current-file-census.json) and
[reference candidates](s13-r624-unobserved-reference-candidates.json).
