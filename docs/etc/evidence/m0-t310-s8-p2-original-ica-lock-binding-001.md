# M0 T310 S8 P2 — original ICA-lock binding

## Question

Can the selected original SoftPC host ICA synchronization path execute on both
supported host architectures without MONITOR, Bochs, or a replacement lock
controller?

## Inputs

- Exact mirror: `mvdm-host/softpc.new/host/src/nt_eoi.c`.
- Original reachable functions: `InitializeIcaLock`, `host_ica_lock`, and
  `host_ica_unlock`.
- Original declaration carriers under `opennt-abi` and `opennt-host`.
- Focused fixture and graph:
  `tests/mvdm-host/softpc_ica_lock_fixture.c` and
  `tools/build/New-T310IcaNinja.ps1`.

## Procedure

Compile the original translation unit with the same product source selection
as the S8 candidate, retaining the original ABI include ordering. Link only a
focused fixture and public `kernel32`/`ntdll` imports. The fixture initializes
the original ICA critical section, holds it on one thread, proves a second
thread cannot enter, releases it, then proves the second thread enters and
exits normally.

`nt_eoi.c` contains delayed-IRQ and WOW functions outside that narrow lock
path. The fixture supplies guarded test-only definitions for their unresolved
outbound forms; any call sets `unexpected_call`, which must remain zero. They
are not production providers and do not select a PIC, MONITOR, Bochs, or
kernel-VDM route.

## Result

The following formal MSVC `/MT` runs passed outside the sandbox:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310IcaNinja.ps1 `
  -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p2-ica-lock/x86 verify

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310IcaNinja.ps1 `
  -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T310/S8/p2-ica-lock/x64 verify
```

Both builds retain only original-source warnings: historical product macro
redefinitions, an unused original local, and the existing callback pointer
diagnostic. No mirror source was changed.

## Interpretation and follow-up

The original host lock mechanics are directly composable and require no new
adapter. This closes only their focused initialization and mutual-exclusion
evidence. `ica.c` interrupt arbitration, `nt_eoi.c` delayed interrupts/IRET
hooks, timer delivery, and WOW wake semantics remain P2 work and must be
composed from their original source families rather than activated by the
fixture stubs.
