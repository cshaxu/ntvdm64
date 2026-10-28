# WOW palette conversion and host boundary

`wow32/wgpal.c` matches both pinned original editions. The [observer](wow-palette-image-observer.py)
and [receipt](wow-palette-image-observations.json) verify its selected object
against the published WOW32 code and bind ten initial GDI table entries and
two USER entries, including their original object relocations. This establishes
initial linkage only: guest invocation, runtime aliases and lifetime are not
proved. The shared GDI observer remains compatible with its earlier 28-entry
checkpoint, which was independently replayed after parameterization.

The original thunks convert 16-bit palette/DC handles and palette entries;
Windows owns native palette objects, actual device capability and the palette
operations. The original `WG32GetSystemPaletteEntries` includes its own
256-entry RGB fallback on devices above eight bits per pixel, rather than
merely forwarding the native call. The original USER entries select and
realize the native palette. Correct rendering and message timing across tasks,
device changes, palette ownership and deletion remain open.

One source-level candidate requires controlled follow-up: `WG32GetPaletteEntries`
allocates the requested count, calls the host, then passes the requested count
to `putpaletteentry16` even if the returned count is smaller or zero. The
original `malloc_w` uses `HeapAlloc` without `HEAP_ZERO_MEMORY`, and
`putpaletteentry16` copies each requested element. Determine which short and
failure results are actually possible with the supported host and valid guest
inputs, then verify guest copyback and failure behavior. This is an original
code path; no guest exposure has been demonstrated here. A current x86
[native host probe](wow-palette-host-short-result.json) did produce both a
short return (one of four entries) and a zero return for an invalid handle;
the untouched buffer tail retained its sentinel. The probe does not call the
WOW thunk, so it does not establish guest-side copying or a product defect.

Source retention is 100%; no numerical estimate of behavioral restoration is
supported by these bindings. Keep original palette conversion, host resource
ownership, nested calls and teardown in S16. No guest or product changed.

The standalone host-only probe source is [wow_palette_host_short_test.c](../../../tests/mvdm-host/wow_palette_host_short_test.c).
It was compiled as x86 with the MSVC `/MT` runtime and `gdi32.lib`; the
recorded executed source copy is byte-identical to the retained file. It is
not currently wired into the full product gate.
