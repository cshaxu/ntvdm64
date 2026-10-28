# WOW multimedia host boundary and S16 execution requirements

On this machine the read-only PE export scan of x86
`C:/Windows/SysWOW64/winmm.dll` finds all eleven names recorded by T466:
the nine guest multimedia message/notification names plus
`WOW32DriverCallback` and `WOW32ResolveMultiMediaHandle`. The observed DLL
version is `10.0.26100.9549` and its SHA-256 is recorded in the
[receipt](wow-winmm-boundary-evidence.json). The
[observer](wow-winmm-boundary-observe.py) repeats the named-export scan and
binds the selected source and cached map hashes. Names and PE exports do not
prove actual loading, private ABI/layout compatibility, device availability
or behavior on any other host version.

In the original Win16 source, `ThunkInit` obtains the WINMM notification
function and publishes `vpCallbackData`, then resolves the other driver
message entries. `ThunkTerm` calls `StackTerminate` and removes the interrupt
handler before passing NULL to `Notify_Callback_Data`, then unlocks and frees
the storage. Original `stack.asm::DoInterrupt` snapshots send count, copies
arguments from the slot indexed by the **old** receive count, publishes the
incremented modulo-16 receive count, then calls `DriverCallback`. The prose
comment above the assembly describes indexing the incremented count; tests
must use the executed source instruction sequence rather than silently
choosing that comment. This observation does not prove which convention the
currently installed immutable guest binary implements or what modern WINMM
expects, and it does not by itself prove a bug.

Selected WOW32 uses the original `WMM32CallProc32` path with an execution
boundary adapter; the cached map identifies both the wrapper and original
body. The old `wmmedia2.c` and `wmmstru2.c` structured converter bodies are
disabled by their original `#if 0`; the captured map does not establish an
active `GetMMedia16/32` or `FreeMMedia16` path. Historical T466 synchronous
timer and export-resolution results are bounded witnesses, not asynchronous
callback or wave/MIDI acceptance. Do not enable the old converter merely to
manufacture a second owner.

S16 must bind the immutable guest DLL and tested host/WOW32 hashes, then
observe normal and failed device open, callback argument identity/order,
ring wrap/full behavior, unregister and callback-in-flight task exit. Include
the original receiver's indexing/publication sequence and the actual
interrupt-removal versus pointer-withdrawal order. A controlled host fixture
may isolate queue or failure behavior but cannot prove real WINMM produces
the same private protocol; a real device result must identify its provider.
If hardware or an applicable original caller is unavailable, record that
specific gap rather than claiming device acceptance or inventing a new
runtime owner. No source, guest, product, build or shared ledger was changed.
