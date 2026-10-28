# Historical DOS build-tool descriptions — shard 3

Scope: the four shard 3 `review_class=pending` paths below. `dos-buildtools-observer.py` and captured JSON compare them and five reference makefiles to both original trees, and record the hashes of two present project historical build scripts. All nine inspected original files match both reference editions. This is a read-only documentation/build-graph audit: no tool was compiled or run and no guest modified.

| path | proposed file-role disposition | evidence / limit |
|---|---|---|
| `dos/v86/tools/src/buildidx/buildidx.def` | excluded, historical tool metadata only | DESCRIPTION says DOS Message Index Builder; messages/makefile invokes buildidx to index COUNTRY messages, and buildidx/sources declares TARGETNAME=buildidx. Surprisingly this original DEF names STRIPZ. It is not evidence of current target identity; historical use of this DEF or why the NAME differs is unproven. |
| `dos/v86/tools/src/stripz/stripz.def` | excluded, historical tool metadata only | Historical STRIPZ target description; DOS kernel makefile invokes stripz after reloc to create NTDOS.SYS. Its DESCRIPTION says text-to-RTF, which differs from the makefile's image-packing purpose; original inconsistency is observed, no speculative correction. |
| `dos/v86/tools/src/h2inc/h2inc.mak` | excluded, historical recipe file only | Describes building and binding h2inc.exe from h2inc.obj/def with an OS/2 toolchain. `dos/v86/redir/makefile:92-96` uses h2inc to generate `.inc` from `.h`. Actual selected tool binary and source provenance are not established. |
| `dos/v86/tools/src/h2inc/h2inc.doc` | excluded, historical documentation role only | Explains H-to-INC conversion and options; not an executable or a selected guest payload. |

These exclusions concern the specific metadata/recipe/document files, not internationalized message support, DOS kernel contents, redirector ABI, or historical tool functionality. The repository also retains historical `tools/build/Build-T225S8PrimaryNtio.ps1` and `Build-T225S8PrimaryNtdos.ps1` that name buildidx and stripz; presence is not evidence those scripts were used in the current release, nor authorization to rebuild guest. The historical mismatch between DEF descriptions/names is not a demonstrated current product bug. No runtime coverage or provenance percentage is asserted.
