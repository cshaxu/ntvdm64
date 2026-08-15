# M0 T196 S3 Retired Bochs Diagnostic Removal

## Objective

S2 classified the diagnostic/fixture removal surface as completed historical
provenance work. It did not implement machine mechanics, the adapter ABI, BOP
routing or a host capability. This record proves its removal and the
associated current tool/test cleanup.

## Removed Surface

The following S2 rows are removed from `refs/bochs`:

| S2 hunks | Former diagnostic | Register disposition |
| --- | --- | --- |
| H02 | `ntdos64-observation-fixture` duplicate Make target | No register row; obsolete fixture target removed. |
| H09--H10 | prefetch predecessor | BX-TRACE-056 removed. |
| H12--H13 | IRET TF | BX-TRACE-050 removed. |
| H14 | real-mode far jump | BX-TRACE-051 removed. |
| H15--H18 | MOV-SP/MOV-SS stack transfer | BX-TRACE-048 removed. |
| H19--H21 | IRQ13 transfer | BX-TRACE-037 removed. |
| H23--H24 | real-mode vector | BX-TRACE-049 removed. |
| H25 | unmatched `#UD` | BX-TRACE-039 removed. |
| H28--H29 | x87 compatibility snapshot | BX-TRACE-035 and BX-TRACE-038 removed. |

The removed diagnostics had no result, resume, RAM, port, device, adapter or
provider branch. The original CPU, FPU and interrupt statements around them
remain in place.

## Direct-Reference Cleanup

The removal also retires the diagnostic-only boundary tests and T136/T146/T148
derivative generators. Shared T95S6/T95S7/T98 and deferred adapter generators
remain, but their removed diagnostic switches, generated replacement objects,
manifest fields and mode assertions were deleted. In particular, no current
script recompiles `event.o` for an IRQ13 log or exposes unmatched-UD/vector
diagnostic switches.

## Verification

The following source/test/tool scan returned no match:

```text
rg -n "PREFETCH_PREDECESSOR|IRET_TF|REAL_MODE_FAR_JUMP|STACK_TRANSFER|IRQ13_TRANSFER|REAL_MODE_VECTOR|UNMATCHED_UD|X87_COMPAT" refs/bochs tests tools
```

The reproducible upstream comparison remains:

```text
git -c core.safecrlf=false diff --no-index --unified=0 -- refs/bochs O:\repos.external\bochs-2.6-compat\bochs-2.6
```

After the removal it contains 43 `@@` hunks, down from S1's corrected 60.
The change count is 17 rather than a row-for-row subtraction because the
removed unmatched-UD declaration shared `exception.cc`'s large generic
mechanics hunk with retained code. The remaining 43 hunks are not approved
merely by this reduction: S4 must minimize and verify their mechanical,
embedding and build-only groups.

The following current boundary checks passed without rebuilding or running a
guest: adapter-state trace, deferred startup plan, empty-port source-object,
execution plan, FMT lexical, machine-composition, ordinary-RAM observation,
generic `#UD` interception, minimal-machine lifecycle and minimal-SIM
lifecycle.

Three retained tests are deliberately recorded as **pre-existing stale test
contracts**, not passing evidence and not S3 changes:

| Test | Stale assertion | Current fact / S4 handoff |
| --- | --- | --- |
| `Test-BochsStartupTransactionBoundary.ps1` | Requires `prepare_from_environment` in Bochs startup code. | The current narrow transaction dispatch installs from `exception.cc`; `main.cc` owns the distinct execution-plan entry. S4 must restate this test against the typed opaque contract. |
| `Test-BochsUdDefaultOffBoundary.ps1` | Requires the pre-T195 `bx_ntvdm_try_intercept_ud` seam. | T195 replaced that identifier with the selector-blind copied-event/result mechanics. S4 must update the test without reviving the old seam. |
| `bx-ntvdm-minimal-mechanics-boundary.ps1` | Requires the BX-IO-025 block in `devices.cc`. | The accepted source-object separation places it in `iodev/minimal_port_space.cc`. S4 must test that current location. |

## Limits And Handoff

This is a source/tool cleanup, not a new Bochs build or guest-runtime claim.
It does not remove the retained generic `#UD`, checked ordinary-RAM, typed
result/stop, opaque startup, or adapter machine-composition mechanics. S4 owns
their individual minimization and boundary proof.
