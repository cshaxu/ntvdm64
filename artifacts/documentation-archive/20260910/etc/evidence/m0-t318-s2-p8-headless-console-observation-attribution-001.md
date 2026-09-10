# M0 T318 S2 P8 — Headless console observation attribution

## Question

Does the `The handle is invalid` dialog observed after the V7 VGA recovery
identify a product-side SoftPC startup blocker?

## Procedure and source path

The P7 observer deliberately launched the product with inherited standard
streams ignored, then enumerated only that child process's public Win32
windows.  A second attempt used `cmd start` from the same pipe-owned agent
process; no target process remained after the observation interval.

Original `host/src/nt_reset.c::host_applInit` calls
`init_host_uis`, whose original `host/src/nt_uis.c::InitScreenDesc` obtains
`STD_INPUT_HANDLE` and `STD_OUTPUT_HANDLE` and immediately calls the original
console-mode setup.  A child with no usable inherited console handles therefore
reaches the original `GetConsoleMode` error path and reports Win32
`ERROR_INVALID_HANDLE`.

## Result

The dialog is caused by the test transport's absent console streams. It proves
that P7 passed the prior `vga.rom` lookup, but it does **not** establish an
additional product defect, an NTIO failure, or a missing BOP/adapter.

## Disposition

No source change is admitted. A runtime-continuity observation must be made
from a genuine interactive console (or a separately admitted console-owning
test harness that supplies valid `CONIN$`/`CONOUT$` handles). Until then, the
formal x86/x64 compile/link closure and the V7 VGA firmware-selection evidence
remain valid, while guest execution remains unproven.
