# T228 S1 guest control-flow and CPU owner map

## Purpose

Classify the paired `CS:IP=5A5F:9F51` budget terminal without converting it
into a trace-selected BOP implementation task.

## Established route

| Stage | Source/current evidence | Owner | Consequence |
| --- | --- | --- | --- |
| Initial COMMAND handoff | Original NTDOS `$Exec` COM transfer (`msproc.asm`) and the accepted `50:36` ledger record (`0041:5A70`, `DS:SI=0C41:0100`) | Guest NTDOS plus bx-core real-mode mechanics | The host BOP provider only acknowledges the handoff; it does not load or execute COMMAND itself. |
| Host services before terminal | Existing COMMAND/DEM code-complete package fixtures; only the `50:36` ledger record in the new paired run | bx-vdm/OpenNT host-service plane | No new selector appears after `50:36`; this evidence cannot name a missing provider. |
| Earlier segment-limit boundary | `t225-s56-softpc-realmode-segment-limit-source-abi-failure-map-001.md` and S56 grouped result | bx-core whole real/V86 SoftPC compatibility profile | The earlier `0C41:FFFF` word-crossing candidate was handled as a full CPU profile, not a COMMAND or BOP exception. It is not evidence that `5A5F:9F51` has the same cause. |
| x87/IRQ13 history | T95 retained source/observation ledger: original CR0.NE=0 `math_abort` path; r10 saw `IF=0` before the original IRQ13 raise | bx-core/FPU and native machine interrupt eligibility | An x87 diagnostic in the current log cannot by itself prove missing PIC/IVT/IRQ13 behavior. The historical IRQ13 transfer was not eligible, and that older route was superseded before the current source-built sequence. |
| New terminal | T228 paired immutable result, equal in Direct/Readonly | Unclassified guest-control-flow / minimal-machine boundary | `5A5F:9F51` is a runtime real-mode location rather than a static offset in the active `0C41` COM candidate. Its `LOCK` diagnostic is compatible with data execution after an earlier control-flow loss, but is not instruction provenance. |

## Profile conclusion

The two supported host mutation profiles have byte-identical stdout and stderr
at the 100M-tick boundary. Therefore Direct versus Readonly host capability is
not the owner of the observed divergence. Overlay is not selected and gains no
inference.

## Rejected repairs

- Do not add a BOP provider, DOS shim, synthetic PSP/EXEC return, or COMMAND
  image patch.
- Do not special-case `5A5F:9F51`, the `LOCK` byte sequence, a guest address,
  or an x87 diagnostic in bx-core.
- Do not enable PIC, IVT, BIOS, VGA, or host-device behavior based only on the
  historical x87 tail.

## Remaining evidence gap

The current terminal position lacks the preceding instruction window and the
control-transfer history needed to connect it to guest source or to an
existing whole CPU/machine profile. The retained unaccepted S58
instruction-history work remains separate and is not imported by this map.
A future owner-domain repair may be admitted only after a source/ABI/failure
map identifies a selector-blind, whole compatibility or guest-control-flow
component and its complete regression scope.