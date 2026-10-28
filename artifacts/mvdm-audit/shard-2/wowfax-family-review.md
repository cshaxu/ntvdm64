# Original WOWFAX optional driver and UI family

All seven shard-2 pending entries match both original editions. The original `fax/wowfax/sources` declares a separate GDI driver; `fax/wowfaxui/sources` declares a separate UI DLL, and `printer.inf` associates WOWFAX and WOWFAXUI DLL names with a Windows 3.1 compatible fax driver. Neither captured worker nor WOW32 Ninja graph contains the seven exact source paths; the captured `assets/winnt.zip` has no same-name WOWFAX.DLL, WOWFAXUI.DLL or DUMMY.PPD. This is not proof of absence from other installations, renamed DLLs, another configuration or live OS spooler state.

| File | Actual source/build relationship and restricted proposed classification |
| --- | --- |
| `fax/dummy.ppd` | Retained original bytes contain only `ECHO is on.`; no actual PPD semantics can be inferred from its extension. **Historical ancillary data**, with its real producer/consumer pending. |
| `fax/faxcomm.h` | Shared declarations included by both `wowfaxdd.h` and `wowfaxui.h` for the driver/UI data interface. **Historical separate-driver declaration** in captured profile; data layout and communication remain untested. |
| `fax/wowfax/wowfax.def` | Exports DrvEnableDriver and DrvDisableDriver for the original driver. **Historical driver metadata**; exact original linker consumption and exported behavior unverified. |
| `fax/wowfaxui/wfsheets.c` | Selected by the historical UI DLL's `sources`; directly includes wfsheets.h and wowfaxui.h. **Historical separate-UI implementation** in captured profile, not evidence that its sheets work. |
| `fax/wowfaxui/wfsheets.h` | Direct declaration dependency of wfsheets.c. **Historical separate-UI declaration**; ABI and lifecycle remain untested. |
| `fax/wowfaxui/wowfaxui.c` | Selected by UI DLL `sources`, includes wowfaxui.h. **Historical separate-UI implementation**; initialization, printer interaction and callbacks remain untested. |
| `fax/wowfaxui/wowfaxui.h` | Includes shared faxcomm.h and is used by UI sources and resource. **Historical separate-UI declaration**; no implication about actual linked state or layout correctness. |

Any excluded classification must be restricted to this historical separate-driver/profile role; do not imply that generic printing, Win16 printing or fax functionality is unneeded or works. Preserve unresolved alternate build, spooler registration, driver deployment and runtime lifecycle as distinct requirements. No original tools, DLLs or guest were run and no product, shared ledger or build tree changed.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/wowfax-family-observer.py` and compare JSON.
