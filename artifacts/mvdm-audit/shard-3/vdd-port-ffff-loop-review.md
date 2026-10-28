# VDD inclusive port 0xffff: actual-object bounded witness

`nt_vdd.c` declares port iterator `w` as `WORD`. Both `VDDInstallIOHook` and `VDDDeInstallIOHook` iterate `for (w = First; w <= Last; w++)`. If `Last == 0xffff`, increment wraps the 16-bit iterator to zero and `w <= Last` remains true for every possible value. The install rollback contains a related inclusive range loop that must be reviewed together. This arithmetic is a source-level proof of the unbounded loop provided control reaches the loop; the result is independent of a particular timeout.

`vdd-port-ffff-fixture.c` extends the existing actual-object VDD/IOS/CCPU fixture with test-only `--ffff-install` and `--ffff-uninstall` cases. The latter reserves slot zero and installs its owner in slot one, ensuring the existing raw-index-zero uninstall bug does not return before this loop. Each case uses exact range `[0xffff,0xffff]` and flushes a marker before invoking the public API. `run-vdd-port-ffff-probe.ps1` runs each **owned hidden child** independently, waits 2 seconds for diagnosis, and kills and waits for that child only if still running. Result:

```text
install entered=True returned=False completed=False
uninstall entered=True returned=False completed=False
```

The original fixture controls also pass: `checks=305 failures=0 callbacks=82 locks=27 dma_accesses=16`. Test process time budgets are not a proposed product timeout. The observed stalls plus integer proof establish the controlled actual-object defect; they do not prove a real VDD requests port `0xffff`, test a physical port or guest, or prove a fix.

S13 should use a wider bounded loop iterator or an equivalent inclusive loop that terminates after processing `Last`, covering install, rollback and uninstall without changing valid port ownership. Separately fix adapter-number teardown and its slot concurrency; the present uninstall witness does not prove old ports are properly disconnected. Add actual-object cases for single `0xffff`, ranges ending at `0xffff`, adjacent ordinary ports, rollback and unrelated owner; confirm no out-of-range access and correct return/teardown, then run applicable product regression and register mirror exception. No arbitrary timeout in runtime.

Replay: build the test-owned fixture with the same MSVC x86 driver, header flags and original objects as `vdd-slot-reuse-interleave-review.md`, replacing the input with `vdd-port-ffff-fixture.c` and output with `build/t473-s10/vdd-ffff/fixture.exe`; run `powershell -NoProfile -ExecutionPolicy Bypass -File artifacts/mvdm-audit/shard-3/run-vdd-port-ffff-probe.ps1`. Fixture SHA-256 `542441158B115D91AB97D24313CB506996400DE578B67191C93B4522484BD6A3`; runner `15D18974C77744704A6376989FEB3DA066798966E9E844C49E71740F54EEE27D`; independently rebuilt replay EXE `451821FE3B2C6469756968CCC1558A4D081F2B4B011D634FEDEB48A1D2932FA2`. The independent replay reproduced both hangs and all 305 controls; compilation printed the existing NTVDM macro redefinition warning. No production, guest or shared ledger changed.
