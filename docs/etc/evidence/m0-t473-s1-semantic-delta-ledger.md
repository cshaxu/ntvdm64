# M0 T473 S1 — semantic delta owner ledger

## Scope

This ledger resolves the 13 source-like semantic deltas left by the
whole-closure baseline without treating a build as behavioral proof. The rows
are the current `r26` inventory's files that have neither an inline
`DIVERGENCE` marker nor a direct `src/mvdm/README.md` full-path reference. It
does not establish whole-package reachability or behavioral acceptance.

| Current original path | Compile selection | Actual change and authority | Owner / disposition | Evidence and remaining boundary |
| --- | --- | --- | --- | --- |
| `dos/command/cmd.c` | worker graph | Original `GetSystemDirectory` becomes selected-product `NtvdmGetSystemDirectoryA`; COMMAND initialization remains original. | Retain project firmware/root adapter as sole path authority. | T427 S4 root-layout evidence and `d0c2fc1d0`; startup remains a runtime regression. |
| `dos/command/cmddata.c` | ntvdm product | Removes durable host aliases for SCS fields from the global data carrier. | Retain source layout and field updates, but bind their guest addresses only through the bounded per-operation lease. | `MVDM-HOST-DIV-111` in the MVDM register; `cmdexec.c` owns the local SCS lease. |
| `dpmi32/data.c` | ntvdm product | Adds CPU40 guest GDT/LDT/PM-stack projections and final DOSX IDT descriptor decoding. | Retain the CPU40 descriptor carrier; never reinterpret host/kernel VDM state as guest data. | T420 S18 DPMI package evidence and descriptor-domain fixture. |
| `dpmi32/dpmi32p.h` | ntvdm product | Declares the matching CPU40 descriptor projections and the bounded WOW LDT entry seam. | Retain as declarations of the `data.c`/DPMI owner; no independent policy body exists here. | T420 S18; current WOW LDT exercise remains package evidence. |
| `dpmi32/dpmiint.h` | worker graph | Kernel-VDM `i386` carrier is unavailable; CCPU40 selects the original non-monitor carrier storage. | Retain narrow `CPU_40_STYLE` profile; do not import NT4 kernel-VDM state. | T404/T430 CCPU evidence; package reachability remains S1 work. |
| `dpmi32/dpmimscr.c` | worker graph | Original portable FastBOP failure conditionally publishes an adapter endpoint and reports the result. | Retain provisionally as T422 bridge, owned by `mvdm_softpc_fast_bop`, not a DPMI engine. | `806598b9b` and T422 FastBOP fixture; carrier/teardown reconciliation remains open. |
| `softpc.new/base/ccpu386/iret.c` | worker graph | Privilege-return stack write follows loaded SS width via `set_current_SP`, not operand width. | Retain CCPU40 stack-width correction. | `065ba3b3f` and T430 stack-transition fixture. |
| `softpc.new/base/ccpu386/ret.c` | worker graph | Same lower-privilege stack-width correction as IRET. | Retain atomically with IRET. | `065ba3b3f` and shared T430 fixture. |
| `softpc.new/host/inc/nt_fdisk.h` | declaration carrier | Removes the durable native `pFDAccess` declaration. | Retain the packed source field and obtain it only through the bounded guest lease. | `MVDM-HOST-DIV-111`; no new disk policy is introduced. |
| `softpc.new/host/src/nt_com.c` | ntvdm product | Adds default-off Setup probe records and fixes the original baud argument's explicit unsigned width. | Retain original serial ordering; the observer cannot affect device selection or timing. | `MVDM-HOST-DIV-325` and T406 COM ABI evidence. |
| `softpc.new/host/src/nt_lpt.c` | ntvdm product | Adds default-off Setup probe records around existing printer status work. | Retain the unchanged original status/open/flush semantics; probes are observational only. | `MVDM-HOST-DIV-325` and T420 host-package evidence. |
| `wow32/winsockp.h` | WOW32 graph | Async Winsock contexts gain task-thread serial identity and cancellation declarations. | Retain original WOW32 provider plus one task-lifecycle identity bridge. | `96eb20b87`; full socket teardown remains package evidence. |
| `wow32/wow32.def` | WOW32 graph | Exports `W32TerminateTaskTicket` for selected parent BOP task termination. | Retain narrow parent-to-WOW ABI; implementation is `wow_task_termination.c`. | T449 S3 and `7bdc5169f`; export alone is not lifecycle acceptance. |

## Complete marker/register accounting

Of the 195 semantic differences, 167 source-like paths carry an inline
`DIVERGENCE` marker, the 13 markerless/unregistered paths are resolved above,
and the following 14 are markerless but explicitly registered by their exact
leaf in the MVDM divergence register. The remaining non-source path is
`softpc.new/obj.vdm/sources`, whose inline `MVDM-HOST-DIV-031` records removal
of the unavailable kernel-V86 monitor library from the CPU40 profile.

| Paths | Register authority / disposition |
| --- | --- |
| `dos/command/cmdkeyb.c` | `MVDM-HOST-DIV-114`: preserve the original KEYB packet algorithm while using bounded Registry/root inputs. |
| `dpmi32/dpmi32.c` | `MVDM-HOST-DIV-223` and `-164`: CPU40 descriptor publication plus default-off BOP attribution; no kernel monitor reconstruction. |
| `softpc.new/base/ccpu386/c_bsic.h` | `MVDM-HOST-DIV-071`: declaration-only fixed-width EFLAGS ABI. |
| `softpc.new/base/ccpu386/fpu.c` | `MVDM-HOST-DIV-076` and `-130`: CRT declaration and bounded FPU host-layout/rounding correction. |
| `softpc.new/base/cvidc/evidfunc.h`, `j_c_lang.h` | `MVDM-HOST-DIV-121` and `-075`: declarations matching existing generated C-VID bodies. |
| `softpc.new/base/inc/gmi.h`, `timeval.h` | `MVDM-HOST-DIV-088` and `-032`: selected C-VID callback ABI and original host-time declaration. |
| `softpc.new/base/system/idetect.c`, `base/video/ega_vide.c` | `MVDM-HOST-DIV-024` and `-090`: mirror include-root repair and original `memset4` provider contract. |
| `softpc.new/host/inc/cfpu_def.h`, `nt_inthk.h` | `MVDM-HOST-DIV-270` and `MVDM-SUPPORT-DIV-005`: selected FPU bitfield order and typed interrupt-callback declarations. |
| `softpc.new/host/src/nt_aorc.c`, `nt_eoi.c` | `MVDM-HOST-DIV-102` and `-050`: typed original CCPU accessor/EOI ABI seams; retain source behavior. |

Thus the 195 rows have an explicit marker or owner-ledger disposition. This
does not prove their package lifecycle or behavior; it only removes the
previous unowned-delta accounting gap.

## Removed non-functional deltas

S1 removed an unused `mvdm_shadow_registry` include from `dpmi32/vxd.c` and
restored exact macro whitespace in `dpmi32/xlathlp.h`. It also classifies
trailing whitespace and line endings separately from semantic modifications.
No guest, provider ABI, generated source, or package byte changes result from
these audit cleanups.

## Consequence for recovery order

The first five rows are parent `ntvdm.exe` owner-package concerns; the final
two are `wow32.dll` provider concerns. They do not justify importing an
unselected kernel VDM monitor, a private USER subsystem, or a duplicate
project implementation. The next ledger phase must reconcile complete
original package selection and link/extraction before deciding whether any
retained bridge can be removed.
