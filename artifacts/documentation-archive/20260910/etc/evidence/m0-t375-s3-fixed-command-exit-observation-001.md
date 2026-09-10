# M0 T375 S3 — Fixed original COMMAND `EXIT` observation

## Question

Does the rebuilt CPU40/SoftPC product complete the declared built-in when the
CLI receives the correct input shape, `--command EXIT`?

## Inputs and procedure

- Product: `build/M0-T371/S2/formal-x86/original-softpc-process.exe`,
  SHA-256 `350d4a84db26972132ba728703d2af1275c1ed1d609f40b6eaec900b05ef11fb`.
- Fixed stage: `O:\ntvdm64`, format-3 manifest SHA-256
  `3bc14060c3eea8369f4c5f707b54d62ff1aad73ce969ff7875a7f2f06f2d0c78`;
  its 17 media hashes remain the fixed media manifest
  `1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.
- Reused the admitted console-owning, non-debug observer once.  The CLI input
  was `EXIT`; app is the sole owner that serializes it as `/C EXIT`.

## Observation

- The observer produced its controlled eight-second timeout, not a crash.
- It reached original boot, NTIO/NTDOS, COMMAND initialization and the first
  `54:01`; the first record remained the expected
  `TO_TAKE_A_COMMAND → BUSY` transition.
- The later `54:01` again reached original `cmdGetNextCmd` with
  `first=0`, `repeat=0`, `dos-state=00000002`, then blocked in the BaseVDM
  no-next-command wait.  No `.COM` child, MZ child, guest loader or media
  mutation occurred.
- Unlike the discarded long-root attempt at `O:\ntvdm64-t375-s3`, this run
  retained the fixed ten-character product root and emitted the normal BOP
  report.  The long-root attempt is not accepted evidence.

## Source attribution

The fixed command shape removes S1's nested `COMMAND.COM /C EXIT` ambiguity.
Inspection of original `cmdmisc.c` then identifies the next complete owner:
its original `VDMInfo.AppName` is a direct
`CMDINFO.ExecPathSeg:ExecPathOff` guest alias.  The current mirrored function
captured the BaseVDM response in a local `AppName` buffer but did not commit
that result back to the guest path.  That is a bounded, source-defined
COMMAND bootstrap-payload defect, not a BaseVDM wait policy or an external DOS
execution issue.

## Follow-up

T375 S4 owns the complete `CMDINFO` executable-path writeback through the
existing mapping-manager lease, followed by one fixed-container re-observation.
It does not start external `.COM`/MZ execution.
