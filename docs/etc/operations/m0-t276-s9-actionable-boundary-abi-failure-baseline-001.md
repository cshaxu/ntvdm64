# M0 T276 S9 actionable boundary ABI and failure baseline

## Result

S9 expands the 12 S8 rows that are not already selected original provider
bodies. Each row retains its exact original declaration form, the observed
caller shape, profile guard or availability condition, source-proven result
evidence, and package prerequisite.

The detail is in
[`m0-t276-s9-actionable-boundary-abi-failure-ledger.tsv`](m0-t276-s9-actionable-boundary-abi-failure-ledger.tsv).

## Consequences for wave planning

- The five CPU flag setters share one `adapter-softpc` CPU-status contract;
  they are not five independently invented adapters.
- `init_gen_drivers`, `hunter_init`, and `do_hunter` are build-profile choices.
  Their guarded void callers do not authorize a runtime body.
- `host_validate_pathname` is a host-path contract with an observable false →
  `EG_MISSING_FILE` path, making it a future public-Win32 compatibility issue,
  not a generic machine mechanism.
- `ReadMem` and `WriteMem` remain debugger callback bindings. Their exact
  callback ABI is external to the selected MVDM declaration and therefore
  requires an adapter-debugger ABI review before any binding.
- `malloc` remains a CRT/build-profile input and does not enter an adapter
  recovery wave.

## Limit

The ledger explicitly leaves owner, implementation, build, profile and
recovery state `unreviewed`. It supplies evidence for later package-led work;
it does not choose a provider or enable a build edge.
