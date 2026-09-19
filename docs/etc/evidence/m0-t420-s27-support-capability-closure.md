# T420 S27 support capability closure

## Scope and source shape

S27 closes the complete selected six-body
`src/mvdm/softpc.new/base/support` capability unit:
`terminat.c`, `time_day.c`, `ios.c`, `main.c`, `get_env.c` and `xt.c`.

The pinned OpenNT union remains the source authority.  Normalized source
comparison confirms that `terminat.c`, `time_day.c`, `ios.c` and `get_env.c`
are exact.  `main.c` retains only the selected CCPU40/C-VID profile binding:
the C-VID vector binder is published before configuration and the CCPU40
access-vector binder is installed after `cpu_init` and before DOS
initialization.  `xt.c` excludes only the retired CPU30 monitor's duplicate
register, descriptor and parity-table state; CCPU40 owns that state.  Neither
delta replaces a support algorithm or adds adapter policy.

`get_env.c` has no current external environment consumer: its apparent
`CCPU_HISTORY_SIZE` query is inside `ccpupig.c`'s excluded `PIG` profile.
The formal product defines `PROD`, not `PIG`.  This is a source-proven selected
profile exclusion, rather than a fabricated environment workload.

## Capability matrix

| Family | Disposition | Evidence |
| --- | --- | --- |
| Worker startup and initialization order | Real formal worker startup | The original `ntvdm.c` entry reaches `host_main`; the formal guest fixture executes only after original support initialization has completed. |
| Time of day | Real DOS guest pass | INT 1Ah AH=00 reads ticks, AH=01 writes and restores them, and AH=00 observes the controlled value. |
| RTC | Real DOS guest pass | INT 1Ah AH=02 and AH=04 both return with carry clear. |
| IOS/device port registration | Real DOS guest pass | The fixture safely reads original PPI `61h`, CMOS `70h/71h` and video-status `3DAh` providers. |
| Environment | Source-proven selected profile exclusion | The only support environment consumer is PIG-only and is not in the formal graph. |
| Normal termination/resource release | Real package regression | Direct guest, native child, nested COMMAND, repeated MEM and EDIT-return sessions all exit through the original worker termination route without an owned process remaining. |
| Failure disposition | Source-proven boundary plus regression | The standalone version-mismatch negative requires an intentionally substituted old worker; it is exercised by the dedicated version-peer fixture, not falsely inferred from a same-version package run.  Ordinary command-resolution failure is observed and recovered in the normal matrix. |

## Real guest witness

`tests/observation/support_capability.asm`, built by
`tools/build/Build-T420S27SupportGuestTest.ps1`, is a test-only COM image.  It
does not install handlers, mutate media, or use a CPU30 path.  The formal
deployment produced:

```text
S27_TIME_TICKS_OK S27_RTC_TIME_OK S27_RTC_DATE_OK S27_IOS_PORTS_OK S27_SUPPORT_OK
```

The exact ConPTY transcript is
`O:\winnt\logs\m0-t420-s27-support-r4.raw`.  The observer's
`--support-capability` assertion requires the final marker and rejects guest
command-resolution failure.

## Formal build and integrated regression

`build/M0-T420/S27/formal-x86-r1` was generated from the current source and
linked all five formal package artifacts: `run16.exe`, `basesrv.exe`,
`ntvdm.exe`, `dtmgr.exe` and `VDMREDIR.dll`.  The VdmTib ownership check passed.
The deployed artifact hashes are in
`O:\winnt\logs\m0-t420-s27-formal-artifacts.json`.

The established transcript-gated routes passed in split bounded runs:

- `m0-t420-s27-fixture-r1`: native seven, stdout/stderr, EOF, COMMAND `/c`,
  direct DOS COM seven;
- `m0-t420-s27-interactive-r1`: nested COMMAND, nested MEM and repeated MEM;
- `m0-t420-s27-command-tail-r1`: direct MEM and COMMAND `/c` MEM;
- `m0-t420-s27-edit-r1`: EDIT return followed by MEM;
- `m0-t420-s27-formal-command-r4`: the same current formal package's first
  uninterrupted matrix segment through MEM, with all captured transcripts.

The matrix helper no longer uses `SUBST` for a spawned worker's DOS fixture
path.  It verifies the admitted build fixture hash, copies that test-only COM
file to `O:\winnt\tests`, verifies the copied hash, and uses that declared
runtime test root.  This corrects test-process path visibility only; no product
media, product source or runtime behavior changed.

S27 therefore closes every selected support body as a real capability pass or
a source-proven current-profile exclusion, with original source ownership and
no new product implementation.
