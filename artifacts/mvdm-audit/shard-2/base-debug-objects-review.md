# Base debug selected files: object and release-map audit

Captured scope: shard 2 pending `softpc.new/base/debug/{dasm,ega_dump,profile,trace}.c`. All four current files are byte-identical to both available OpenNT sources. The worker Ninja graph compiles each and places it in its corresponding library; selected source is not itself proof of active release behavior.

- `dasm.c`: the captured release object has no executable section and no map ownership. Source disassembly body is guarded by `#ifndef PROD`.
- `ega_dump.c`: captured object has no executable section and no map ownership. Its EGA dump body requires `EGG` and non-PROD.
- `profile.c`: unlike the first two, its captured object contains executable sections. The source supplies three empty functions in the `#else` of `PROFILE`; the release map has no profile.obj owners. Do not call this object empty, or infer that a profiling implementation shipped merely from its code sections.
- `trace.c`: captured object contains executable sections and initialized/BSS data. The map retains `_io_verbose` and `_sub_io_verbose` as data from trace.obj, but no trace.obj function owner appears. Do not classify this entire file as absent from release. Map ownership does not establish the variables' runtime readers or meaningful tracing operation.

These statements apply to this x86 Release build and captured map only; no debug/PROFILE/EGG configurations or runtime paths were tested, and linker folding/optimization requires caution when interpreting no map entry. The cached and released executables have identical executable sections but different complete SHA-256 values; source identity, object sections and map alone do not establish release-wide functional equivalence.

Reproduce from repository root: `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/base-debug-objects-observer.py`, comparing output with its JSON. Observer checks membership at capture, three source hashes, build edges, archive selection, COFF code flags, map owners and executable-section comparison. No product, guest, shared ledger or build tree was modified.
