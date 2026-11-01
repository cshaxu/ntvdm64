# S12: unselected historical 16-bit mouse implementation

The observed MVDM product commands do not select MOUSE_16_BIT; no definition
or undefinition was found in project/generated C or header sources, or in the
installed Windows SDK and Visual Studio headers. The historical path calls
`mouse16bInstall`, for which the tree has a declaration and call but no
implementation. The selected NTVDM code installs the INT 33 handler; the
separate `mouse31` guest driver uses that host mouse interface. Eighteen
original-source branch bodies or alternatives were replaced with markers,
preserving conditionals and selected paths. The companion JSONL records
ranges and hashes. No guest driver or guest binary was modified.

Independent reconstruction from `3671e76ca` verified 18 body hashes and both
complete files. Cached x86/x64 product builds, a separate VDMREDIR build and
documentation governance passed. All ten images matched the previous release
exactly or differed only by four PE linker timestamp bytes; runtime tests and
publication were not repeated for equivalent images. Other build profiles are
not covered by this evidence.
