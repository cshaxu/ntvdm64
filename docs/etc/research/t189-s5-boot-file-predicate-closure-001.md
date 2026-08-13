# T189 S5 boot-file predicate closure 001

Date: 2026-08-12  
Packet: M0 T189 S5  
Disposition: source, focused-test, four-object derivative, and one valid frozen observation closed.

## Question

Does the real `54:0C` decline recorded by T189 S2--S4 identify a missing
COMMAND/Bochs function, or does the derivative retain a stale adapter object?

## Inputs and procedure

The source condition in
`src/bx-ntvdm-adapter/bx_ntvdm_cmd_boot_file_service.c` was factored without
changing its boolean outcome.  Its same-island diagnostic has only these
condition groups: argument, event ABI, CPU ABI, instruction-window ABI,
fixed BOP contract, namespace/address, path and transaction.  The existing
boolean API delegates to it.  The provider gains a matching wrapper so the
runtime does not bypass its provider boundary.

`tools/New-T189S5BootFilePredicateDerivative.ps1` begins with the pinned T187
base and permits exactly four compilations: `cpu/exception.o`, adapter runtime,
COMMAND boot-file service, and boot-namespace provider.  It does not compile
an archive, a device, the CLI, or any other Bochs object.  The final bundle is
made by `tools/New-T189S5BootFilePredicateBundle.ps1`; its manifest binds the
engine and the retained ROM hashes.

Focused checks passed:

- `Test-AdapterRuntimeDiagnosticStateBoundary.ps1`;
- `Test-BochsAdapterStateTraceBoundary.ps1`;
- MinGW `bx-ntvdm-boot-namespace-provider-test`, including accepted and
  fixed-contract-decline cases; and
- MinGW `bx-ntvdm-adapter-runtime-test --t181-v5-install`.

The exact MSVC/x86 `nmake` command built the four declared objects and linked
the derivative.  It retained the prior C4244, LNK4042, LNK4098 and LNK4217
warnings; none was repaired or reclassified.

The first bundle attempt (`...predicate-bundle-r1`) had no generated bundle
manifest.  It terminated in 343 ms with engine-bundle validation failure and
is retained as failed composition evidence; it was not retried.  A new,
manifest-bound `...predicate-bundle-r2` was then observed exactly once with
the existing 30-second one-shot controller.

## Observations

| Item | Value |
| --- | --- |
| Four-object derivative manifest | `artifacts/build/current/t189-boot-file-predicate-derivative-r2/t189-s5-boot-file-predicate-derivative.json` |
| Final engine SHA-256 | `A008832FBE0E6094A1F91D196E14ED9527BDAAE99FDE1042EC507BFDBE0C8608` |
| Bundle manifest | `artifacts/build/current/t189-boot-file-predicate-bundle-r2/ntdos64-bochs-engine-bundle-v1.txt` |
| Valid observation | `artifacts/analysis/t189-s5-boot-file-predicate-observation-r2` |
| Observation record SHA-256 | `DAFDC6F1574F4539917B94E1BAC1CDE938B2C364CFA80616D89A51A58FEDA1B9` |
| Trace SHA-256 | `7D82E88A1F571AC775474FA43F1069B1B603F86126B77DDEE79702DDCC463ECD` |
| Valid observation exit | `0` after 3396 ms; no watchdog termination |

The valid trace reaches `54:0C`, reports `multi-write transaction committed
writes=1 bytes=e`, then reaches the existing controlled stop and exits zero.
No failure-category trace follows this BOP because an accepted transaction
returns before the unchanged pass-through trace.  The observation does not
claim a complete DOS session, terminal-result transport, or continuous guest
execution.

## Interpretation and follow-up

The former `boot-file-stage=3` result was not evidence of a missing
OpenNT/Bochs feature.  The T189 S3/S4 derivative recompiled only runtime and
exception objects while retaining stale command-service/provider objects;
the real call therefore used the old boolean implementation.  Recompiling
the complete local ownership closure produces the existing accepted
transaction without a Bochs semantic change, new BOP handler, guest-memory
inspection, or host capability.

T189 S6 must now audit the reached post-commit BOP sequence from this valid
trace and map only the next unresolved owner/boundary.  It must not infer a
repair from this success or start another runtime observation.
