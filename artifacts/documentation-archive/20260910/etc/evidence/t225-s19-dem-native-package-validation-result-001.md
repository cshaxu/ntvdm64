# T225 S19 DEM native package-validation result 001

## Question

After the 73-slot DEM package route reconciliation and Direct/Readonly family
regression, does the current source-built native CLI preserve the bounded,
selector-observation behavior of the immutable primary guest input?

This is package validation only.  It is not an admission to implement any BOP
identity newly seen in the trace.

## Inputs

- Current formal native closure: `build/M0-T225-S19/native-package-validation-r1/`.
- Formal graph: `tools/build/t225-s7-full-module-manifest.json`, MSVC x64
  `/MT`, CPU5/P-MMX configuration projection SHA-256
  `DDB0D13E35C2FD9E7EC29896E6296F83ECE7F6BF5D2AD474E8937D389F97872F`.
- Immutable S10 primary input:
  `build/M0-T225-S10/primary-bootstrap-inputs-r2/`.
- Current DEM package evidence: [full route map](../research/t225-s19-dem-full-package-route-reconciliation-001.md),
  [Direct/Readonly family regression](t225-s19-dem-direct-readonly-package-regression-result-001.md),
  [PDB ordering result](t225-s19-dem-pdb-lifecycle-ordering-result-001.md), and
  [DPB result](t225-s19-dem-dpb-failure-regression-result-001.md).

## Procedure

A fresh graph built all 302 edges through `bin/ntdos64-native.exe`.  One
Direct and one Readonly invocation used the same immutable inputs, startup
configuration and autoexec sources, `--instruction-tick-budget 100000`,
`--observe-bop-sequence`, and `--observe-ud-sequence`.  The process was not
externally terminated; each run returned through the CLI's internal bounded
lifecycle.

Captured logs are disposable build evidence under
`build/M0-T225-S19/native-package-validation-r1/native-run/`.

## Observation

| Mode | BOP / #UD records | First BOP | Last BOP | Terminal |
| --- | ---: | --- | --- | --- |
| Direct | 33 / 33, no overflow | `0070:0475 50:11` | `0041:5a70 50:36` | `terminal=4`, budget |
| Readonly | 33 / 33, no overflow | identical | identical | `terminal=4`, budget |

The Direct and Readonly stdout logs have the same SHA-256:
`4512F3D7C428B3C919CA37D8A83998FE49796511D1AEC81FC7BE72FFD44D68D0`.
The copied BOP and generic-UD record sequences are byte-identical.  The first
BOP is the known `50:11` NTIO entry at `0070:0475`; the last is `50:36`.
Both runs report `detail=0`, `lifecycle=5`, `presentation=5`, and
`cancellation=0`.

## Interpretation

The completed DEM package route and failure corrections do not disturb the
existing bounded native composition behavior in either supported product mode.
This supplies the S19 native package-validation exit evidence together with
the all-family fixture; it does not prove guest program completion, COMMAND
package closure, or continuous execution.

No new BOP/provider work follows from these observations.  Later work may use
the trace only to validate an already admitted whole owner package.
