# M0 T392 S12 — Console-owning acceptance repair

## Scope and result

S12 repairs the existing clean-Console fixture's child-lifetime defect.  The
fixture now terminates and waits for only the `CreateProcess` child belonging
to a timed-out row; the same cleanup happens when its Console input write
fails.  It does not change the product, guest media, BOPs, parsers, or guest
input route.

The current Codex execution session is a detached pipe-backed launcher.  The
fixture therefore allocates its one Console when `CONIN$` is absent, exactly
as its existing contract specifies.  This is the first remaining harness
boundary: the historical ordinary/RDP Console results are not reproducible in
this detached execution surface.  The boundary is reported below and is not
attributed to either product.

## Reproduction

The fixture was built from the project generator/source with MSVC x86 host
tools into `build/M0-T392/S12-clean-baseline/`.  Each product was launched
from its published package directory, `O:\ntvdm64`, with its report path in
that build directory.  The fixture owns the Console, uses inherited
`CONIN$`/`CONOUT$`, clears only observer diagnostics around `CreateProcess`,
waits at most 30 seconds per row, and writes ordinary Console key events for
the interactive row.  No user input, UI automation, guest-memory injection,
or additional Console was used.

## Results

| Product | `command/c ver` | `command.com /c ver` | nested `command.com /c ver` | interactive `command.com` + `exit` | Product PID after runner exit |
| --- | ---: | ---: | ---: | ---: | --- |
| `O:\ntvdm64\ntvdm32.exe` | timeout (12) | timeout (12) | timeout (12) | timeout (12) | none |
| `O:\ntvdm64\ntvdm64.exe` | nonzero child exit (13) | nonzero child exit (13) | nonzero child exit (13) | timeout (12) | none |

During the x86 run, each 30-second transition replaced the sole launched
`ntvdm32.exe` PID; no prior PID survived into the next row.  The final report
and process query showed no `ntvdm32.exe`.  The x64 runner likewise exited
with no `ntvdm64.exe` remaining.  This proves the S12 cleanup repair on both
published products.

## Disposition

The fixture's original four legacy rows reach the detached-Console boundary
before the requested second-`COMMAND.COM`/`MEM.EXE` rows can be made a valid
product acceptance claim.  S12 is closed under its stated alternative exit
criterion: it records the first exact harness boundary and preserves product
semantics.  T392 closure requires a Console surface equivalent to the
historical ordinary/RDP run before the five owner-requested output assertions
can be promoted to final product acceptance.
