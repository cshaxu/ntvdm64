# S12: unselected physical keyboard and ROM options

The observed MVDM product commands do not select REAL_KBD, REAL_ROM or A_VID;
no definitions or undefinitions were found in project/generated C/header
sources or the installed Windows SDK and Visual Studio headers. Thirteen
original-source branches or alternatives in the observed closure were replaced
by markers with their conditional shells retained. The current virtual
keyboard and software ROM paths are unchanged. The companion JSONL records
original ranges and removed-body hashes. Unobserved profiles are not covered.

Independent reconstruction from `3ea7b4ef3` verified all 13 hashes and three
complete files. Cached x86/x64 product builds and documentation governance
passed. All ten images matched the release exactly or differed only by four
PE linker timestamp bytes; runtime tests and publication were not repeated for
equivalent images.
