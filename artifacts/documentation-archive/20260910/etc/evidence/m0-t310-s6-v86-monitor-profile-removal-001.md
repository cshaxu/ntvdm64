# M0 T310 S6 — CCPU V86/MONITOR production-profile removal

## Decision

The selected original SoftPC runtime is the `i386 + CCPU` interpreter profile.
The NT4 kernel-V86 monitor and its standalone product scaffold are not part of
that profile and may not remain disguised as production MVDM host sources.

## Physical result

- `src/mvdm-host/v86/monitor/` was moved byte-for-byte to
  `docs/etc/legacy_code/mvdm-host/v86/monitor/`.
- `src/mvdm-host/v86/scaffold/` was moved byte-for-byte to
  `docs/etc/legacy_code/mvdm-host/v86/scaffold/`.
- The independent `v86/util/QUIT.COM` source was moved byte-for-byte to
  `src/mvdm-tools/v86/util/`; it is tool-only and not a host-runtime input.
- The only `monitor.lib` entry in the historical aggregate
  `softpc.new/obj.vdm/sources` was removed under `MVDM-HOST-DIV-031`.  The
  remaining original library order is unchanged.

The per-path source/role/disposition record is
[`m0-t310-s6-v86-monitor-production-input-ledger.tsv`](../operations/m0-t310-s6-v86-monitor-production-input-ledger.tsv).

## Retained forms

The original `softpc.new/base/ccpu386` source still contains V86 terminology.
Those files are retained because they implement the selected CCPU interpreter's
guest CPU semantics.  They are not the removed NT4 kernel VDM monitor.  The
same-shaped `adapter-mvdm-host-out/monitor` declarations are likewise retained
only where their README identifies a modern session-owned or explicit
unavailable contract; no kernel VDM body is retained as a production input.

`src/mvdm-host/build.log` retains a historical textual `monitor.lib` mention.
It is an immutable provenance artifact, not a compile, link, runtime or
include input.

## Verification

The following formal commands passed after the move on the MSVC `/MT` x86 and
x64 graphs:

```text
ninja -C build/M0-T310/S5/ccpu/x86 -j8 ccpu-bounded-execution
build/M0-T310/S5/ccpu/x86/ccpu-bounded-execution.exe
ninja -C build/M0-T310/S5/ccpu/x64-clean -j8 ccpu-bounded-execution
build/M0-T310/S5/ccpu/x64-clean/ccpu-bounded-execution.exe
```

Both runs printed the original access-function setup plus `start`,
`returned-start`, `reenter`, and `returned-recursive`, and exited zero.  The
production-path scan also verified all of the following:

- no file remains under `src/mvdm-host/v86/`;
- `softpc.new/obj.vdm/sources` has no `monitor.lib` entry; and
- `src`, `app`, `tools`, and `CMakeLists.txt` have no live reference to the
  relocated `mvdm-host/v86/monitor` or `mvdm-host/v86/scaffold` roots.

This is profile cleanup only.  Firmware and device composition remain S7.
