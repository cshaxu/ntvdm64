# M0 T436 S4 — CCPU40 device declaration

## Question

Can the standalone CCPU40 guest machine avoid advertising serial and parallel
ports for which this product has no usable backend, so original Windows 3.1
Setup can perform its normal hardware scan without taking the historical NT4
kernel-VDM `$VDM*` path?

## Inputs

- Original SoftPC BIOS reset code: `src/mvdm/softpc.new/base/bios/reset.c`.
- Original static host declaration: `host_def.h` sets four serial and three
  parallel ports.
- The standalone product has no corresponding NT4 kernel-VDM COM/LPT carrier.
- Owner's normal Setup reproduction: original Setup reaches “Please wait while
  Setup collects system information” and does not progress; `/I` is rejected.

## Change

`MVDM-HOST-DIV-328` adds a self-owned host-device-profile adapter.  Its default
profile declares zero serial and zero parallel ports.  The minimal marked
reset hooks now:

1. publish zero COM/LPT counts in the BIOS equipment word;
2. clear all COM/LPT BDA base-address slots at every reset; and
3. skip COM/LPT initialization when the declared count is zero.

This does not emulate a port, query host hardware, patch Setup or guest media,
or affect PS/2/INT33 mouse paths.

## Verification

On 2026-10-07, selected x86 CCPU40 `ntvdm.exe` built successfully from
`build/M0-T436/S4/r001-device-profile` with the new adapter linked into the
selected BIOS path.  `git diff --check` reported no whitespace errors.

The source-level invariant is direct: both equipment fields read the adapter's
zero counts; BDA slots are cleared before the now-zero initialization loops;
therefore the old COM/LPT initialization and its `$VDM*` carrier path are not
reachable in the default profile.  The build's `ntvdm.exe` SHA-256 is:

```
C53CA859D0FC63A3793DFAE84FC692D58CE74A8F3BFBDAA72AAC031C946772A5
```

It was deployed to `O:\winnt\system32\ntvdm.exe`, and source/destination
hashes matched.  A noninteractive launcher smoke returned `87`; that shell has
no usable interactive Console handoff and is not accepted as a DOS functional
pass or a regression claim.

## Remaining integration gate

The required proof is still an owner-session run of the original, unmodified
Setup through the owner-selected PATCH entry point, without `/I`, selecting an empty
installation directory.  It must cross the hardware-detection page.  If it
does not, capture the existing default-off diagnostic trace and investigate
the actual Setup probe; do not broaden this profile into host-device
enumeration or synthetic serial/LPT emulation.

## First runtime result — hypothesis rejected

The owner reran normal Setup with the S4 candidate and it stopped at the same
page. A preserved run with `MVDM_WIN31_SETUP_HARDWARE_TRACE_PATH` established
that two INT 17h printer-status calls return immediately and no serial path is
entered. The last completed hardware calls are INT 15h configuration-table
queries (`AH=C0`), followed by the SoftPC keyboard path's `AX=9002`
device-busy notification and an INT 16h blocking read. The process then
becomes quiescent, rather than spinning or waiting in a COM/LPT carrier.

Thus zero COM/LPT declaration is not the repair for this stall. Its deployed
candidate must not be treated as a passing product change; the remaining
diagnosis must explain why Setup reaches an unexpected blocking keyboard read
after its configuration-table probe, and whether the failure is guest flow,
screen/input routing, or a machine-state fact not covered by COM/LPT.

## Candidate retirement

The device-profile adapter and its reset/build/ledger edits were removed after
the reproduced negative result.  The retained S4 code is diagnostic-only:
the evidence establishes that the serial/parallel hypothesis is false, not
that changing the BIOS equipment declaration is safe or useful.

## Second runtime result — direct hard-disk error, not a probe wait

The next bounded reproduction used the original `SETUP.EXE` without `/I` and
advanced each actual Setup page by a newly visible marker: the `SUBST.EXE`
warning, Welcome, Custom Setup, and the upgrade confirmation.  The observer
proved that each key was committed to the guest BIOS keyboard buffer before
the next page appeared.  After the confirmation it reproduced the same
unchanged page:

```
Please wait while Setup checks your system for configuration information.
```

The selected x86 diagnostic image records the original `MS_BOP_9` argument
immediately before this wait:

```
direct-access-error-bop value0=00000001
```

`1` is the source-defined `NOSUPPORT_HARDDISK` value.  The mapped execution
stack is `MS_bop_9 -> host_direct_access_error -> ErrorDialogBox`; the latter
waits for the original Abort/Ignore dialog response.  Therefore Setup is
attempting an unsupported direct hard-disk operation after its configuration
scan.  It is not blocked in COM/LPT discovery, BIOS keyboard delivery, or an
unbounded hardware-enumeration loop.

In the current NTCON session, that original host dialog is not surfaced to the
user, so the expected modal decision appears as a frozen DOS Setup page.  The
next repair decision must be narrowly about preserving and presenting this
original error interaction (and its real Abort/Ignore result), or separately
about a verified virtual-disk capability.  S4 must not silently ignore the
error, fake a hard-disk result, or reintroduce the rejected COM/LPT profile.

The diagnostic addition is default-off and observation-only.  The reproduced
trace is retained in the owner-session diagnostic log; its selected
`ntvdm.exe` SHA-256 is
`4BC8E13DB1269FDB1CC52DCBE48B0629C8508DEAA9EEAB9D4CE30C942CA2CCCA`.
