# M0 T287 S5 — XMS UMB and IVT/INT15 source-family binding evidence

## Result

S5 is closed at its source-family boundary. The original `xmsumb.c` and
`xmsmisc.c` source bodies compile and execute on x86 and x64 with their exact
local outcomes. No `52:xx` route, host pointer, private mapping manager, or
Bochs-side UMB/XMS policy is enabled.

## Recovered source contracts

- `xmsInitUMB`, `xmsRequestUMB`, `xmsReleaseUMB`, and
  `xmsReleaseUMBNotify` remain original source. A deliberately empty
  `ReserveUMB` test owner proves the original no-physical-span outcomes:
  request returns `AX=0, BL=B1`; release returns `AX=0, BL=B2`.
- `xmsSysPageSize` uses its unchanged public `GetSystemInfo` body and returns
  `AX=4096` in the focused runs. `xmsQueryExtMem` retains the direct original
  `xmsMemorySize` result and returns `AX=640` in the fixture.
- `xmsNotifyHookI15` remains unchanged. `getCS` is now available through the
  same-shaped `adapter-mvdm-host-out/softpc` register facade and its neutral
  typed `adapter-bochs` mechanical read. The fixture proves unchanged
  `UpdateKbdInt15(CS, AX)` call ordering and the subsequent `CX=xmsMemorySize`
  result.

## Explicit ownership transfers

- A successful physical UMB span, `ReserveUMB`/`ReleaseUMB` ownership
  transitions, `VDDReserveUMB`, and `UMBNotify` remain original
  `mvdm-host/softpc.new/host/src/nt_umb.c` work. Its historical `MONITOR`
  branch uses NT4 private VM/section APIs and cannot be replaced by a test
  span. The next queue candidate, **Original VDM monitor substrate**, owns
  the same-shaped monitor/SAS physical-span seam; until then only the original
  empty-span B1/B2 behavior is product-admitted.
- The production implementation of `UpdateKbdInt15` remains the original
  `mvdm-softpc-firmware/softpc.new/base/keymouse/keybd_io.c` owner. The
  existing keyboard/IVT/INT15 recovery plan is its named successor. The
  fixture carrier is test-only and does not claim keyboard optimization,
  controller, IRQ, console-input, or BIOS behavior.

## Verification

`tools/build/New-T287S5XmsUmbIvtNinja.ps1` generated formal MSVC `/MT` Ninja
graphs, with source SHA-256 manifests and explicit `src.old` exclusion.

- `build/M0-T287/S5/x86/bin/t287-s5-xms-umb-ivt-fixture.exe`: exit `0`.
- `build/M0-T287/S5/x64/bin/t287-s5-xms-umb-ivt-fixture.exe`: exit `0`.

Both graphs rebuilt the original UMB and misc source bodies. The retained
upstream `xmsumb.c` uninitialized-local warnings are source observations; no
original control flow was changed to suppress them.
