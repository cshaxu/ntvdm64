# T198 S27 Post-Drive Machine Audit 001

## Question

Does the diagnostic continuation beyond the completed `50:0F` DEM drive
result prove a missing VGA/device capability, or does it first expose an
unproven guest-control-flow or machine-state prerequisite?

## Inputs

- S26 P2's exact source-built NTIO (`0x8400` bytes) and NTDOS (`0x6cd2`
  bytes) CPU5 run, under the single MSVC x64 `/MT` process closure.
- Its bounded witness stops immediately after a valid `50:0F` result.  A
  retained diagnostic continuation instead reported real-mode `LSL`, then
  `prefetch: getHostMemAddr vetoed direct read, pAddr=0x00000000000a2e63`,
  and finally `vga called in trigger_timer stub`.
- OpenNT `base/mvdm/dos/v86/doskrnl/dos/msinit.asm`, `demgset.c`, and adopted
  `bx-core/cpu/protect_ctrl.cc` and `bx-core/memory/misc_mem.cc`.

## Procedure

1. Re-read the S26 controlled witness and its deliberately unbounded
   diagnostic observation; do not rerun the unbounded path or alter its
   fixture.
2. Map the original `SVC_DEMGETDRIVES` caller and its immediate continuation
   from `msinit.asm`.
3. Compare `LSL` semantics and the `0xA2E63` physical address against the
   adopted CPU and memory implementation.
4. Classify each observation by owner and identify the smallest next evidence
   needed to distinguish an incorrect guest state from a machine prerequisite.

## Observations

| Observation | Direct source evidence | Classification |
| --- | --- | --- |
| `50:0F` is a DOS-init input, not a video operation. | `msinit.asm` calls `SVC_DEMGETDRIVES`, stores `AL` in `numio`, then tests `SCS_ISDEBUG`. `demgset.c` describes that service as the logical-drive count used by NTDOS initialization. | `bx-vdm` provider result already closed by S26; it does not authorize a device. |
| The normal source order is `50:0F`, optional debug-only `50:45`, then `50:1B`, `50:32`, and `54:05`. | The adjacent `msinit.asm` instructions show this order explicitly. The `50:45` pair sits inside `test [SCS_ISDEBUG],ISDBG_DEBUGGEE` / `je` guarded code. | The earlier observed collection of BOP markers is not yet a proof of one source-continuous NTDOS execution path. |
| `LSL` is invalid in real or virtual-8086 mode. | `BX_CPU_C::LSL_GvEw` in `protect_ctrl.cc` logs that condition then raises `#UD` when `protected_mode()` is false. | A real-mode `LSL` report is evidence of an instruction/state mismatch until its fault CS:EIP and bytes are captured; it is not evidence for VGA. |
| `0xA2E63` is inside the standard VGA aperture. | `misc_mem.cc` defines `0xA0000..0xBFFFF` as VGA memory and intentionally vetoes direct execute/read host mappings there. `0xA2E63` lies in that interval. | The prefetch veto is the native expected mapping guard. It says the guest attempted execution there; it does not establish that VGA was correctly required. |
| The VGA timer-stub panic is downstream of the access attempt. | The diagnostic log reaches the stub only after the real-mode `LSL` reports and the prefetch veto. | A mantle lifecycle/device frontier, but causality to an OpenNT requirement is unproven. |

## Interpretation And Confidence

The evidence rejects the shortcut “enable VGA because the log mentioned VGA.”
The bounded path proves a drive-count BOP result only.  The extension first
shows that the currently available trace does not establish the faulting
guest CS:EIP, bytes, or how execution reached the VGA aperture.  The source
order also makes any inferred sequence from independent marker bits
insufficient to prove an NTDOS-continuous initialization path.

Confidence is high for the CPU and memory classifications, and medium for the
root-cause classification: the fault address is retained, but the diagnostic
run did not preserve a pre-fault copied CPU record or instruction window.

## Owner Matrix

| Requirement under investigation | Owner | Current disposition |
| --- | --- | --- |
| Copy and record a generic `#UD` fault CS:EIP/window after the typed ingress result | `bx-core` mechanical event, with `bx-mantle`/test observation only | Not yet admitted; must remain selector-blind. |
| Interpret a BOP selector/service or continue a proven host service | `bx-vdm` | Not implicated by this audit. |
| Correct real/protected mode state, code location, or memory lifecycle | `bx-core`/`bx-mantle` | Candidate only after fault-state evidence. |
| VGA, PIC, BIOS, x87 or other device enablement | native Bochs/mantle | Explicitly unadmitted. |

## Follow-Up

Admit one narrow instrumentation S before any continuation: retain a
default-off, selector-blind post-commit observation of the copied generic
`#UD` event's CS:EIP and four instruction bytes.  It must not read guest
memory, parse BOP/services, mutate CPU state, change the watchdog, or enable
any device.  Its result decides whether the next task is guest-entry/state
analysis or an owner-correct minimal-machine prerequisite audit.
