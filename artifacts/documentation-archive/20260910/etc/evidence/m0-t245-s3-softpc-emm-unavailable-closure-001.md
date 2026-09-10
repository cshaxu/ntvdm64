# M0 T245 S3 — SoftPC EMS-unavailable predecessor closure

## Question

Can the reached `spcemm.asm:InitSpcEmm` call receive its original no-EMS
outcome without enabling an EMS device, changing Bochs, or manufacturing a
DOS lifecycle result?

## Source And Recovery Decision

The direct owner is OpenNT `base/mvdm/softpc.new/base/bios/emm_fncs.c`,
function `emm_init`. `spcemm.asm` initializes `BX` to zero before `BOP 66`;
after return it treats zero as unavailable and does not install `INT 67h`.

The historical translation unit's optional NTVDM block relies on its PIF/LIM
configuration, physical-page allocation and DMA product shell. That product
composition is neither independently linkable nor admitted. The recovery
therefore uses the smallest source-shaped fallback: mirrored `emm_init` keeps
the original DS:DX capture, total-pages query, BX result, and conditional DMA
ordering; its shim supplies zero total pages. No EMS page frame, allocation,
PIF, BOP 68, IVT write or guest-memory action exists in this implementation.

## Implementation

- `src/bx-vdm/bop/opennt/softpc/emm_fncs.c` — source-shaped `emm_init` body.
- `src/bx-vdm/bop/shim/softpc_emm_unavailable_shim.{h,c}` — per-thread copied
  DS:DX/BX context; no host pointer or retained guest object.
- `src/bx-vdm/bop/softpc_emm_unavailable_v2_generic_ud_bridge.{h,c}` — exact
  real-mode `C4 C4 66`, `RIP+3`, low-BX result only, flags untouched.
- The global bx-vdm ingress includes the route; bx-core and bx-mantle remain
  selector-blind and unchanged.

## Verification

Outside-sandbox formal Ninja graph
`build/M0-T245-S3/formal-r2` source-built the actual module closure: 256
steps, x64 `/MT`, CPU5/P-MMX. The focused fixture validates acceptance,
global ingress, `BX=0`, `RIP+3`, unchanged flags, protected-mode rejection,
wrong-selector rejection and RIP-overflow rejection.

One finite `1,000,000`-tick run staged fresh original-toolchain source-built
`NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM` and `SHARE.EXE` identities from the
recorded T225 S8 producers. Its BOP sequence contains:

1. `8E08:45A6` selector `5F`, accepted;
2. `8E08:79C5` selector `66`, accepted;
3. `8E08:2428` `50:3E`, accepted; then
4. `8E08:78CB` selector `C8`, declined, followed by the original guest
   invalid-instruction vector `06` at `0749:0740`.

The finite run ends in the expected budget terminal after that declined
predecessor. It makes no claim of mouse support, NTDOS EXEC reachability or
guest-child continuity.

## Result And Transfer

S3 is closed. `BOP 66` is code complete, formally linked, locally tested and
observed in the source-built sequence with its source-defined unavailable
result. The next blocker is not a reason to extend EMS: OpenNT maps `C8` to
`v_host_mouse_install1`. Its table/IVT/second-phase dependencies are a
headless mouse-vector owner package and must be source/ABI mapped as a package
before any route is admitted.
