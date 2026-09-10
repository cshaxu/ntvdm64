# M0 T299 S6 — definition-body binding correction

The source scanner now accepts a function definition only when the first
non-whitespace/comment token after its parsed parameter list is the function
body `{`. A later brace in an enclosing `if`, `while` or other control block
does not make a call expression a definition.

Reproducible generation from the two original OpenNT source roots yields 112
physical first-degree definitions in 20 physical source files and 670
cross-file second-degree candidates. The previously false `GetTickCount`
entries in NetDDE, Paintbrush and Shell printer files are absent; the genuine
Base-client and IPX definitions remain. Control-construct pseudo-definitions
remain absent.

The selected-file closure assertion remains active: no second-degree row
resolves in its caller file or to an externally-linkable definition in another
selected first-degree file. No source package, build input or runtime behavior
changed.
