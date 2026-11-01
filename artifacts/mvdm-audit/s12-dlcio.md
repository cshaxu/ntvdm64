# S12: OpenNT 4.5 DLCIO table branch

The selected `private/inc/dlcio.h` comes from OpenNT 4.5, not the OpenNT
comparison tree. Its `INCLUDE_IO_BUFFER_SIZE_TABLE` body matches
`nt/private/inc/dlcio.h` in that source. The observed x86/x64 product commands
do not select that label; no definitions or undefinitions were found in
project/generated C/header sources or the installed Windows SDK and Visual
Studio headers. The unselected table body is replaced by a marker, with the
conditional shell and selected `extern aDlcIoBuffers` declaration retained.
The companion JSONL records its body hash and original source path. Other
profiles are not covered.

Independent reconstruction from `55afe8a77` verified the original body hash,
the OpenNT 4.5 match, the complete file and the retained declaration. Cached
x86/x64 product builds, a separate VDMREDIR build and documentation governance
passed. All ten resulting product images matched the release exactly or
differed only by four PE linker timestamp bytes. Runtime tests and publication
were not repeated for equivalent images.
