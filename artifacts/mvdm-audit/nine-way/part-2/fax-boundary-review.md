# Logical part 2: WOW fax and two RASDD external candidates

This is an incremental audit of owned keys only: all 16 part-2 MVDM `fax/` keys and two individually identified part-2 external source-edition keys. It is not a classification of the other 4,771 external rows. All 16 MVDM mirror files match both frozen original editions by SHA-256; the two external working files match their pinned Git blob identities after CRLF-to-LF normalization, while their raw working SHA-256 values are retained in the JSON. Captured x86 worker/WOW32 Ninja graphs contain none of the 16 exact MVDM paths; the current ZIP contains no files named WOWFAX.DLL, WOWFAXUI.DLL or RASDD.DLL. This is a profile/name snapshot, not evidence of all possible installed print drivers.

Original `fax/wowfax/sources` defines a separate WOWFAX GDI driver; `fax/wowfaxui/sources` defines a separate UI DLL. `printer.inf` associates these two with the Windows 3.1 compatible fax driver. WOWFAX's `DrvEnableDriver` and its own driver function table are its direct source-level owners. The separate original RASDD recipe selects `enabldrv.c` and `compress.c` into a printer driver named RASDD. Sharing a GDI entry-point name or RASDD in a description does not establish a call, dependency, ABI equivalence, or permission to import RASDD; original GDI/spooler services and host printing still require separate tracing.

| MVDM key | File-level finding and suggested bounded disposition |
| --- | --- |
| `fax/dummy.hlp` | Original ancillary bytes are `ECHO is on.`; real consumer/producer unknown. **Pending**. |
| `fax/dummy.ppd` | Same literal text, not evidence of real PPD semantics. **Pending**. |
| `fax/faxcomm.h` | Shared declarations included by original WOWFAX driver/UI headers. **Historical separate-driver declaration** for captured profile; state ownership/ABI untested. |
| `fax/printer.inf` | Associates separate WOWFAXUI/WOWFAX DLLs with a fax option. **Historical driver-install metadata**; no live spooler registration demonstrated. |
| `fax/wowfax/wowfax.c` | Selected original WOWFAX driver source and exports/engine callbacks. **Pending** whether its original fax behavior is required and how host printing supplies it; not a generic host GDI provider. |
| `fax/wowfax/wowfax.def` | Original driver entry export metadata. **Historical separate-driver definition**, original linker consumption and callback semantics remain unverified. |
| `fax/wowfax/wowfax.rc` | Listed in original WOWFAX sources. **Historical separate-driver resource**, shipped resource and build provenance unverified. |
| `fax/wowfax/wowfaxdd.h` | WOWFAX driver header including faxcomm.h. **Historical separate-driver declaration**, ABI/lifetime untested. |
| `fax/wowfaxui/wfhelper.c` | Listed in original UI sources. **Pending** original UI/fax behavior and current owner. |
| `fax/wowfaxui/wfsheets.c` | Listed in original UI sources, includes wfsheets.h. **Pending** property-sheet behavior and owner. |
| `fax/wowfaxui/wfsheets.h` | Included by wfsheets.c. **Historical UI declaration** within captured profile. |
| `fax/wowfaxui/wfupgrad.c` | Listed in original UI sources. **Pending** upgrade behavior and owner. |
| `fax/wowfaxui/wowfaxui.c` | Listed in original UI sources. **Pending** UI callbacks/printing behavior and owner. |
| `fax/wowfaxui/wowfaxui.def` | Separate UI DLL's export definitions. **Historical UI metadata**, exact linkage/ABI unverified. |
| `fax/wowfaxui/wowfaxui.h` | Included in UI sources, itself includes faxcomm.h. **Historical UI declaration**; layout/lifetime untested. |
| `fax/wowfaxui/wowfaxui.rc` | Listed in original UI sources. **Historical UI resource**, image binding unproved. |

| External key | Direct evidence and decision |
| --- | --- |
| `SRC:a867497988c30606f28d` | OpenNT-4.5 `rasprint.sp1/rasdd/enabldrv.c`, verified pinned blob after line-ending normalization; original RASDD sources selects it, and it defines its own DrvEnableDriver. **Relevance pending**, since no selected WOW fax → RASDD call or current host seam established. |
| `SRC:1d405e9fd6f5f186c8c4` | OpenNT-4.5 `rasprint.sp1/rasdd/compress.c`, verified pinned blob; original RASDD sources selects printer raster compression. **Relevance pending**, as actual product or selected fax dependency is unproved. |

No fax UI, spooler, actual printer, error or destruction path was run. Even if historical file roles are excluded from this particular graph, printing, fax, cross-process state and alternate profile obligations remain open. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/fax-boundary-observer.py` and compare JSON. No shared ledger, guest, product or build tree was changed.
