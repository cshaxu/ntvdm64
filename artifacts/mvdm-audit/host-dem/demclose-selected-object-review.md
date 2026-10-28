# DEM close selected-object lifetime probe

`demClose` reconstructs the host handle; zero is accepted without close; a requested seek precedes close; on either failure it calls `demClientError`; on close success it notifies the VDM redirector if loaded, then clears CF. Windows owns the actual handle, DEM owns the sequence, while redirector state belongs to its callback. `demhndl.c` is an existing mirror diff; this probe does not classify every change or exercise its guest-memory lease modifications.

Fixture `demclose-selected-object-fixture.c` SHA-256 `2F066370D8A72D88F92AEF2BC13CDDAC5316ECF933CED9B0C8F7D0C3C9ACAF3B` links selected `demhndl.obj` SHA-256 `706D3670AF83AA52497EEF567E78927AFA3658546BBA039CFAF7BFE724A31EC6`; MinGW i686 GCC `-Wl,--gc-sections` gave `Warning: corrupt .drectve at end of def file` and succeeded. Real Win32 temporary file operations were used; CPU registers, DEM error and redirector callback are controlled seams. Test exit 0:

```text
seek failure cf=1 error=131 handle_live=1 callbacks=0
close cf=0 removed=1 closed_at_callback=1
stale cf=1 error=6 callbacks=1
```

The successful close invalidated the real handle before the controlled redirector callback; failed negative seek left it live, with no callback; reuse of that now-closed handle reached the failure seam. During fixture development, calling `CloseHandle` through DEM with `INVALID_HANDLE_VALUE` (-1) returned success on this host, so it was not used as the invalid-handle oracle; the retained test uses a previously closed handle and no intervening allocation. This does not establish behavior on every host. The seam sets CF and does not execute actual DEM error mapping or real redirector teardown. No DOS guest, lifecycle manager or installed image was exercised; future tests must cover real named-pipe, nested task teardown and handle-reuse races. No product, guest, or ledger was changed.
