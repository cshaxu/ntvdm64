# S12: non-V7VGA alternatives in selected Video7 builds

The current x86 worker and WOW32 Ninja command/dependency join observed 443
MVDM C sources, each compiled with an explicit `V7VGA` definition. Of 434
headers, all observed consumers explicitly define it for 305; 129 have mixed
or not-explicitly-defined command contexts and were excluded. No undefinition
was found in project/generated C/header sources or installed Windows SDK and
Visual Studio headers.

In explicitly selected contexts, 24 original-source non-V7VGA branch bodies
or alternatives were replaced by markers, retaining conditionals and the
selected V7VGA implementation. The project-added DAC fallback in `nt_graph.c`
was not an original-source match and remains untouched. The companion JSONL
records ranges and removed-body hashes. This result does not cover unobserved
configurations or assert that a V7VGA-disabled build is supported.

Independent reconstruction from `4a08da6f5` checked all 24 hashes, eight
complete C files and the corresponding consumer flags. Cached x86/x64 product
builds and documentation governance passed. All ten product images matched
the release exactly or differed only by four PE linker timestamp bytes;
runtime tests and publication were not repeated for equivalent images.
