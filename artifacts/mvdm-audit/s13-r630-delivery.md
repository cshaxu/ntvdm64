# S13: unselected utility sources and preserved binaries

Ten unselected C/H files from the historical PIFEDIT, GRAFTABL, video-stub and
empty-debug source groups were removed, together with four obsolete build
inputs. Original/local hashes and disposition are recorded in
[r629](s13-r629-unbuilt-tool-sources.json). Their original standalone build
recipes no longer run. GRAFTABL.COM remains both in the source tree and in
`assets/winnt.zip`; PIFEDIT.HLP, the icon and other retained resources are
unchanged. Runtime behavior and binary provenance are not asserted.

All five existing EXE/DLL/COM files under `src/mvdm-host` were matched by
SHA-256 to entries in the ZIP; see
[host binary evidence](s13-r630-retained-host-binaries.json). Guest-root
files were not modified.

Cached x86, x86-extra and x64 builds passed. Eight product images are
byte-identical to the previous release and two differ only by PE timestamps;
see [image comparison](s13-r630-build-image-compare.json). Full 17/17 from
the earlier verified package is reused by owner instruction; no fresh product
tests were run. All ten product components were published and hash-verified
in both destinations. Mirror audit and supported-context enumeration remain
open.
