# W16Style

Thin demonstration of NTW16S's public DLL API. main.c checks the interface
version; gallery.c creates 27 sample forms and their application content,
native commands, message loops and scrolling. It contains no retro painter,
embedded style artwork, automatic probe, capture or fault-injection harness.
Owner-drawn example menu items call the DLL's public text/measurement API.

One source builds x86 and x64 W16Style.exe. Release selects x64, beside
ntw16s64.dll in the configured system32; x86 remains a tested build artifact
beside ntw16s32.dll. No hard-coded drive, NTVDM worker or launch service is
required. The PE import graph selects only the same-width style DLL and host
APIs. Hook and WOW consumers integrate the DLL independently, not this demo.

The authored control-page grid is scaled from16px coordinates to20px (5/4),
matching the shared font. Group spacing, list extents, viewport padding and
focused-control pages use that same proportion; the renderer does not resize
arbitrary application controls. The external boundary probe captures page01
and checks its button/list sizes and20px viewport scrolling.

Automatic regression and external owned-window observation live under tests.
The application does not expose test switches or turn itself into a probe.
