# M0 T313 S2 P2 — original SoftPC host-root CRT interposition closure

Date: 2026-08-30  
Packet: M0 T313 S2  
Scope: selected Win32/x86 CCPU40 formal host-root archive only.

## Finding

The original SoftPC host diagnostic package intentionally supplies its own
`printf` and `fprintf` bodies in `softpc.new/host/src/fprt.c`.  Compiling only
that one translation unit with `_NO_CRT_STDIO_INLINE` was insufficient under
the selected `/MT` UCRT route: other original host roots could still emit the
inline CRT bodies, causing `LNK4006` duplicate-definition diagnostics when the
archive was formed.

## Disposition

`New-T310OriginalSoftpcNinja.ps1` now applies
`_NO_CRT_STDIO_INLINE` to the whole selected MVDM graph.  This does not replace
an original caller or the original `fprt.c` bodies.  It restores the package's
single-provider contract: `fprt.c` is the sole selected original provider of
those stream entry points.

The corresponding declaration/build-binding exception is registered as
`ADAPTER-WIN32-024` in `src/adapter-mvdm-host-out/win32/README.md`.

## Verification

After regenerating the disposable external formal root
`build/M0-T313/S2/r1`, Ninja completed:

```text
[54/54] ... lib.exe /nologo /out:original-softpc-host-roots.lib ...
```

The resulting `original-softpc-host-roots.lib` is present (372,444 bytes), and
the completed build logs contain no `LNK4006`, compiler error, or stopped-build
marker.  Original-source warnings remain visible; none were globally disabled
or rewritten by this closure.

## Boundary

This establishes source and formal-link evidence for the selected host-root
cohort.  It does not claim a runnable CPU/SAS/firmware/reset lifecycle; that
bounded behavior evidence remains within T313 S2.
