# M0 T241 Keyboard, IVT And INT 15 Recovery Plan

## Scope

T241 is the second dependency-ordered machine package. It recovers only the
original XMS `52:09` notification contract and the minimum SoftPC keyboard
INT15/IVT optimization state that this contract demonstrably needs. It is not
permission to enable a general keyboard controller, console-input service,
timer, mouse, video or generic BIOS.

## Ordered S plan

| S | Work | Exit evidence |
| --- | --- | --- |
| S1 | Single-person dual-role source/ABI/lifecycle audit of `xmsmisc.c:xmsNotifyHookI15`, `xmsdisp.c`, `keybd_io.c:UpdateKbdInt15`, keyboard initialization and current source/mantle seams. | Four-rung source ledger, exact IVT/BDA state map, owner split, required vs explicitly rejected code surface, and failure behavior. |
| S2 | Admit only the smallest selector-blind mechanical state/IVT capability proved by S1; compose the original XMS source body through a named bx-vdm shim. | Exact vector matching/update/reset behavior, no BOP/DOS/OpenNT terms in bx-core/mantle, and registered exception only if source evidence proves a Bochs change unavoidable. |
| S3 | Run reset/lifecycle and `52:09` local regressions, formal Ninja build, boundary scan and one bounded native observation. | `52:09` source body succeeds or preserves a source-proven failure; native result is recorded without extending the package. |

## Exclusions

- Do not make the existing `UpdateKbdInt15` no-op succeed.
- Do not put keyboard, INT15, XMS or BOP names into `bx-core`/`bx-mantle`.
- Do not implement `BOP 5C`, COMMAND console input, PS/2/PPI ports, IRQ1,
  keyboard polling, `host_simulate`, timer threads, mouse or video unless a
  later source audit proves an independently admitted owner package.
