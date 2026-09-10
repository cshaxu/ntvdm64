# M0 T325 S2 — public workstation-query lease recovery

## Selected source recovery

This S retains the original bodies of `VrGetComputerName`,
`VrGetUserName(BX=1)`, and `VrRemoteApi`'s local-computer fallback.

- The two `ES:DI` result writes no longer form an unbounded `GetVDMAddr`
  alias. `mvdm_redirector_guest_copy` derives the original numeric real-mode
  location and performs exactly one synchronous writable lease transaction.
  No native pointer survives the call.
- `VrGetComputerName` retains its original `LM20_CNLEN` check and AX/CF
  success sequence. A missing or stale session lease returns
  `ERROR_INVALID_ADDRESS` without committing a guest write.
- `VrGetUserName(BX=1)` retains its original CX capacity gate and
  `NetApiBufferFree` pairing. The public modern NetAPI result is Unicode, so
  the adapter converts it to the original OEM target encoding before the
  single guest lease copy. `BX=0` remains deliberately unadmitted because the
  original source has no bounded destination span.
- `VrRemoteApi` retains its original local computer fallback and subsequent
  RAP branch. Its historical Unicode-as-ASCII `strcpy` is replaced only with
  a fixed-capacity `CP_OEMCP` conversion. The later RAP/SMB transaction stays
  outside this S and is not claimed to work.

`apiworke.h` was selected as a byte-identical declaration-only OpenNT carrier
from both source baselines (SHA-256
`805d632a97f750e4b15544d67fc0cc2e36070cbcbb44dd4600a031a528f85c75`).
It supplies constants for the original fallback and no RAP/Xs/Rx provider.

## Verification

The generated formal Ninja graph is
`tools/build/GenerateT325S2WorkstationQueryNinja.mjs`.

- x86: generated, compiled both original `vrnetapi.c` and `vrremote.c`, linked
  and executed `redirector-guest-copy-fixture.exe` successfully.
- x64: the same graph compiled, linked and executed successfully.
- The fixture proves: no active session rejects a copy; a bound session commits
  ANSI and OEM-converted Unicode result bytes; an out-of-range location is
  rejected without a write; and a terminated guest-memory lease rejects a
  subsequent copy.

The full original translation units continue to emit visible historical
diagnostics in unselected RAP/SMB and other Redirector branches. No warning is
suppressed here, and these diagnostics do not constitute a claim that the
unselected remote provider is x64-complete.

## Boundary retained

No RAP/SMB/Xactsrv/Rx/Xs/NetBIOS/DLC/VDD success path, remote request, raw host
identity, new mapper, or durable guest pointer is enabled by this S.
