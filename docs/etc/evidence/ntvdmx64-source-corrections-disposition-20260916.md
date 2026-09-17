# NTVDMx64 correction-candidate disposition

## Question

Should the unadmitted “Selective NTVDMx64 source-correction recovery” package
remain ahead of WOW/debugger restoration?

## Inputs and procedure

The review froze the candidate proposal, its pinned NTVDMx64 patch tree at
`84a13d2e7bb1a55d11148971e5b9c8ec99f670bf`, current selected x86 sources,
the T413 exception evidence, the T419 x87 evidence, and the DPMI/CPU40 owner
ledgers. It compared each candidate family by its reached owner, current
equivalent, source-policy eligibility, and available reproduction evidence.
No product source, build graph, or runtime artifact changed.

## Disposition

| Candidate family | Finding | Disposition |
| --- | --- | --- |
| `old-src/fpu.patch` | T419 restored the required SoftPC layout and both conversion hunks with focused layout proof and formal x86 build. | Already recovered. |
| `common/ccpu.patch` exception scope | T413 applied the five source-proven conditional scopes; the original negative control fails 101 assertions. | Already recovered. |
| `common/spurint.patch` | Current `DIV-221` preserves the source PIC `-1` rejection before unsigned narrowing, so no fabricated `FFh` vector reaches CCPU. | Already equivalent at the stronger caller boundary. |
| `minnt/callconv.patch` | The only reached original worker-thread mismatch is already bounded as `DIV-023`; bulk declaration changes are not justified. | Already recovered where needed; reject the bundle. |
| `minnt/mouse.patch` | Its integer scale state conflicts with the current verified original INT 33 absolute-coordinate callback path. | Reject. |
| `ccpu_segperm`, `ccpu_optim_*`, `sas`, `printerprt` | These are an upstream HACK, performance changes, or product compatibility policy, without a current reproduced defect. | Reject. |
| `ega_*` | Rendering changes require a demonstrated C-VID/graphics workload and are not evidence of a current DOS/WOW defect. | Defer to a reproduced graphics issue. |
| `common/dpmi.patch` | It mixes potentially relevant selector/client teardown cases with excluded MONITOR, HAXM, HungApp, debugger, and guest-artifact changes. Current CPU40 DPMI has its own original-owner ledger and unresolved lifecycle records. | Transfer only a reached prerequisite to the WOW16 lifecycle package; reject bundle import. |
| `common/8253.patch` | Current timer source retains the old mode-zero division, but no accepted DOS or Win16 run reproduces a timer fault. | Record debt; no queue position. |
| `pcibug.patch` | It changes immutable guest BIOS behavior without a reached caller or guest-media admission. | Reject. |

## Conclusion

The package is withdrawn. It is not an original-owner recovery package and
its remaining material cannot be honestly grouped into one implementation
scope. The package-completion candidate becomes the queue head. It owns
whole-package DPMI/CPU40 and WOW/debugger recovery without inheriting a blanket
external patch exception. The sole retained untested timer observation is
recorded in TODO.
