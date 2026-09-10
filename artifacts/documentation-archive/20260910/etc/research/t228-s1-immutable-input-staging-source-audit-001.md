# T228 S1 immutable guest-input staging source audit

## Selected recovery rung

The T228 staging shim is a minimal reuse of
`tools/build/Stage-T225S51GuestExecIntegrationInputs.ps1`. It retains the
same original-toolchain source-built NTIO, NTDOS, COMMAND and SHARE stage
paths, stage-manifest validation, byte counts, SHA-256 identities, copied
byte-for-byte validation, and SHARE-to-TARGET profile alias.

## Required T228 change

The predecessor's emitted manifest allows only T225 S51 observation, so using
it as a T228 runtime input would violate its own asset contract. T228's script
changes only task/schema/compatibility-group/owner labels and `allowedUse` to
T228 S1 immutable Direct/Readonly multi-program integration. It does not
change any guest byte, guest path, profile layout, boot input, target alias,
or forbidden-use rule.

## Recovery ladder

1. Original source-built artifacts: retained and hash-locked; selected.
2. Smallest build shim: selected because scope labeling is the only missing
   composition contract.
3. External intrusion: not applicable.
4. Newly authored runtime behavior: not applicable.

The shim is build metadata only; it neither interprets DOS/BOP semantics nor
changes the CLI/Bochs runtime boundary.
## Observation wrapper recovery

`tools/build/Invoke-T228S1GuestExecIntegration.ps1` is the smallest
source-preserving evolution of the retained T225 native watchdog wrapper.
The original CLI remains the runtime owner: the wrapper only supplies its
existing profile/root/mutation/tick/lifecycle-ledger arguments, captures
stdout/stderr, and terminates only its own child after the declared watchdog.

The older wrapper is not reused verbatim because it requires an S10-only
manifest and its `Start-Process` record did not reliably preserve this native
CLI's exit code. T228 therefore uses the same bounded process lifecycle via
`System.Diagnostics.Process`, with no guest-memory access, BOP interpretation,
CPU/machine behavior, or host capability. The existing default-off lifecycle
ledger is a copied whole-path observation; it names only `50:36`, `50:3C`, and
`54:0B` completion phases and does not select individual BOP work.