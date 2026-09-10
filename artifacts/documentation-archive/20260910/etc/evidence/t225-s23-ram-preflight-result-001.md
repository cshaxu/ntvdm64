# T225 S23 P7 — No-Copy Ordinary-RAM Write Preflight

`BX-MANTLE-068` now admits a selector-blind `PREFLIGHT_WRITE` action kind. It validates every ordinary-RAM write range using the existing native writable predicate, then returns success without reading or changing any guest byte. The record layout and all copied fields remain unchanged.

Fresh formal MSVC x64 `/MT` Ninja root: `build/M0-T225-S23/ram-preflight-r2`. `t225-s23-ram-preflight-fixture.exe` exited zero. It proves inactive rejection, successful preflight after initialization preserving an existing `5Ah` byte, and out-of-range preflight rejection while preserving that same byte. This is a mechanical prerequisite only: it adds no selector route, OpenNT behavior, port action, profile rule, or native trace.
