# Missing and zero-byte excluded host C roles

At the host C exclusion census checkpoint, twelve paths were absent under `src/mvdm` and two existing files were zero bytes. For every one of the 14 paths, both supplied original OpenNT and OpenNT-4.5 files exist and their freshly computed SHA-256 exactly matches the respective frozen-ledger identity. Thus **missing means absent from the current mirror**, not absent from the originals. The two empty files have the genuine empty-file SHA-256 in all three locations. No current guest, product, shared ledger or source was modified and no runtime test was run.

## Absent original CPU30 monitor/scaffold roles

The original paths below all belong to the `v86/{monitor,scaffold}/i386` historical CPU30/kernel-VDM route, which current source policy expressly excludes from product selection. Their functional responsibilities are not exempted from the T objective: current CPU40/SoftPC/adapter owners must be checked independently, and the named owners below are investigation/acceptance boundaries, not claims that their behaviors are proven equivalent.

| Original physical source | Original observed role | Current functional boundary that remains in scope |
| --- | --- | --- |
| `v86/monitor/i386/int.c` | Monitor hardware interrupt/IRQ support, including a DPMI interrupt handler and IRQ13 EOI coordination. | Original selected SoftPC `ica.c`/host `nt_eoi.c`, CPU exception and DPMI owners; verify real IRQ/EOI, nesting, reset and errors. |
| `v86/monitor/i386/monitor.c` | User-mode x86 monitor, event and VDD fault support tied to its monitor environment. | CPU40 execution, event and public VDD boundaries; verify faults, reentry, VDD buffer/handle and task retirement. |
| `v86/monitor/i386/sas.c` | Monitor SAS memory, A20 and VDD mapping route. | Selected CPU40 SAS and bounded guest-mapping leases; verify guest memory, A20, aliases, failures and lifetime without treating host pointers as guest addresses. |
| `v86/monitor/i386/thread.c` | Monitor-thread list and thread tracking. | Current worker/WOW task identities and session/thread lifecycle; verify task exit, callback/reentry and service loss. |
| `v86/monitor/i386/proflib.c` | Historical kernel profiling support. | Original optional profiling/debug scope remains pending; production guest execution is not proof of a profiler. |
| `v86/monitor/i386/spcstubs.c` | Monitor-specific SoftPC scaffolding and quick-event/CPU stub interfaces. | Original selected CPU40 and NT qevnt provider; retain event scheduling, cancellation and callbacks and do not treat stub functions as a valid implementation. |
| `v86/scaffold/i386/fakebop.c` | Test scaffold with fake BOP functions, plus DEM/WOW dispatch plumbing; many fake functions are empty. | Original selected BIOS BOP table, `nt_bop`, DEM and WOW dispatch; verify guest-visible entry, invalid cases and lifecycle, not the fake no-ops. |
| `v86/scaffold/i386/fakekbd.c` | Test scaffold keyboard input and event thread. | Selected `nt_event`/`nt_keycd` and `keyba` controller path; verify real keys, EOI/read order, queue and retirement. |
| `v86/scaffold/i386/fakevid.c` | Test scaffold BIOS-video mapping and entry. | Selected original VGA/Video7/C-VID and frontend frame path; verify modes, transitions and cleanup. |
| `v86/scaffold/i386/fakeinit.c` | Scaffold BIOS/ROM initialization. | Selected original reset, ROM initialization and guest startup; no claim full guest startup follows from scaffold absence. |
| `v86/scaffold/i386/softpc.c` | Standalone scaffold launcher, console, configuration and termination code. | Current NTSRV/worker/frontend lifecycle and original selected initialization; verify startup, teardown and failure. |
| `v86/scaffold/i386/stubs.c` | Test scaffold empty placeholder functions including IO, CPU, ROM and timer names. | Preserve the real selected providers and tests; never recover behavior by importing placeholders. |

## Two zero-byte original physical sources

| Existing path | Original identity and current owner | Qualification |
| --- | --- | --- |
| `softpc.new/base/cvidc/vglob.c` | Exactly zero bytes, as are both corresponding original files. The actual selected original C-VID accessor input is `base/cpu/src/evid/vglob.c` (not this zero-byte namesake); see `vga-accessor-env-trace-review.md`. | Its zero-byte physical role can remain excluded. VGA accessors/state and all modes still require their own evidence. |
| `suballoc/debug.c` | Exactly zero bytes, as are both original files. Original `suballoc/sources` uses `suballoc.c`, with separate `tsa` test program. | Only the empty file is excluded; allocator policy, callback failures, rollback, lifetime and any useful diagnostics remain independent work. |

No wrong physical-file exclusion was established under the stated CPU30 stopping rule and exact empty-file identities. That conclusion does **not** establish that the CPU40 or project-owned replacements pass their behavioral/lifecycle tests, nor that optional profiling is inapplicable. Recheck the current authoritative scope/policy, identities, selected source and actual artifact before relying on this result after later changes.
