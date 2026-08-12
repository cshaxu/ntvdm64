# T179 S4 SHARE machine-composition observation 001

## Inputs and controller result

The admitted existing-binary observation used the exact S3 inputs. The direct
controller started `t177-s4-native-post-machine-composition-r1` binary
`8758F4335CB32B4FB97688ED3860E8B0C9E86D8155E6AA316E54F346177E8BFA` with the
retained 882-byte source-built SHARE target, T182 profile/configuration and
POST ROM identities. The empty-tail parameter-binding attempt made before the
actual controller call never started Bochs or produced a log; the actual run
is the sole execution.

`observation.json` records 459 ms elapsed, no watchdog termination, exit code
1 and Bochs log SHA-256
`565DED1A18826A0AF7F7A8C8CDBD5D0248E492F09D3718C5823FDE43601BE1BF`.

## Observed markers

| Marker | Count/result |
| --- | --- |
| `50:3C` PDB-termination BOP | Reached and resumed with zero delta. |
| Selector `06h` | 88 observations. |
| Machine composition report/outcome | 88 / 88; representative report names original fault `CS:IP=95AB:03C1`, bytes `C4 C4 54 01`. |
| Old self-loop stack fault | Zero `stackPrefetch` markers. |
| Guest continuation | Continues through later INT 21h/2Fh activity after the former loop. |
| COMMAND `54:11` | Zero observations. |
| Last relevant host boundary | A later `54:01` at `95AB:03C1` passes through, vectors to `06h`, and is handled by the original machine island. |
| Terminal | `cpu loop quit`, exit 1; not a DOS/CLI result. |

## Interpretation

The result validates the permitted machine-composition exception: the original
SoftPC `illegal_op_int` handler consumes the source-defined INT 06 pseudo-ROM
path, advances the saved fault frame and prevents the former recursive
frame-push/stack-limit failure. Bochs remains owner of exception delivery and
the adapter remains pass-through; no vector, device, stack or service result
was fabricated.

The normal-return path still does not reach COMMAND `54:11`. Its next visible
boundary is `54:01`, so the remaining question is COMMAND lifecycle/provider
disposition after the declared target completes. This observation cannot
justify a CLI exit-status transport or a generic machine treatment for BOP
selectors.
