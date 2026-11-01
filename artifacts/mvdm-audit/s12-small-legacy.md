# S12: remaining small historical diagnostic and host branches

The observed MVDM product commands do not select the eleven diagnostic,
historical tool and host labels recorded in the companion JSONL. No definitions
or undefinitions of those labels were found in project/generated C/header
sources or installed Windows SDK and Visual Studio headers. Eleven original
branch bodies in the observed closure were replaced with markers, preserving
the conditional shells. Their source ranges and hashes are recorded in the
JSONL. Other build profiles are not covered.

Independent reconstruction from `4dad5f52c` verified all eleven body hashes
and eight complete files. Cached x86/x64 product builds, a separate VDMREDIR
build and documentation governance passed. All ten resulting product images
matched the release exactly or differed only by four PE linker timestamp
bytes; runtime tests and publication were not repeated for equivalent images.
