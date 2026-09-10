# M0 T257 S4 — DOSX/DPMI Protected Frame Map

## Scope and verdict

This is a source/ABI/failure map, not activation of a `53:xx` route.  It
closes the protected interrupt, fault and IRET *admission* question for the
selected non-WOW i386 DOSX profile.  The original frame family cannot yet be
composed safely through the current generic ABI: it requires one atomic,
source-shaped protected CPU/frame transaction. The ordinary BOP result ABI
can update only low GPRs, selector values and CF/ZF. The existing wider
`generic_ud_outcome_v2` can also carry 32-bit GPRs, segments, a restricted
full EFLAGS mask and RIP, but is a #UD-only sequential context-result path:
it cannot validate and commit the coupled guest-frame byte writes with that
CPU state as one transaction.

Consequently no `53:02`, `53:0A`, `53:0C`, `53:0D`, `53:11`, or `53:14..18`
ingress is admitted in S4.  Native Bochs remains the exclusive architectural
exception and IRET owner.  A later DPMI owner-package slice may introduce a
new, selector-blind *typed* CPU-frame transaction only after its ABI is
separately designed, tested and registered; it must not add DPMI policy to
`bx-core` or `bx-mantle`.

## Original source family

| Source | Original role | S4 disposition |
| --- | --- | --- |
| `dpmi32/dpmi32.c:44..69` | One 25-entry dispatcher: `02`, `0A`, `0C`, `0D`, `11`, `14..18` are one protected-frame lifecycle. | Retain exact table identity already composed in S3; no active leaf route. |
| `dpmi32/dpmiint.h:30..72` | Handler tables, `Frame32`, locked-stack constants and host `VdmTib` aliases. | Session-owned fixed-width state is required; direct `VdmTib` reuse is refused. |
| `dpmi32/dpmiint.c:52..150` | `53:02`/`53:0A` read DOSX stack layouts and install interrupt/fault records. | Source can be mirrored only with bounded stack copies plus a session handler table; `INT 21h` `NtVdmControl` branch is unavailable. |
| `dpmi32/dpmiint.c:357..452` | `53:11`, `BeginUseLockedPMStack`, `EndUseLockedPMStack`; saves/restores EIP, SS and ESP as one nesting lifecycle. | Not composable with the current narrow CPU result; candidate for a later typed frame transaction. |
| `dpmi32/dpmiint.c:494..650` | Software/hardware interrupt reflectors; construct 16/32-bit frames then replace CS:EIP:SP/ESP. | Historical emulator hook callbacks (`VdmInstall*`) are refused; CPU interrupt delivery stays native. |
| `dpmi32/dpmiint.c:655..1054` | `53:14..17` consume DOSX IRET BOP stubs and restore stacked CPU state. | Requires the same frame transaction; no adapter-owned IRET implementation. |
| `dpmi32/dpmiint.c:154..355,861..975` | `53:18` exception reflection and historical fault callback path. | Requires handler table, bounded protected stack access and atomic frame state; no exception intercept is activated. |
| `dpmi32/dpmiint.c:1057..1142` | Descriptor-aware stack sizing/limit check. | May reuse protected range validation only after a typed transaction owns the resulting CPU state. |
| `dpmi32/dpmiint.c:1144..1208`; `dpmimscr.c:58..108` | Install/remove historical emulator interrupt and IRET hooks. | Direct reuse refused: `VdmInstall*` / `EnableEmulatorIretHooks` / `DisableEmulatorIretHooks` are NT4 product hooks, not public APIs. |
| `dpmi32/stack.c:24..171` | DOSX stack switch, RM IVT reflection and carry-preserving IRET simulation. | Its raw `Sim32GetVDMPointer` accesses require bounded copies; it is not a mantle DOS/VDM feature. |
| `dpmi/486/dxintr.asm:441..490` | DOSX publishes 32 fault BOP stubs, four IRET BOP stubs, then native `iret`/`iretd` targets. | Guest-side ABI owner; exact BOP-to-frame pairing recorded below. |
| `dpmi/486/dxint31.asm:1160..1238,1243..2031` | INT 31 vector services and PM↔RM/stack reflector callers. | Guest owner; consumes the same handler/stack lifecycle and may not be partially enabled first. |

## BOP and frame ABI ledger

| BOP | Original entry | Caller input / frame | Required state transition | Direct historical dependency | Current disposition |
| --- | --- | --- | --- | --- | --- |
| `53:02` | `DpmiSetProtectedmodeInterrupt` | DOSX SS:SP: `EIP@+0`, `CS@+4`, vector `+6`, flags `+8`. | Persist vector/flags/CS:EIP then AX=0. | `Sim32GetVDMPointer`; vector `21h` calls `NtVdmControl(VdmSetInt21Handler)`. | Deferred as whole-frame family. Bounded copy/session table is possible later; direct `NtVdmControl` is refused. |
| `53:0A` | `DpmiSetFaultHandler` | DOSX SS:SP: `ESP@+0`, `SS@+4`, `EIP@+6`, `CS@+10`, vector `+12`, flags `+14`. | Persist 0..31 handler record then AX=0. | `Sim32GetVDMPointer`, `VdmTib` handler table. | Deferred with `02`; range/vector checks must precede any source body. |
| `53:0C/0D` | `DpmiDpmiInUse` / `NoLongerInUse` | No guest parameters. | Enable/disable interrupt and IRET hook lifecycle. | `EnableEmulatorIretHooks`, `DisableEmulatorIretHooks`, `VdmInstall*`. | Explicit refusal, not no-op success. Later package may define a bx-vdm lifecycle state only after it has a frame ABI. |
| `53:11` | `DpmiPassPmStackInfo` | ES = locked PM stack selector; fixed offset `1000h`. | Reset nesting; historical x86 returns a `VdmTib.PmStackInfo` host address in CX:DX. | Direct `VdmTib` pointer publication. | Host-address return is forbidden. Requires source-derived fixed-width guest-visible info contract before admission. |
| `53:14/15` | `DpmiIntHandlerIret16/32` | DOSX `PMIntHandlerIRET/IRETD` BOP, stack frame from handler. | Balance locked stack; restore CS:EIP:EFLAGS and adjust SP/ESP, or transfer to DOSX native IRET stub. | Raw stack pointer; locked stack globals. | Deferred; cannot be represented by current low-GPR result. |
| `53:16/17` | `DpmiFaultHandlerIret16/32` | DOSX `PMFaultHandlerIRET/IRETD` BOP, expanded fault frame. | End locked stack; restore EIP/CS/FLAGS/SP/SS atomically. | Raw stack pointer; locked stack globals. | Deferred with `0A`; no adapter IRET emulation. |
| `53:18` | `DpmiUnhandledExceptionHandler` | DOSX fault-vector stub contains fault byte; current PM fault frame on SS:(E)SP. | Reflect to installed handler or fatal path; rewrite nested return frame and CPU context. | Host handler tables, `Ldt[]`, `Sim32GetVDMPointer`. | Deferred with fault lifecycle. Native CPU still raises/delivers architectural faults. |

## Minimum seam decision

The existing `bx_ntvdm_protected_range_action_v1` is sufficient only for a
bounded, selector-blind byte copy. `bx_ntvdm_cpu_result_v2` contains a 16-bit
GPR/selector delta and only CF/ZF flag writes. The pre-existing
`generic_ud_outcome_v2` can express the larger CPU subset, but is restricted
to the #UD bridge and applies fields independently; it has no bounded,
all-or-nothing guest-frame write set. None of these forms can atomically bind
the 16/32-bit CPU frame fields to the matching checked guest-memory writes.
Combining them would expose an observable partially-restored guest state on
validation failure.

Therefore S4 rejects a speculative “frame-session shim” implementation.  The
future minimum must instead be one versioned, selector-blind typed request
whose validation happens before it commits all guest-frame writes and CPU
state.  Its caller and policy remain `bx-vdm`; mantle may execute only opaque
typed mechanical data; `bx-core` continues its normal interrupt/exception and
IRET mechanics.  This is a future T257 child admission, not a Bochs intrusion
selected by S4.

## Independent review

The review rejects three tempting but invalid shortcuts:

1. Treating `53:0C/0D` as successful no-ops would claim that IRET hooks are
   enabled without the frame contract that makes them safe.
2. Reusing either the exception ABI or `generic_ud_outcome_v2` directly would
   turn a CPU-exception/#UD callback into an OpenNT DPMI dispatcher and still
   lacks the matching all-or-nothing guest-frame write set.
3. Publishing the old `VdmTib.PmStackInfo` pointer or `Ldt[]` address would
   leak host pointers into guest-visible ABI and fail on x64.

The next admitted scope is thus source/ABI design for the typed transaction,
not a BOP leaf, a Bochs patch, or a legacy NT4 import.
