# M0 T420 S39 guest DPMI/DOSX capability review

## Admission

S39 owns the selected immutable guest DPMI/DOSX package, not the DPMI32 host
providers already delivered by S38. The owner authorizes automatic sequential
S admission through T420, but requires an owner acceptance audit before T420
itself may close.

## Initial acceptance matrix

1. Freeze the selected DOSX guest-media identity and complete source manifest.
2. Attribute every selected `BOP 53` call site and its caller-visible inputs,
   returns, selectors and cleanup owner.
3. Prove normal first entry, protected-to-real return, direct and nested
   COMMAND re-entry, and ordinary client resource release from real guest
   workloads.
4. Exercise representative failure without altering guest media; classify an
   original defect as a TODO negative, never a host repair or passing result.
5. Run matching formal x86 and the established direct/interactive product
   matrix before S delivery.

No host-only descriptor fixture, DOSX file hash, or S38 host result is counted
as S39 guest acceptance.

## Frozen guest identity and source call graph

`src/mvdm/dpmi/486/dosx.exe` and the deployed
`O:/winnt/system32/DOSX.EXE` are both the immutable original file with SHA-256
`C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF`.
The selected `src/mvdm/dpmi` tree has no semantic or formatting diff against
its mirrored source baseline.

The selected 486 assembler sources contain 52 `DPMIBOP`/`FBOP BOP_DPMI`
sites.  The complete mechanical caller listing is retained under
`build/M0-T420/S39/source-audit-r1/bop53-callers.txt`.  The entry/return
owners needed for this S are:

| Original guest owner | BOP contract | S39 runtime witness |
| --- | --- | --- |
| `486/dxboot.asm` | table address publication (15) | D36 normal, stress and lifecycle clients enter DOSX and allocate/free through its tables. |
| `486/dxstrt.asm` | `InitDosx` (4), PM-stack/fault setup and return | D38 16/32 interrupt, fault and hardware-IRQ clients return through the original hooks. |
| `486/dxutil.asm` / `dxfunc.asm` | protected-mode entry, app init/in-use | direct and twice-nested D36 clients produce their guest entry and allocation markers. |
| `486/dxstrt.asm` final path | terminate (16), free all XMS (19), no-longer-in-use (13) | D36T exits four clients with live allocations and proves capacity restoration. |

## Current-main formal build and guest execution

`New-T310OriginalSoftpcNinja.ps1` generated
`build/M0-T420/S39/formal-x86-r2` with the already installed Node 22.22.1 and
the x86 MSVC environment.  `ninja product-programs dpmi-debug-tests` succeeds.
The current-main worker SHA-256 is
`5720B61CBCD2C27FCAFD36EBAED5A482A46EED1CBB25C2925E9B1C8914FF6A70`.

Native CCPU/DPMI fixtures pass: `ccpu-debug-match-test`,
`debug-register-binding-test`, and `ccpu-halt-reset-test`.

For real guest acceptance, the formal three-program candidate was temporarily
placed at `O:/winnt`, never changing guest media, and every replacement was
restored by SHA-256 in `finally` cleanup.  The following direct and twice-
nested COMMAND matrices passed:

| Matrix | Logs | Result |
| --- | --- | --- |
| normal allocation/free and ES boundary | `s39-formal-dpmi-normal-r1` | D36N and D36E, 4/4 pass |
| forced relocation/failure preservation | `s39-formal-dpmi-stress-r1` | D36R, 2/2 pass |
| final DOSX task cleanup | `s39-formal-dpmi-lifecycle-r1` | D36T, 2/2 pass |
| 16-bit and 32-bit IRQ/interrupt/fault return | `s39-formal-hwirq16-r1`, `s39-formal-hwirq32-r1` | 4/4 pass |

The smaller r2 interrupt probes also exit zero and emit the expected
`S38_INT16_RETURN_OK`/`S38_FAULT16_RETURN_NEGATIVE_OK` and 32-bit equivalents
in `s39-formal-int16-r1` and `s39-formal-int32-r1`.  Their verifier invocation
was deliberately not counted as a pass because it incorrectly required a
hardware-IRQ marker that those two probes do not contain; the dedicated
hardware-IRQ probes above are the accepted checks.

## Diagnostic boundary

An isolated package rooted at `O:/winnt/tests/D39TRACE` exits before worker
completion even when its three root binaries are byte-identical to the
published S36 package.  The same guest probes pass at the supported product
root `O:/winnt`.  This is a package-root startup-layout restriction, not a
DOSX, BOP, CCPU or guest-media failure; isolated-root output is excluded from
acceptance.  The temporary full-BOP tracing wrapper is also excluded: it
wrapped calls that may make non-local DOSX mode-switch returns.  Source caller
attribution plus non-interfering real guest witnesses remain the valid S39
evidence.

## Established product regression

The same current-main candidate passed all 17 established transcript-gated
product routes under `s39-formal-product-r2`: interactive and direct COMMAND,
native streams/EOF, normal and repeated MEM, direct and nested COMMAND `/c`,
guest and native exit-status propagation, and EDIT return.  The summary records
all expected return codes and actual codes as equal.  The matrix cleanup was
made reliable by changing only its test helper: it now terminates and waits for
the observer-recorded, exact-path test process tree.  No product policy or
guest behavior was changed.

## Immutable original negative disposition

S39 inherits the standing immutable-guest disposition for the original DOSX
allocation/reallocation failure contracts, INT31/0500 invalid output span, and
original debug-service limitations already registered in `docs/states/TODO.md`
rows 5, 6, 9 and 10.  They are retained as negative reproducers and are not
counted as S39 capabilities.  No additional guest defect was discovered by the
normal, forced-failure or final-cleanup matrices above.
