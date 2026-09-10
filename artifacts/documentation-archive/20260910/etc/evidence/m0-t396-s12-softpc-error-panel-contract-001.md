# M0 T396 S12 — SoftPC error-panel contract 001

## Admission

The owner directed that error interactions use the same visible Win32 model:
the original caller owns the offered choices, default and result semantics;
public Win32 presents them. S11's fixed-container observation maps the DOSX
real-mode `ARPL` probe to the original `Int6`, BIOS vector `F000:FF30`, BOP
06, `illegal_op_int`, then `host_error(EG_BAD_OP, ERR_QU_CO_RE, ...)`.

## Original contract

`softpc.new/host/inc/error.h` documents a general `ERR_*` family:

| Source option | Required selected meaning |
| --- | --- |
| `ERR_QUIT` | generic-contract terminate description |
| `ERR_CONT` | generic-contract continuation description |
| `ERR_RESET` | generic-contract reset description |
| `ERR_DEF` | generic-contract no-action description |

The actual selected OpenNT NTVDM body is authoritative over that generic
comment: both the external baseline and current mirror call
`ErrorDialogBox(message, NULL, RMB_ICON_STOP | RMB_ABORT | RMB_IGNORE)` and
return `ERR_CONT` unconditionally. Its visible buttons are Terminate and
Ignore; it does not implement `ERR_RESET`/`ERR_DEF` dispatch. The existing
original `ERRORPANEL` is therefore the first source-reuse route, not a
template authorizing new button semantics. S10's `SEB_*` facade remains
separate because its required result is a parameter position.

## Current recovery

No new `ERR_*` mapping is admitted. The original `host_error`,
`RcErrorDialogBox`, and `RcMessageBox` already reach `ERRORPANEL`; the S10
change makes the same original panel visible for WOW's `RMB_*` branch. The
only reached private path is `WOWSysErrorBox`/`NtRaiseHardError`, for which
S10 retains an exact `SEB_*` facade with its own fixture.

The current formal x86 and x64 graphs compile the affected original
`obj/host/nt_error.obj`. S12 requires final cross-family source/diff review
and the governance gate before closure; it does not claim that a user has
selected Continue in the S11 product.

## Baseline comparison

The reviewed external source is
`O:\repos.external\opennt-src-2\nt\private\mvdm\softpc.new\host\src\nt_error.c`.
Its `host_error` body and the current mirror both call the same
`ErrorDialogBox(message, NULL, RMB_ICON_STOP | RMB_ABORT | RMB_IGNORE)` and
return `ERR_CONT`. A focused no-index source diff finds no `host_error`
semantic delta and no remaining `RMB_HOST_*` spelling. The only related
current delta is registered `MVDM-HOST-DIV-232`: the WOW-only invocation of
the same `RMB_*` panel no longer crosses private CSRSS and instead uses the
original public-Win32 `ERRORPANEL` callback. `WOWSysErrorBox` remains the
separate S10 exact-shape facade.
