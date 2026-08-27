# M0 T290 S5 P4 — original NetAPI user-name source closure

## Question

Can the exact original `VrGetUserName` body compose its bounded
`BX=1` form through public `NetWkstaUserGetInfo` without importing a second
username provider or allowing an unbounded guest pointer?

## Source contract

`vrnetapi.c` defines `NET_UNICODE`, but that historical marker is not the
modern SDK's `UNICODE` preprocessor selector. The selected original source
therefore retains its ANSI `LPTSTR`/`NetpCopyTStrToStr` (`strcpy`) form for
this body. Its contract is:

- `BX=1`, `CX` is the supplied byte capacity; a name with `strlen(name) <=
  CX - 1` clears carry and copies to `ES:DI`.
- `BX=1` with a smaller positive capacity returns `NERR_BufTooSmall` with
  carry set before it dereferences `ES:DI`.
- `BX=0` gives no capacity. It cannot receive a checked guest lease and stays
  selector-disabled.

## Inputs and procedure

- Exact original `src/mvdm-host/vdmredir/vrnetapi.c`.
- Public `NetWkstaUserGetInfo` and `NetApiBufferFree` from `netapi32.lib`.
- The existing session-owned Redirector `ES:DI` pointer scope.
- `tests/mvdm-host/vdmredir/t290_s5_vrnetapi_user_name_fixture.c` and
  `tools/build/New-T290S5VrNetApiUserNameNinja.ps1`.

The fixture first queries the host's current ANSI workstation username to
derive a nonzero exact `CX` capacity. It declares that single writable guest
span, calls the unchanged body, and verifies copied bytes plus `AX=0` and
carry clear. For a nonempty name it then calls the same original body with
`CX=strlen(name)` and no pointer scope, verifying source
`NERR_BufTooSmall`/carry-set behavior.

## Observations

- MSVC x86 `/MT` and x64 `/MT` graphs both compile the full untouched
  `vrnetapi.c` object and execute the fixture with exit code zero.
- The test link retains `/FORCE:UNRESOLVED` solely for uncalled sibling
  Lanman/SMB/Rx/Xs/SoftPC bodies in that same object. Its `netapi32.lib`
  binding resolves the two public calls used by this body.
- No conversion facade, native handle, host pointer, selector ingress or
  private service was added.

## Interpretation

The original bounded `BX=1` `VrGetUserName` contract is locally composed on
both supported host widths. `BX=0` remains an explicit safe non-admission:
the source gives no byte span from which a session lease can be created. This
is a boundary disposition, not a replacement username implementation.
