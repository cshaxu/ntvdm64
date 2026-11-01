# S12: selected host-definition alternatives

`host_def.h` unconditionally defines EGG, VGG and LIM for this host. No
undefinitions were found in project/generated C/header sources or the installed
Windows SDK and Visual Studio headers. The observed MVDM C sources considered
here include `host_def.h` unconditionally before their first use of the
corresponding flag. Only original-source non-EGG, non-VGG and non-LIM
alternatives in those sources were considered; header consumers and include
orders without that proof were left untouched.

Nineteen branch bodies in eight C files were replaced with markers, preserving
their conditionals and selected behavior. The companion JSONL records the
original ranges and body hashes. Independent reconstruction from `13fc83f61`
checked the hashes, full files, include order and absence of the held `i386`,
`SPC486` and `TAKE_REAL_MODE_LIMIT_FAULT` labels in removed bodies. Cached
x86/x64 product builds and documentation governance passed. All ten product
images matched the release exactly or differed only by four PE linker timestamp
bytes; runtime tests and publication were not repeated for equivalent images.
Other profiles and unobserved consumers are not covered.
