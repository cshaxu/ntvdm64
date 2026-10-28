# S16: minimal original WOW multimedia handle-resolver repair design

## Evidence and scope

The selected original `wow32/wmmedia.c` exports `WOW32ResolveHandle` and
`WOW32DriverCallback`. The [original WINMM contract trace](wow-winmm-original-contract-review.md)
shows the historical multimedia provider's six-parameter `BOOL APIENTRY`
mapping interface and output ownership. The [controlled selected-object
fixture](wow-winmm-resolve-object-review.md) observes missing-provider
return value 6 and overwritten fake-provider outputs. The [local WINMM
null/invalid comparison](wow-winmm-real-null-review.md) additionally shows
the cached original wrapper returning TRUE for input native handle `1` while
this machine's real x86 WINMM provider returns FALSE. These are bounded
observations; no actual immutable guest/extension caller or valid device
handle has yet been bound to the current delivery.

## Minimal source-shaped correction

In `wmmedia.c`, keep the original provider lookup, cached pointers, handle
classes, WOW/native ownership, ordinary USER/GDI conversion and public ABI.
Do not change the shared `GET_MULTIMEDIA_API` macro: its callers may have
non-BOOL error contracts. For the two BOOL APIs, pass `FALSE` as their own
missing-provider result. This makes module and export failures report failure
instead of the nonzero `MMSYSERR_NODRIVER` value.

In `WOW32ResolveHandle`, retain the original direction switch and direct
USER/GDI conversion, including output-zero and guarded output writes. In each
wave/MIDI arm in each direction, return the native provider's BOOL result
directly after passing the original arguments and output pointer. This keeps
the provider as the sole authority for native multimedia handle identity and
prevents the wrapper from overwriting its output with an uninitialized local.
Do not copy or synthesize a handle on the host side. For an unsupported handle
type, return FALSE without writing either output; the old local was
uninitialized, so it did not specify a reliable value. Preserve the existing
unsupported-direction FALSE behavior and the existing lookup order; do not
silently change USER/GDI availability when WINMM is missing. Do not expand
the original exception boundary around provider calls without evidence.

`WOW32DriverCallback` otherwise remains a direct native forwarding call; its
missing-provider branch returns FALSE. Do not replace the original callback
ring, create a project-owned handle registry, change the guest, or import NT4
WINMM. Record the resulting minimal `wmmedia.c` mirror diff with a precise
DIVERGENCE marker, original and modern contract evidence, ownership,
affected profile and acceptance results. This is a proposed correction, not
an already approved change to arbitrary legacy error semantics: if a real
caller depends on a stable nonzero missing-provider result, surface that
observable semantic boundary before extending the change.

## Acceptance matrix

Use real original selected objects and independent processes where static
`mmAPI` caching would otherwise contaminate cases. A resolver seam can control
module/entry availability and valid multimedia results; every test must state
whether it calls an actual original wrapper, a controlled provider or native
WINMM. Do not use a fake provider result as proof of actual device behavior.

| Family | Required observations |
| --- | --- |
| Loader | Separate module-missing and entry-missing cases for both BOOL APIs return FALSE, do not call provider, and leave unrelated output sentinels unchanged. Test fresh process and cached successful provider. Check last-error only against an established original contract; do not invent one. |
| Multimedia success | All wave-in/out and MIDI-in/out types, both directions, nonzero distinct guest/native handles; fake provider writes distinct sentinel, returns TRUE, and caller sees its exact output/result without cross-output mutation. Repeat with a real valid device handle and immutable guest/extension where an admitted provider is available. |
| Multimedia failure | Both directions and each type, provider returns FALSE with output unchanged and separately with a provider-owned written value; return FALSE and do not overwrite either provider outcome. Test zero and invalid handles against the actual local provider only in isolated processes; record provider behavior and host identity rather than assuming all versions match. |
| USER/GDI | Both directions: preserve successful original conversions and zero/failure behavior with correct type/width; when WINMM lookup fails preserve existing lookup dependency apart from the approved BOOL failure result. Verify mapped aliases are neither created twice nor retired by the resolver. |
| Inputs and faults | Unsupported direction/type return FALSE without touching outputs; null/invalid output addresses and bad provider behavior are tested only in isolated processes with the existing exception boundaries explicitly recorded. Verify calling convention/stack and argument order on x86. |
| Callback forwarding | Valid original/native callback result and arguments pass through exactly; missing module/entry returns FALSE. Test relevant callback types, a nested callback and failed provider using explicitly scoped seams and then a real admitted guest/device when available. |
| Lifecycle | Initialize/repeat/unload and task exit with nested callback and in-flight native completion; check guest/native handle ownership, no use-after-free, stale mapping or sibling-task impact. Test module/provider failure before and after caching, only within the actual loader and NTSRV/WOW lifetime contract. |

For valid handles, obtain them from an actual admitted provider/device or a
well-defined controlled provider. Never fabricate an integer and call it a
valid native handle. If no real device or extension is available, report that
gap rather than equating seam execution with end-to-end guest acceptance. Tie
all real runs to exact guest, WINMM, WOW32 and worker hashes. Verify the
private modern provider's ABI and documented/exported availability per target
host; original NT4 source and a single Windows build are not a universal
compatibility guarantee. Preserve the existing positive direct-provider
failure observations as regressions and distinguish any genuine semantic
exception from unchanged original behavior.
