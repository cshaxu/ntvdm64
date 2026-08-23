# M0 T241 S2 IVT-Watch Source Recovery

## Question

Can XMS `52:09` execute the original `xmsNotifyHookI15` order without a
keyboard device, a link-only no-op, or an adopted-Bochs change?

## Inputs

- OpenNT `base/mvdm/xms.486/xmsmisc.c:xmsNotifyHookI15` and dispatcher.
- OpenNT `base/mvdm/softpc.new/base/keymouse/keybd_io.c:UpdateKbdInt15`.
- Existing active-stage ordinary-RAM transport and XMS source mirrors.

## Recovery

`src/bx-vdm/bop/opennt/softpc/update_kbd_int15.c` is the direct source
fragment. Its comparison, clear and replacement sequence, including the
production-only diagnostic conditional, remains in source order. The full
`keybd_io.c` is not composed: it owns keyboard/PPI/timer/mouse/video and
historical `host_simulate` product state outside this packet.

The only divergence is documented in the fragment: its two historical global
words are loaded/stored by a named shim around the original body. The shim
uses `bx-mantle`'s generic IVT watch, whose interface carries only vector and
segment:offset values. The stage request configures vector `15h` after its
initial/startup copied actions; reset discards the watch. `bx-mantle` contains
no BOP, XMS, DOS, OpenNT or SoftPC vocabulary, and adopted Bochs sources are
unchanged.

`xms_invoke` now proves that watch state is active before dispatching service
9. It no longer rejects the service outright; after the original dispatcher
returns, it commits the staged pair. An unavailable stage still declines the
provider before the source body can produce CX.

## Observations

- Formal MSVC x64 `/MT` Ninja fixture built at
  `build/M0-T241-S2/formal-r1` and exited `0`.
- The fixture started an actual minimal machine stage, sampled IVT `15h`,
  entered `52:09`, verified original match/update with AX `4567h`, then
  verified original mismatch/clear while CX carried the configured XMS size.
- `Test-T237S3XmsV1Retirement.ps1` passed; the machine-semantic boundary gate
  passed.
- An early link exposed an existing DEM `sas_loadw` global. The fragment now
  macro-binds its source-shaped call through its private shim symbol; no
  cross-owner symbol override remains.

## Interpretation

The source body is locally/formally recovered. This is not keyboard input,
INT15 execution, `BOP 5C`, or guest-continuity proof. Those remain separate
owners; S3 will record its bounded integration observation and final packet
closure.
