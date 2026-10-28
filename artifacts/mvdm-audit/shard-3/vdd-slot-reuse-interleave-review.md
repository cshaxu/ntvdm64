# VDD adapter slot reuse during port retirement

## Source contract

`nt_vdd.c:157–181` scans and writes global `VddAdapter[]`, and `:199–213` clears it before returning. Public `VDDInstallIOHook` registers handlers and ports after allocation, while `VDDDeInstallIOHook:415` calls `FreeVddAdapter` before disconnecting the requested ports. Neither API body takes a lock; the `host_ica_lock` occurrences in this file protect the separate IRQ-reservation functions, not adapter installation. Original `ios.c:902–911` disconnects a port only if its adapter number matches, then clears its tables. The public SDK exposes the APIs to VDD modules; sample VDDs call them. We have **not** established that a real workload actually calls the APIs concurrently, or ruled out every possible caller-level synchronization. This is a conditional correctness issue with a concrete reachable intermediate state, not a witnessed guest failure.

## Controlled two-thread actual-object witness

`vdd-slot-reuse-fixture.c` extends the existing VDD fixture with events and two controlled test threads. A installs original handler on port 2e0, then the test calls the actual `FreeVddAdapter` directly to model the point inside uninstall immediately after it clears slot zero. Its thread signals and waits. B installs a different handler at 2e1, which reuses the freed slot. After B signals, A reads its **still-connected original port**. Result: `VDD_SLOT_REUSE old_port_after_other_install=91 new_owner=1`; originally A's handler returned 56, B's returned 91. Eleven checks passed; exit 0, and no real port, guest or VDD DLL was used.

The test uses actual cached x86 `nt_vdd.obj`, `ios.obj`, `in.obj` and `out.obj`; it does not execute full public uninstall between the two event gates, nor does it test the proposed corrected return value. That distinction matters: the present raw-index bug already leaves old ports connected even without concurrency. This witness isolates the additional consequence of reusing a slot before its old ports are disconnected. Source ordering and lack of local locks make that interleaving plausible if concurrent callers are allowed; proving the real calling convention remains an explicit S13 task.

## Repair boundary

The four-line adapter-number/DMA correction in `vdd-uninstall-dma-repair-recheck.md` remains necessary but is not sufficient to establish safe concurrent retirement. If independent VDD threads may call these public services concurrently, serialize adapter slot ownership, registration, and retirement in the existing appropriate owner domain; prevent new slot reuse until its old port associations and any in-flight callbacks are safely retired. Do not hold a lock across arbitrary VDD callback execution or a blocking operation without first proving its reentrancy/teardown contract. Test overlapping same/different ports, callback-in-flight and module unload as well as repeat uninstall/install and failure rollback. If a real authoritative serialization guarantee makes concurrency impossible, record and test that guarantee instead of adding unnecessary synchronization.

## Replay

From the repository root with the existing project x86 MSVC driver and cached objects:

```powershell
New-Item -ItemType Directory -Force build/t473-s10/vdd-slot-reuse | Out-Null
cmd /c '"build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" cl.exe /nologo /W4 /MT /Od /D_CRT_SECURE_NO_WARNINGS /DNTVDM /DSPC386 /DANSI /DPROD /Isrc\mvdm\softpc.new\host\inc /Isrc\mvdm\softpc.new\base\inc /Isrc\opennt-abi\source\public\sdk\inc artifacts\mvdm-audit\shard-3\vdd-slot-reuse-fixture.c build\.ninja\msvc-x86-release\worker\obj\host\nt_vdd.obj build\.ninja\msvc-x86-release\worker\obj\support\ios.obj build\.ninja\msvc-x86-release\worker\obj\ccpu\in.obj build\.ninja\msvc-x86-release\worker\obj\ccpu\out.obj /Fo:build\t473-s10\vdd-slot-reuse\fixture.obj /Fe:build\t473-s10\vdd-slot-reuse\fixture.exe /link kernel32.lib'
& build/t473-s10/vdd-slot-reuse/fixture.exe --slot-reuse
```

Independently rebuilt fixture SHA-256 `BC2E4039D3EA227A76AA09603C9ED0BB92AD1EDADD2B7288C4A2563100BC3553`; `nt_vdd.obj` `75C48554F12D179F4DC3FD95051BB788E9D879C13F7792901F4EB4FDA2E08F5B`; `ios.obj` `00F21423007F39DDB26312B4EB41A41911456B062EA5B035D394577C498D4DF9`; replay executable `28D5B2869D3D1232D53C69D207C39E15404E6EED2D5C38E042DD5EA2852F8DA6`. Compilation emitted the existing NTVDM macro-redefinition warning. No product/guest/shared ledger was changed.
