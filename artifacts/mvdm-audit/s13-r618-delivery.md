# S13 ten-file condition and diagnostic-line batch

The two DEM and eight CCPU source edits are recorded in the r616 and r617
audit records. The original ANSI-selected text matches in all eight CCPU
files. The CCPU call.c and jmp.c reviews remain partial because their other
condition groups still require examination. The remaining six CCPU files and
the two DEM files have their original conditional structure reviewed.

The x86, x86-extra and x64 cached product builds succeeded. Eight staged
product images are byte-identical to the previous release and NTVDM and
nthook32 differ only in their four-byte PE timestamp; see the independently
parsed [image comparison](s13-r618-build-image-compare.json). Under the owner's
rule, the earlier Full 17/17 product evidence is reused; this is not a fresh
product Full run. An MSVC preprocessor test passed 22 isolated witnesses for
the excluded branch diagnostics and restored original line numbers; it does
not prove untested whole-program conditions.

All ten images were installed and hash-verified in both release destinations.
This is a progress delivery; the full supported-context inventory and the
remaining mirror reviews are not complete.
