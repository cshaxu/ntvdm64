# MSCDEX guest and VDD package boundary

The original sample's `tsr/mscdexnt.asm` hooks INT 2F and on first MSCDEX
request calls RegisterVDD with `VCDEX.DLL`; failure takes the original fallback
path. Its companion VDD is built separately by `vdd/sources`. The selected
`nt_bop.c::ISV_RegisterModule` translates RegisterModule through
`SafeLoadLibrary` and resolves the supplied exports. This source chain does
not by itself prove which image or search path a running guest uses.

The [observer](mscdex-package-observer.py) and [snapshot](mscdex-package-observations.json)
record that the installed MSCDEXNT.EXE, ZIP copy and retained source-tree
binary have the same SHA-256. Installed `autoexec.nt` names it. The standard
release and installed system32 directories and ZIP contain no `VCDEX.DLL` by
that name at observation. A retained historical VDD binary is present under
the source sample, but its existence neither places it on the runtime search
path nor establishes that it is suitable for modern delivery. External search
paths, alternate providers and guest configuration have not been excluded.

This is a concrete package/dispatch mismatch candidate, not proof that a
particular machine's CD-ROM or every MSCDEX call fails. Validate actual guest
load, VDD ABI and registration, optical availability, reads and error/teardown
using a suitable isolated fixture before recovering or delivering a provider.
Guest and user installations were not modified or executed by this review.
