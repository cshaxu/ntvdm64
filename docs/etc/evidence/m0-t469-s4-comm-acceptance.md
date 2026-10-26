# M0 T469 S4 — COMM final acceptance

## Scope

T469 closes the bounded original-owner COMM capability without inventing a
serial provider, virtual pair, polling subsystem or replacement task cleanup.

## Evidence reconciliation

| Contract | Evidence | Result |
| --- | --- | --- |
| Original WOW32 ownership, resource lifetime and task-loss boundary | [S1 owner matrix](m0-t469-s1-comm-owner-matrix.md) | retained; multi-port task-loss remains an inherited original limit |
| Original correction availability | [S2 disposition](m0-t469-s2-comm-source-disposition.md) | no admissible correction found in inspected original sources |
| Provider success-side lifecycle | [S3 retained route](m0-t469-s3-comm-retained-route.md) | pass in a test-only controlled host contract; no product substitute |
| External serial peer | [T420 external boundary](m0-t420-s30-external-com-boundary.md) | non-pass: the available COM3 lacks CTS/DSR peer and receive data |
| Current guest controller loopback | [S3 retained route](m0-t469-s3-comm-retained-route.md) | non-pass: one-shot launch lacks a recorded COM3-enabling profile, so it cannot demonstrate controller activation; no synthetic PIF/config policy added |

The only source change is the test fixture's inert diagnostic callback.  It
keeps the direct compilation of the selected original `nt_com.c` body valid
after the imported setup-observation symbol was added.  It has no runtime
owner and does not affect product behavior.

## Closure result

The project-owned portion is complete: selected original COMM code remains
the sole owner, its controlled success lifecycle is executable, and all
unavailable external/configuration edges are explicit non-passes rather than
fabricated successes.  No image publication is required because no product
binary changed.  The next ordered WOW32 candidate is ToolHelp/debugger.
