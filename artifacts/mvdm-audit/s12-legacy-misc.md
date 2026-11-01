# S12: unselected legacy platform and licensing branches

The observed MVDM product commands do not select JOKER, PC_CONFIG, CGAMONO,
PROFILE, LICENSING or DOS_APP_LIC. No definitions or undefinitions of those
labels were found in project/generated C/header sources or the installed
Windows SDK and Visual Studio headers. Twenty-eight original-source branch
bodies or alternatives in the observed closure were replaced with markers;
conditionals and existing divergence records remain. The companion JSONL
records the ranges and hashes. Unobserved configurations are not covered.

Independent reconstruction from `069825a93` verified all 28 body hashes and
14 complete files. Cached x86/x64 product builds, a separate VDMREDIR build
and documentation governance passed. All ten product images matched the
previous release exactly or differed only by four PE linker timestamp bytes;
runtime tests and publication were not repeated for equivalent images.
