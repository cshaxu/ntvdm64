# M0 T265 S6 — Bochs-core bridge-file retirement

## Question

Can the four self-authored bridge files be removed from the adopted Bochs
mirror without changing the reached opaque-event or headless-8042 lifecycle?

## Changes

- Removed `cpu/opaque_callback.{h,cc}` and moved their two selector-blind
  forwards to the existing `cpu.{h,cc}` mirror paths.
- Removed `iodev/keyboard_bridge.{h,cc}`.  Restored the adopted keyboard body
  to its original `iodev/keyboard.cc` path; its same-shaped plugin entry owns
  creation, while private teardown remains in
  `bochs-core-overlay/iodev/keyboard_lifecycle.cc`.
- Updated the formal source manifest and the 8042 fixture to explicitly own
  its adapter-port lifecycle and consume the original timed `F2` response
  sequence (`FA`, `AB`, `41`).

## Verification

- Formal MSVC x64 `/MT` Ninja graph: `build/M0-T265-S6/r001`.
- `lib/bochs-core.lib` and `lib/adapter-bochs.lib` build successfully.
- `t261-s5-bop-ingress-fixture.exe` exits zero.
- `t244-s2-headless-8042-lifecycle-fixture.exe` exits zero twice in one
  process.
- Pinned-tree presence scan finds no non-upstream production C/C++/header
