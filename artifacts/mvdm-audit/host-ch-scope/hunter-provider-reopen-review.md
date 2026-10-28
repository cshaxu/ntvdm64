# HUNTER host provider and declarations: profile decision remains open

This corrects the preliminary verbal description: `base/inc/hunter.h` is currently **partial**, not pending; `base/inc/hosthunt.h` is **pending**. `host/inc/nt_hunt.h` and `host/src/nt_hunt.c` were **excluded** at the review snapshot and have since been reopened to pending. The four current files match the frozen original identities; the selected NT PROD route is not claimed to use HUNTER.

## Original source relationships

- `base/inc/hunter.h` describes the Hunter debugger and wraps declarations in `#ifdef HUNTER`. `base/inc/hosthunt.h` describes host-specific coordinate conversion and file modes, also under HUNTER. The latter currently has no confirmed exact consumer in the selected worker source; do not claim a direct edge from it to the two NT files without additional evidence.
- `host/inc/nt_hunt.h` declares the HUNTER menu IDs and `TrapperDump`. `host/src/fprt.c:6-8,65-68` includes the header under HUNTER and writes selected trace text to `TrapperDump`. `host/src/nt_hosts.c:28-30,321-370` uses the same guarded menu identifiers and invokes host console menu control in that alternative configuration.
- Original `host/src/nt_hunt.c:36-250` is a HUNTER-conditional host implementation; it defines `TrapperDump` and its initialization path opens a dump file. Original `host/src/sources` selects `fprt.c` and `nt_hosts.c` but does **not** name `nt_hunt.c`. The current observed x86 PROD build likewise does not select HUNTER or the latter file. This demonstrates an incomplete/alternate original NT diagnostic profile, not that the profile is runnable simply by toggling a flag.
- Selected normal `fprt.c` and `nt_hosts.c`, and the general debugger/guest error behavior, are separate owners. Current `host-tail-role-review.md` excludes the nt_hunt physical role based on the selected original NT recipe; that is valid evidence of nonselection, but not a complete dependency closure or a decision that all original HUNTER capability is outside the present audit.

## Recommendation and S11 boundary

Given that the original HUNTER diagnostic capability remains unresolved and `hosthunt.h` is pending, **reopen `nt_hunt.h` and `nt_hunt.c` to pending** alongside that profile. Keep `hunter.h` partial only for its already evidenced selected original role, and do not promote its HUNTER branch to tested. This is a classification consistency correction, not a finding that current NTVDM fails or an instruction to enable HUNTER in the product.

S11 must establish the original core/host dependency and build closure, exact supported profile and any unavailable legacy console facilities; then decide whether the requested original diagnostic capability can be preserved with a bounded modern host boundary. If required and feasible, validate setup/menu or explicitly chosen equivalent, output-file open/write/failure/close, debug activation, guest/worker lifecycle and teardown. If a facility cannot be retained, document observable impact and use the existing exception process. No runtime execution, guest change, shared-ledger edit or product modification was performed here.
