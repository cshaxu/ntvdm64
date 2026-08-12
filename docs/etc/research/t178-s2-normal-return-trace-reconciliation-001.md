# T178 S2 normal-return trace reconciliation 001

## Inputs

This record reconciles the retained native-POST T177 S4 trace with the only
source-built normal-return candidate trace, T176 S13--S20. The latter uses
OpenNT SHARE.EXE, whose source path is `INT 21h/AH=4Ch`, and a bounded direct
controller. No new run or code change is made.

## Reconciled evidence

| Transition | Best retained evidence | Classification |
| --- | --- | --- |
| Native lifecycle and target handoff | T177 S4 reaches `50:36` and normal CPU-loop shutdown; T176 S14 reaches `54:01`, `50:36`, then SHARE's `0D10:0106` INT 21h return point. | Proven, but `50:36` is pre-target transfer only. |
| Target's normal termination request | T176 S14 binds the observed `0D10:0106` to SHARE's `mov ah,4Ch; xor al,al; int 21h`. | Proven guest request, exit code zero. |
| PDB termination | T176 S19 records `50:3C` at `0032:532F`, then the finite original-shaped provider resumes at `0032:5333` with zero delta. | Last directly proven BOP boundary on the normal-return candidate path. |
| Guest process teardown | T176 S20 maps the immediate post-`50:3C` instructions to arena release, abort cleanup and parent-stack restoration in original NTDOS. | Proven owner/order; individual guest-state completion is not directly traced. |
| COMMAND `54:11` | Absent from the S13/S19 trace; source makes it conditional on restored COMMAND execution after teardown. | First unproven normal-return boundary. |
| Trace terminal condition | The S19 trace later reaches the pre-existing stack-prefetch/third-exception fault and its watchdog, while T177 S4 separately exits the simulator after `50:36`. | Neither is a guest DOS result or a `54:11` result. |

## Result

The strongest present statement is:

```text
SHARE INT 21h/4Ch -> 50:3C accepted -> original DOS teardown -> [unproven] -> COMMAND 54:11
```

Consequently, the first missing predecessor is not target execution, a DEM
PDB-termination provider, or a COMMAND return provider. It is completion of
the original guest DOS teardown/parent restoration to the COMMAND continuation.
No current trace proves a `54:11` call or its CF-clear continuation.

## Boundary decision for S3

An engine-to-CLI result transport cannot be admitted on present evidence:
there is no captured normal guest-return status at its intended boundary.
S3 need only decide whether this evidence closes the package (recommended) or
whether a future independent guest-control/stack predecessor package is
required. It may not convert simulator exit code, process watchdog outcome,
or the contained `54:11` no-next implementation into proof of a result.

## Confidence

High for the last proven `50:3C` boundary and source ordering; medium for the
precise guest-state cause of the later stack failure; zero for a present
engine/CLI DOS-result transport claim.
