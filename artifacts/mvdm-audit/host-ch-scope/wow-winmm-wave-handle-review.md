# Test-owned native wave handle: provider and original-wrapper observation

The existing x86 noninteractive multimedia feasibility test reported one
wave-output and one MIDI-output endpoint on this host; this is a count, not
proof that every device can be opened or a Win16 alias exists. A separate
test-owned x86 process per mode queried 22050 Hz, mono, 16-bit PCM support,
opened wave-output device 0 with `CALLBACK_NULL`, submitted **no** wave buffer
and immediately closed it. Both observed queries, opens and closes returned
zero; the processes exited zero, without an exception or stack mismatch. No
guest, audio playback, UI or system setting was involved.

The direct local `WOW32ResolveMultiMediaHandle` observation returned FALSE
for the test-owned ordinary Win32 wave handle in 32→16 direction and left the
16-bit output sentinel unchanged. The original `WOW32ResolveHandle` wrapper,
linked from the unmodified selected cached `wmmedia.obj`, returned TRUE and
wrote the low 16 bits of its process's own native handle. The two calls use
**different processes and different handles**, so this is a matched test shape,
not a same-handle A/B. An independent repetition produced the same respective
FALSE/TRUE outcomes and the wrapper again wrote its native handle's low 16 bits.
The direct result does not prove that a native handle has a Win16 mapping;
indeed no real Win16 caller or alias was created here. `GetLastError=126` was
observed after both calls but its internal cause or contract is not established.

Each process preloads its local x86 WINMM; the wrapper process assigns that
module to the original object's `hWinmm` and uses fail-fast seams for unrelated
GDI/memory/directory entrypoints. This therefore does not exercise ordinary
WOW module loading, the full execution boundary, a guest extension or a
published DLL. The module's final file path resolves to
`C:\Windows\SysWOW64\winmm.dll`, SHA-256
`FF2ABD18BF1404A2D41831CEB6BFD25A812FABA7C29E66C0F2E0CFF5389FC344` on this
machine. Behavior on other host versions remains unknown.

The [observer](wow-winmm-wave-handle-observe.py) and
[receipt](wow-winmm-wave-handle-evidence.json) record executable, source,
build recipe, selected object, wrapper map and local DLL hashes, both process
outputs and exit values. The build uses no `/force:unresolved`; the map names
`wmmedia.obj` as the wrapper provider. Handle values are process-specific, so
future raw receipts need not be byte-identical. An unavailable device or
unsupported format is recorded as a nonzero process exit, not a fabricated
passing result.

From the repository root, build with
`cmd /c artifacts\mvdm-audit\host-ch-scope\wow-winmm-wave-handle-build.cmd build\t473-s10\winmm-wave-handle`,
then run `python artifacts/mvdm-audit/host-ch-scope/wow-winmm-wave-handle-observe.py build/t473-s10/winmm-wave-handle`.
MSVC x86 and `objdump` are required. S16 still needs a genuine Win16 alias and
caller, both directions, all handle types, callbacks, errors and task/device
retirement before any end-to-end claim. No guest or product source was changed.
