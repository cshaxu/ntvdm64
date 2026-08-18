# T228 S1 P12 - large reverse-copy and far-pointer mechanics result

## Question

Can the current minimal CPU5 machine execute the complete generic mechanical
shape required by the original COMMAND transient handoff: a large reverse
real-mode byte move, copied far-pointer preservation, indirect far call, and
far return?

## Fixture

The mantle-only fixture owns 50,640 opaque bytes at `0E00:0000`, copies them
in reverse with `STD; REP MOVSB; CLD` to `3000:0000`, and checks twenty bytes
surrounding a copied four-byte opaque pointer at destination offset `95F0h`.
The pointer addresses a test-owned one-byte `RETF` target at `0041:0193`; the
caller continues to fixture-only `UD2`.

The test uses no COMMAND image, historical service, BOP, adapter, host
capability, or production ABI. Its ranges are fixture-local and do not assert
any live guest address.

## Result

The fresh MSVC x64 `/MT` CPU5 graph at
`build/M0-T228-S1/large-reverse-copy-20260818a` built the focused target.
`t228-s1-large-reverse-copy-fixture.exe` exited zero. The terminal snapshot
matched all eight leading sentinels, the copied four-byte pointer
`93 01 41 00`, and all eight trailing sentinels after the `RETF` returned to
the controlled stop.

## Interpretation

This eliminates the complete generic bx-core/bx-mantle mechanical shape of
large reverse string transfer, opaque far-pointer data preservation, and
ordinary real-mode indirect far call/return as the direct explanation for the
invalid live HeadCall target. It does not prove the original COMMAND/NTDOS
lifecycle copied the right source data at the right phase.

The remaining live owner domain is the original lifecycle that selects
`TrnSeg`, moves/reallocates the transient, and transfers `TranVars` to
`HeadCall`. Any repair must address that entire COMMAND/NTDOS package and not
introduce an address-specific Bochs, adapter, or BOP workaround.

## Remaining verification

The complete default formal Ninja graph in the same fresh root then completed
all declared targets successfully. A subsequent Ninja invocation reported no
remaining work.