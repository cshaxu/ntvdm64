# S10 fax C/H boundary audit

Scope: the nine pending physical C/H files in `src/mvdm/fax`. This is a source and existing build-evidence audit, not a runtime fax compatibility test. No product, guest, build cache, or shared inventory was changed.

## Original and current ownership

The DOS source catalog marks these files `excluded_win16`; that classifies the DOS audit, not the WOW user feature. The WOW32 catalog lists both OpenNT editions with `pending_boundary_scan` / `provider_membership_pending`. For each of the nine files, its current Git blob (after Git-style CRLF normalization) matches the recorded blob in both editions. Its catalog `sha256` says `unknown_not_content_scanned` and is not a SHA-256 evidence value.

The original `wowfax/sources` builds `wowfax.c` as a GDI driver with the original driver dependencies. `wowfaxui/sources` builds the four UI C files as a separate spooler UI DLL, with Winspool and USER dependencies. `printer.inf` associates those providers. Neither DLL's C sources appear in the inspected current production build graph; this establishes build membership only. The existing package evidence likewise does not establish whether an independently installed provider can be loaded or whether a Win16 application needs this feature.

The selected WOW side is a different part of the chain: `src/mvdm/wow32/wow32fax.c` handles mapped requests in `WowFaxWndProc`, and `src/mvdm/wow16/test/shell/wowexfax.c` participates on the guest side. `mvdm_shadow_registry.c` also recognizes the WOWFAX registry path. Presence of those pieces does not demonstrate that the separate original GDI driver and UI provider are supplied or work. Conversely, their absence from this build does not establish that fax functionality is out of scope. The driver is not WOW32 itself; the UI is not a UI helper statically linked into WOW32.

## File-by-file disposition

| Physical file | Original role and established present build role | Disposition |
| --- | --- | --- |
| `faxcomm.h` | Shared FAXDEV and mapping protocol between original driver/UI and WOW side. | ABI-bearing boundary; S16 trace actual producer/consumer and mapping lifetime. Do not discard as mere driver-private code. |
| `wowfax/wowfax.c` | Original GDI driver, including PDEV, escape and object lifetime; not selected by inspected production graph. | S16 determine provider applicability and needed behaviour; not proof that fax feature is unnecessary. |
| `wowfax/wowfaxdd.h` | Driver-side DDI declarations and structures; consumed with the original GDI driver. | Physically driver-private; S16 retain or remove with the provider decision, not independently substitute its ABI. |
| `wowfaxui/wfhelper.c` | UI provider registry, shared mapping and WOW notification helper; not selected by inspected production graph. | S16 verify actual provider and peer failure/notification lifecycle; conditional spin finding below. |
| `wowfaxui/wfsheets.c` | Spooler property-sheet data and controls in original UI DLL. | S16 trace original ownership, allocations and externally visible dialog behaviour if provider is required. |
| `wowfaxui/wfsheets.h` | Private property-sheet structures and declarations for that UI DLL. | Physically UI-private; S16 disposition follows the property-sheet implementation. |
| `wowfaxui/wfupgrad.c` | Original provider upgrade and path handling. | S16 verify whether upgrade is a supported deployment path and caller path bounds. Do not assert an exploitable overflow from a local concatenation alone. |
| `wowfaxui/wowfaxui.c` | Original spooler UI entry points, document properties and start-document notification. | S16 trace real spooler/provider integration, including modal versus same-process notification. |
| `wowfaxui/wowfaxui.h` | UI provider's private declarations and shared types. | Physically UI-private; S16 disposition follows its actual owners and caller contract. |

None of these nine is established as a production WOW32-linked object by the available graph. No blanket full/partial functionality claim is supported. Any physical exclusion decision must state its build/profile boundary, and the end-to-end fax user capability remains unverified until S16 resolves the provider question.

## Conditional source finding: modal notification completion

`wfhelper.c`'s `InterProcCommHandler(DRVFAX_SENDNOTIFYWOW)` calls `SendNotifyMessage` and then repeatedly checks mapped `status` while using `PeekMessage`; an empty queue has no blocking wait and notification failure is not checked locally. `wowfaxui.c` uses that mode for relevant cross-process prompt and start-document flows. In `wow32fax.c`, `WowFaxWndProc` marks `status` after processing a successfully opened and mapped request; an OpenFileMapping/MapViewOfFile failure can return without setting it. Thus, if this original provider is actually used and the peer does not complete the mapping protocol, the original wait may spin indefinitely. This is a source-level conditional finding, not a reproduced defect in the currently deployed product. `status` means processing completed, while the operation's result is separate `retvalue`; neither is evidence of successful fax operation by itself.

S16 should first establish the actual provider and entry path, then use a controlled shared-mapping/peer-failure test to establish completion, returned result, disconnect and object lifetime. Preserve original modal dispatch and same-process behaviour where required; do not import the busy loop as a new host design, assume a wakeup implies the right request completed, or introduce arbitrary timeout/success. Original provider `FillUiData` allocation failure and `BuildPathW` caller bounds also require ownership and caller evidence before being classified as live defects.

## Evidence and limits

Original recipes: `src/mvdm/fax/wowfax/sources`, `src/mvdm/fax/wowfaxui/sources`, `src/mvdm/fax/printer.inf`. Runtime chain: `src/mvdm/wow32/wow32fax.c`, `src/mvdm/wow16/test/shell/wowexfax.c`, `src/ntvdm-exe/softpc/mvdm_shadow_registry.c`. Earlier independent file/graph analysis is retained in `nine-way/part-2/fax-boundary-review.md`, `host-ch-scope/wowfax-shared-driver-owner-review.md`, `host-ch-scope/wowfax-ui-guest-entry-review.md`, `host-ch-scope/wowfax-property-sheets-owner-review.md`, and `host-ch-scope/wowfax-helper-upgrade-owner-review.md` under `artifacts/mvdm-audit/`.

S16 owns the finite functionality and lifecycle investigations above. S17 only needs to consider additional build configurations if a supported alternate configuration is actually established; it must not absorb a live feature simply because the inspected production graph omits these files. No builds, fax/spooler deployment tests, guest tests, or fault-injection tests were run for this report.
