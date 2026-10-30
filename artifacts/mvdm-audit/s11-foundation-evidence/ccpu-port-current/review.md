# Current selected CCPU port-permission witness

The prior 117-case fixture used an older CCPU archive hash. It was relinked
against the current cached CCPU and SoftPC support libraries and all 117
cases passed. These include scalar and string input/output, byte/word/dword
widths, controlled TSS permissions, zero and nonzero REP counts, and
controlled page-fault ordering. The fixture map binds its decoder and port
callbacks to the selected original objects. The current worker map names the
same object owners, and the cached unsealed worker hash matches the input
hash recorded for the published sealed NTVDM. Inputs and full case results
are retained here.

This is a controlled host fixture, not execution by an immutable DOS guest;
its link uses `/force:multiple` seams and emits existing duplicate-symbol
warnings. Matching owner names and the worker input hash do not independently
prove the exact library bytes linked into the earlier worker. The current
Ninja dry run requests a worker relink because `opennt-base-bindings.lib`
has a newer recorded timestamp; no product was rebuilt or published by this
check. The guest interrupt and device boundaries remain for S11/S13.
