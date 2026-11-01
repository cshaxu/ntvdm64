# S12: unreferenced GISP hardware VGA interface

`hwvga.h` is the original interface for the historical GISP hardware VGA
implementation. After the unselected MOUSE_16_BIT includes were cleared, no
project C/header code references this header or its `HostHwVgaH` include
alias; the remaining names are explanatory comments. The observed worker and
WOW32 dependency logs do not list the header, and the GISP_SVGA platform is
not selected in the recorded MVDM product commands. This is a historical
hardware VGA interface, not the selected software VGA implementation and not
a V86 monitor.

The file matched OpenNT. Its unselected outer GISP_SVGA body was replaced by
a registered marker, retaining the outer conditional and provenance comments.
The companion JSON records the original-file and removed-body hashes;
independent reconstruction verified the complete result. Cached NTVDM and
VDMREDIR targets reported no work, and documentation governance passed. No
runtime image changed, so no runtime test or publication was repeated. Other
unobserved configurations are not covered.
