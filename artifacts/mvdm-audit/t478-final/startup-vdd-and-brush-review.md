# S42 startup VDD and inherited brush-boundary reconciliation

[Identity-bound results](startup-vdd-and-brush-results.json) record the unchanged
S41 product and current-source observations. No production repair is made here.

## Original startup VDD owner

SEAM:9bb0ea6f8b8d82105384 is not missing original startup wiring.
nt_msscs.c retains the original reset-then-SetupInstallableVDD call, registry
query/type/allocation error branches, ordered SafeLoadLibrary loop and close/free.
The old commented call before reset is not evidence the later live call is absent.
The selected nt_msscs.obj contributes the public lifecycle owners to the current
worker map. The actual current-package test below establishes execution rather
than inferring it solely from source or symbol presence.

The existing verify-ntvdm-export-live.ps1 writes only a disposable runtime's
NTVDM.REG, selects ntvdm-export-live-vdd.dll, and runs original MEM through run16.
Current package startup-vdd-r1 passes actual name/ordinal allocation operations,
failed-reallocation preservation, matching task-termination callback and normal
MEM completion. The private stage is identity-stamped and removed by its existing
cleanup. No O:/winnt, host-registry write, guest patch or product change occurs.

Historical T420 S44 evidence said the binding never read the machine registry.
That sentence is stale: current DIV-148 and mvdm_shadow_registry.c admit selected
KEY_READ snapshots with package-local override and private handles. Neither the
old statement nor the audit's proposed new manifest/broker/drain architecture
is used as the present product contract. Original allocation/malformed-input
limits and process-lifetime plugin ownership remain; arbitrary plugins and
stronger unload guarantees are not certified. S41 separately covers the selected
MSCDEX plugin, BOP dispatch and controlled unregister/hook cleanup.

Disposition: original startup owner is wired and bounded current runtime evidence
is attached; no new registry loader, broker or tolerance layer is needed. This is
not a claim of exhaustive third-party plugin, DMA or physical-device testing.

## WINMINE handoff: confirmed current code-level collision

The inherited black-background and width handoff is not closed by window-startup
observations. The current alias allocator begins at index1 and returns index<<2.
GETWNDCLASS16 treats values <= COLOR_ENDCOLORS as system-color pseudo-brushes;
the current private header defines that bound as COLOR_INFOBK(24).

The independent wow_gdi_class_brush_boundary.c probe compiles the actual current
alias source as x86, creates a real gray brush, allocates alias4, then applies the
same class-brush decision. It obtains pseudo-brush4 instead of the native brush;
exit1 is the intentional counterexample, not a passed product acceptance test.
The existing alias fixture tests slot layout/reuse/exhaustion but never tests
this consumer distinction, so its success cannot exclude this collision.

This confirms a still-relevant local boundary defect candidate. It does not show
which alias WINMINE uses at registration or prove the cause of its observed
pixels. The next finite step is actual class registration/paint observation on
the current package. Width remains a separate requested-versus-actual geometry
question. Do not hard-code an application size, patch guest, or change aliases
until original handle semantics and the live consumer have been established.

Both cases reuse existing named handoffs; they do not expand the source audit.
The exact11-image S41 release and its Full16/16 remain unchanged.

## Current read-only WINMINE observation

The window observer now also reports native rectangle/client geometry, DPI,
class brush and styles. This adds no input or foreground changes. A first
standalone launch inherited the shell's long PATH and reached an illegal
instruction dialog rather than WINMINE; it is retained as failed observation.
The second run uses the same short package/system32 PATH as the passing Full
harness and reaches Minesweeper. This environment correction is not claimed as
a product repair or a complete explanation of the first failure.

All four current samples report class-brush8 (within the pseudo-color range),
DPI96 and native style34CA0000. This ties the low-valued class-brush ambiguity
to a real current WINMINE class, beyond the separate allocation counterexample.
The observed window is minimized (WS_MINIMIZE is set), with237x39 outer and
zero client size. Therefore its width is not comparable to the historical
normal-window237x240 report, and no board-pixel/normal-geometry success or
failure is inferred from these snapshots. The next observation must establish
normal/restored state and requested-versus-actual sizing separately.

The existing frontier runner intentionally records an observation timeout;
its zero script exit is not application completion or gameplay acceptance.
Its owned processes/stage were cleaned. Source and observer identities plus
four matching samples are retained in the result receipt. No product change.
