# Selected WGMAN source file role

Both original editions and the current `wow32/wgman.c` contain only a comment,
the precompiled-header include, `hdrstop` and `MODNAME`; they declare no GDI
functions. The [observer](wow-gdi-empty-owner-observer.py) checks the selected
Ninja compile/link edges and actual object sections. The selected object has no
code section or map-visible functions. It does carry standard C runtime linker
directives, also present in the separately selected `wgdi.obj`; it is therefore
not described as absent from the build.

The proposed exclusion is limited to this physical source file's empty
behavioral role. It does not exclude GDI, guest calls, resources, or the other
selected GDI files, including [wgdi.c](wow-gdi-core-review.md). The
[receipt](wow-gdi-empty-owner-observations.json) records exact source and
object identities. Original bytes are retained; no guest or product changed.
