# Console provider implementation ownership

## S14 P50 source review

Baseline 804ad42ae. Question: which parts of console_client.c and
console_compat.c can execute independently of product routing, without hiding
an outer implementation behind a B callback? Inputs are the complete bodies
of src/ntvdm-exe/win32/console_client.c (1094 lines) and console_compat.c
(540 lines), their existing provider crosswalk and prior presentation review.
Procedure: read complete bodies in bounded chunks; trace resource creation,
activation, requests, acknowledgement and disposal. This is source evidence,
not a migration patch or new runtime acceptance.

| Implementation | Ownership and required preservation |
| --- | --- |
| Services installation | console_client.c:621 installs four F functions into compat_console_services: input code page, keyboard layout, get title and set title. B declarations do not remove that runtime dependence. Do not claim this chain clean or replace it with another callback table. Resolve operation ownership together with actual endpoint routing before migration. |
| Endpoint and session | Connection grants, command-ready activation, execution capabilities, authenticated Console identity, release acknowledgement and transport failure latch stay outer. They describe a product protocol, not independent resource storage. Do not move the entire console_client/session into B. |
| Native resources | Kernel-object identity comparison, retained handle ownership, bounded text storage, palette cache and native text conversion are separable mechanisms suitable for B or existing A helpers. Preserve duplicate-handle identity rather than numeric handle equality; CONIN and CONOUT remain distinct. This requires actual implementation relocation, not wrappers. |
| RegisterConsoleVDM | The compatibility text-buffer implementation returns no hardware fullscreen state. Unregistration intentionally does not free the buffer: original SoftPC retains its pointer across native child handoff. Separate resource lifetime from outer ACTIVE admission; preserve the same buffer owner and teardown, not a second session. |
| Activation and release | Deactivation checkpoint can retain ownership; otherwise drain/barrier precedes endpoint close. Channel disposal follows actual physical close. Mouse-route reset occurs only after successful inactive transition and producer quiescence. Failed release must remain failed, not falsely clear state. |
| Publication and teardown | Frame payloads are copied before asynchronous publication. Configuration/retirement drains asynchronous work first. End unbinds the command callback, stops and joins publisher/input watch, then closes channels/handles and destroys locks/graphics. Resource extraction must retain these lifetime boundaries. |
| Native input normalization | Scan-bearing records retain physical semantics; scanless records use layout/code-page conversion including modifiers and bounded Alt-numeric expansion. Conversion can be independent B mechanics only with explicit relevant layout/code page. Do not substitute ambient host settings for the frontend's values. Existing surrogate limitation is not repaired by this audit. |
| Input consumption | Mouse queue capacity bounds consumption before reading. Zero capacity returns to suspension rather than blocking with ICA held. Single physical transitions retain receipts; synthetic multiple transitions do not invent receipt identities. Existing history remains the only history owner. |
| Read and return protocol | input_operation checks reply counts/byte sizes, decodes records, then acknowledges the input watch. An acknowledgement failure remains an error even after decode. Prepend returns one bounded atomic batch, preserving selected physical receipt order; native absence remains unsupported rather than emulating append. This protocol is outer, unlike independent record storage. |
| Screen-cell transport | cells_operation clips once before tiling under the client lock, uses bounded row tiles, validates returned rectangle and byte count, and copies only returned cells. Wholly off-screen semantics are delegated to the real provider. Native screen and code-page semantics cannot be replaced by generic buffer arithmetic alone. |
| Failure and fallback | Absence of a configured endpoint permits native API fallback; a failed configured endpoint does not silently switch to a different Console. Title publication after an authoritative title operation is supplemental. These distinctions must survive relocation. |

## Result and implementation order

These two complete bodies account for 34 open symbol records (24 client,
10 compatibility). They increase combined provider-body coverage to 78/79
files and 342/345 symbols. They eliminate zero dependencies. Remaining direct
provider body is mvdm_softpc_wow_page_domain.c; complete four-root coverage and
migration design are broader than these 79 providers.

First separate independently specified resource mechanisms using existing
B ownership; keep original C display/mouse/keyboard algorithms with C. Then
resolve C's current call sites that request authenticated outer operations:
those cannot be declared independent B mechanics merely because an API looks
like Win32. A complete migration design must state where operation execution
and admission occur without C calling outer policy, including the four service
callbacks. No new B-to-C exception or callback disguise is approved here.

Required later verification: duplicate/reused handle identity; buffer retention
across child handoff; producer join before disposal; active/release failure;
partial and off-screen cell transfers; receipt-preserving input return;
keyboard layout/code page; Win3.x window/fullscreen mouse handoff. No such
runtime test was executed for this documentation-only review. No production,
mirror, guest or release files changed.
