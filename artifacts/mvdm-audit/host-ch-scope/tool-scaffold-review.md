# Tool and scaffold candidates: bounded host-build review

The [observer](tool-scaffold-observe.py) and [receipt](tool-scaffold-evidence.json)
record all 72 provisional candidates, current and two original file hashes,
their nearest original `sources` file and basename occurrence, and exact-path
references in four named current Ninja graphs and recorded dependencies. The
receipt was independently replayed with identical JSON. A basename in a
manifest is lexical evidence, not proof of active compilation or all header
consumers. The named graph/dependency boundary does not cover every profile.

All 72 exact source paths are absent from those graphs and dependency records.
However, 14 `v86/scaffold/i386` files are absent from the current tree while
present in the first original. Original `v86/scaffold/sources` specifies
`TARGETNAME=ntvdm` and `TARGETTYPE=PROGLIB`; its `i386/sources` names six
scaffold C inputs, including a `softpc.c` with a `main` entry. The presence of
fake device modules does not by itself establish which original configurations
used the target or that the currently selected NTVDM preserves its semantics.
The existing NTVDM entry, console initialization and relevant functionality
need owner-by-owner comparison before any exclusion decision.

The other groups have distinct original product targets: `bde` and `vdmexts`
are `DYNLINK` debugger-extension targets; `ieuvddex` is a separate `DYNLINK`
target named `ntvdm`; `vdmutils` recipes specify `forcedos`, `graftabl`,
`pifedit` and `win` applications. Physical separation from current worker
inputs does not establish whether the T's desired debugger or utility
capabilities are supplied elsewhere. Headers and architecture-specific
sources also need their own conditional owner checks.

Two basename collisions require care: production `cmdpif.c` and `nt_pif.c`
spell `pif.h`, but the current x86 worker dependency snapshot resolves the
header to `src/opennt-abi/source/public/internal/windows/inc/pif.h`, not
`vdmutils/forcedos/pif.h`. Likewise, `dpmi32` dependencies resolve
`dpmi32/precomp.h`, not `bde/precomp.h` or `vdmexts/precomp.h`. This is
recorded-resolution evidence for the selected builds, not a claim about all
original or future include paths.

No classifications, source files, guests or product builds changed. Scope and
any subsequent restoration decision remain separate from this observation.
