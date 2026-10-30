# Native return publication callers during SOL

An isolated WOW32 diagnostic sampled one caller address per 16 calls to the
public `wow_user_native_call_end` entry, retaining the loaded module base,
target HWND and monotonic time. It restores LastError before the unchanged
completion function. Three SOL Options interactions passed with the integrated
page-domain package; the staged manifest, diagnostic source and binary hashes,
map, raw records and analysis are retained here.

Among game-thread samples during dialog open, six mapped to
`wow_native_DefWindowProcA`, three each to `wow_native_SendMessageW` and
`create_bound`, plus individual default, dispatch and message operations.
During close, five mapped to `call_previous_window_proc`, four to
`wow_native_EndPaint`, three to `wow_edit_NtUserGetControlColor`, and two each
to `wow_native_BeginPaint` and `wow_native_DefWindowProcA`. These are samples,
not exact call counts or inclusive timings; the sampler misses `end_wait` and
direct calls to the internal end helper, and nearest map symbols or optimized
inlining can obscure actual callers. Diagnostic file I/O changes timing.

The result shows that painting and default/message processing participate in
the repeated return-publication cost; it does not establish that any of these
operations is callback-free or safe to finish without publication. In
particular default procedures and SendMessage can reenter application code,
and BeginPaint can involve window callbacks. A safe narrower path must prove
the actual operation, scheduler, nested callbacks, window identity and guest
observation contract together. No skip-publication change has been adopted.
