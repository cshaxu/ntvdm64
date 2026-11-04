# S13 per-file checkpoint r1102

Twenty files were reviewed since r1082; ten mirror headers changed. The
per-file inventory links their original comparison and observed-context
evidence. The changed files are egaports.h, equip.h, gmi.h, gvi.h, host_sas.h,
ica.h, ios.h, mouse_io.h, printer.h and rs232.h. Their retained branches
remain in place; excluded branches and original diagnostic lines were
normalized. Guest source and media were not changed.

The cached x86, x86-extra and x64 build targets exited successfully. The
[ten-image comparison](s13-r1102-build-image-compare.json) found six exact
matches and four PE timestamp-only changes relative to the previous release;
there were no substantive image differences. The prior Full suite evidence
`s13-r682c-full-evidence.json` is therefore reused as authorized; no new
runtime suite is claimed. All ten candidate images were published and their
hashes checked at both `assets/release` and `O:/winnt/system32`. An initial
publication encountered a mapped file and its rollback was incomplete; after
identifying and stopping a cmd process holding nthook64.dll, the complete
package was republished and verified at both destinations.

The [census](s13-r1102-counts.json) records 1,058 present C/H files: 170
reviewed identical to their original, 746 reviewed with differences, and 142
not yet fully reviewed. The 578 rename-aware net deleted C/H paths against
baseline c97d4ee2c are separate from that present-file count. Existing
classifications still require the final supported-context audit; the count
does not assert that unreviewed files or older evidence are complete.
