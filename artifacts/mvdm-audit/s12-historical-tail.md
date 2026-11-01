# S12: unselected historical diagnostics and host branches

The observed MVDM product commands do not select the labels recorded in the
companion JSONL, including legacy diagnostics, Unix/Mac host options and
explicit unused placeholders. No definitions or undefinitions of these labels
were found in project/generated C/header sources or the installed Windows SDK
and Visual Studio headers. Thirty-eight original-source positive branches in
the observed closure were replaced by markers, preserving conditionals and
existing divergence records. The JSONL records every label, range and
removed-body hash; other build profiles are not covered.

Independent reconstruction from `3e44c1ff7` verified all 38 body hashes and
21 complete files. Cached x86/x64 product builds, a separate VDMREDIR build,
and documentation governance passed. All ten product images matched the
previous release exactly or differed only by four PE linker timestamp bytes;
runtime tests and publication were not repeated for equivalent images.
