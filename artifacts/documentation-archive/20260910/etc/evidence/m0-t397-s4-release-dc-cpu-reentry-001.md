# M0 T397 S4 — ReleaseDC CPU re-entry diagnostic

## Question

Does the first original `WU32ReleaseDC` return during OLECLI initialization
leave the WOW callback without resumed guest execution?

This is a bounded diagnostic question. It does not ask whether `WRITE.EXE`
has reached its own initialization, owns a native window, receives input, or
can exit.

## Method

The ordinary Console-owning `system32\WRITE.EXE` route was first observed
without any inherited diagnostic selector. It loaded the complete WOW32
provider and retained the established visible `WOWExecClass` shell result.

A separate, non-product diagnostic build then did only the following:

1. armed a capped CCPU instruction witness from the original provider's
   already-reached `WU32ReleaseDC` return path;
2. wrote that witness to a compile-time path, so no report environment value
   could change app or guest startup; and
3. ran the same Console-owning observer against the isolated `O:\t396`
   stage for 30 seconds.

The instrumentation was removed by recompiling its three affected objects
without the diagnostic macro, rebuilding `original-ccpu386.lib` and
`softpc-bindings.lib`, relinking both images, and checking the final EXE/DLL
for the two diagnostic strings before restoring the stage.

## Observation

The capped witness records 96 decoded guest instructions immediately after
the provider return. It begins in the already-mapped Kernel31 segment `021F`,
returns to the caller at `1507:0939`, and crosses the adjacent Kernel segment
`0227` through `0227:01BB`. The selected retail-Kernel mapping used by this
packet already resolves `021F` as Kernel segment 1; `0227` is its next
selector, not an unowned host module.

The corresponding ordinary observer still reports:

```text
loaded-wow32=yes
visible-top-level-window-first-class=WOWExecClass
visible-top-level-window-first-title=WOWExec
result=timeout
```

The 96-instruction cap ends while the original OLECLI/KERNEL continuation is
still active, so it does not establish whether the later `GetProcAddress` or
`UnlockData` call has completed. A later fixed provider trace independently
reaches original `WU32LoadCursor`; that later fact does not change this
CPU-only witness's narrower conclusion.

## Disposition

This excludes two previously plausible explanations for the missing WRITE
window: a non-returning `WU32ReleaseDC` host thunk, and CCPU40 failing to
resume guest decode at that return. It does not turn the remaining live state
into a busy-loop claim or a Win16 lifecycle claim. The active S4 frontier
remains the original Kernel/WOW shell-to-application task and message hand-off
after OLECLI's normal continuation.
