# M0 T396 S32 — optional WOW registry recovery

## Scope

The standalone CLI has no NT4 product-global
`HKLM\SYSTEM\CurrentControlSet\Control\WOW` registry service.  This packet
recovers only the two free-build optional consumers in original `W32Init`:
`ThunkNLS` retains its documented default and `KnownDLLs` retains the existing
empty-list behavior.  A present key and every present value still take the
original query path.  The packet does not write the registry or emulate a
global WOW service.

## Change

`src/mvdm-host/wow32/wow32.c` no longer treats an absent whole key as an
immediate `W32Init` failure, and avoids querying `ThunkNLS` through a null
handle. `src/mvdm-host/wow32/wkman.c` maps that same absent-key case to the
already-existing missing-`KnownDLLs` result.  All other key/value behavior is
unchanged.

## Build and fixed-stage observation

The complete selected 77-body provider rebuilt normally, without `/FORCE`.
Its SHA-256 is:

```text
2AFC6938C47CE95C03451493F9F3C2FC141DDB329CB59D86EE6CB95406E91D99
```

It was staged by the existing stage tool with the S20 parent into disposable
`O:\t396`; the resulting manifest SHA-256 is:

```text
C6498C73ED22102833D7D403B8527C7A43F4088003E3FD60AF2874D283B383DC
```

The existing console-owning non-debug observer ran
`ntvdm32.exe -f -o --command system32\WOWEXEC.EXE`. At 5.8 seconds the
product was still live with 23 modules and `WOW32.DLL` loaded. The 15-second
repeat exited after 7281 ms with `0xc0000005`, rather than the preceding
`W32Init == FALSE -> TerminateVDM -> _abort` fast-fail.

The product's default-off exception report identifies an execute-through-null
fault (`address=0`, `ip=0`) and return `0x6a320c34`. A build-only observer
records `WOW32.DLL` at `0x6a300000`, so that return maps to provider RVA
`0x20c34`. The provider map places it in original
`WK32InitializeHungAppSupport`, at the indirect call through
`pfnOut.pfnRegisterUserHungAppHandlers`.

## Disposition

S32 is complete: the absent registry key is no longer the terminal
`W32Init` branch. The new boundary is not a registry problem. It proves the
current exported `UserRegisterWowHandlers` did not populate the NT4
`PFNWOWHANDLERSOUT` contract required by the original provider. S33 owns a
source/ABI audit of that private USER registration contract before any
facade is admitted.
