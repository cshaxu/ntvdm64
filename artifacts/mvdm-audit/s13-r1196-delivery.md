# S13 per-file checkpoint r1196

Ten mirror sources changed: nt_graph.c, nt_inthk.c, nt_msscs.c, nt_reset.c,
wkman.c, wow32.c, wuclass.c, wuwind.c, dlgmgr.c and mngrayc.c. Their
per-file records link original conditional structure and local binding
evidence. Product-selected CCPU, WOW and dialog bindings remain. The
non-i386 alternative in nt_msscs.c and four unselected dialog-owner
alternatives are explicitly rejected; existing original error anchors were
normalized. No guest source or media was changed.

Cached x86, x86-extra and x64 builds passed. The [ten-image comparison](s13-r1196-build-image-compare.json)
found one image with two substantive bytes outside its timestamp fields. The
[difference analysis](s13-r1196-ntvdm-difference.json) maps them to two error-report `__LINE__`
arguments in nt_graph.c after original-line normalization. No other
non-timestamp image bytes differ.

Because the diagnostic bytes changed, the candidate received a new
[Full verification](s13-r1196-full-evidence.json), not reused evidence. Its
[owned process](s13-r1196-full-process.json) exited 0; Console 17/17, Window
17/17 and the Full selected gates passed. WOW frontier checks cover startup,
not gameplay; the DOS4GW witness covers a visible active graphics window,
not gameplay or pixels. The complete ten-image package was published and
hashes verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1196-counts.json) lists 1,058 present C/H files: 181
reviewed identical to their original, 825 reviewed with differences, and 52
still incomplete. Rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. The final audit must still verify all supported
contexts, including non-Ninja addon consumers and shared guest headers.
