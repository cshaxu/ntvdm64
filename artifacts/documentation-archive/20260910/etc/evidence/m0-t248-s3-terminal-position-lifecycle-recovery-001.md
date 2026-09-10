# M0 T248 S3 — terminal-position and finite-lifecycle recovery

## Minimal recovery

S2 showed that the enabled terminal-position observer did not retain a
position for a non-fault/non-stop/non-cancellation `cpu_loop()` return. The
mantle now copies the final `CS:EIP` whenever that observer is enabled and the
finite stage will classify either a watchdog budget or an unexpected return.
The copy remains excluded for first fault, typed stop, pending and
cancellation. It changes neither CPU state nor the finite result.

The diagnostic is named `--observe-terminal-position`, replacing the former
misleading budget-only spelling. The ABI record remains fixed-width and
selector-blind.

## Recovered lifecycle prerequisites

Focused testing found two independent teardown/negative-contract defects:

* Registered exception `BX-MANTLE-091` now makes the adopted 8042 controller
  unregister its four original `60h/64h` byte handlers and controller timer
  before destruction. This is lifecycle cleanup only; live controller
  semantics are unchanged.
* Registered `BX-IO-025`/`BX-MANTLE-083` now expose only a boolean
  non-default-port query to the typed mantle port-action ABI. A request to an
  empty/default slot fails, while direct Bochs `inp/outp` retains original
  all-ones/ignored unmapped behavior.

No selector, BOP, DOS, OpenNT, host or guest data term entered bx-core or
bx-mantle.

## Formal and native evidence

`build/M0-T248-S2/formal-r1` (MSVC x64 `/MT`, CPU5/P-MMX, both S2 CPU
diagnostics) rebuilt the changed module closure.

* `t228-s1-budget-terminal-position-fixture.exe` passes. It verifies disabled
  observation, normal watchdog capture, typed-stop exclusion, and a test-only
  untracked kill-latch return that remains
  `EXECUTION_UNEXPECTED_LOOP_RETURN` while yielding a copied final position.
* `t225-s23-opennt-int06-provider-fixture.exe` passes, retaining the existing
  source-derived `02/06` provider contract.
* The separate historical `t244-s2-headless-8042-lifecycle-fixture.exe` now
  reaches but fails its auxiliary-command negative assertion (`exit 11`). This
  is recorded as reopened `BOP-DEPENDENCY-106`; it is not hidden by this
  recovery and is not a T248 BOP/provider defect.

Two same-input source-built native runs prove the observer does not alter the
terminal result:

```text
without observer: terminal=4 detail=0 lifecycle=5 presentation=5
with observer:    terminal-position cs=5859 eip=000078b0
                  terminal=4 detail=0 lifecycle=5 presentation=5
```

Both retain the original T247 NTDOS entry PDB observation. In the current
reproducible composition this is a watchdog-budget terminal (`detail=0`), not
the older one-off unexpected-loop result. The precise next source attribution
for physical `0x5FE40` is S4 work; no service patch is selected.
