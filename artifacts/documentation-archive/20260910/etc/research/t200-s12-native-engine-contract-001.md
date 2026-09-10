# T200 S12 — native engine contract foundation

## Question

Can the planned native engine boundary be represented and validated as a standalone copied C ABI without importing machine, guest, or host-service semantics?

## Inputs

- `src/bx-mantle/bx_ntvdm_engine_contract_v1.[ch]`;
- `tests/bx-mantle/bx_ntvdm_engine_contract_v1_test.c`;
- `tools/probe/Invoke-T200S12NativeEngineContractProbe.ps1`;
- the S11 owner/ABI map.

## Procedure

Build and run the focused two-source closure with MSVC x64 `/MT`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S12NativeEngineContractProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s12-native-engine-contract-r1
```

Review the public header and source dependency names for `bochs.h`, BOP, typed CPU-result, finite-run, and minimal-machine symbols. No machine is initialized by this closure.

## Observations

The probe compiled and ran successfully (`runExitCode: 0`) with exactly:

- `src/bx-mantle/bx_ntvdm_engine_contract_v1.c`;
- `tests/bx-mantle/bx_ntvdm_engine_contract_v1_test.c`.

The public request is versioned and bounded: copied UTF-16 descriptors, admitted/excluded drive masks, and a nonzero instruction budget. Validation rejects bad struct identity, zero budget, missing descriptor terminators, and embedded descriptor NULs. The result is also versioned and independently validates every declared terminal category plus reserved-field rejection.

The generated record is `artifacts/build/t200-s12-native-engine-contract-r1/t200-s12-native-engine-contract.json`; it records x64, `/MT`, its exact two-file closure, and the forbidden dependency set.

## Interpretation and confidence

High confidence: a C-only product boundary can now carry setup inputs and terminal categories without exposing Bochs headers, guest locators, host handles, provider identities, or the BOP-local CPU result. This is a contract-only result. It neither initializes a native machine nor assigns a CLI exit code, and it does not replace the retained external-product shell.

## Follow-up

T200 S13 may implement a non-executing `bx_ntvdm_engine_run_v1` lifecycle skeleton that validates the request, invokes composition setup/reset through its owner, and returns categorized setup failure. It must retain the S12 contract, avoid guest execution, and leave the legacy product-startup hook intact until a later full lifecycle substitution is evidenced.
