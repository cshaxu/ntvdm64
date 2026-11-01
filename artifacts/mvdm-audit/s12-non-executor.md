# S12: non-SPC386 and non-C_VID source alternatives

The observed x86 worker and WOW32 Ninja command/dependency joins show explicit
SPC386 and C_VID definitions for all 443 observed MVDM C sources. Of 434
MVDM headers, all observed consumers explicitly define each flag for 305;
129 have mixed or not-explicitly-defined command contexts and were excluded.
No undefinitions of either flag were found in project/generated C/header
sources or installed Windows SDK and Visual Studio headers.

Sixteen original-source alternatives in four observed C files were replaced
with markers, preserving their conditionals and selected executor and video
code. Their ranges and body hashes are in the companion JSONL. Existing
divergence markers and modified bodies were not removed. An independent
review checked that the removed bodies do not contain the held `i386`,
`SPC486` or `TAKE_REAL_MODE_LIMIT_FAULT` labels. Other build profiles and
unobserved targets are not covered.

Independent reconstruction from `8d4d7d56d` verified all 16 hashes, all
four complete files and the corresponding command contexts. Cached x86/x64
product builds and documentation governance passed. All ten product images
matched the release exactly or differed only by four PE linker timestamp
bytes; runtime tests and publication were not repeated for equivalent images.
