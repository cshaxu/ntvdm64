# S12: unselected CPU and display alternatives

Under the observed build conditions documented in `s12-legacy-engines.md`,
twelve GISP_CPU, GISP_SVGA and HERC alternatives to selected branches were
replaced with registered markers, retaining the original conditionals. The
companion JSONL records the affected source ranges and removed-body hashes.

Independent reconstruction from `bcb6636df` verified all twelve hashes and
eight complete files. Cached x86/x64 product builds and documentation
governance passed. All ten resulting product images matched the previous
release exactly or differed only by four PE linker timestamp bytes; runtime
tests and republication were not repeated for equivalent images.
