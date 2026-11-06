# T478 S26: original spool failure ownership

S26 owns finding 58 under the original-semantics ceiling. Parent is 7a96b58ce.
This stage corrects confirmed untransferred resource ownership, not the accepted
absence of legacy raw-printer profiles or original abrupt-task-exit cleanup.

## Corrected audit premise: ClosePrinter consumes the handle

The earlier source audit correctly noticed that WG32CloseJob frees its Win16
carrier after a failed ClosePrinter with a nonzero last error, but did not
establish that this was wrong. It is consistent with the provider contract:
[ClosePrinter](https://learn.microsoft.com/en-us/windows/win32/printdocs/closeprinter)
invalidates the native handle on either result. The pinned OpenNT implementation
in printscan/print/spooler/spoolss/client/change.c independently confirms the
valid-handle path calls FreeSpool before RpcClosePrinter can report an error or
exception. Keeping the carrier for another close would retain a stale handle.

WG32CloseJob is therefore unchanged. Its original final-close result precedence
over EndDocPrinter remains; no stronger error aggregation is introduced. Tests
cover both EndDoc outcomes crossed with both ClosePrinter outcomes, including
nonzero failure errors. A provider returning FALSE with no meaningful error is
not a newly supported contract.

## Minimal repair (DIV-880)

- OpenJob closes the opened native printer when StartDoc fails, preserving the
  previously captured error. It also closes that printer if no guest carrier
  can be produced. It does not add EndDoc, DeletePrinter, job rollback or retry.
- GetPrn16 publishes only a successfully written carrier. Failed allocation
  returns zero; failed lock/mapping retires the local allocation and balances
  an acquired lock. Prn32 initializes its failed lookup result to NULL.
- DeviceMode, ExtDeviceMode, DeviceCapabilities and GetEnvironment release
  their copied guest strings before the existing module-load failure return.
  Their normal conversion, callback and driver paths stay unchanged.

The shared LoadLibraryAndGetProcAddresses resolver remains unchanged. It serves
spooler, OLE and Winsock callers, so this stage does not change its table/module
lifetime or invent an alternate provider. Missing exports, partial tables and
reentrancy remain original unsupported-provider limitations, not repaired or
claimed tested by this cleanup. All fifteen requested exports are present in this host's x86 Winspool.drv
export table (binary identity retained); this is availability evidence, not an
actual raw job. Source/object selection and the original
release bodies/entry prefixes are compared with both pinned source families.

## Evidence and precise test boundaries

`verify-wow-spool-failure.ps1` compiles five exact source bodies with isolated
native printer and Win16 local-carrier providers. Baseline: 82 checks, eight
failures. Current: 89 checks, zero failures. It covers normal open/close,
no-profile and failed open, failed StartDoc, failed allocation/lock/mapping,
returned error preservation and normal/failed close consumption. These are
forced boundary failures, not an observed hardware/printer incident or proof
that guest local-memory failure was induced in production.

`verify-wow-spool-loader-failure.ps1` compiles exact entry prefixes through the
module-load decision for four functions. Its appended sentinel is unreachable
in the exercised failed-load paths. Baseline: 72 checks, 15 failures. Current:
72 checks, zero failures. String allocation successes and failures verify that
only allocated copies are freed. This is deliberately a failure-prefix test,
not a full device-capabilities implementation or normal-path emulator.

The image observer binds ten selected functions from actual objects to the
frozen WOW32 executable and verifies CloseJob/shared-resolver bodies unchanged.
Static binding complements, but does not replace, dynamic provider or guest
execution. No live printer installation, host profile changes or real Registry
mutation is performed by either fixture. Successful LocalFree/Unlock is the
retirement boundary; arbitrary callback failure is not made transactional.

## Delivery

Evidence lives in build/t478-s26 and the committed
artifacts/mvdm-audit/shard-3/spool-s26-results.json receipt. Coherent x86/x64 build passes. Frozen package ID is
c701c3dd-9acd-4070-b08b-9ef0acd5021b (version 0.0.478, protocol 67). Existing real
Win16 controls pass: raw OpenJob returns zero without a legacy profile; PrintDlg
default returns both handles and frees them; hook cancellation returns zero,
no extended error and retires its ProcInstance. These do not exercise an actual
raw native job. Runtime copies have independent package identities, recorded
separately from the frozen source. Full passes 16/16 gates in 412371 ms. All ten images and manifest are published
to this worktree assets/release and hash-verified against the frozen package.
DOS4GW/NESticle is owner-excluded and skipped, not passed. O:/winnt, guest/media and
remote repositories remain unchanged. Earlier S23/S24 diagnostic risks are
not claimed fixed by this stage.
