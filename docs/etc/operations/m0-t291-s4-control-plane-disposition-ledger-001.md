# M0 T291 S4 — Control-plane disposition ledger

This live S4 ledger records the original layer first; operation-number reuse
does not merge the owners.

| ID | Original callers / contract | Current disposition | S4 owner / next proof |
| --- | --- | --- | --- |
| CP-01 | `cmdexec.c`, `cmdmisc.c`, `config.c`, `wow32/wkman.c` → `GetNextVDMCommand(VDMINFO)`; BaseSrv/CSR notification, capture and re-entry counting | Partial: adapter-monitor exports the original entry; app owns one immediate single-session command source. No wait/wake queue, WOW caller or broker notification is enabled. | P2 audits the complete source forms and defines the typed single-session command-provider limits; broker expansion stays a named later package. |
| CP-02 | `demsrch.c` → `NtVdmControl(VdmQueryDir, VDMQUERYDIRINFO)` | Closed for binding shape: separate typed monitor capability, unavailable unless explicitly bound. | P1 formal x86/x64 proof; later DEM source composition supplies the actual directory capability. |
| CP-03 | Monitor/SoftPC `NtVdmControl` service classes such as start, interrupt, feature, LDT and printer control | Explicit unavailable. They have kernel-VDM/monitor/device owners and may not use CP-01 or CP-02. | P2 maps each reached service to monitor, firmware/device, DPMI or VDD owner. |
| CP-04 | `wow32/wcall16.c` non-fast `CallBack16` → stack switch, `host_simulate`, restored IP/stack, callback frame AX:DX | Closed as bounded mechanical foundation only. It directly calls SoftPC-shaped mechanics and does not enter CP-01/CP-02. | P3 records loaded callback-stream and stop/re-entry prerequisites before a WOW provider admission. |
| CP-05 | `demdasd.c`, DPMI, monitor, `wkman.c`, firmware/ROM/video host-simulate calls | Shared name only; semantic result is not globally closed. | P3 inventories caller preconditions, stop reason and register/stack postconditions; each remains with its original package owner. |
| CP-06 | `CURRENTPTD` / WOW TD and private TEB lifecycle | Bounded one-thread numeric projection only; duplicate/unbound use rejects. | P4 maps the source TD/TEB lifecycle required before worker or multi-task expansion; no global task state is added. |
