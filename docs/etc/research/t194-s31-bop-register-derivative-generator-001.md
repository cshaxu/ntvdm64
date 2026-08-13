# M0 T194 S31: Exact BOP-Register Derivative Generator

Packet: M0 T194 S31  
Disposition: generator-only closure complete; no compiler or executable invoked.

## Change

`New-T98S1CurrentAdapterEngineDerivative.ps1` now exposes
`-BopRegisterObservation`. It reuses the already registered generic listener
and creates a named derivative that replaces exactly `cpu/exception.o` with:

```text
BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1
BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1
BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0
BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0
```

The switch has negative assertions against `main.o`, real-mode-vector,
machine-composition and deferred-startup macros. It does not edit Bochs source,
change adapter ABI, add a selector-specific condition, or invoke a compiler.

## Dry-Run Evidence

The generator produced
`artifacts/build/current/t194-s31-bop-register-observation-dry-r1`. Its
manifest records `bochsReplacementCount: 1` with only
`cpu\\exception.o`; the generated shim has one `cpu\\exception.o:` rule and
no `main.o:` rule. The shim contains the four exact macros above and contains
none of the unrelated diagnostic/composition/deferred macros. Existing archive
prerequisite and host-namespace rejection checks also passed.

## Follow-up

S32 may source-build this exact derivative and verify its map/hash while
retaining all other Bochs inputs. A runtime observation remains separately
blocked by the S28 controller process-tree termination permission issue.
