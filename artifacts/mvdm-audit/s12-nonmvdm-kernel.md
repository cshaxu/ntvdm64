# S12: unselected kernel USER and historical architecture branches

The observed x86/x64 product commands do not select `_USERK_`, `_IA64_`,
`_MIPS_`, `_PC98_` or `notdef`. No project/generated C/header definitions
or undefinitions of these labels were found. The installed Windows SDK can
define `_IA64_` conditionally for IA64, so absence from project flags alone
was not used as proof: actual MSVC x86 and x64 `/EP` probes including
`<windows.h>` both produced `S12_IA64_NOT_SELECTED`. The probe used
`#ifdef _IA64_` and emitted `S12_IA64_SELECTED` in the opposite branch.
The other four labels were not defined in the installed Windows SDK or Visual
Studio headers.

Nine original-source branch bodies were replaced by markers, preserving their
conditionals and the selected user-mode and x86/x64 paths. The companion JSONL
records ranges and removed-body hashes. Other architectures and unobserved
profiles are not covered.

Independent reconstruction from `cbff5cac2` verified nine hashes and five
complete files. Cached x86/x64 product builds, a separate VDMREDIR build and
documentation governance passed. All ten product images matched the release
exactly or differed only by four PE linker timestamp bytes; runtime tests and
publication were not repeated for equivalent images.
