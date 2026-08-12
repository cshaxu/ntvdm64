# M64 DEM Error, Hard-Error, and Retry ABI Ledger

Status: source-derived shared error-path record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/dem/demerror.c`
- `src/opennt/base/mvdm/dos/dem/dem.h`
- `src/opennt/base/mvdm/inc/dossvc.h`
- `src/opennt/base/mvdm/dos/dem/demdisp.c`

## Guest Structures and Setup

`SVC_DEMSETHARDERRORINFO` (`32h`) registers two raw guest pointers:

| Register input | Stored DEM global | Structure / purpose |
| --- | --- | --- |
| `DS:DX` | `pHardErrPacket` | `VHE`: `fbInt24`, hard-error code, drive number. |
| `DS:BX` | `pDeviceChain` | `SYSDEV` device-chain head. |

The `VHE` payload has three one-byte fields. `DemDispatch` clears
`pHardErrPacket->vhe_fbInt24` at the beginning of every in-range service call
when this registration exists. Therefore a future backend must treat VHE as
validated guest-owned state with an explicit lifetime, not as an unbounded
host pointer cached across arbitrary memory/topology changes.

## Normal Versus Hard Failure

`demClientErrorEx(handle, drive, setRegisters)` reads the current Win32 last
error; zero is normalized to `ERROR_ACCESS_DENIED`.

For errors outside the range `ERROR_WRITE_PROTECT` through
`ERROR_GEN_FAILURE`, except `ERROR_WRONG_DISK`, normal failure applies:

- when `setRegisters` is true, AX receives the error value;
- CF is set when `setRegisters` is true;
- no VHE hard-error payload is written.

For write-protect through general-failure errors, plus wrong-disk:

1. values above general-failure normalize to general-failure;
2. VHE `fbInt24` becomes one;
3. drive comes from the caller or, for a valid handle, an NT object/volume
   query that attempts to recover the drive letter;
4. VHE hard-error code becomes `error - ERROR_WRITE_PROTECT`; and
5. the retry snapshot is saved.

If `setRegisters` is true, CF still sets. The hard-error branch does not set
AX to the original error code. This distinction is essential: a future
contained runner must not mistake an error return for an ordinary opaque file
capability failure, nor reach into the host object namespace to derive a drive
letter. It needs its own per-session token-to-drive association.

## Retry Contract

`SVC_DEMRETRY` (`33h`) has no declared register input. It restores this
`SAVEDEMWORLD` snapshot:

```text
AX BX CX DX DS ES SI DI BP CurrentISVC
```

It intentionally does **not** restore CS, IP, SS, or SP. The original comment
states that those registers are not SVC parameters and that an INT 24 hook
cannot make a DEM call, making `CurrentISVC` safe. The retry service then calls
the original table entry at `CurrentISVC` again. It is not a generic host I/O
retry loop and it has no independent policy for idempotence, cancellation, or
asynchronous completion.

The historic service handler must avoid changing client registers before an
operation that can produce a hard error, otherwise the saved retry state is
wrong. This is a cross-handler invariant, not an implementation detail of
`demRetry` alone.

## Boundary Consequences

The contained future design needs a single session-scoped error coordinator
with these responsibilities:

1. validate and retain the guest VHE/device-chain registration only while its
   mapping remains valid;
2. map capability failures to the locked-profile normal-versus-hard outcome;
3. associate opaque file/media tokens with a configured DOS drive, rather than
   querying ambient host object names; and
4. retain a validated copied register/service snapshot for one explicit retry
   operation, with a defined invalidation point on reset/stop.

This does not justify an arbitrary handler to restore CPU state or switch mode.
The governed backend remains responsible for applying a finite, validated
register patch and for rejecting stale guest-memory mappings. No error or retry
implementation was added in this project.
