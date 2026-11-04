# S13 per-file checkpoint r1237

Ten mirror C/H paths changed, including two deleted headers. The selected
`timer.c` original continuation and `base_def.h` TRUE-guard continuation were
corrected; `dlgmgr2.c` and `ntstubs.c` retain the original excluded guards
and diagnostic lines. The SIM32 original condition tree was reviewed with its
existing CPU40 adapter as a documented local binding. Per-file records are in
the [inventory](s13-r489-per-file-observed-inventory.json).

Two whole mirror headers were removed, together with their inactive host
includes. The sole `ntdbg.h` product consumer compiled with the header's entire
body suppressed by `_NTDBG_`; the now-useless define was removed. For
`ntdddisk.h`, [preprocessor witnesses](s13-r1233-ntdddisk-preprocess.json)
show the full file guard already defined in all 17 actual x86 object contexts;
the 17 corresponding x64 reference-worker commands had identical macro and
include inputs. [Removal evidence](s13-r1227-ntdbg-unselected.json) and
[include mapping](s13-r1234-ntdddisk-unselected.json) record the limitations.
No guest source or binary changed.

Cached x86, x86-extra and x64 product builds [passed](s13-r1237-full-build-results.json).
The [ten-image comparison](s13-r1237-build-image-compare.json) shows five
identical images and five differing only in PE/debug timestamps; no substantive
image bytes differ. Accordingly the verified [r1214 Full](s13-r1214-full-evidence.json)
was reused, and the changed structure/line [guard test](s13-r1237-guard-evidence.json)
passed independently. The complete ten-image candidate was published and
hash-verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1237-counts.json) records 1,056 present C/H files: 181
reviewed identical, 842 reviewed with differences and 33 pending. The
rename-aware net 580 deleted paths against `c97d4ee2c` are separate. Historical
Ninja dependency databases can retain obsolete records until those particular
reference graphs are rebuilt; current product x86 dependency records no
longer contain either removed header. Enumeration of every supported context
and the remaining original/local condition mismatches remains open.
