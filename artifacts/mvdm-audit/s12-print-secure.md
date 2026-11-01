# S12: unselected secure-boot and PostScript-flushing branches

The observed MVDM product commands do not select SECURE or PS_FLUSHING. No
definitions or undefinitions of those flags were found in project/generated C
and header sources, or in installed Windows SDK and Visual Studio C headers.
Twenty-three original-source branch bodies were replaced by markers while
retaining their conditionals; an existing divergence was preserved. The
companion JSONL records ranges and original-body hashes. This describes the
observed product configurations, not other profiles.

An independent reconstruction from `816ce695f` verified all 23 hashes and
11 complete files. An initial build overlapped a premature unrelated fdisk edit;
that exact incomplete edit was independently reconstructed and restored, and
the complete product build was rerun on the verified clean batch. Cached
x86/x64 product builds, a separate VDMREDIR build and documentation governance
passed. All ten product images matched the existing release exactly or differed
only by four PE linker timestamp bytes; runtime tests and publication were not
repeated for equivalent images.
