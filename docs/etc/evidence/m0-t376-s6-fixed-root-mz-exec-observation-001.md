# M0 T376 S6 — Fixed-root original MZ execution observation

## Frozen invocation and identity

S6 made exactly one console-owning, non-debug observation:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T375/S1/observer/console-startup-observer.exe \
  --product build/M0-T371/S2/formal-x86/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t376-s6-fastopen.txt \
  --product-command FASTOPEN.EXE \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t376-s6-fastopen.txt.bop-return.txt
```

The observer reported `result=exited`, `exit=0x00000000`, with the fixed root
`O:\ntvdm64`.  Its JSON sidecar recorded:

- product SHA-256 `1b1fc86d15524bf269ff3a770e0f2ec85c85e33cf42ba21389dc33b105262c2f`;
- stage manifest SHA-256 `7bfc49d5e790f17dbac85b284a21078a9d5856204c3ec872b0402ac700dfaed6`;
- fixed media manifest SHA-256
  `3317dea91de479d2a37cbc44c641cdf31f25afc842c484ae6cb04b09fe6f0ef0`.

## Source-owned markers

The DEM-open sidecar contains the complete original MZ workload admission:

```text
MVDM-DEM-OPEN phase=0 ... path=O:\NTVDM64\FASTOPEN.EXE
MVDM-DEM-OPEN phase=1 ... cf=0 path=O:\NTVDM64\FASTOPEN.EXE
```

The original `50:12`, `50:42` and `50:02` file lifecycle markers surround
the workload.  The independent original DEM seek sidecar records four
successful copied-result seeks; the final one returns `0000:C4D0`.  These are
MZ load-path markers absent from the selected `.COM` witness.

After the MZ file activity and the original `50:3C` termination crossing, the
continuation sidecar reaches a later original `54:01` with
`first=0`, `repeat=0`, `dos-state=00000002`.  That is the existing
source-shaped COMMAND next-record acquisition after ordinary guest work; the
app's one-shot terminal policy ends the empty session rather than inventing a
host child or a synthetic parent result.

## Scope conclusion

Together with S3, this proves the selected byte-identical original `.COM` and
the selected byte-identical original MZ artifact both traverse the original
guest-facing DEM file bridge and return to original COMMAND continuation in
the same frozen CPU40/SoftPC container.  It does not claim arbitrary DOS
program compatibility, interactive continuation, 32-bit/native child launch,
or Win16 execution.
