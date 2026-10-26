# M0 T464 S4 — WOW hard-error acceptance and handoff

## Result

T464 closes the admitted WOW hard-error response capability.  The selected
original client and caller tails remain the behavior owners; the project-owned
receiver decodes only the response form that the original client actually
emits.  It does not recreate NT4 CSRSS/USER, invent a scheduler, or terminate a
task or worker.

The released identity is `0.0.464`.  The coherent ten-image manifest is
`assets/release/release-manifest.json`; its deployed twin is
`O:\winnt\system32`.  Publication `build/t464-s2/publication-r2` retained
recoverable pre-publication copies and verified every manifest hash after
replacement.  Guest media and guest source remain unchanged.

## Accepted contract and ownership

- The decoder keeps the original response **source slot** rather than a
  compacted button ordinal, omits zero/unknown descriptors, makes a later
  valid default override an earlier one, and records the actual Cancel source
  slot.
- The modern dialog adapter maps its visible button and Close/Cancel result
  back to that source slot.  It remains a narrow receiver, not a generic
  common-dialog implementation.
- `WOWSysErrorBox` keeps the original timer suspension bracket.  The original
  callers retain the sole response consequences documented by the S3
  [fatal-tail audit](m0-t464-s3-hard-error-fatal-tail-audit.md): raw USER
  return, retry/ignore continuation, debugger/continue branch, or the one
  existing forced-task/hung-app tail.
- The x86 `ntvdm.exe` is the only behavioral response-delivery image change.
  Rebuilding the shared application identity updates the six protocol-bearing
  host programs (`run16`, `ntsrv`, `ntcon`, `ntvdm`, `ntvwm`, and `ntmon`), so
  all peers report `0.0.464`; `WOW32.DLL`, `VDMREDIR.DLL`, and both hook DLLs
  remain hash-identical.

## Executed evidence

1. `tests/observation/verify-wow-hard-error-decoder.ps1` at
   `build/t464-s2/decoder-r11` passed all five source-slot/default/cancel
   assertions.
2. The cached x64 host components, x86 original-SoftPC worker and versioned
   RPC fixtures were rebuilt through `VsDevCmd` and the existing Ninja trees.
   The required application-version bump exposed stale test images; rebuilding
   them was required because the version-negative gate correctly rejects mixed
   peers rather than permitting a partial package.
3. `Invoke-ProductVerification.ps1 -Suite Full` at
   `build/t464-s2/full-gate-r5` passed all fourteen gates in 321,106 ms:
   independent WOW frontiers; Console-17; Window-17; RPC; native GUI; version
   negatives; strict directory; modern EDIT return; cooked return; rapid and
   interactive relaunch; session isolation; retirement wiring; and nested
   window handoff.  Each gate's cleanup also passed.
4. Publication `build/t464-s2/publication-r2/result.txt` reports the exact
   ten-image assets and installed package match the tested manifest.

## Finite retained limits and successor handoff

The original private `DisplayVDMHardError` server is not composable on the
modern host, so this delivery does not claim an automatic real-modal click,
focus, localized private button-label, or exact private title-bar witness.
The receiver's decoder and source-slot contract are executable evidence; the
unavailable private server presentation is explicitly non-pass, not simulated
with hidden windows, polling, or an autonomous replacement.

This candidate does not absorb DDE conversation/data/reply/reentry/cancellation
or peer-death work.  Those retain their own original-owner scope in the next
candidate, **M0 T465 S1 — WOW32 DDE conversation contract**.
